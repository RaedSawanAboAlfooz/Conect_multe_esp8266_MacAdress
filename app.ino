bool brek_s=0;
int x=0;
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <espnow.h>
// SoftAP
IPAddress local_IP(192,168,4,22);
IPAddress gateway(192,168,4,9);
IPAddress subnet(255,255,255,0);
WiFiServer server(80);

int id_R1,id_R2,id_R3,id_R4,id_R5,id_R6;
bool xb1=0,xb2=0,xb3=0,xb4=0,xb5=0,xb6=0,  xb_s=0,bool_on_off=0;;
float volt_R1,volt_R2,volt_R3,volt_R4,volt_R5,volt_R6;
bool bool_on_off_R1,bool_on_off_R2,bool_on_off_R3,bool_on_off_R4,bool_on_off_R5,bool_on_off_R6;
String tim_R1,tim_R2,tim_R3,tim_R4,tim_R5,tim_R6;
bool bool_on_off_stc_R1 = 0,bool_on_off_stc_R2 = 0,bool_on_off_stc_R3 = 0,bool_on_off_stc_R4 = 0,bool_on_off_stc_R5 = 0,bool_on_off_stc_R6 = 0;
int timF1 =0,timF2 =0,timF3 =0,timF4 =0,timF5 =0,timF6 =0;

//#include <LiquidCrystal_I2C.h>    
//LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);
// LiquidCrystal_I2C lcd(0x3F, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);
#include <Wire.h>
String value = "-"; 
uint8_t broadcastAddress1[] = {0x2C, 0xF4, 0x32, 0x14, 0x4F, 0x7C};
uint8_t broadcastAddress2[] = {0x2C, 0xF4, 0x32, 0x7A, 0x56, 0x2C};
uint8_t broadcastAddress3[] = { 0x2C, 0xF4, 0x32, 0x14, 0x49, 0x43 };
uint8_t broadcastAddress4[] = {0x2C, 0xF4, 0x32, 0x79, 0x8F, 0x48};
uint8_t broadcastAddress5[] = {0x2C, 0xF4, 0x32, 0x14, 0x4F, 0x1C };
uint8_t broadcastAddress6[] = {0x2C, 0xF4, 0x32, 0x7A, 0x59, 0xD1};
const long interval = 2000;
unsigned long previousMillis = 0;
String success;
//Structure example to send data
//Must match the receiver structure
typedef struct struct_message {
  int id ;
  float volt;
  int tim;
  bool bool_on_off;
  int tim_F;
  bool bool_on_off_stc;
  bool brek ;
} struct_message;


typedef struct struct_message1 {
  int id ;
  float volt;
  int tim;
  bool bool_on_off;
  int tim_F;
  bool bool_on_off_stc;
  bool brek ;
} struct_message1;

struct_message1 Recev_data;
struct_message1 board1;
struct_message1 board2;
struct_message1 board3;
struct_message1 board4;
struct_message1 board5;
struct_message1 board6;
struct_message1 boardsStruct[6] = {board1, board2,board3, board4,board5, board6};

struct_message sending_car1;
struct_message sending_car2;
struct_message sending_car3;
struct_message sending_car4;
struct_message sending_car5;
struct_message sending_car6;


