#include <Blynk.h>
#define BLYNK_PRINT Serial
#include <BlynkSimpleEsp8266.h>
#include <DHT.h>  // Including library for dht

#include <ESP8266WiFi.h>
String apiKey = "D95DUHUH760LVQTK";     //  Enter your Write API key from ThingSpeak
char auth[] = "tLKmgTjhwPUt_nSS9AKNijF2H6flXVS4";
const char *ssid1 =  "CC4";     // replace with your wifi ssid and wpa2 key
const char *pass1 =  "12345678";
const char* server = "api.thingspeak.com";
char ssid[] = "CC4";    // Your WiFi credentials.
char pass[] = "12345678";
#define LED 5 // Led in NodeMCU at pin GPIO5 (D1).
#define DHTPIN 0          //pin where the dht11 is connected
//MQ135 gasSensor = MQ135(ANALOGPIN); 
int outputpin= A0;
int s1,s2;
DHT dht(DHTPIN, DHT11);

WiFiClient client;
 
void setup() 
{      
       Serial.begin(115200);
       delay(10);
       Blynk.begin(auth,ssid,pass);
       dht.begin();
       pinMode(LED, OUTPUT);
       Serial.println("Connecting to ");
       Serial.println(ssid1);
       WiFi.begin(ssid1, pass1);
 
      while (WiFi.status() != WL_CONNECTED) 
     {
            delay(500);
            Serial.print(".");
     }
      Serial.println("");
      Serial.println("WiFi connected");
 
}
 
void loop() 
{

   int analogValue = analogRead(outputpin);
    //float rzero = gasSensor.getRZero(); //this to get the rzero value, uncomment this to get ppm value
    //float ppm = gasSensor.getPPM();
    //Serial.println(rzero); // this to display the rzero value continuously, uncomment this to get ppm value
    //Serial.println(ppm);
    digitalWrite(LED, HIGH);// turn the LED off.(Note that LOW is the voltage level but actually
                      //the LED is on; this is because it is acive low on the ESP8266.
    delay(1000);          // wait for 1 second.
    digitalWrite(LED, LOW); // turn the LED on.
    delay(500);         // wait for 1 second.
    
    s1 = analogRead(0);       // read analog input pin 0
    s2 = digitalRead(0);
    Serial.print("Air Quailty: ");
    Serial.print(s1);
    Serial.print("ppm ");
      float h = dht.readHumidity();
      float t = dht.readTemperature();
      
      
              if (isnan(h) || isnan(t) || isnan(s1)) 
                 {
                     Serial.println("Failed to read from DHT sensor!");
                      return;
                 }

                         if (client.connect(server,80))   //   "184.106.153.149" or api.thingspeak.com
                      {  
                            
                             String postStr = apiKey;
                             postStr +="&field1=";
                             postStr += String(t);
                             postStr +="&field2=";
                             postStr += String(h);
                             postStr +="&field3=";
                             postStr += String(s1);
                             postStr += "\r\n\r\n";
                            
                             client.print("POST /update HTTP/1.1\n");
                             client.print("Host: api.thingspeak.com\n");
                             client.print("Connection: close\n");
                             client.print("X-THINGSPEAKAPIKEY: "+apiKey+"\n");
                             client.print("Content-Type: application/x-www-form-urlencoded\n");
                             client.print("Content-Length: ");
                             client.print(postStr.length());
                             client.print("\n\n");
                             client.print(postStr);
 
                             Serial.print("Temperature: ");
                             Serial.print(t);
                             Serial.print(" degrees Celcius, Humidity: ");
                             Serial.print(h);
                             Serial.println("%. Send to Thingspeak.");
                             if(s1>500 && t>25.0 && h>60.0)
                             {
                              Serial.println("Unhealth Air");
                              digitalWrite(LED, HIGH);
                              delay(5000);
                              digitalWrite(LED, LOW);
                              delay(500);
                              Blynk.email("Shashwat.negi2017@vitstudent.ac.in", "ESP8266 Alert", "Unhealthy Air!");
                              Blynk.notify("ESP8266 Alert - Unhealthy Air!");
                          }
                             
                             
                        }
          client.stop();
          Blynk.virtualWrite(V0, t);
          Blynk.virtualWrite(V1, h);
          Blynk.virtualWrite(V2, s1);
          Serial.println("Waiting...");
  
  // thingspeak needs minimum 15 sec delay between updates, i've set it to 30 seconds
  delay(5000);
}
