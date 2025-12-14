
# Advanced debugging


To debug this tool you can set a DEBUGLEVEL with a define like this:

`#define DEBUGLEVEL n`

where n can be 0,1,2,3
 
***Example*** 

        `// enable the logging`
        `#define DEBUGLEVEL 3`
        `#include <AES128ESP32.h>`


_*Note_* the **#define** will need to be before the ***#include***.


>Because the Arduino IDE is a tool for beginnings there is no way to
>
> perform a clean build. After a lot of research I've found you need to
>
touch one of the source code files in the library directory.

(*that means modify the date of the file*).
>
>
>
On a ***mac*** (using Arduino IDE 2.3.6) the path to the library file to touch is:

        /Users/???/Documents/Arduino/libraries/AES128ESP32_Library/src/AES128ESP32.h


On windows the path to the library file to touch is :

        C:\Users\???\Documents\Arduino\libraries\AES128ESP32_Library\src\AES128ESP32.h


Add the **#define** statement to your _.ino_ sketch, then from the IDE and do a save as.. and give it a new name.

If that doesn't work you may need to clear out the cache directory.
On the mac it's located at

        /Users/???/Library/Caches/arduino/sketches

The best solution to this short coming would be if the IDE actually had a clean build option but after doing the
research I've found that the IDE team have deliberately choosen to not have that ability. 


-If you don't want the debugging output you will need to repeat the process and remove the #define.
