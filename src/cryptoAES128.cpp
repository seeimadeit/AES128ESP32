#include <cstring>
#include "psaerr2str.h"
#include "AES128ESP32.h"
// #include "psaerr2str.h"
// #include "AES128ESP32.h"
// #include "cryptoAES128.h"
#include <list>
/**
\class cryptoAES128

\brief    For decryption to be successfull the following need to be identical to the values used for the encryption.
      the key data
      the iv data
    For encryption to be successful the following need to exist.
      the key data
      optionally the iv data. If the iv data does not exist a new iv will be generated.

    To setup the key data call the setkeyaccess()

    responsabilities
        create a pass2Key object
          create a masterkey (with makeMasterkey() ) and persist the data (with getMasterKey() ) - you don't want to loose the master key once it has been created.
            the masterkey cannot encrypt or decrypt data because of security added to the data. it can only generate newkey data.
          if you already have the masterkey available, use the function setMasterKey() function.
        generate a newkey from the plain text password. deriveNewKeyfromText().
          you cannot save the newkey data, it always needs to be generated.

        create a cryptoAES128() object
          set the keys to the crypto object using setKeyaccess() * if you change the pass2Key object (masterKey) you need to update the cryto object again with setKeyaccess()
          now you can encrypt but not decrypt.
          if you have a IV already you can call the setIVbytes()
            after you set the IV you can decrypt.
          if you do not have a IV when you call Encrypt() it will generate a new IV, use getIVbytes() so you can save the IV data.
        * if the pass2Key object (masterkey) goes out of scope the cryptoAES128 class will not function. it will probably fail/crash. Thats because the keys (masterkey and crypto key)
        are deleted when the pass2Key object goes out of scope (deleted)

    what to know
      masterkey: can only generate new keys. you can persist the masterkey
      newkey: can only encrypt or decrypt data. you cannot persist the newkey. it is considered secret
      IV: is not considered secret. you can persist the IV, and you should because you will need it to decrypt
      password: is considered secret, it must never be saved.



*/

cryptoAES128::cryptoAES128() {
  DEBUG3("");
  psa_crypto_init();
}

/**
    update the encryption key from the pass2Key object by reference

    errors:
      AES128_ERROR2 : the newkeyid is not valid.
*/
bool cryptoAES128::setKeyaccess(pass2Key &keyholder) {
  DEBUG3("");
  if (!keyholder.newkeyid) {
    cryptoerrortoString(AES128_ERROR2);
    return false;
  }
  key = keyholder.newkeyid;
  return true;
}

/**
    return the IV bytes.
    If the user has not performed an encrypt operation the IV will not have been generated and will return all 0 bytes.
    This condition is not an error, it just means there's no IV data.
    Without an IV it will not be possible to decrypt data, so the user must supply the IV data.

    errors:
      AES128_ERROR1 : the ivdata.size is not valid.
*/
bool cryptoAES128::getIVbytes(ivData &ivdata) {
  DEBUG3("");
  if (ivdata.size == AES128IVSIZE) {
    memcpy(ivdata.data, iv, ivdata.size);
    return true;
  }
  cryptoerrortoString(AES128_ERROR1);
  return false;
}


/**
  set the initialisation vector, this must be done before decryption can happen. This can also happen before encryption
  if you want to use a known IV. Once the IV bytes are set the encryption function will not generate a new IV. If you want to
  clear the IV bytes so a new IV is generated, use the clearIV() function.

    errors:
      AES128_ERROR1 : the ivdata.size is not valid.
*/

bool cryptoAES128::setIVbytes(ivData &ivdata) {
  DEBUG3("");
  if (ivdata.size == AES128IVSIZE) {
    memcpy(iv, ivdata.data, ivdata.size);
    givlen = ivdata.size;
    return true;
  }
    cryptoerrortoString(AES128_ERROR1);
  return false;
}
/**
    clear the IV bytes, you can either generate a new IV during the Encryption function or set a previously existing IV with setIVBytes

    errors:
      none
*/
void cryptoAES128::clearIV() {
  DEBUG3("");
  memset(iv, 0, AES128IVSIZE);
  givlen = 0;
}

/** build the crypto operation, set the key and initialization vector*/
// probably should be private or protected??

bool cryptoAES128::buildKeyForAESEncryptOrDecrypt(bool shouldEncrypt) {
  if ((shouldEncrypt != PSA_KEY_USAGE_DECRYPT) || (shouldEncrypt != PSA_KEY_USAGE_ENCRYPT))
    assert(true);

  psa_status_t status;
  /*psa_aead_operation_t */ op = PSA_AEAD_OPERATION_INIT;
  if (shouldEncrypt == true) {
    status = psa_aead_encrypt_setup(&op, key, PSA_ALG_AEAD_WITH_SHORTENED_TAG(PSA_ALG_GCM, 8));
    if (status != PSA_SUCCESS) {
      FALSEORSTOP(status, "psa_aead_encrypt_setup");
    }
    if (!givlen) {
      status = psa_aead_generate_nonce(&op, iv, sizeof(iv), &givlen);
      if (status != PSA_SUCCESS) {
        FALSEORSTOP(status, "psa_aead_generate_nonce");
      }
      DEBUG3("generated iv size = " + String(givlen));
      assert(givlen == AES128IVSIZE);
    } else {
      // 2. Set the nonce with psa_aead_generate_nonce() or psa_aead_set_nonce().
      status = psa_aead_set_nonce(&op, iv, sizeof(iv));
      if (status != PSA_SUCCESS) {
        FALSEORSTOP(status, "psa_aead_set_nonce");
      }
    }
  } else {
    status = psa_aead_decrypt_setup(&op, key, PSA_ALG_AEAD_WITH_SHORTENED_TAG(PSA_ALG_GCM, 8));
    if (status != PSA_SUCCESS) {
      FALSEORSTOP(status, "psa_aead_decrypt_setup");
    }
    // 2. Set the nonce with psa_aead_generate_nonce() or psa_aead_set_nonce().
    status = psa_aead_set_nonce(&op, iv, sizeof(iv));
    if (status != PSA_SUCCESS) {
      FALSEORSTOP(status, "psa_aead_set_nonce");
    }
  }
  return true;
}

