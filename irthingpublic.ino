
/*
 * 
 * Dikkat klimanızda oluşacak hasarlardan yazar sorumlu değildir. 
 * Uygulama esnasında klimanıza çok sık komut göndermeyiniz. Özellikle aç/kapat yapmak sıkıntı çıkarabilir. 
 * program olası tüm yaşam senaryoları için geliştirilmemiş ve test edilmemiştir. 
 */

#define kanal 1113959//<<<ts kanal no
#define writeAPIkey "1111111111111111"//<<< TS api yazma anahtarı

#define readAPIkey "22222222222222222222222"//<<< TS api okuma anahtarı
#define fieldno 5 // <<<ts kanaldaki komut alanı numarası



#include <strings.h>
#include <ThingSpeak.h>


#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <ESP8266WiFi.h>

#include <ESP8266WiFiMulti.h>
WiFiClient  client;
const uint16_t kIrLed = 4;  // ESP8266 Infrared led bağlanacak bacak---> 4 (D2).
IRsend irsend(kIrLed);

ESP8266WiFiMulti WiFiMulti;


void setup() {

  irsend.begin();
  Serial.begin(74880, SERIAL_8N1, SERIAL_TX_ONLY);
  delay(100);
  WiFi.mode(WIFI_OFF);
  delay(100);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);

  WiFi.mode(WIFI_STA);

  ThingSpeak.begin(client);
  WiFiMulti.addAP("zzzzzz", "zzzzzzz");//<<<< mekandaki internet için wifi adı ve şifresi
 // WiFiMulti.addAP("zzzzzzz", "zzzzzzzzz");

  Serial.println("wifi");
  int conc = 15;
  while (WiFiMulti.run() != WL_CONNECTED && conc > 0)
  {
    delay(900);
    conc--;
    Serial.println(String(conc));
  }
  if ((WiFiMulti.run() != WL_CONNECTED)) {
    digitalWrite(LED_BUILTIN, LOW); //led sinyali ver
    delay(750);
    digitalWrite(LED_BUILTIN, HIGH);
    ESP.restart();  //internet yok ise esp restart edilecek.
  }

  int i = 0;

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");


  IPAddress myip = WiFi.localIP();
  String sFullip = String(myip[0]) + "." + myip[1] + "." + myip[2] + "." + myip[3];
  Serial.println(sFullip);
  delay(1000);

}
//ht tps://api.thingspeak.com/update?api_key=sdfsfsdfsdf&field1=0 komut sistemi örnek yazma kullanıcıdan

int laststate = -1;
void loop() {

  int klimakomut = ThingSpeak.readFloatField(kanal, fieldno, readAPIkey);
  //komut durumu 200 ise aşağısı çalışmalı veri gelmemiş olabilir hatalı işlem oluşmuş olabilir.
  Serial.println("Klima komutu: " + String(klimakomut));
  if (klimakomut != laststate ) {
    switch (klimakomut)
    {
      case 1:
        irsend.sendVestelAc(0xF0159001FEF201, 56, 0);
        delay(50);
         irsend.sendVestelAc(0xF0159001FEF201, 56, 0);//
        Serial.println("vestel acildi 25 cool f1");
        break;
      case 2:
        irsend.sendVestelAc(0xC0159001FF1201, 56, 0);
        Serial.println("vestel kapandi");       
        break;
      case 3:
        irsend.sendLG(0x8800606, 28, 1);
        Serial.println("LG Beko Arçelik açıldı 21 cool f1");
        break;
      case 4:
        irsend.sendLG(0x8808A02, 28, 1);
        Serial.println("LG Beko Arçelik 25 derece, fan düşük, ION kapalı, DIF kapalı yapıldı.");
        break;
      case 5:
        irsend.sendLG(0x88C0051, 28, 1);
        Serial.println("LG Beko Arçelik kapatıldı");
        break;
        case 6: 
        irsend.sendVestelAc(0xF015A001FEF201, 56, 0);
        Serial.println("vestel 26c f1");
        break;
         case 7: 
        irsend.sendVestelAc(0xF01B8001FEF201, 56, 0);
        Serial.println(" Power: On, Mode: 1 (COOL), Temp: 24C, Fan: 11 (HIGH), Sleep: Off, Turbo: Off, Ion: Off, Swing: Off");
        break;
         case 8: 
        irsend.sendVestelAc(0xF01B9001FEE201, 56, 0);
        Serial.println(" Power: On, Mode: 1 (COOL), Temp: 25C, Fan: 11 (HIGH), Sleep: Off, Turbo: Off, Ion: Off, Swing: Off");
        break;
    }

    Serial.println( String(klimakomut) + " gonderildi.");
    ThingSpeak.writeField(kanal,6, klimakomut,writeAPIkey);
    
    laststate = klimakomut;
    for (int i = 0; i < klimakomut; i++)
    {
      digitalWrite(LED_BUILTIN, LOW);
      delay(40);
      digitalWrite(LED_BUILTIN, HIGH);
      delay(100);
    }
  }

  delay(10000);

}
