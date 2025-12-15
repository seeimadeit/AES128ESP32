
// enable the logging (see AdvancdedDebugging.md)
#define DEBUGLEVEL 3
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

  if (false == keymanager.deriveNewKeyfromText("user password goes here")) {
    Serial.println("There was an error generating the newkey from the user password");
    for (;;)
      ;
  }

  if (false == crypto.setKeyaccess(keymanager))
  {
    Serial.println("failed to setkeyaccess");
    for(;;);
  }

  encryptedData *thebinaryencrypteddata;
  String insecret="This is the text I want to keep a secret.One day I will let you know what the secret is.";
  // if (false == crypto.Encrypt("this is the text I want to keep a secret", &thebinaryencrypteddata)) {
    //                                  1.        2.        3
    //                         123456789012345678901234567890
  if (false == crypto.Encrypt(insecret, &thebinaryencrypteddata)) {
    Serial.println("There was an error encrypting the secret");
    psa_status_t errorcode = getLastAES128error();
    Serial.println(cryptoerrortoString(errorcode));
  }
  DUMP("The encoded secret is:", thebinaryencrypteddata->data, thebinaryencrypteddata->size);

String rtn;
  crypto.Decrypt(thebinaryencrypteddata,rtn);
  Serial.println("input secret =  :"+ insecret +":");
  Serial.println("output secret = :"+ rtn +":");
  Serial.println("compare: " + String(insecret.compareTo(rtn)));
}

void loop() {
  // put your main code here, to run repeatedly:
}
