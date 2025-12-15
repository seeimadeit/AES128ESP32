#pragma once

/** change the DEBUGLEVEL from 0, 1 ,2 or 3 or just leave it commented out*/
// #define DEBUGLEVEL 3
/** if your application just stops and freezes, set DEBUGLEVEL to 2. This will allow you to see
where it stopped.
if you just want to have a look around then DEBUGLEVEL 3 will give you more information.
DEBUGLEVEL 2 is what I used in my own programs. 
DEBUGLEVEL 1 will not give you any more information, I use it in my own programs.
DEBUGLEVEL 0 will turn off the DEBUG information and will not display a warning
if DEBUGLEVEL is just not defined, instead of freezes the function will true false on an error. everything will still work but you will get a warning from the
compiler saying it's not defined - it will not cause any problems.
There is a macro called DUMP() which calls dump if the DEBUGLEVEL is 3.
DEBUGx(text) whre x is 1,2 or 3, is what I use to output to the serial port.
*/
#include <Arduino.h>
#include "psa/crypto.h"
#include "delays.h"
#include "psaerr2str.h"
#include "cryptostructs.h"
#include "pbkdf2.h"
#include "cryptoAES128.h"

// some helpful webpages
// this code was derived from
//https://github.com/Mbed-TLS/TF-PSA-Crypto/blob/cb9d0ed64831da3e7b85ea8741a57fdc27c010e6/programs/psa/aead_demo.c
//https://mbed-tls.readthedocs.io/en/latest/kb/development/sample_applications/#aes
// information about psa can be found here :
// https://arm-software.github.io/psa-api/crypto/1.2/overview/intro.html

