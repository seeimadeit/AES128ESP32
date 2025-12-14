#pragma once

#include "AES128ESP32.h"
// #include <Arduino.h>
// #include <cstring>

class pass2Key;
// #include "psa/crypto_values.h"

// #define DEBUGLEVEL 2
// #include "delays.h"
// #include "psaerr2str.h"
// #include "pbkdf2.h"


// #include "psa/crypto.h"
// AES128

/**
    For decryption to be successfull the following need to be identical to the values used for the encryption.
      the key data
      the iv data
    For encryption to be successful the following need to exist.
      the key data
      optionally the iv data. If the iv data does not exist a new iv will be generated.

    To setup the key data call the setkeyaccess()

    responsabilities
        create a pass2Key object
          create a masterkey (with makekey() ) and persist the data (with getmasterkey() ) - you don't want to loose the master key once it has been created.
            the masterkey cannot encrypt or decrypt data because of security added to the data. it can only generate
            newkey data.
          if you already have the masterkey available use the function setmasterkey()
        generate a newkey from the plain text password. derivekeyfromtext().
          you cannot save the newkey data, it always needs to be generated.

        create a crpto object
          set the keys to the crypto object using setkeyaccess() * if you change the pass2Key object you need to update the cryto object again
          now you can encrypt but not decrypt.
          if you have a IV already you can call the setIVbytes()
            after you set the IV you can decrypt.
          if you do not have a IV when you call Encrypt() it will generate a new IV, use getIVbytes() so you can save the IV data.
        * if the pass2Key goes out of scope the cypto class will not function. it will probably fail/crash.

    what to know
      masterkey: can only generate new keys. you can persist the masterkey
      newkey: can only encrypt or decrypt data. you cannot persist the newkey. it is considered secret
      IV: is not considered secret. you can persist the IV.
      password: is considered secret, it must never be saved.



*/



class cryptoAES128 {

  unsigned char iv[AES128IVSIZE] = { 0x00 };
  size_t givlen = 0;  // generated IV
  psa_key_id_t key;   // this is the PSA key
  size_t key_bits = AES128BYTES * 8;
  psa_aead_operation_t op;
public:
  cryptoAES128();
  bool setKeyaccess(pass2Key *keyholder) { return setKeyaccess(*keyholder); }
  bool setKeyaccess(pass2Key &keyholder);
  bool getIVbytes(ivData &ivdata);
  bool setIVbytes(ivData &ivdata);
  void clearIV();
  bool Decrypt(encryptedData &encdata, String &rtn);  
  inline bool Decrypt(encryptedData *encdata, String &rtn) {return Decrypt(*encdata,rtn);}
  bool Encrypt(String datatoEncrypt,encryptedData  **);
private:
  bool buildKeyForAESEncryptOrDecrypt(bool shouldEncrypt = true);
};

