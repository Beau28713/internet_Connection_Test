 /*
  Things to do 
  1. put all the timers into there own dedicated functions for reuse if needed
  2. Replace Serial out puts with LCD screen outputs after LCD is added. 
*/

#include <SPI.h>
#include <Ethernet.h>

byte mac[] = { 0xA8, 0x61, 0x0A, 0xAE, 0xA9, 0xB9 };

char server_0[] = "www.google.com";

EthernetClient client;

unsigned long time_passed_0;
unsigned long time_passed_1;
unsigned long time_passed_2;
unsigned long prev_time_0 = 0;
unsigned long prev_time_1 = 0;
unsigned long prev_time_2 = 0;
unsigned long time_interval = 10000; //1800000; // 30 mins
unsigned long power_off_time = 60000; // 1 min
unsigned long wait_time = 300000; //5 mins

bool connection_status = true;

int relay_pin = 3;
int count_1 = 0;
int count_2 = 0;

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

//Check the internet connection every 30 mins
 if(time_passed_0 - prev_time_0 >= time_interval)
 {
   prev_time_0 = time_passed_0;
   connection_status = connection_test();
  }

//If connectio lost, start router reset procedure
 if(!connection_status)
 {
   Serial.println("Connection lost");
   connection_status = true;
   
   //Power down and reset the router
   digitalWrite(relay_pin, HIGH);   
   while(count_1 <= 1)
   {
     time_passed_1 = millis();

     if(time_passed_1 - prev_time_1 >= power_off_time)
     {
       prev_time_1 = time_passed_1;
       count_1 += 1;
      }
    }

  //Power the router back on
   digitalWrite(relay_pin, LOW);
   count_1 = 0;
   
  //Wait for the router to boot back up
   while(count_2 <= 1)
   {
     if(time_passed_2 - prev_time_2 >= wait_time)
     {
       prev_time_2 = time_passed_2;
       count_2 += 1;
      }
    }

    count_2 = 0;

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