void setup()
{
  pinMode (D8,1);
  Serial.begin(115200);
//  lcd.begin(16,2);// Columnas y filas de LCD 
  delay(10);
  Serial.println();

  Serial.print("Setting soft-AP configuration ... ");
  Serial.println(WiFi.softAPConfig(local_IP, gateway, subnet) ? "Ready" : "Failed!");

  Serial.print("Setting soft-AP ... ");
  Serial.println(WiFi.softAP("control cars","123123123") ? "Ready" : "Failed!");

  Serial.print("Soft-AP IP address = ");
  Serial.println(WiFi.softAPIP());
  server.begin();
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
  esp_now_add_peer(broadcastAddress1, ESP_NOW_ROLE_COMBO, 1, NULL, 0);
  esp_now_add_peer(broadcastAddress2, ESP_NOW_ROLE_COMBO, 1, NULL, 0);
  esp_now_add_peer(broadcastAddress3, ESP_NOW_ROLE_COMBO, 1, NULL, 0);
  esp_now_add_peer(broadcastAddress4, ESP_NOW_ROLE_COMBO, 1, NULL, 0);
  esp_now_add_peer(broadcastAddress5, ESP_NOW_ROLE_COMBO, 1, NULL, 0);
  esp_now_add_peer(broadcastAddress6, ESP_NOW_ROLE_COMBO, 1, NULL, 0);
  // Register for a callback function that will be called when data is received
  //  esp_now_register_recv_cb(OnDataRecv);
  esp_now_set_self_role(ESP_NOW_ROLE_SLAVE);
  esp_now_register_recv_cb(OnDataRecv);
}


void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus) {
  Serial.print("Last Packet Send Status: ");
  if (sendStatus == 0) {
    Serial.println("Delivery success");
  }
  else {
    Serial.println("Delivery fail");
  }
}
void OnDataRecv(uint8_t * mac_addr, uint8_t *incomingData, uint8_t len) {
  char macStr[18];
//  Serial.print("Packet received from: ");
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
//  Serial.println(macStr);
  memcpy(&Recev_data, incomingData, sizeof(Recev_data));
//  Serial.printf("Board ID %u: %u bytes\n", Recev_data.id, len);
  // Update the structures with the new incoming data
  boardsStruct[Recev_data.id - 1].id = Recev_data.id;
  boardsStruct[Recev_data.id - 1].volt = Recev_data.volt;
  boardsStruct[Recev_data.id - 1].tim_F = Recev_data.tim_F;
  boardsStruct[Recev_data.id - 1].bool_on_off_stc = Recev_data.bool_on_off_stc;

    id_R2 = boardsStruct[1].id;
    volt_R2 = boardsStruct[1].volt;
    timF2 = boardsStruct[1].tim_F;
    bool_on_off_stc_R2 = boardsStruct[1].bool_on_off_stc;
//    Serial.println (volt_R2);
    
    id_R1 = boardsStruct[0].id;
    volt_R1 = boardsStruct[0].volt;
    timF1 = boardsStruct[0].tim_F;
    bool_on_off_stc_R1 = boardsStruct[0].bool_on_off_stc;
    
    
    id_R3 = boardsStruct[2].id;
    volt_R3 = boardsStruct[2].volt;
    timF3 = boardsStruct[2].tim_F;
    bool_on_off_stc_R3 = boardsStruct[2].bool_on_off_stc;
    
    id_R4 = boardsStruct[3].id;
    volt_R4 = boardsStruct[3].volt;
    timF4 = boardsStruct[3].tim_F;
    bool_on_off_stc_R4 = boardsStruct[3].bool_on_off_stc;
    
    id_R5 = boardsStruct[4].id;
    volt_R5 = boardsStruct[4].volt;
    timF5 = boardsStruct[4].tim_F;
    bool_on_off_stc_R5 = boardsStruct[4].bool_on_off_stc;

    id_R6 = boardsStruct[5].id;
    volt_R6 = boardsStruct[5].volt;
    timF6 = boardsStruct[5].tim_F;
    bool_on_off_stc_R6 = boardsStruct[5].bool_on_off_stc;
}

