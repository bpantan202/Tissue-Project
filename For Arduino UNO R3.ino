// set LCD
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27,16,2); 

// for transfer with esp8266
#include <SoftwareSerial.h>
SoftwareSerial Uno(2, 3); // RX | TX

// rfid setting
#include <SPI.h>
#include <MFRC522.h>
constexpr uint8_t RST_PIN = 9;
constexpr uint8_t SS_PIN = 10;
MFRC522 rfid(SS_PIN, RST_PIN);
MFRC522::MIFARE_Key key;

//limit switch
//#define LIMIT_SWITCH_PIN 7

//servo
#include <Servo.h>
Servo myservo;

int sensor_read;
int percent_left;
int first_noti = 0;
int second_noti = 0;
String tag;
int tissue_status = 0;
int true_card = 0;
int count_loop = 0;
int close_activate=1;
int door_status = 0; //0:lock 1:open
int transfer_percent = 0;
int status_open = 0; //1:true(หมด)
int status_close = 0; //1:true(เติมแล้ว)

void setup()
{
  Serial.begin(9600);
  // initialize the lcd
  lcd.init();                      
  lcd.backlight();
  // for transfer with esp8266
  pinMode(2, INPUT);
  pinMode(3, OUTPUT);
  Uno.begin(9600);
  // rfid
  SPI.begin(); // Init SPI bus
  rfid.PCD_Init(); // Init MFRC522
  //limit switch
//  pinMode(touch_switch,INPUT);
  //servo
  myservo.attach(6);
  myservo.write(90); //ปิดประตูตอนเริ่ม
}


