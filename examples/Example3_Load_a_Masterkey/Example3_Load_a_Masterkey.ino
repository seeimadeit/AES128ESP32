
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

  Serial.println("--Load a masterkey---");
  pass2Key keymanager;

  if (LittleFS.exists("/ms.bin")) {
    MasterKeyData masterkeyinfo;
    Serial.println("Reading the masterkey from disk.");
    File f = LittleFS.open("/ms.bin");
    f.readBytes((char *)masterkeyinfo.data, masterkeyinfo.size);
    f.close();
    DUMP("masterkey key data", masterkeyinfo.data, masterkeyinfo.size);
    Serial.println("Masterkey is loaded, install the masterkey into the keymanager.");
    keymanager.setMasterKey(masterkeyinfo);
    // this is not nessisary because it will clear the data
    // when it goes out of scope. I've included it here to show you can
    // clear the data when your finished with it.
    masterkeyinfo.cleardata();
  } else {
    Serial.println("masterkey is missing from the disk.");
    for (;;)
      ;
  }
}

void loop() {
  // put your main code here, to run repeatedly:
}