void loop() {
  // Consulta si se ha conectado algún cliente.
  WiFiClient client = server.available();
  if (!client) {
    return;
  }
 
  // Espera hasta que el cliente envíe datos.
  Serial.println("Nuevo cliente.");
  while(!client.available()){
    delay(1);
  }
 
  // Lee la primera línea de la petición.
  String request = client.readStringUntil('\r');
  Serial.println(request);
  client.flush();
  // Consulta la petición
  if (request.startsWith("mensaje=1", 5))  {
   digitalWrite(D8,1);
    
   brek_s=0;
   sending_car1.brek =brek_s ; 
   sending_car1.bool_on_off =1 ;
   esp_now_send(broadcastAddress1, (uint8_t *) &sending_car1, sizeof(sending_car1));
   delay(50);
   sending_car2.brek =brek_s ;
   sending_car2.bool_on_off =1 ;
   esp_now_send(broadcastAddress2, (uint8_t *) &sending_car2, sizeof(sending_car2));
   delay(50);
   sending_car3.brek =brek_s ;
   sending_car3.bool_on_off =1 ;
   esp_now_send(broadcastAddress3, (uint8_t *) &sending_car3, sizeof(sending_car3));
   delay(50);
   sending_car4.brek =brek_s ;
   sending_car4.bool_on_off =1 ;
   esp_now_send(broadcastAddress4, (uint8_t *) &sending_car4, sizeof(sending_car4));
   delay(50);
   sending_car5.brek =brek_s ;
   sending_car5.bool_on_off =1 ;
   esp_now_send(broadcastAddress5, (uint8_t *) &sending_car5, sizeof(sending_car5));
   delay(50);
   sending_car6.brek =brek_s ;
   sending_car6.bool_on_off =1 ;
   esp_now_send(broadcastAddress6, (uint8_t *) &sending_car6, sizeof(sending_car6));
   delay(50);
   sending_car1.bool_on_off =0 ;
   esp_now_send(broadcastAddress1, (uint8_t *) &sending_car1, sizeof(sending_car1));
   delay(50);
   sending_car2.bool_on_off =0 ;
   esp_now_send(broadcastAddress2, (uint8_t *) &sending_car2, sizeof(sending_car2));
   delay(50);
   sending_car3.bool_on_off =0 ;
   esp_now_send(broadcastAddress3, (uint8_t *) &sending_car3, sizeof(sending_car3));
   delay(50);
   sending_car4.bool_on_off =0 ;
   esp_now_send(broadcastAddress4, (uint8_t *) &sending_car4, sizeof(sending_car4));
    delay(50);
   sending_car5.bool_on_off =0 ;
   esp_now_send(broadcastAddress5, (uint8_t *) &sending_car5, sizeof(sending_car5));
    delay(50);
   sending_car6.bool_on_off =0 ;
   esp_now_send(broadcastAddress6, (uint8_t *) &sending_car6, sizeof(sending_car6));
//   delay(1450);
   digitalWrite(D8,0);
    }
   else if (request.startsWith("mensaje=2", 5))  {
    digitalWrite(D8,1);
   bool_on_off=0;
   brek_s=1;
   sending_car1.brek =brek_s ;
   esp_now_send(broadcastAddress1, (uint8_t *) &sending_car1, sizeof(sending_car1));
    delay(50);
   sending_car2.brek =brek_s ;
   esp_now_send(broadcastAddress2, (uint8_t *) &sending_car2, sizeof(sending_car2));
    delay(50);
   sending_car3.brek =brek_s ;
   esp_now_send(broadcastAddress3, (uint8_t *) &sending_car3, sizeof(sending_car3));
   delay(50);
   sending_car4.brek =brek_s ;
   esp_now_send(broadcastAddress4, (uint8_t *) &sending_car4, sizeof(sending_car4));
   delay(50);
   sending_car5.brek =brek_s ;
   esp_now_send(broadcastAddress5, (uint8_t *) &sending_car5, sizeof(sending_car5));
   delay(50);
   sending_car6.brek =brek_s ;
   esp_now_send(broadcastAddress6, (uint8_t *) &sending_car6, sizeof(sending_car6));
//   delay(1750);
   digitalWrite(D8,0);
    }
    
  else  if (request.startsWith("mensaje=3", 5))  { }
  
  else if (request.startsWith("mensaje=4", 5))  { }
  
  else if (request.startsWith("x", 15))  {  // Si en la posición 15 viene una x...
 
    value = request.substring(17).toInt();     // ... un mensaje comienza en la 17.
    value.replace("+", " ");           // Para que los espacios no salgan con +
    value.replace(" HTTP/1.1", " ");  
    x=value.toInt();
    Serial.print("x= ");
    Serial.println (x);// Para quitar HTTP/1.1
    sending_car1.tim = x ;
    esp_now_send(broadcastAddress1, (uint8_t *) &sending_car1, sizeof(sending_car1));
    delay(50);
    sending_car2.tim = x ;
    esp_now_send(broadcastAddress2, (uint8_t *) &sending_car2, sizeof(sending_car2));
    delay(50);
    sending_car3.tim = x ;
    esp_now_send(broadcastAddress3, (uint8_t *) &sending_car3, sizeof(sending_car3));
    delay(50);
    sending_car4.tim = x ;
    esp_now_send(broadcastAddress4, (uint8_t *) &sending_car4, sizeof(sending_car4));
    delay(50);
    sending_car5.tim = x ;
    esp_now_send(broadcastAddress5, (uint8_t *) &sending_car5, sizeof(sending_car5));
    delay(50);
    sending_car6.tim = x ;
    esp_now_send(broadcastAddress6, (uint8_t *) &sending_car6, sizeof(sending_car6));
//  Serial.println(value);
  }
//  lcd.clear(); // Borra pantalla
//  lcd.setCursor(0,0); // Inicio del cursor
//  lcd.print(value);

  // Devuelve el mensaje.
  if (bool_on_off_stc_R1==0&&bool_on_off_stc_R2==0&&bool_on_off_stc_R3==0&&bool_on_off_stc_R4==0&&bool_on_off_stc_R5==0&&bool_on_off_stc_R6==0){
    if (bool_on_off==1){
      bool_on_off=0;
      digitalWrite(D8,1);
//      delay(2000);
      digitalWrite(D8,0);
    }
  }
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println(""); //  Comillas importantes.
  client.print("tim1 = ");
  client.print(timF1);
  client.print ("         ");
  client.print("volt1 = ");
  client.print (volt_R1);
  client.print ("         ");
  if (bool_on_off_stc_R1==1){
  client.println("car1 on");
  bool_on_off=1;}
  else
  client.println("car1 off");
  client.println ("");
//////////////////////////////////////////////////
  client.print("tim2 = ");
  client.print(timF2);
  client.print ("         ");
  client.print("volt2 = ");
  client.print (volt_R2);
  client.print ("         ");
  if (bool_on_off_stc_R2==1){
  client.println("car2 on");
  bool_on_off=1;}
  else
  client.println("car2 off");
  client.println ("");
///////////////////////////////////////////////////
  client.print("tim3 = ");
  client.print(timF3);
  client.print ("         ");
  client.print("volt3 = ");
  client.print (volt_R3);
  client.print ("         ");
  if (bool_on_off_stc_R3==1){
  client.println("car3 on");
  bool_on_off=1;}
  else
  client.println("car3 off");
  client.println ("");
  ////////////////////////////////////////////
   client.print("tim4 = ");
  client.print(timF4);
  client.print ("         ");
  client.print("volt4 = ");
  client.print (volt_R4);
  client.print ("         ");
  if (bool_on_off_stc_R4==1){
  client.println("car4 on");
  bool_on_off=1;}
  else
  client.println("car4 off");
  client.println ("");
  //////////////////////////////////////////////
   client.print("tim5 = ");
  client.print(timF5);
  client.print ("          ");
  client.print("volt5 = ");
  client.print (volt_R5);
  client.print ("         ");
  if (bool_on_off_stc_R5==1){
  client.println("car5 on");
  bool_on_off=1;}
  else
  client.println("car5 off");
  client.println ("");
  /////////////////////////////////////////////////////
  client.print("tim6 = ");
  client.print(timF6);
  client.print ("         ");
  client.print("volt6 = ");
  client.print (volt_R6);
  client.print ("         ");
  if (bool_on_off_stc_R6==1){
  client.println("car6 on");
  bool_on_off=1;}
  else
  client.println("car6 off");
 
//  client.println("Cliente desconectado.");
  }
