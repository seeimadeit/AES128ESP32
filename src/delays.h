#pragma once
#include <Arduino.h>
// #include <source_location>
#undef STOP
#define STOP   Serial.print(__FILENAME__); \
  Serial.print(":"); \
  Serial.print(__LINE__); \
  Serial.print(":"); \
  Serial.print(__ASSERT_FUNC);\
  for(;;);

#ifdef DEBUGLEVEL
  #warning("DEBUGLEVEL is defined")
#endif

#define DBMSG(l, s) \
  Serial.print("LVL:"); \
  Serial.print(String(l)); \
  Serial.print(" "); \
  Serial.print(__FILENAME__); \
  Serial.print(":"); \
  Serial.print(__LINE__); \
  Serial.print(":"); \
  Serial.print(__ASSERT_FUNC); \
   Serial.print(" "); \
  Serial.println(s); \
  Serial.flush(); 
  // delay(100);

#ifndef DEBUG1
#define DEBUG1
#endif

#ifndef DEBUG2
#define DEBUG2
#endif

#ifndef DEBUG3
#define DEBUG3
#endif

#if DEBUGLEVEL > 0
#undef DEBUG1
#define DEBUG1(s) \
  DBMSG(1, s)
#endif

#if DEBUGLEVEL > 1
#undef DEBUG2
#define DEBUG2(s) \
  DBMSG(2, s)
#endif

#if DEBUGLEVEL > 2
#undef DEBUG3
#define DEBUG3(s) \
  DBMSG(3, s)
#endif

