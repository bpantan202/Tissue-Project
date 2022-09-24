
#include <ESP8266WiFi.h>
#include <SoftwareSerial.h>
SoftwareSerial NodeMCU(D3, D2); // RX | TX

#define SSID        "SUNAN"     
#define PASSWORD    "0622695363"  

// set line notify
#include <TridentTD_LineNotify.h>
#define LINE_TOKEN  "KyOtvySI7WubJNASXDe9AMvj5RbgOHKjGOfyxRX9gXM"   

// set blink
#define BLYNK_PRINT Serial
#define BLYNK_TEMPLATE_ID "TMPLvtJzZjHM"
#define BLYNK_DEVICE_NAME "SaTu"
#include <BlynkSimpleEsp8266.h>
char auth[] = "tE-BLnpzX824wm3PB0OCR4MR2kX6wCjC";
char ssid[] = "SUNAN";
char pass[] = "0622695363";

String user;
int transfer_num;
int change_check;
int left;
int tissue_use_count;
void setup() {
  //set input from arduino R3
  pinMode(D3, INPUT);
  pinMode(D2, OUTPUT);
  Serial.begin(9600); // Hardware Serial ที่แสดงออกหน้าจอ Serial Monitor
  NodeMCU.begin(9600); // Software Serial ที่ NodeMCU ใช้สื่อสากับ Arduino Uno
  
  //esp82666 wifi setting
//  Serial.begin(9600); 
  Serial.println();
  Serial.println(LINE.getVersion());
  WiFi.begin(SSID, PASSWORD);
  Serial.printf("WiFi connecting to %s\n",  SSID);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(400);}
  Serial.printf("\nWiFi connected\nIP : ");
  Serial.println(WiFi.localIP());
  // กำหนด Line Token
  LINE.setToken(LINE_TOKEN);  


  Blynk.begin(auth, ssid, pass);
  Blynk.run();

  change_check=1;
}

void loop() {
  Blynk.virtualWrite(V1,left);
  transfer_num = NodeMCU.parseInt();

  if(transfer_num!=0){
//  change_check=0;
  Serial.print(transfer_num);

  if(transfer_num <= 3100){
   left = transfer_num-3000;
    if(left<=5){Blynk.virtualWrite(V3,"Out Of Tissue");Blynk.virtualWrite(V4," ");}
    else if(left<=30){Blynk.virtualWrite(V3,"Almost Out");Blynk.virtualWrite(V4," ");}
    else{Blynk.virtualWrite(V3,"Available");Blynk.virtualWrite(V4," ");}
    }
  else if (transfer_num == 45126){
    tissue_use_count += 1;
    Blynk.virtualWrite(V2,tissue_use_count);
    Blynk.virtualWrite(V3,"Used count");
    Blynk.virtualWrite(V4," ");
    }
  else if(transfer_num >= 44000 and transfer_num <= 46000){
    if (transfer_num == 44003){user = "อธิพงษ์";}
    else if (transfer_num == 44019){user = "ปัณณวิชญ์";}
    else if (transfer_num == 44029){user = "ณัฐฐ์วัฒน์";}
    else if (transfer_num == 44051){user = "admin01";}
    else if (transfer_num == 44052){user = "admin02";}
    Blynk.virtualWrite(V3,"Door Closing");
    Blynk.virtualWrite(V4," ");
    LINE.notify("ปิดประตูสำเร็จโดย "+user);
    delay(1000);
  }
  else if(transfer_num >= 67000){
    if (transfer_num == 67003){user = "อธิพงษ์";}
    else if (transfer_num == 67019){user = "ปัณณวิชญ์";}
    else if (transfer_num == 67029){user = "ณัฐฐ์วัฒน์";}
    else if (transfer_num == 67051){user = "admin01";}
    else if (transfer_num == 67052){user = "admin02";}
    Blynk.virtualWrite(V3,"Door Opening");
    Blynk.virtualWrite(V4," ");
    LINE.notify("เปิดประตูสำเร็จโดย "+user);
    delay(1000);
    Blynk.virtualWrite(V3,"Please close");
    Blynk.virtualWrite(V4,"the door.");
  }
  else if (transfer_num == 66249){LINE.notify("พบการเข้าสู่ระบบโดยผู้ใช้ที่ไม่รู้จัก");}
  else if (transfer_num == 50541){LINE.notify("กระดาษชำระใกล้หมด");}
  else if (transfer_num == 50542){LINE.notify("กระดาษชำระหมด!!!");}
//  else if (transfer_num == 44201){LINE.notify("ปิดประตูสำเร็จโดย "+user); user = "";}
  }
}
