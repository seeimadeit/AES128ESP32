#include "psaerr2str.h"
#include "AES128ESP32.h"
// #include "pbkdf2.h"

/** class constructor: initialize the psa cypto library*/
pass2Key::pass2Key() {
  DEBUG3("");
  psa_crypto_init();
}

/**
  class destruction: unloads the masterkey and the newkey if they exist.
*/
pass2Key::~pass2Key() {
  DEBUG3("");
  if (masterkeyid) {
    psa_destroy_key(masterkeyid);
    masterkeyid = 0;
  }
  if (newkeyid) {
    psa_destroy_key(newkeyid);
    newkeyid = 0;
  }
  // mbedtls_psa_crypto_free(); // this is not a good idea because it will erase ALL crypto data. so if there are multiple crypto objects running this will break them
  // perhaps we should give a static method that can be called by the when it's safe to free the crypto store.
}

/**
  provides access to the masterkey data, you can save this data for future use. The masterkey data is only available after a call to makeMasterKey().

  returns true if successful or false if not.

  errors:
  AES128_ERROR3: masterkey is not valid
  AES128_ERROR4: masterkey invalid size
  all other errors will be psa errors

     note: If DEBUGLEVEL is defined and an error happens, it will freeze forever and display a message on the serial port.
*/
bool pass2Key::getMasterKey(MasterKeyData &mkd) {
  DEBUG3("");
  // assert(masterkeyid);
  if (!masterkeyid) {
    cryptoerrortoString(AES128_ERROR3);
    return false;
  }

  DEBUG3("psa_export_key");
  size_t key_size = 0;
  psa_status_t status = psa_export_key(masterkeyid, mkd.data, mkd.size, &key_size);
  if (status != PSA_SUCCESS) {
    FALSEORSTOP(status, "psa_export_key");
  }
  // assert(key_size == mkd.size);
  if (key_size!=mkd.size) {
    cryptoerrortoString(AES128_ERROR4);
    return false;
  }
  return true;
}
/**
    sets the masterkey data and creates the internal masterkey id.

    errors:
      all errors will be psa errors

       note: If DEBUGLEVEL is defined and an error happens, it will freeze forever and display a message on the serial port.
*/
bool pass2Key::setMasterKey(MasterKeyData &mkd) {
  DEBUG3("");
  if (masterkeyid) {
    DEBUG3("masterkeyid is already set. removing masterkey, this should never happed");
    psa_destroy_key(masterkeyid);
  }
  DEBUG3("adding masterkey");
  psa_key_attributes_t attributes = getattributes(true);

  psa_status_t status = psa_import_key(&attributes, mkd.data, mkd.size, &masterkeyid);
  if (status != PSA_SUCCESS) {
    FALSEORSTOP(status, "psa_import_key");
  }
  return true;
}

/**
    setup the attibutes
*/

/** attributes for the masterkey and newkey are different. The masterkey is permitted to be exported. The newkey cannot be exported.
The masterkey can only generate a newkey, it is not capable of encryption.
The newkey can only encode or decode, it is not capable of key generation nor can it exported.*/
psa_key_attributes_t pass2Key::getattributes(bool formaster) {
  DEBUG3("");
  psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
  if (formaster) {
    psa_set_key_usage_flags(&attributes, PSA_KEY_USAGE_DERIVE | PSA_KEY_USAGE_EXPORT);
    psa_set_key_algorithm(&attributes, PSA_ALG_HKDF(PSA_ALG_SHA_256));
    psa_set_key_type(&attributes, PSA_KEY_TYPE_DERIVE);
  } else {
    psa_set_key_usage_flags(&attributes, PSA_KEY_USAGE_ENCRYPT | PSA_KEY_USAGE_DECRYPT);
    psa_set_key_algorithm(&attributes, PSA_ALG_AEAD_WITH_SHORTENED_TAG(PSA_ALG_GCM, 8));
    psa_set_key_type(&attributes, PSA_KEY_TYPE_AES);
  }
  psa_set_key_bits(&attributes, AES128BYTES * 8);
  return attributes;
}

/**
    creates a new masterkey. Use getMasterKey() and save the data. Without it your screwed.

    errors:
      AES128_ERROR5: masterkey has already been created
      all other errors will be from psa

       note: If DEBUGLEVEL is defined and an error happens, it will freeze forever and display a message on the serial port.
  */
bool pass2Key::makeMasterKey() {
  DEBUG3("");

  // assert(!masterkeyid);
  if (masterkeyid) {
    cryptoerrortoString(AES128_ERROR5);
    return false;
  }
  psa_key_attributes_t attributes = getattributes(true);

  psa_status_t status = psa_generate_key(&attributes, &masterkeyid);
  if (status != PSA_SUCCESS) {
    FALSEORSTOP(status, "psa_generate_key");
  }
  return true;
}

