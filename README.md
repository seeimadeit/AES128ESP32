# ReadME.txt


Download the library as a .ZIP file (click on \<\>Code , Download ZIP), 
then from the Arduino IDE click on Sketch -> Include Library -> Add .ZIP Library.
Some examples are available from File -> Examples -> ASP128ESP32 Library.


# AES128ESP32 Library
=====================

>This is a tool that builds on the psa crypto library that is a part of the Arduino for ESP32. 
> The Idea is to make it easy to implement encrypted data. This library does not work for wire transfer.
> This tool will encrypt/decrypted text with the purpose of saving/reading it to the ESP32.

>_Examples for usage might be:_
>  
1. Saving configuration information. Like the WiFi password.
2. Saving Protected Health Information (PHI). Like heart rates/blood presure.

>
This is not designed for commercial products. 
Know this only implements AES 128bit. So it's secure but not the
most secure. 
 
AES is symmetric cryptography, which means the data is both encrypted and decrypted
with the same key. There is no concept of public and private key here.

Cryptography is hard. I'm not an expert by anymeans, so there could be problems I just don't know about. 
I've made the source code available so other people can review the source. Hopefully people that really know
about this subject can give feedback.

# Repository Contents

**./examples** - Example sketches for the library (.ino). Run these from the Arduino IDE. 

**./src** - Source files for the library (.cpp, .h).

**.keywords.txt** - Keywords from this library that will be highlighted in the Arduino IDE. 

**.library.properties** - General library properties for the Arduino package manager. 

# Documentation

**/html/index.html**

License Information
-------------------
This product is _**open source**_! 

Please review the LICENSE.md file for license information. 
Please use, reuse, and modify these files as you see fit. 
Distributed as-is; no warranty is given. 

Peter Hall
