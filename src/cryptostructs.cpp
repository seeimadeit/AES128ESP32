// #include <Arduino.h>
#include "AES128ESP32.h"

/** \class databytes
\brief baseclass for memory allocation and size information.
*/

/**. creates and allocates memory or size s.*/
databytes:: databytes(size_t s) {
  DEBUG3("");
    data = (unsigned char*)malloc(s);
    size = s;
  }

/** destructor will clear and delete the allocated memory if there is any*/
 databytes::~databytes() {
  DEBUG3("");
    deletedata();
  }

/** zeros out the allocated memory*/
   void databytes::cleardata() {
    DEBUG3("");
    if(size)
      memset(data,0,size);
  }


/** clear and delete the allocated memory. this functions is private. if the deletedata if called, it is useless*/
   void databytes::deletedata(){
    DEBUG3("");
    if (size) {
      cleardata();
      free(data);
      data=NULL;
      size=0;
    }
  }