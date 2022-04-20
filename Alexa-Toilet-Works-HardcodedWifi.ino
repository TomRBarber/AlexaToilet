/*
 * Rui Santos 
 * Complete Project Details http://randomnerdtutorials.com
*/

#include <IRremoteESP8266.h>
#include <IRsend.h>

const uint16_t kIrLed = 4;  // ESP8266 GPIO pin to use. Recommended: 4 (D2).

IRsend irsend(kIrLed);  // Set the GPIO to be used to sending the message.

#include <Arduino.h>
#ifdef ESP32
  #include <WiFi.h>
  #define RF_RECEIVER 13
  #define RELAY_PIN_1 12
  #define RELAY_PIN_2 14
#else
  #include <ESP8266WiFi.h>
  #define RF_RECEIVER 5
  #define RELAY_PIN_1 4
  #define RELAY_PIN_2 14
#endif
#include "fauxmoESP.h"

#include <RCSwitch.h>

#define SERIAL_BAUDRATE 9600

#define WIFI_SSID "SSID HERE"
#define WIFI_PASS "password"

#define LAMP_1 "Seat"
#define LAMP_2 "Flush"

fauxmoESP fauxmo;

uint16_t Seat[245] = {6028, 2970,  592, 526,  594, 1644,  600, 518,  594, 526,  596, 522,  598, 520,  592, 528,  594, 524,  598, 522,  600, 520,  592, 526,  596, 1644,  602, 518,  594, 526,  596, 524,  598, 520,  654, 466,  594, 524,  598, 522,  600, 518,  592, 528,  646, 472,  598, 522,  600, 1638,  596, 1644,  590, 1648,  598, 1642,  592, 526,  596, 1644,  602, 1638,  596, 524,  598, 1640,  594, 1644,  600, 1640,  596, 1644,  600, 520,  592, 1646,  598, 1640,  594, 524,  596, 31058,  6026, 2972,  600, 518,  592, 1646,  598, 520,  592, 528,  594, 526,  596, 522,  600, 520,  592, 528,  596, 524,  598, 522,  600, 518,  592, 1648,  598, 520,  590, 528,  594, 526,  596, 522,  600, 520,  592, 526,  596, 524,  598, 520,  652, 466,  594, 526,  596, 524,  598, 1642,  594, 1644,  600, 1640,  594, 1644,  602, 518,  594, 1646,  600, 1640,  594, 524,  596, 1642,  592, 1646,  598, 1642,  594, 1646,  600, 520,  592, 1646,  596, 1642,  592, 526,  596, 31376,  6024, 2974,  600, 518,  594, 1646,  600, 520,  592, 528,  594, 526,  596, 522,  600, 520,  654, 466,  594, 524,  598, 522,  602, 518,  594, 1646,  598, 520,  592, 528,  596, 524,  598, 522,  600, 518,  592, 526,  596, 524,  598, 520,  590, 528,  594, 526,  598, 522,  600, 1640,  596, 1644,  600, 1638,  596, 1644,  590, 530,  592, 1646,  600, 1640,  594, 524,  598, 1642,  592, 1648,  598, 1640,  594, 1646,  600, 520,  592, 1646,  598, 1642,  592, 526,  596}; 
uint16_t Flush[163] = {6028, 2970,  594, 526,  594, 1644,  602, 518,  594, 526,  596, 522,  598, 520,  592, 528,  594, 526,  598, 522,  598, 520,  592, 528,  594, 1646,  600, 520,  592, 528,  594, 524,  596, 522,  600, 520,  592, 528,  594, 524,  596, 522,  600, 520,  592, 526,  594, 524,  598, 1642,  592, 526,  594, 1644,  600, 1640,  596, 524,  598, 522,  600, 518,  592, 526,  596, 1644,  600, 518,  592, 1646,  598, 1642,  592, 526,  596, 524,  598, 520,  590, 530,  592, 37778,  6034, 2964,  598, 522,  600, 1638,  596, 524,  598, 520,  600, 518,  594, 526,  596, 522,  598, 522,  600, 518,  594, 524,  596, 522,  598, 1642,  592, 526,  596, 524,  598, 520,  592, 528,  594, 524,  596, 524,  598, 520,  592, 528,  594, 524,  596, 522,  600, 520,  592, 1646,  598, 522,  600, 1640,  594, 1646,  600, 518,  592, 526,  596, 524,  598, 522,  600, 1640,  594, 524,  598, 1640,  594, 1646,  598, 520,  602, 518,  594, 526,  596, 522,  598};  // UNKNOWN 43BD67B3




RCSwitch mySwitch = RCSwitch();

