#include <ESP8266WiFi.h>
#include <espnow.h>
//#include <Adafruit_Sensor.h>
int val=0;
bool brek_r=0;
int out=0;
float volt =0.00;
const  float R1 = 2160; // resistance of R1 (100K) 
const float R2 = 99.50; // resistance of R2 (10K) 
uint8_t broadcastAddress[] = {0xA4, 0xCF, 0x12, 0xD5, 0xA9, 0x33};
bool x=0;
// Define variables to store incoming readings
int ID=3;
float send_volt ;
int recev_tim =0;
bool recev_bool_on_off;
// Updates DHT readings every 10 seconds
const long interval =2000; 
unsigned long previousMillis = 0; // will store last time DHT was updated 
//const long interval1 = 90; 
//unsigned long previousMillis1 = 0; 
// Variable to store if sending data was successful
String success;
unsigned long tim2 =0;
unsigned long st =0;
unsigned long tim22 =0;
unsigned long st1 =0;
int pin_read =0;
//Structure example to send data
//Must match the receiver structure
typedef struct struct_message {
    int id =ID;
    float volt;
    int tim ;
    bool bool_on_off;
    int tim_F;
    bool bool_on_off_stc;
    bool brek ;
} struct_message;
// Create a struct_message called sending to hold sensor readings
struct_message sending;
// Create a struct_message to hold incoming sensor readings
struct_message incomingReadings;

// Callback when data is sent
void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus) {
  Serial.print("Last Packet Send Status: ");
  if (sendStatus == 0){
    Serial.println("Delivery success");
  }
  else{
    Serial.println("Delivery fail");
  }
}

// Callback when data is received
void OnDataRecv(uint8_t * mac, uint8_t *incomingData, uint8_t len) {
  memcpy(&incomingReadings, incomingData, sizeof(incomingReadings));
  Serial.print("Bytes received: ");
  Serial.println(len);
  recev_bool_on_off = incomingReadings.bool_on_off;
  recev_tim = incomingReadings.tim;
  brek_r = incomingReadings.brek;
  out=recev_tim*1000;
  delay(1000);
}
void getReadings(){

   val = analogRead(A0);//reads the analog input
   volt = (val * 3.18) / 1024.00; // formula for calculating voltage out i.e. V+, here 5.00
   send_volt = volt / (R2/(R1+R2));
  // Read Temperature
//  send_volt+=0.1;
}

 
void setup() {
  // Init Serial Monitor
  Serial.begin(115200);
  pinMode (D8,1);
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  // Init ESP-NOW
  if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  // Set ESP-NOW Role
  esp_now_set_self_role(ESP_NOW_ROLE_COMBO);

  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_register_send_cb(OnDataSent);
  
  // Register peer
  esp_now_add_peer(broadcastAddress, ESP_NOW_ROLE_COMBO, 1, NULL, 0);
  
  // Register for a callback function that will be called when data is received
  esp_now_register_recv_cb(OnDataRecv);
}
 
void loop() {
  unsigned long currentMillis = millis();
   
  if (currentMillis - previousMillis >= interval) {
    // save the last time you updated the DHT values
    previousMillis = currentMillis;
    //Get DHT readings
    getReadings();
    //Set values to send
    sending.id = ID;
    sending.volt = send_volt;
    sending.tim_F = recev_tim;
    sending.bool_on_off_stc = pin_read;
     Serial.print(" recev_bool_on_off = ");
     Serial.print(recev_bool_on_off);
     Serial.print("     recev_tim = ");
     Serial.print(out);
     Serial.print ("     brek_r = ");
     Serial.print (brek_r);
     Serial.print("     Reding pin = ");
     Serial.print (digitalRead(D8));
      
    // Send message via ESP-NOW
    esp_now_send(broadcastAddress, (uint8_t *) &sending, sizeof(sending));
    // Print incoming readings
//  Serial.println ();
//    printIncomingReadings();   
    }
    
//   if (brek_r==1){
//    digitalWrite(D8,0);
//   }
    tim2 = millis();
    tim22=millis();
     if (recev_bool_on_off==1){
      
      st=tim2;
      digitalWrite(D8,1);
      st1=tim22;
      Serial.println ("oon//////////////" );
     }
     
     if (brek_r==1){
      out =0;
     }
      if (tim2>st+out){
      digitalWrite(D8,0);
      Serial.println (":::::::::::::::::end:::::::::::::::::::");
    }   
   if(tim22>st1+recev_tim+15000){
    x=0;
   }
  
   pin_read=digitalRead(D8);
}