/**
    Return the decrypted string. before the decryption can happen the key information and IV must be set.
      setKeyaccess()
      setIVBytes()

      errors:
        all errors will be psa specific errors
*/
bool cryptoAES128::Decrypt(encryptedData &encdata, String &rtn) {
  DEBUG3("");
  psa_status_t status;
  /*String*/ rtn = "";
  unsigned char outbuf[key_bits / 8];
  assert(sizeof(outbuf) == 16);
  unsigned char inpbuf[key_bits / 8];
  assert(encdata.size % 16 == 0);

  buildKeyForAESEncryptOrDecrypt(false);
  DEBUG3("len = " + String(encdata.size));
  int lastcharactercount = 0;
  size_t olen;
  unsigned char *bytes = encdata.data;
  while (lastcharactercount < encdata.size) {
    DEBUG3("while lastcharactercount = " + String(lastcharactercount));
    for (int i = 0; i < sizeof(inpbuf); i++)
      inpbuf[i] = *bytes++;

    // DUMP("inpbuf decrypt", inpbuf, sizeof(inpbuf));
    lastcharactercount = lastcharactercount + sizeof(inpbuf);
    status = psa_aead_update(&op, inpbuf, sizeof(inpbuf), outbuf, sizeof(outbuf), &olen);
    if (status != PSA_SUCCESS) {
      FALSEORSTOP(status, "psa_aead_update");
    }

    for (int i = 0; i < olen; i++)
      if (outbuf[i])
        rtn.concat((char)outbuf[i]);

    DUMP("outbuf decrypt", outbuf, sizeof(outbuf));
  }  // while
  psa_aead_abort(&op);
  // return rtn;
  return true;
}


/**
  encrypt the string and return the encrypted bytes. NOTE: 
  example (this example code is outdated - it needs replacing)
  \code crypto cry;
  if (!cry.begin())
    while(true) ; // oops it failed to initial the object
  cry.setkeyaccess(&k); // k is the pass2Key object
  size_t numOfbytes=0; // number of bytes in the allocated memory
  unsigned char* encoded=cry.Encrypt("1234567890abcdefG",numOfbytes);
  String rtn = cry.Decrypt(encoded, numOfbytes);
  free(encoded);
  \endcode

        errors:
        all errors will be psa specific errors
*/
bool cryptoAES128::Encrypt(String datatoEncrypt, encryptedData **returndata) {
  DEBUG3("");
  psa_status_t status;
  unsigned char outbuf[key_bits / 8];
  assert(sizeof(outbuf) == 16);
  unsigned char inpbuf[(key_bits / 8)]; 

  buildKeyForAESEncryptOrDecrypt();

  size_t olen;
  std::list<unsigned char>  outputbytes;
  int lastcharactercount = 0;
  int maxlength = datatoEncrypt.length();  
  const char* thetext = datatoEncrypt.c_str();

  // copy at most key_bits /8
  while (lastcharactercount < maxlength) {
    DEBUG3("start of loop");
    memset(inpbuf,0,sizeof(inpbuf));
    size_t copymax = (lastcharactercount + 16 < maxlength-lastcharactercount) ? lastcharactercount + 16 : maxlength-lastcharactercount;
    DEBUG3("copy max = " + String(copymax));
    memcpy(inpbuf, thetext+lastcharactercount, copymax);
    lastcharactercount += 16;

    DUMP("inpbuf", inpbuf, sizeof(inpbuf));

    status = psa_aead_update(&op, inpbuf, sizeof(inpbuf), outbuf, sizeof(outbuf), &olen);
    if (status != PSA_SUCCESS) {
      FALSEORSTOP(status, "psa_aead_update");
    }
    // move encrytped data to output buffer
     Serial.println(heap_caps_check_integrity_all(true));
    for (int i = 0; i < olen; i++)
      outputbytes.push_back(outbuf[i]);
    DUMP("outbuf", outbuf, olen);
  }

  status = psa_aead_abort(&op);
  if (status != PSA_SUCCESS) {
    FALSEORSTOP(status, "psa_aead_update");
  }

  int numbytes = outputbytes.size(); //
  /*encryptedData */ *returndata = new encryptedData(numbytes);
  unsigned char *rtn = (*returndata)->data;
  for (int i = 0; i < numbytes; i++) {
    rtn[i] = outputbytes.front();
    outputbytes.pop_front();
  }
  return true;
}
