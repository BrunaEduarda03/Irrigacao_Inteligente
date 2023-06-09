/* OLED */
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#define SCREEN_WIDTH 128 // OLED width,  in pixels
#define SCREEN_HEIGHT 64 // OLED height, in pixels
Adafruit_SSD1306 oled(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
/*
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp8266.h>
*/
/*DHT11*/
#include "DHT.h"
#define DHTPIN 4  
#define DHTTYPE DHT11 

/*-------- Token de Autenticação ----------- */
//char auth[] = "Qyvl_WEZVA4TEx0aOWgYQxdTLvplmwY2";
/*-------- Configurações de Wi-Fi ----------- */
//char ssid[] = "Bruna";
//char pass[] = "bruna0303"; 

//BlynkTimer timer;
DHT dht(DHTPIN, DHTTYPE);
float airHum = 0;
float airTemp = 0;

/* Soil airHumidity */
#define soilHumPIN 2
float soilHum = 0;

/* rain sensor */
#define runPIN 15
float rainnSensor = 0;

/* Automatic Control Parameters Definition 
#define DRY_SOIL      30
#define WET_SOIL      70
#define COLD_TEMP     12
#define HOT_TEMP      26
#define TIME_PUMP_ON  15
#define TIME_LAMP_ON  15
*/
/*
void myTimer() 
{
  // This function describes what will happen with each timer tick
  // timer é criado para impedir que aconteça uma sobrecarga no servidor
  Blynk.virtualWrite(V0,airTemp);      // variável Temp é alocada na variável virtual V0
  Blynk.virtualWrite(V1,airHum);
  Blynk.virtualWrite(V2,soilHum);
}
*/
void setup()
{
 //Blynk.begin(auth, ssid, pass);            // Inicializa o Blynk
  //timer.setInterval(1000L, myTimer);            // Inicializa o Timer do blynk para enviar os dados para o APP
  Serial.begin(115200);
  delay(10);
  dht.begin();
  oledStart();
}

void loop()
{
  // Runs all Blynk stuff
  //Blynk.run(); 
  // runs BlynkTimer
  //timer.run(); 
  getSensors();
  displayData();
  delay(1000);
  
}

/***************************************************
 * Start OLED
 **************************************************/
void oledStart(void)
{
  Wire.begin();  
  oled.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  oled.clearDisplay();              // Clear screen       
  oled.setTextColor(WHITE);    // set text color
  oled.setCursor(0,0);       // set position to display     
  oled.println("Proj.Irrigacao");
  oled.display();
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
  Serial.print("oC   souilHumidity: ");
  Serial.print(soilHum);
  Serial.println("%");

  
  oled.setCursor(0,0);       // set position to display     
  oled.println("Proj.Irrigacao");
  oled.display();
  //air temperature
  oled.setTextSize(1);
  oled.setCursor(0,20);              // Set cursor position, start of line 2
  oled.print("airTemp: ");
  oled.print(airTemp);
  oled.print(" ");
  oled.setTextSize(1);
  oled.cp437(true);                  // Escrever o símbolo º 
  oled.write(167);
  oled.setTextSize(1);
  oled.print("C");
  oled.display();

  //air Humidity
  oled.setTextSize(1);
  oled.setCursor(0,30);              // Set cursor position, start of line 2
  oled.print("airHum: ");
  oled.print(airHum);
  oled.print(" ");
  oled.setTextSize(1);
  oled.print("%");
  oled.display();

  //soil Humidity
  oled.setTextSize(1);
  oled.setCursor(0,40);              // Set cursor position, start of line 2
  oled.print("soilHum: ");
  oled.print(soilHum);
  oled.print(" ");
  oled.setTextSize(1);
  oled.print("%");
  oled.display();
  delay(100);
  oled.clearDisplay(); 
}

void getSensors(void)
{
  airTemp = dht.readTemperature();
  airHum = dht.readHumidity();
  float sensorValue = analogRead(soilHumPIN);
  soilHum = map(sensorValue, 0, 4095, 0, 100); // 0-100%
  
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
