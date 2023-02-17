 /*
  Things to do 
  1. Create 10 min timer to allow the router time to reboot and come back online before checking again.
  2. put all the timers into there own dedicated functions for reuse if needed
  3. Replace Serial out puts with LCD screen outputs after LCD is added. 
*/

#include <SPI.h>
#include <Ethernet.h>

byte mac[] = { 0xA8, 0x61, 0x0A, 0xAE, 0xA9, 0xB9 };

char server_0[] = "www.google.com";

EthernetClient client;

unsigned long time_passed_0;
unsigned long time_passed_1;
unsigned long prev_time_0 = 0;
unsigned long prev_time_1 = 0;
unsigned long time_interval = 10000; //1800000; // 30 mins
unsigned long power_off_time = 60000; // 1 min

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
 time_passed_0 = millis();

 if(time_passed_0 - prev_time_0 >= time_interval)
 {
   prev_time_0 = time_passed_0;
   connection_status = connection_test();
 }

 if(!connection_status)
 {
   Serial.println("Connection lost");
   connection_status = true;
   digitalWrite(relay_pin, HIGH);

   while(count <= 1)
   {
     time_passed_1 = millis();

     if(time_passed_1 - prev_time_1 >= power_off_time)
     {
       prev_time_1 = time_passed_1;
       count += 1;
       Serial.println("Inside the IF statment: "); // For debuging
     }
     
   }

   digitalWrite(relay_pin, LOW);
   count = 0;
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
