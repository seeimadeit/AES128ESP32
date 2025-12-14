#pragma once
#include "AES128ESP32.h"
// #include <Arduino.h>
// #include <psa/crypto.h>
// #include "delays.h"

// masterkey type is based on PSA_ALG_HKDF(PSA_ALG_SHA_256)
// which requires 32 bytes

/** #define MSTRKEYSALT
\brief  When a newkey is generated it will use this value as the salt/iv. 
This is only used for the newkey generation, this salt value is not used for encryption/decryption*/
// put whatever you want to in here, but do not make it empty/blank.
#define MSTRKEYSALT "Something Random"

/**
\class pass2Key

\brief Create suitable keys for key generation (the masterkey) and encryption/decryption (newkey)

  pass2Key is used to generate a new crypto key suitable for use with AES128 encryption. It uses the PSA library which on the esp32 uses a hardware device (i think).
  The idea is to create and store a masterkey. The master key can be stored, but it should never be distributed. It can never be used for encryption. Instead
  after the masterkey has been created or reloaded you use a plain text password to generate a newkey. The newkey should never be saved. The newkey can be
  used to for AES128 encryption.

*/

// known issues:
// todo:  add salt generation

class pass2Key {
  psa_key_id_t masterkeyid = 0;
  psa_key_id_t newkeyid = 0;
#ifdef DEBUGLEVEL
  String derivedkeytext;
#endif
  friend class cryptoAES128;
public:
  pass2Key();
  ~pass2Key();

  bool getMasterKey(MasterKeyData &mkd);
  bool setMasterKey(MasterKeyData &mkd);
  bool makeMasterKey();
  bool deriveNewKeyfromText(String text);
private:
  psa_key_attributes_t getattributes(bool formaster = false);
};
