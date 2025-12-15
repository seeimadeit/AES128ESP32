
// enable the logging (see AdvancdedDebugging.md)
// #define DEBUGLEVEL 3 in AES AES128ESP32.h
#include <AES128ESP32.h>
#include "LittleFS.h"



void setup() {
  Serial.begin(460800);
  delay(1000);

  if (!LittleFS.begin()) {
    Serial.println("Error LittleFS, is the disk formatted?");
    for (;;)
      ;
  }

  pass2Key keymanager;
  cryptoAES128 crypto;

  // the masterkey should be on disk. if it's not, run Example_CreateAndSave_a_Masterkey
  // this is needed to generate the newkey when the user supplies the password. without it
  // it's just not going to work
  if (LittleFS.exists("/ms.bin")) {
    MasterKeyData masterkeyinfo;
    File f = LittleFS.open("/ms.bin");
    f.readBytes((char *)masterkeyinfo.data, masterkeyinfo.size);
    f.close();
    keymanager.setMasterKey(masterkeyinfo);
  } else {
    Serial.println("masterkey is missing from the disk.");
    for (;;)
      ;
  }

  // the iv data we saved with the encrypted data is loaded here
  ivData ivdata2;

  File infile = LittleFS.open("/privatedata.enc");
  // we need to read the size first because it might change in the future for different encryption
  infile.read((uint8_t *)&ivdata2.size, sizeof(ivdata2.size));
  // now we read the ivdata
  infile.read(ivdata2.data, ivdata2.size);


  //read the binary encrypted data size
  size_t sizeofdata = 0;
  infile.read((uint8_t *)&sizeofdata, sizeof(sizeofdata));
  // now we know the size are can create the object
  encryptedData thebinaryencrypteddata(sizeofdata);

  // now read the data
  infile.read((uint8_t *)thebinaryencrypteddata.data, thebinaryencrypteddata.size);
  // load the keymanager with the ivdata - if we don't load the same iv data that was used
  // to generate the newkey it's not going to work
  crypto.setIVbytes(ivdata2);

  Serial.println("Enter the user password (hint:user password goes here) :");
  while (!Serial.available())
    ;  // wait until the user enters some password - from the serial monitor
  String thepassword = Serial.readStringUntil('\n');
  // generate the newkey using the user supplied password that was used to encrypt the data
  if (false == keymanager.deriveNewKeyfromText(thepassword)) {
    Serial.println("There was an error generating the newkey from the user password");
    for (;;)
      ;
  }
  // tell the crypto object to use the keys
  if (false == crypto.setKeyaccess(keymanager)) {
    Serial.println("failed to setkeyaccess");
    for (;;)
      ;
  }

  String plaintext;
  crypto.Decrypt(thebinaryencrypteddata, plaintext);

  Serial.println(plaintext);
  // if the data returned is junk then the user entered the wrong password.
}

void loop() {
  // put your main code here, to run repeatedly:
}