// #warning derived key salt is hardcoded
/**
    the masterkey id must already have a value. If the masterkey is not available it will assert (reboot esp32).
    using the masterkey derive a new key from the masterkey and the supplied text.
    before calling this function the following must be setup
      i) a masterkey must exist.
    optional setup
      i) edit the \b MSTRKEYSALT #define value in pbkdf2.h

    return true if the newkey was generated. return false if it failed.
    
    errors:
      AES128_ERROR6: masterkey has not been set.
      all other errors will be psa errors

    note: If DEBUGLEVEL is defined and an error happens, it will freeze forever and display a message on the serial port.
*/
//    future: handle the salt information also as parameters, what about the number of loops counts? I'm not currently do anything about that.
// update esp32 does not support PBKDFK2 so no counts are supported for this type of key gen.

bool pass2Key::deriveNewKeyfromText(String text) {
  DEBUG3("");

  String tsalt = MSTRKEYSALT;
  const unsigned char *salt = (const unsigned char *)tsalt.c_str();
  const size_t saltlen = tsalt.length();
  // assert(masterkeyid);
  if (!masterkeyid) {
    cryptoerrortoString(AES128_ERROR6);
  }

#ifdef DEBUGLEVEL
  derivedkeytext = text;
#endif
  size_t secretlength = text.length();
  unsigned char secret[secretlength + 1];
  memcpy(secret, text.c_str(), secretlength);
  psa_status_t status = PSA_SUCCESS;
  psa_key_attributes_t attributes = getattributes();
  psa_key_derivation_operation_t operation = PSA_KEY_DERIVATION_OPERATION_INIT;

  /* leaving this here for historical reasons - basically the esp32 does not support PBKDF2 algorithms
    if the above was working it would need a COST value
    status = psa_key_derivation_input_integer(&operation, PSA_KEY_DERIVATION_INPUT_COST, iteration_count);
    */

  DEBUG3("psa_key_derivation_setup PSA_ALG_HKDF");
  status = psa_key_derivation_setup(&operation, PSA_ALG_HKDF(PSA_ALG_SHA_256));
  if (status != PSA_SUCCESS) {
    psa_key_derivation_abort(&operation);
    FALSEORSTOP(status, "psa_key_derivation_setup");
  }

  DEBUG3("psa_key_derivation_input_bytes PSA_KEY_DERIVATION_INPUT_SALT =" + tsalt);
  status = psa_key_derivation_input_bytes(&operation, PSA_KEY_DERIVATION_INPUT_SALT, (uint8_t *)salt, saltlen);
  if (status != PSA_SUCCESS) {
    psa_key_derivation_abort(&operation);
    FALSEORSTOP(status, "psa_key_derivation_input_bytes");
  }

  DEBUG3("psa_key_derivation_input_key PSA_KEY_DERIVATION_INPUT_SECRET");
  status = psa_key_derivation_input_key(&operation, PSA_KEY_DERIVATION_INPUT_SECRET, masterkeyid);
  if (status != PSA_SUCCESS) {
    psa_key_derivation_abort(&operation);
    FALSEORSTOP(status, "psa_key_derivation_input_key");
  }

  DEBUG3("psa_key_derivation_input_bytes PSA_KEY_DERIVATION_INPUT_INFO");
  status = psa_key_derivation_input_bytes(&operation, PSA_KEY_DERIVATION_INPUT_INFO, (const uint8_t *)secret, sizeof(secret));
  if (status != PSA_SUCCESS) {
    psa_key_derivation_abort(&operation);
    FALSEORSTOP(status, "psa_key_derivation_input_bytes");
  }

  DEBUG3("psa_key_derivation_set_capacity ");
  status = psa_key_derivation_set_capacity(&operation, AES128BYTES /*NUMBYTESX*/);
  if (status != PSA_SUCCESS) {
    psa_key_derivation_abort(&operation);
    FALSEORSTOP(status, "psa_key_derivation_set_capacity");
  }

  DEBUG3("psa_key_derivation_output_key");
  status = psa_key_derivation_output_key(&attributes, &operation, &newkeyid);
  if (status != PSA_SUCCESS) {
    psa_key_derivation_abort(&operation);
    FALSEORSTOP(status, "psa_key_derivation_output_key");
  }

  // always clean up the operation
  DEBUG3("psa_key_derivation_abort");
  status = psa_key_derivation_abort(&operation);
  if (status != PSA_SUCCESS) {
    FALSEORSTOP(status, "psa_key_derivation_abort");
  }

  return true;
}
