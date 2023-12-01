#include <Arduino.h>
#include <SPI.h>
#include <EthernetENC.h>
#include <voltage.h> 

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
// if you don't want to use DNS (and reduce your sketch size)
// use the numeric IP instead of the name for the server:
IPAddress server(85,89,32,58);
// Set the static IP address to use if the DHCP fails to assign
IPAddress ip(192, 168, 0, 177);
IPAddress myDns(192, 168, 0, 1);

EthernetClient client;
int TTET = 60;
  


int freeRam() {

  extern int __heap_start,*__brkval;

  int v;

  return (int)&v - (__brkval == 0  

    ? (int)&__heap_start : (int) __brkval);  

}

void display_freeram() {

  Serial.print(F("- SRAM left: "));

  Serial.println(freeRam());

}

void makeHTTTP(float voltageH , float currentH) {
// String URL = "GET /input/post?node=BatteryVoltage&apikey=17bda09eb30a8f93c375d009a6066c2c&fulljson={";

String URL2 = "GET /input/post?node=BatteryVoltage";
String APIKEY = "&apikey=17bda09eb30a8f93c375d009a6066c2c&fulljson={";
String URLEND = "} HTTP/1.1";

URL2 += APIKEY;
URL2 += "\"Voltage\":";
URL2 += voltageH;
URL2 += ",\"Amp\":";
URL2 += currentH;
URL2 += URLEND;



  // if (client.connect(server, 80)) {
  //   Serial.print("connected to ");
  //   Serial.println(client.remoteIP());
  //   Serial.println(URL + "\"Battery1\":" + TTET + "\"Battery2\":" + TTET +"\"Battery3\":" + TTET +"\"Battery4\":" + TTET + URLEND);
  //   client.println(URL + "\"Battery1\":" + TTET + "\"Battery2\":" + TTET +"\"Battery3\":" + TTET +"\"Battery4\":" + TTET + URLEND);
  //   client.println("Host: 85.89.32.58");
  //   client.println("Connection: close");
  //   client.println();
  // } else {
  //   // if you didn't get a connection to the server:
  //   Serial.println("connection failed");
  // }

    if (client.connect(server, 80)) {
    Serial.print("connected to ");
    Serial.println(client.remoteIP());
    Serial.println(URL2);
    client.println(URL2);
    client.println("Host: 85.89.32.58");
    client.println("Connection: close");
    client.println();
    client.stop();
  } else {
    // if you didn't get a connection to the server:
    Serial.println("connection failed");
  }

}


void setup() {
    //Robojax.com ACS758 Current Sensor 
    Serial.begin(9600);
display_freeram();
Serial.println("Initialize Ethernet with DHCP:");
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    // Check for Ethernet hardware present
    if (Ethernet.hardwareStatus() == EthernetNoHardware) {
      Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware. :(");
      while (true) {
        delay(1); // do nothing, no point running without Ethernet hardware
      }
    }
    if (Ethernet.linkStatus() == LinkOFF) {
      Serial.println("Ethernet cable is not connected.");
    }
    // try to congifure using IP address instead of DHCP:
    Ethernet.begin(mac, ip, myDns);
  } else {
    Serial.print("  DHCP assigned IP ");
    Serial.println(Ethernet.localIP());
  }
  // give the Ethernet shield a second to initialize:
  delay(1000);
  Serial.print("connecting to ");
  Serial.print(server);
  Serial.println("...");
}


unsigned long byteCount = 0;
bool printWebData = true;  // set to false for better speed measurement

void loop() {
  
  display_freeram();
  BatteryVoltage();

  makeHTTTP(voltage,current);

  int len = client.available();
  if (len > 0) {
    byte buffer[80];
    if (len > 80) len = 80;
    client.read(buffer, len);
    if (printWebData) {
      Serial.write(buffer, len); // show in the serial monitor (slows some boards)
    }
    byteCount = byteCount + len;
  }

  delay(2000);
}
 