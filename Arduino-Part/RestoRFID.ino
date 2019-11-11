 // ================================= RFID DECLARATION ===========================

#include <SPI.h>
#include <MFRC522.h>
 
#define SS_PIN 53
#define RST_PIN 49
MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.

//=======================================DECLARATION ETHERNET =================================


#include <SPI.h>
#include <Ethernet.h>

IPAddress server(192,168,1,101);

IPAddress ip(192, 168, 1, 177);
IPAddress myDns(192, 168, 1, 1);

EthernetClient client;

boolean captured = false ;

byte mac[] = {  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED }; //ethernet shield mac address
byte ipstatic[] = {  192, 168, 1, 177 }; // arduino IP in lan
byte gateway[] = {  192, 168, 1, 1 }; // internet access via router
byte subnet[] = {  255, 255, 255, 0 }; //subnet mask

String readString;
// ================================= I2C DECLARATION =============================


#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x3F,20,4);  // set the LCD address to 0x3F(hethi tetgle bel scanner) for a 16 chars and 2 line display

// ================================== SETUP ======================================


void setup() 
{   Serial.begin(9600);   // Initiate a serial communication
pinMode(12,OUTPUT);
pinMode(11,OUTPUT);
pinMode(8,OUTPUT);
pinMode(4,OUTPUT);
pinMode(3,OUTPUT);
pinMode(5,INPUT);
 digitalWrite(12,LOW) ; 
 digitalWrite(11,LOW) ; 
 digitalWrite(3,LOW) ; 
 digitalWrite(4,HIGH) ; 
  //=========================== RFID SETUP ==============================
  SPI.begin();      // Initiate  SPI bus
  mfrc522.PCD_Init();   // Initiate MFRC522


  
//============================ I2C SETUP=============================

  lcd.init();
  lcd.backlight();
 

//=================================ETHERNET SETUP ===============================

  Ethernet.init(10);  // Most Arduino shields
 // Ethernet.begin(mac, ipstatic, gateway, gateway, subnet) ;

  if (  Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("ERROR 0");
    lcd.setCursor(0,1);
    lcd.print("DHCP Failed");
      
    if (Ethernet.hardwareStatus() == EthernetNoHardware) {
      Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware. :(");
      lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Subscribe to");
    lcd.setCursor(0,1);
    lcd.print("ShorTech <3 <3  ");
 while(1){
  
 }
    }
    if (Ethernet.linkStatus() == LinkOFF) {
      Serial.println("Ethernet cable is not connected.");
          lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("ERROR 2");
    lcd.setCursor(0,1);
    lcd.print("No Cable");
      
    }
    // try to congifure using IP address instead of DHCP:
    Ethernet.begin(mac, ip, myDns);
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Static IP");
    lcd.setCursor(0,1);
    lcd.print("192.168.1.177");
  } else {
    Serial.print("DHCP Assigned IP ");
    Serial.println(Ethernet.localIP());
     lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("DHCP Assigned");
    lcd.setCursor(0,1);
    lcd.print(Ethernet.localIP()); 
      
  }
  // give the Ethernet shield a second to initialize:
  delay(1000);
   lcd.clear();
  lcd.setCursor(4,0);
  lcd.print("Welcome!");
  lcd.setCursor(1,1);
  lcd.print("Scan Your Card");

}
//================================ LOOP ========================================
int modeAdmin = 0;
void loop() 
{
  int var = digitalRead(5) ;
  Serial.println(var) ;
if (  var ==  HIGH){
  modeAdmin=1 ;
  digitalWrite(8,HIGH);
    lcd.setCursor(15,0);
    lcd.print("A");
} 
else {
  modeAdmin=0;
  digitalWrite(8,LOW);
      lcd.setCursor(15,0);
    lcd.print(" ");
}
  // Look for new cards
  if ( ! mfrc522.PICC_IsNewCardPresent()) 
  {
    return;
  }
  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) 
  {
    return;
  }

  digitalWrite(12,HIGH);
delay(500);
  digitalWrite(12,LOW);

  Serial.println("i am here ");

  
  String content= "";
  byte letter;
  
  for (byte i = 0; i < mfrc522.uid.size; i++) 
  {
    // Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
     // Serial.print(mfrc522.uid.uidByte[i], HEX);
     content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? "-0" : "-"));
     content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
//  lcd.setCursor(0,0);
  content.toUpperCase();
  //lcd.print("ID: " + content.substring(1));
  //Serial.println("i am here 2");
Serial.println( content.substring(1)) ;
  //lcd.setCursor(0,1);
repeatConnect :
   if(client.connect(server, 80)) {
    if(modeAdmin==0){
         client.println(String("GET /resto/test.php?id=")+String(content.substring(1))+String(" HTTP/1.1")); 
    } 
    else{
         client.println(String("GET /resto/scan_card.php?Card_ID=")+String(content.substring(1))+String(" HTTP/1.1"));  
    }
    client.println("Host: 192.168.1.101");
    client.println("Connection: close");
    client.println();
  } 
else {
    Serial.println("connection failed");
   
          lcd.clear();
          if (modeAdmin==1){
    lcd.setCursor(15,0);
    lcd.print("A");
          }
    lcd.setCursor(0,0);
    lcd.print("ERROR 3 - Server");
    lcd.setCursor(0,1);
    lcd.print("Connexion Failed");
    goto repeatConnect ;
  }
boolean done=false ;
while(!done){
    int len = client.available();
           String reponse ;
           //  Serial.println("i am here 4");
Serial.println(len);
 if (len > 0) {
  captured = true;
    byte buffer[500];
    if (len > 500) len = 500;
    client.read(buffer, len);
      
for (int i =0 ; i<len ; i++ ){
  if (buffer[i]==10){ reponse=""; }
  else{ reponse = reponse + char(buffer[i]) ; }
}

        client.stop();
 
        if( reponse=="noSuchID" ) { 
  lcd.clear();
      
  lcd.setCursor(0,0);
  lcd.print("Card Does");
  lcd.setCursor(0,1);
  lcd.print("Not Exist!");
  }
        else if ( reponse=="1" ){
          if (modeAdmin == 0 ){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Last meal ");
  lcd.setCursor(0,1);
  lcd.print("Please Recharge!");}
  else {
      lcd.clear();
    lcd.setCursor(15,0);
    lcd.print("A");
          
  lcd.setCursor(0,0);
  lcd.print("Current Card");
  lcd.setCursor(0,1);
  lcd.print("Changed");
    }
  }
  
        else if (reponse=="0" ){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("No Leals Left !");
  lcd.setCursor(0,1);
  lcd.print("Recharge!");
}
        else {reponse=String(reponse.toInt()-1) ; 
  lcd.clear();
  
  lcd.setCursor(0,0);
  lcd.print("You Have ");
  lcd.setCursor(0,1);
  lcd.print(String(reponse)+" Meals Left !");}
  // Serial.println("i am here 3");

  delay(5000);
      done=true;
      //   Serial.println("i am here 3");

  }
}
  lcd.clear();
          lcd.setCursor(15,0);
    lcd.print("A");
  lcd.setCursor(4,0);
  lcd.print("Welcome!");
  lcd.setCursor(1,1);
  lcd.print("Scan Your Card");
} 
