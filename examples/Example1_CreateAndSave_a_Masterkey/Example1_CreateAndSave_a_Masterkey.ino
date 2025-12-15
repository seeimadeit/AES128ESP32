
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

  Serial.println("--Create and save a masterkey---");

  pass2Key keymanager;
  MasterKeyData masterkeyinfo;

  if (!LittleFS.exists("/ms.bin")) {
    // create a master key
    keymanager.makeMasterKey();
    // access the masterkey binary data
    keymanager.getMasterKey(masterkeyinfo);

    Serial.println("Writing masterkey to disk");
    File f = LittleFS.open("/ms.bin", FILE_WRITE, true);
    // save the masterkey data to disk
    f.write(masterkeyinfo.data, masterkeyinfo.size);
    f.close();
  } else {
    Serial.println("masterkey is already save to disk.");
  }
}

void loop() {
  // put your main code here, to run repeatedly:
}
