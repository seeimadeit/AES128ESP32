#pragma once
// #include "delays.h"
#include "AES128ESP32.h"


#ifdef DEBUGLEVEL
  #if DEBUGLEVEL > 1
    #define FALSEORSTOP(status, function) \
      DEBUG2(String(function) + " failed " + cryptoerrortoString(status)); \
      STOP;
  #else
    #define FALSEORSTOP(s,f) return false;
  #endif
#else
  #define FALSEORSTOP(s, f) return false;
#endif

#if DEBUGLEVEL > 2
  // #warning debug level set
  #define DUMP(t, b, l) cryptodump(t, b, l)
#else
  #define DUMP(t, b, l)
#endif

#define AES128_ERROR1 1000
#define AES128_ERROR2 1001
#define AES128_ERROR3 1002
#define AES128_ERROR4 1003
#define AES128_ERROR5 1004
#define AES128_ERROR6 1005



psa_status_t getLastAES128error();
void cryptodump(String title, unsigned char *buf, int lenbuff);
String cryptoerrortoString(psa_status_t err);
