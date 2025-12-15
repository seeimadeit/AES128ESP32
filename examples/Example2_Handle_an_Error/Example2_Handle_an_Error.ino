
// enable the logging (see AdvancdedDebugging.md)
/** Example shows how to handle an error condition.
  Note this example is doing something to force an error, changing the data size is not
  something you should do but its a good way to demonstrate how to deal with an error

*/
#define DEBUGLEVEL 3
#include <AES128ESP32.h>

void setup() {

  Serial.begin(460800);
  delay(1000);

  cryptoAES128 crypto;
  ivData saveiv;
  // this will force an error 
  saveiv.size=8; 
  // must of the functions return a false value if there was a problem
  if (false==crypto.getIVbytes(saveiv))
    {
      // retreive the last error code
      psa_status_t lasterror=getLastAES128error();
      Serial.println("last error = " + String(lasterror));
      // translate the error code into something more understandable
      Serial.println(cryptoerrortoString(lasterror));
      // stop forever - your code should choose what to do here.
      for(;;);
    }

}

void loop() {
  // put your main code here, to run repeatedly:
}
