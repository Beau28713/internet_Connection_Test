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
unsigned long time_interval = 7200000; // 2 hours
unsigned long power_off_time = 60000; //1 min
unsigned long wait_time = 600000; //10 mins

bool connection_status = false;
bool post_reboot_connection_check = false;

int relay_pin = 7;
int count_1 = 0;
int count_2 = 0;

int reboot_count = 1;

void setup() {
  pinMode(relay_pin, OUTPUT);
  digitalWrite(relay_pin, LOW);

  Ethernet.init(10);

  Serial.begin(9600);
  while (!Serial) {
    ;
  }
}

void loop() {
  Ethernet.maintain();
  time_passed_0 = millis();

  //Check the internet connection every 2 hours
  if (time_passed_0 - prev_time_0 >= time_interval || post_reboot_connection_check) {
    prev_time_0 = time_passed_0;
    connection_status = connection_test();

    if (connection_status) {
      Serial.println("Connected");
    }

    //If connection lost, start router reset procedure
    else if (!connection_status) {
      Serial.println("Connection lost");

      //Power down the router for 1 min
      digitalWrite(relay_pin, HIGH);
      Serial.println("Router is powered off");  // for debugging
      while (count_1 <= 1) {
        time_passed_1 = millis();

        if (time_passed_1 - prev_time_1 >= power_off_time) {
          prev_time_1 = time_passed_1;
          count_1 += 1;
        }
      }

      //Power the router back on
      digitalWrite(relay_pin, LOW);
      Serial.println("Router is powered back up");  // for debugging
      count_1 = 0;

      //Wait 10 mins for the router to boot back up
      while (count_2 <= 1) {
        time_passed_2 = millis();
        if (time_passed_2 - prev_time_2 >= wait_time) {
          prev_time_2 = time_passed_2;
          count_2 += 1;
        }
      }

      count_2 = 0;
      // check connection after reboot
      if(reboot_count > 0){
        post_reboot_connection_check = true;
        Serial.println(post_reboot_connection_check);
        reboot_count -= 1;
      }
      else{
        post_reboot_connection_check = false;
        reboot_count = 1;
      }
      
    }
  }
}

bool connection_test() {
  Serial.println("Initialize Ethernet with DHCP:");

  if (Ethernet.begin(mac) == 0) {
    if (Ethernet.hardwareStatus() == EthernetNoHardware) {
      Serial.println("Ethernet shield was not found:");
    }

    if (Ethernet.linkStatus() == LinkOFF) {
      Serial.println("Ethernet cable is not connected.");
    }

  }

  else {
    Serial.print("  DHCP assigned IP ");
    Serial.println(Ethernet.localIP());

    delay(1000);
    Serial.print("connecting to ");
    Serial.print(server_0);
    Serial.println("...");

    // port 80 is default for HTTP
    if (client.connect(server_0, 80)) {
      Serial.print("connected to ");
      Serial.println(client.remoteIP());
      client.stop();
      return true;
    } else {
      return false;
    }
  }

  return false;
}
