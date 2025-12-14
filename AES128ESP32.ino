// #define DEBUGLEVEL 3
#include "src/AES128ESP32.h"
#include "LittleFS.h"

void setup() {

  Serial.begin(460800);
  delay(1000);

  if (!LittleFS.begin()) {
    Serial.println("oops little FS");
    // LittleFS.format();
    // if (!LittleFS.begin()) {
    // Serial.println("failed after format....");
    // }
  }

  Serial.println("--crypto test---");
  // set to true to remove the masterkey from disk
  bool deletemasterfile = false;
  if (deletemasterfile) {
    LittleFS.remove("/ms.bin");
  }

  pass2Key keymanager;
  cryptoAES128 crypto;

  MasterKeyData masterkeyinfo;

  if (!LittleFS.exists("/ms.bin")) {
    keymanager.makeMasterKey();

    keymanager.getMasterKey(masterkeyinfo);
    DUMP("masterkeyinfo", masterkeyinfo.data, masterkeyinfo.size);

    Serial.println("Writing masterkey to disk");
    File f = LittleFS.open("/ms.bin", FILE_WRITE, true);
    f.write(masterkeyinfo.data, masterkeyinfo.size);
    f.close();
  } else {
    Serial.println("Reading masterkeyinfo from disk");
    File f = LittleFS.open("/ms.bin");
    f.readBytes((char *)masterkeyinfo.data, masterkeyinfo.size);
    f.close();
    keymanager.setMasterKey(masterkeyinfo);
    DUMP("masterkeyinfo", masterkeyinfo.data, masterkeyinfo.size);
  }
  masterkeyinfo.cleardata();

  keymanager.deriveNewKeyfromText("xx1");

  crypto.setKeyaccess(&keymanager);
  encryptedData *returndata;
  crypto.Encrypt("123456789abcdefG", &returndata);
  ivData saveiv;
  saveiv.size=8;
  if (false==crypto.getIVbytes(saveiv))
    {
      psa_status_t lasterror=getLastAES128error();
      Serial.println("last error = " + String(lasterror));
      Serial.println(cryptoerrortoString(lasterror));
    }
  crypto.setIVbytes(saveiv);
  String rtn;
  crypto.Decrypt(returndata, rtn);
  delete returndata;
  Serial.println("rtn = " + rtn);
}

void loop() {
  // put your main code here, to run repeatedly:
}
