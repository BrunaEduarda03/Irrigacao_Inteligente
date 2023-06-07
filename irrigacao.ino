/* OLED */
#include <ACROBOTIC_SSD1306.h> // library for OLED: SCL ==> D1; SDA ==> D2
#include <SPI.h>
#include <Wire.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp8266.h>

/*DHT11*/
#include "DHT.h"
#define DHTPIN D3  
#define DHTTYPE DHT11 

/*-------- Token de Autenticação ----------- */
char auth[] = "Qyvl_WEZVA4TEx0aOWgYQxdTLvplmwY2";
/*-------- Configurações de Wi-Fi ----------- */
char ssid[] = "connect-izaias";
char pass[] = "244466666"; 

BlynkTimer timer;
DHT dht(DHTPIN, DHTTYPE);
float airHum = 0;
float airTemp = 0;

/* Soil airHumidity */
#define soilHumPIN 0
float soilHum = 0;

/* Automatic Control Parameters Definition 
#define DRY_SOIL      30
#define WET_SOIL      70
#define COLD_TEMP     12
#define HOT_TEMP      26
#define TIME_PUMP_ON  15
#define TIME_LAMP_ON  15
*/

void myTimer() 
{
  // This function describes what will happen with each timer tick
  // timer é criado para impedir que aconteça uma sobrecarga no servidor
  Blynk.virtualWrite(V0,airTemp);      // variável Temp é alocada na variável virtual V0
  Blynk.virtualWrite(V1,airHum);
  Blynk.virtualWrite(V2,soilHum);
}

void setup()
{
  Blynk.begin(auth, ssid, pass);            // Inicializa o Blynk
  timer.setInterval(1000L, myTimer);            // Inicializa o Timer do blynk para enviar os dados para o APP
  Serial.begin(115200);
  delay(10);
  dht.begin();
  oledStart();
}

void loop()
{

  // Runs all Blynk stuff
  Blynk.run(); 
  // runs BlynkTimer
  timer.run(); 
  getSensors();
  displayData();
  delay(2000);
}

/***************************************************
 * Start OLED
 **************************************************/
void oledStart(void)
{
  Wire.begin();  
  oled.init();                      // Initialze SSD1306 OLED display
  clearOledDisplay();
  oled.clearDisplay();              // Clear screen
  oled.setTextXY(0,0);              
  oled.putString("Proj.Irrigacao");
}

/***************************************************
 * Clear OLED Display
 **************************************************/
void clearOledDisplay()
{
  oled.setFont(font8x8);
  oled.setTextXY(0,0); oled.putString("                ");
  oled.setTextXY(1,0); oled.putString("                ");
  oled.setTextXY(2,0); oled.putString("                ");
  oled.setTextXY(3,0); oled.putString("                ");
  oled.setTextXY(4,0); oled.putString("                ");
  oled.setTextXY(5,0); oled.putString("                ");
  oled.setTextXY(6,0); oled.putString("                ");
  oled.setTextXY(7,0); oled.putString("                ");
  oled.setTextXY(0,0); oled.putString("                ");              
}

/***************************************************
 * Display data at Serial Monitora & OLED Display
 **************************************************/
void displayData(void)
{
  Serial.print(" airTemperature: ");
  Serial.print(airTemp);
  Serial.print("oC   airHumidity: ");
  Serial.print(airHum);
  Serial.println("%");
  
  oled.setTextXY(3,0);              // Set cursor position, start of line 2
  oled.putString("airTemp:" + String(airTemp) + "oC");
  oled.setTextXY(5,0);              // Set cursor position, start of line 2
  oled.putString("airHum: " + String(airHum) + "%");
  oled.setTextXY(7,0);              // Set cursor position, start of line 2
  oled.putString("soilHum:" + String(soilHum) + "%");
}

void getSensors(void)
{
  float airTempIni = airTemp;
  float airHumIni = airHum;
  airTemp = dht.readTemperature();
  airHum = dht.readHumidity();
  soilHum = map(analogRead(soilHumPIN), 1023, 0, 0, 100); // %
  if (isnan(airHum) || isnan(airTemp))   // Check if any reads failed and exit early (to try again).
  {
    Serial.println("Failed to read from DHT sensor!");
    airTemp = airTempIni;
    airHum = airHumIni;
    return;
  }
}
/*
0 ~300 solo seco
300~700 solo úmido
700~950 em água

#define DRY_SOIL      30
#define WET_SOIL      70
#define COLD_TEMP     12
#define HOT_TEMP      26
#define TIME_PUMP_ON  15
#define TIME_LAMP_ON  15


void autoControlPlantation(){
  
  // turn water(LED RED) ON/OFF() 
  if(soilHum < DRY_SOIL){
    Serial.println("Irrigando a planta ..."); // Imprime a frase no monitor serial
    //digitalWrite(pinoRele, HIGH); // Altera o estado do pinoRele para nível Alto
    //digitalWrite(ledPin1, HIGH);
    //delay (TIME_PUMP_ON*1000);
  }else if(soilHum < WET_SOIL){
    Serial.println("Atenção!irrigar a planta em breve..."); // Imprime a frase no monitor serial
    //digitalWrite(ledPin1, LOW);
    //digitalWrite(pinoRele, LOW); // Altera o estado do pinoRele para nível Alto
  }else{
    Serial.println("Planta Irrigada ..."); // Imprime a frase no monitor serial
    //digitalWrite(pinoRele, LOW); // Altera o estado do pinoRele para nível Baixo
    //digitalWrite(ledPin1, LOW);
  }

  // LED GREEN ON/OFF
  if(airTemp < COLD_TEMP){
    Serial.println("Ligado sistema de calor...");
    //digitalWrite(ledPin2, HIGH);
  }else if(airTemp < HOT_TEMP){
    Serial.println("Temperatura ideal..."); // Imprime a frase no monitor serial
    //digitalWrite(ledPin1, LOW);
    //digitalWrite(pinoRele, LOW); // Altera o estado do pinoRele para nível Alto
  }else{
    Serial.println("Temperatura elevada ..."); // Imprime a frase no monitor serial
    //digitalWrite(pinoRele, LOW); // Altera o estado do pinoRele para nível Baixo
    //digitalWrite(ledPin1, LOW);
  }

*/
