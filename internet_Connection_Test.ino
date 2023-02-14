#include <SPI.h>
#include <Ethernet.h>

byte mac[] = { 0xA8, 0x61, 0x0A, 0xAE, 0xA9, 0xB9 };

char server_0[] = "www.google.com";

EthernetClient client;

unsigned long time_passed;
unsigned long prev_time = 0;
unsigned long reset_time = 0;
unsigned long time_interval = 1800000; // 30 mins
unsigned long wait_time = 30000;

bool connection_status = true;

int relay_pin = 3;
int count = 0;

void setup() 
{
  pinMode(relay_pin, OUTPUT);
  digitalWrite(relay_pin, LOW);

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
   connection_status = true;
   digitalWrite(relay_pin, HIGH);

   while(count <= 1)
   {
     if(time_passed - reset_time >= wait_time)
     {
       reset_time = time_passed;
       count += 1;
     }
     
   }

   digitalWrite(relay_pin, LOW);
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
