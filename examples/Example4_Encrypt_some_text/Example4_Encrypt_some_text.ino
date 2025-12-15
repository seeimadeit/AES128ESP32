
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

  // the masterkey should be on disk. if it's not, run Example_CreateAndSave_a_Masterkey
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

  if (false == crypto.setKeyaccess(keymanager)) {
    Serial.println("failed to setkeyaccess");
    for (;;)
      ;
  }

  encryptedData *thebinaryencrypteddata;
  String insecret = "This is the text I want to keep a secret.One day I will let you know what the secret is.";
  if (false == crypto.Encrypt(insecret, &thebinaryencrypteddata)) {
    Serial.println("There was an error encrypting the secret");
    psa_status_t errorcode = getLastAES128error();
    Serial.println(cryptoerrortoString(errorcode));
  }
  DUMP("The encoded secret is:", thebinaryencrypteddata->data, thebinaryencrypteddata->size);

  // save the encrpted text to disk.
  // requires saving both the encrypted data and the IV data. because I did not supply
  // the IV it will be generated the first time I called the Encrypt function.

  // create the file to store the encrypted data.
  File outfile =  LittleFS.open("/privatedata.enc",FILE_WRITE,true);
  // lets save the IV data first because it will make it easy to decode it later.
  ivData ivdata;
  crypto.getIVbytes(ivdata);
  outfile.write((uint8_t*)&ivdata.size,sizeof(ivdata.size)); // put the data size first
  outfile.write(ivdata.data,ivdata.size); // next write the ivdata.
  // now we can write the encrypted data
  outfile.write((uint8_t*)&thebinaryencrypteddata->size,sizeof(thebinaryencrypteddata->size)); // do the size first
  outfile.write(thebinaryencrypteddata->data,thebinaryencrypteddata->size); // next the encrypted data
  outfile.close();
  // look at the Example_Decrypt_some_text for reading the data.

  String rtn;
  crypto.Decrypt(thebinaryencrypteddata, rtn);
  Serial.println("input secret =  :" + insecret + ":");
  Serial.println("output secret = :" + rtn + ":");
  Serial.println("compare: " + String(insecret.compareTo(rtn)));
}

void loop() {
  // put your main code here, to run repeatedly:
}