void loop()
{
  //door open
//  if (tissue_status==1){Uno.println(3000);}
  if (true_card == 1){
//    Uno.println(3000);
    if (door_status==0){
      myservo.write(0); 
      door_status=1;
      lcd.setCursor(1,0);
      lcd.print("Close the door");
      lcd.setCursor(2,1);
      lcd.print("to continue.");
      }

      
    if ( ! rfid.PICC_IsNewCardPresent())
    return;
    if (rfid.PICC_ReadCardSerial()) {
    for (byte i = 0; i < 4; i++) {
    tag += rfid.uid.uidByte[i];
    }
    lcd.clear();
    Serial.println(tag);
    lcd.setCursor(0,0);
    lcd.print("----Closing-----");
    lcd.setCursor(0,1);
    lcd.print("By:");
    lcd.setCursor(3,1);
    if (tag == "521719056") {Uno.println(44003); tissue_status=0; lcd.print("Athipong Sta."); close_activate=1;}
    else if(tag == "1641299256") {Uno.println(44019); tissue_status=0; lcd.print("Pannavich Tan."); close_activate=1;}
    else if(tag == "1322068556") {Uno.println(44029); tissue_status=0; lcd.print("Nathavat Sir."); close_activate=1;}
    else if(tag == "971177838") {Uno.println(44051); tissue_status=0; lcd.print("Admin01"); close_activate=1;}
    else if(tag == "35818049") {Uno.println(44052); tissue_status=0 ; lcd.print("Admin02"); close_activate=1;}
    else {
    tag = "";                                                                                                                                                                       
    Uno.println(66249); 
    lcd.clear();
    lcd.setCursor(2,0);
    lcd.print("Unknow User");
    true_card=0;
    close_activate=0;
    }
    delay(1000); //
//    lcd.clear();
    rfid.PICC_HaltA();
    rfid.PCD_StopCrypto1();
    tag = ""; //ไปใส่ส่งส่งไลน์แจ้งเติมแล้ว
    }
    if(close_activate==1)
      {
      //ปิดประตู
//      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("                ");
      lcd.setCursor(2,0);
      lcd.print("Closing in 3");
      delay(750);
      lcd.setCursor(2,0);
      lcd.print("Closing in 2");
      delay(750);
      lcd.setCursor(2,0);
      lcd.print("Closing in 1");
      delay(750);
      lcd.setCursor(0,0);
      lcd.print("                ");
      lcd.setCursor(2,0);
      lcd.print("Door Close !");
      delay(250);
      myservo.write(90);
      Serial.println("ปิดประตู");
      if(status_open == 1 and analogRead(0)>=355){
        Uno.println(45126);
        }

      delay(1000);
      
      true_card = 0;
      tissue_status = 0;
      door_status = 0;
      first_noti = 0;
      second_noti = 0;
      close_activate = 0;
    }
   else{
    Serial.println("OUT");
    }
  }
  else if (tissue_status == 0){
    sensor_read = analogRead(0);
    Serial.println(sensor_read);
    
    // calculate percent and transfer notify tissue out
    if(sensor_read<=285){percent_left = 0;}
    else if (sensor_read>=425){percent_left = 100;}
    else{percent_left = ((sensor_read-285)*0.7142857);}
    Serial.print("per ");
    Serial.println(percent_left);
    
    //sent percent left to blink
    transfer_percent = 3000 + percent_left;
    Uno.println(transfer_percent); 
    
    // print status,percent left
    lcd.clear();
    if(percent_left<=5)
    {
      lcd.setCursor(1,0);
      lcd.print("Out Of Tissue");
      if(second_noti==0){
        Uno.println(50542); 
        second_noti = 1;
        tissue_status = 1;
        Serial.println("tissue_status_update");
        Uno.println(3000);
        }
        status_open = 1;
    }
    else if(percent_left<=30)
    {
      lcd.setCursor(3,0);
      lcd.print("Almost Out");
      if(first_noti==0){
        Uno.println(50541); 
        first_noti=1;
        }
      status_open = 1;
    }
    else
    {
      lcd.setCursor(3,0);
      lcd.print("Available");
      status_open = 0;
    }
    lcd.setCursor(3,1);
    lcd.print("Left:");
    if(percent_left<10){lcd.setCursor(10,1);}
    else if(percent_left<100){lcd.setCursor(9,1);}
    else{lcd.setCursor(8,1);}
    lcd.print(percent_left);
    lcd.setCursor(11,1);
    lcd.print("%");
    delay(1000);
    }
  // Tissue out
  else{
    lcd.setCursor(1,0);
    lcd.print("Out Of Tissue");
    lcd.setCursor(3,1);
    lcd.print("Left:  0%");

    
//    Serial.println("tissue_status_update");
//    if(true_card==1){tissue_status = 0; true_card=0;}
//    first_noti = 0;
//    second_noti = 0;
    }
    
  
if ( ! rfid.PICC_IsNewCardPresent())
return;
if (rfid.PICC_ReadCardSerial()) {
for (byte i = 0; i < 4; i++) {
tag += rfid.uid.uidByte[i];
}
lcd.clear();
Serial.println(tag); 
lcd.setCursor(0,0);
lcd.print("Open By :");
lcd.setCursor(0,1);
if (tag == "521719056") {Uno.println(67003); tissue_status=1; true_card=1; lcd.print("Athipong Sta.");}
else if(tag == "1641299256") {Uno.println(67019); tissue_status=1; true_card=1; lcd.print("Pannavich Tan.");}
else if(tag == "1322068556") {Uno.println(67029); tissue_status=1; true_card=1; lcd.print("Nathavat Sir.");}
else if(tag == "971177838") {Uno.println(67051); tissue_status=1; true_card=1; lcd.print("Admin01");}
else if(tag == "35818049") {Uno.println(67052); tissue_status=1; true_card=1; lcd.print("Admin02");}
else {
  tag = "";                                                                                                                                                                       
  Uno.println(66249); 
  lcd.clear();
  lcd.setCursor(2,0);
  lcd.print("Unknow User");
  }
delay(1000); //
lcd.clear();
rfid.PICC_HaltA();
rfid.PCD_StopCrypto1();
tag = ""; //ไปใส่ส่งส่งไลน์แจ้งเติมแล้ว
}


Serial.print("tc=");
Serial.println(true_card);
}    
