#include <SPI.h>
#include <Ethernet.h>

byte mac[] = { 0xA8, 0x61, 0x0A, 0xAE, 0xA9, 0xB9 };

char server_0[] = "www.google.com";

EthernetClient client;

unsigned long time_passed;
unsigned long prev_time = 0;
unsigned long time_interval = 1800000; // 30 mins

bool connection_status = true;

void setup() 
{
  
  Ethernet.init(10);

  Serial.begin(9600);
  while (!Serial) 
  {
    ; 
  }
  Serial.println("Serial Connected");
}

void loop() 
{
 Ethernet.maintain();
 time_passed = millis();

 if(time_passed - prev_time >= time_interval)
 {
   prev_time = time_passed;
   connection_status = connection_test();
 }

 if(!connection_status)
 {
   Serial.println("Connection lost");
   // Put reset code for the router here

   connection_status = true;
 }


}

bool connection_test()
{
  Serial.println("Initialize Ethernet with DHCP:");

  if (Ethernet.begin(mac) == 0) 
  {
    if (Ethernet.hardwareStatus() == EthernetNoHardware) 
    {
      Serial.println("Ethernet shield was not found:");
    }

    if (Ethernet.linkStatus() == LinkOFF) 
    {
      Serial.println("Ethernet cable is not connected.");
    }
    
  } 

  else 
  {
    Serial.print("  DHCP assigned IP ");
    Serial.println(Ethernet.localIP());
    
    delay(1000);
    Serial.print("connecting to ");
    Serial.print(server_0);
    Serial.println("...");

    // port 80 is default for HTTP
    if (client.connect(server_0, 80)) 
    {
      Serial.print("connected to ");
      Serial.println(client.remoteIP());
      client.stop();
      return true;
    }
    else
    {
      return false;
    }
  }

  return false;
}