// Wi-Fi Connection
void wifiSetup() {
  // Set WIFI module to STA mode
  WiFi.mode(WIFI_STA);

  // Connect
  Serial.printf("[WIFI] Connecting to %s ", WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  // Wait
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(100);
  }
  Serial.println();

  // Connected!
  Serial.printf("[WIFI] STATION Mode, SSID: %s, IP address: %s\n", WiFi.SSID().c_str(), WiFi.localIP().toString().c_str());
}

void setup() {
  // Init serial port and clean garbage
  Serial.begin(SERIAL_BAUDRATE);
  Serial.println();

    irsend.begin();
#if ESP8266
  Serial.begin(115200, SERIAL_8N1, SERIAL_TX_ONLY);
#else  // ESP8266
  Serial.begin(115200, SERIAL_8N1);
#endif  // ESP8266

  // Wi-Fi connection
  wifiSetup();

  // LED
  pinMode(RELAY_PIN_1, OUTPUT);
  digitalWrite(RELAY_PIN_1, HIGH);

  pinMode(RELAY_PIN_2, OUTPUT);
  digitalWrite(RELAY_PIN_2, HIGH);
  
  mySwitch.enableReceive(RF_RECEIVER);  // Receiver on interrupt 0 => that is pin #2

  // By default, fauxmoESP creates it's own webserver on the defined port
  // The TCP port must be 80 for gen3 devices (default is 1901)
  // This has to be done before the call to enable()
  fauxmo.createServer(true); // not needed, this is the default value
  fauxmo.setPort(80); // This is required for gen3 devices

  // You have to call enable(true) once you have a WiFi connection
  // You can enable or disable the library at any moment
  // Disabling it will prevent the devices from being discovered and switched
  fauxmo.enable(true);
  // You can use different ways to invoke alexa to modify the devices state:
  // "Alexa, turn lamp two on"

  // Add virtual devices
  fauxmo.addDevice(LAMP_1);
  fauxmo.addDevice(LAMP_2);

  fauxmo.onSetState([](unsigned char device_id, const char * device_name, bool state, unsigned char value) {
    // Callback when a command from Alexa is received. 
    // You can use device_id or device_name to choose the element to perform an action onto (relay, LED,...)
    // State is a boolean (ON/OFF) and value a number from 0 to 255 (if you say "set kitchen light to 50%" you will receive a 128 here).
    // Just remember not to delay too much here, this is a callback, exit as soon as possible.
    // If you have to do something more involved here set a flag and process it in your main loop.
        
    Serial.printf("[MAIN] Device #%d (%s) state: %s value: %d\n", device_id, device_name, state ? "ON" : "OFF", value);
    if ( (strcmp(device_name, LAMP_1) == 0) ) {
      // this just sets a variable that the main loop() does something about
      Serial.println("RELAY 1 switched by Alexa");
      //digitalWrite(RELAY_PIN_1, !digitalRead(RELAY_PIN_1));
      if (state) {
        irsend.sendRaw(Seat, 245, 38);  // IR code for seat.
       
      } else {
        irsend.sendRaw(Seat, 245, 38);  // IR code for seat
      }
    }
    if ( (strcmp(device_name, LAMP_2) == 0) ) {
      // this just sets a variable that the main loop() does something about
      Serial.println("RELAY 2 switched by Alexa");
      if (state) {
       // digitalWrite(RELAY_PIN_2, LOW);
        irsend.sendRaw(Flush, 163, 38);  //IR code for flush
      } else {
        irsend.sendRaw(Flush, 163, 38);  //IR code for flush
      }
    }
  });

}

void loop() {
  // fauxmoESP uses an async TCP server but a sync UDP server
  // Therefore, we have to manually poll for UDP packets
  fauxmo.handle();

  static unsigned long last = millis();
  if (millis() - last > 5000) {
    last = millis();
    Serial.printf("[MAIN] Free heap: %d bytes\n", ESP.getFreeHeap());
  }
    
  if (mySwitch.available()) {    
  /*Serial.print("Received ");
    Serial.print( mySwitch.getReceivedValue() );
    Serial.print(" / ");
    Serial.print( mySwitch.getReceivedBitlength() );
    Serial.print("bit ");
    Serial.print("Protocol: ");
    Serial.println( mySwitch.getReceivedProtocol() );*/
    if (mySwitch.getReceivedValue()==6819768) {
      digitalWrite(RELAY_PIN_1, !digitalRead(RELAY_PIN_1));
    }
    if (mySwitch.getReceivedValue()==9463928) {
      digitalWrite(RELAY_PIN_2, !digitalRead(RELAY_PIN_2));     
    }
    delay(600);
    mySwitch.resetAvailable();
  }
}
