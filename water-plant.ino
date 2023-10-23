#include <WiFi.h>
#include <PubSubClient.h>

#define BLYNK_TEMPLATE_ID "TMPL6JcJtbdjq"
#define BLYNK_TEMPLATE_NAME "iot101"
#define BLYNK_AUTH_TOKEN "vdJTbdmUxK8m_SLZf1DUuPb0l3jBhjlk"

#define BLYNK_PRINT Serial

#include <BlynkSimpleEsp32.h>

#define LINE_TOKEN  "DOWzyKSCEBvn9XbDhpig6aVJJQPQAerpjBgi7ak2kKl" 
//line token
#include <TridentTD_LineNotify.h>

#include "DHT.h"
#define DHTPIN 27     //Pin ขาสำหรับ เสียบกับ Nodemcu
#define analog  34
const int Relay = 14;
#define DHTTYPE DHT11
WiFiClient espClient;
PubSubClient client(espClient);

const char* mqtt_server = "broker.netpie.io";
const int mqtt_port = 1883;
const char* mqtt_client = "7bdc57cd-a80e-4238-9d87-eac79c57f1ed";
const char* mqtt_username = "JVbymdJFkhN3z7PYJwRaS7Lcu1QJw3LX";
const char* mqtt_password = "QQ4PR4PtsHgQWw6SthGRZzwCMeVJ2ozY";
char ntp_server1[20] = "pool.ntp.org";
char ntp_server2[20] = "time.nist.gov";
char ntp_server3[20] = "time.uni.net.th";

int check =0;
char auth[] = "vdJTbdmUxK8m_SLZf1DUuPb0l3jBhjlk"; 

char ssid[] = "PHURIN";  //ชื่อไวไฟ
char pass[] = "PHURIN14285";  //รหัส


int SR = 0; // สร้างตัวแปร SR มีค่าเท่ากับ 0
int soil = 0; // สร้างตัวแปร temp มีค่าเท่ากับ 0
int c;

DHT dht(DHTPIN, DHTTYPE);

void setup()
{
  Serial.begin(115200);
  pinMode(Relay, OUTPUT); // กำหนด รีเลย์เป็น Output
  dht.begin(); 
  WiFi.mode(WIFI_STA);
  Serial.println(LINE.getVersion());
  Serial.println("WiFi Connecting");
  Serial.println(ssid);
  WiFi.begin(ssid, pass); 
  while (WiFi.status() != WL_CONNECTED)       //ตรวจเช็คสถานะการเขื่อมต่อ wifi
  {
    delay(500);
    Serial.print(".");                      //ถ้าไม่สำเร็จให้แสดง "." จนกว่าจะเขื่อมต่อได้
  }
  Serial.println("");
  Serial.println("WiFi connected"); 
  LINE.setToken(LINE_TOKEN);
}
BLYNK_WRITE(V3)                              
  { 
   float h = dht.readHumidity();                                         
   int val = param.asInt();
    if(soil < 69) {  //ตั้งค่า % ที่ต้องการจะรดน้ำต้นไม้
      if(h > 95) {
         LINE.notify("ความชื้นในอากาศสูงมีแนวโน้มฝนจะตกไม่ควรรดน้ำต้นไม้ --> Humidity : "+String(h)+" % ");
      }
      else{
        digitalWrite(Relay ,val); 
        delay(3000);  
        LINE.notify("รดน้ำต้นไม้เเล้ว: soil moisture "+String(soil)+" %");
      }
    }
    else { 
      LINE.notify("ความชื้นดินเพียงพอเเล้วไม่ควรรดน้ำต้นไม้ --> soil moisture : "+String(soil)+" %");   
     }                 
             
  }  
  
void loop()
{
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  if (isnan(h) || isnan(t)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }
  SR = analogRead(34); // ให้ตัวแปล SR อ่านค่าจาก Pin A0
  soil = map(SR, 4095, 0, 100, 0); // ตัวแปล Temp คือ แปลงค่าของตัวแปร SR จาก 1023 ถึง 0 เป็น 0 ถึง 100  
  Blynk.virtualWrite(V0, soil);
  Serial.print("Soil Moisture = ");
  Serial.print(soil);
  Serial.println(" %");
  if (soil > 55) {  //ตั้งค่า % ที่ต้องการจะรดน้ำต้นไม้
    if (c == 1){
       digitalWrite(Relay, LOW);
       LINE.notify("รดน้ำต้นไม้เเล้ว: soil moisture "+String(soil)+" %");
       c = 0;
    }
    else {
      digitalWrite(Relay, HIGH);
     }
  }
  else {
     if(h < 95) {
        digitalWrite(Relay, LOW);
        LINE.notify("soil moisture "+String(soil)+" %");
        c = 1; 
      }
    else{
      if (soil < 45) {
        digitalWrite(Relay, LOW);
        LINE.notify("soil moisture "+String(soil)+" %");
        c = 1; 
      }
      else{
       digitalWrite(Relay, HIGH);
    }
  }
  }   
  Serial.print(F("Humidity: "));
  Serial.print(h);
  Serial.print(F("%  Temperature: "));
  Serial.print(t);
  Serial.println(F(" C "));
  Blynk.virtualWrite(V1, t);
  Blynk.virtualWrite(V2, h);
  delay(10000); 
}
