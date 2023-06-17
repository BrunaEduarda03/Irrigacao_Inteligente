/* OLED */
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#define SCREEN_WIDTH 128 // OLED width,  in pixels
#define SCREEN_HEIGHT 64 // OLED height, in pixels

/*BLYNK*/
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

/*DHT11*/
#include "DHT.h"

/* Buttons */
#define PUMP_ON_BUTTON 19         //push-button PUMP (blue)
#define PUMP_ON_LED 33            //LED BLUE
#define LAMP_ON_BUTTON 18        //push-button LAMP (yellow)
#define LAMP_ON_LED 32          //LED YELLOW

/* Automatic Control Parameters Definition */
#define DRY_SOIL      40
#define WET_SOIL      70
#define COLD_TEMP     12
#define HOT_TEMP      25
#define TIME_PUMP_ON  5
#define TIME_LAMP_ON  5

#define soilHumPIN 39
#define rainPIN 36
#define DHTPIN 4  
#define DHTTYPE DHT11 

/*-------- Token de Autenticação ----------- */
char auth[] = "Qyvl_WEZVA4TEx0aOWgYQxdTLvplmwY2";
/*-------- Configurações de Wi-Fi ----------- */
char ssid[] = "Bruna";
char pass[] = "bruna0303"; 

BlynkTimer timer;
Adafruit_SSD1306 oled(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

/*Air Temperature & Humidity*/
DHT dht(DHTPIN, DHTTYPE);

// Sensors
float airHum = 0;
float airTemp = 0;
float soilHum = 0;
float rainSensor = 0;

// Atuadores
boolean pumpStatus = 0;
boolean lampStatus = 0;
int timePumpOn = 10; // turn Pump On in minutes
int PUMPstate = 0;
int LAMPstate = 0;

void myTimer() 
{
  // This function describes what will happen with each timer tick
  // timer é criado para impedir que aconteça uma sobrecarga no servidor
  Blynk.virtualWrite(V0,airTemp);      // variável Temp é alocada na variável virtual V0
  Blynk.virtualWrite(V1,airHum);
  Blynk.virtualWrite(V2,soilHum);
  Blynk.virtualWrite(V4,rainSensor);
}

BLYNK_WRITE(V3) // pump water blynk
{
  if(param.asInt() == 1)
  {
    Serial.println("Ligando água");
    turnPumpOn();
  }else{
    digitalWrite(PUMP_ON_LED, LOW);
  }
 
}

BLYNK_WRITE(V5) // led on blynk
{
  if(param.asInt() == 1)
  {
    Serial.println("Ligando led");
    turnLampOn();
  }else{
    digitalWrite(LAMP_ON_LED, LOW);
  }
 
}

void setup()
{       
  Serial.begin(115200);
  Blynk.begin(auth, ssid, pass);                // Inicializa o Blynk
  timer.setInterval(1000L, myTimer);            // Inicializa o Timer do blynk para enviar os dados para o APP
  pinMode(PUMP_ON_LED, OUTPUT);
  pinMode(LAMP_ON_LED, OUTPUT);
  pinMode(PUMP_ON_BUTTON, INPUT_PULLUP); // Button
  pinMode(LAMP_ON_BUTTON, INPUT_PULLUP); // Button
  dht.begin();
  oledStart();
  delay(1000);
}

void loop()
{
  Blynk.run(); 
  timer.run(); 
  readSensors();
  displayData();
  readLocalCmd(); //Read local button status
  //autoControlPlantation();

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
  oled.setTextSize(2);
  oled.setCursor(0,SCREEN_HEIGHT / 2);
  oled.println("Projeto");
  oled.println("Irrigacao");
  oled.display();
}


/***************************************************
 * Display data at Serial Monitora & OLED Display
 **************************************************/
void displayData(void)
{
      oled.clearDisplay(); 
      oled.setTextSize(1);
      oled.setCursor(0,0);       // set position to display     
      oled.println("Proj.Irrigacao");

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

      //air Humidity
      oled.setTextSize(1);
      oled.setCursor(0,30);              // Set cursor position, start of line 2
      oled.print("airHum: ");
      oled.print(airHum);
      oled.print(" ");
      oled.setTextSize(1);
      oled.print("%");

      //soil Humidity
      Serial.println();
      oled.setTextSize(1);
      oled.setCursor(0,40);              // Set cursor position, start of line 2
      oled.print("soilHum: ");
      oled.print(soilHum);
      oled.print(" ");
      oled.setTextSize(1);
      oled.print("%");
      
      //rain Sensor
      oled.setTextSize(1);
      oled.setCursor(0,50);              // Set cursor position, start of line 2
      oled.print("rain: ");
      oled.print(rainSensor);
      oled.print(" ");
      oled.setTextSize(1);
      oled.print("%");
      oled.display();
      delay(100);
}

void readSensors(void)
{ 
  airTemp = dht.readTemperature();
  airHum = dht.readHumidity();
  soilHum = map(analogRead(soilHumPIN),0,4095,100,0); // 0-100%
  rainSensor = map(analogRead(rainPIN), 0,4095,100,0);
}

//controle manual da plantação
void readLocalCmd() 
{  
  PUMPstate = digitalRead(PUMP_ON_BUTTON);
  LAMPstate = digitalRead(LAMP_ON_BUTTON);

  if(PUMPstate == LOW){
    Serial.println("Botão pump pressionado");
     turnPumpOn();
  }

  if(LAMPstate == LOW){
    Serial.println("Botão led pressionado");
    turnLampOn();
  } 
  
}

//Plantação automatizada
void autoControlPlantation(void)
{ 
  PUMPstate = digitalRead(PUMP_ON_BUTTON);
  LAMPstate = digitalRead(LAMP_ON_BUTTON);
/*Ligar torneira*/
  if (soilHum < DRY_SOIL || PUMPstate == HIGH ) {
      turnPumpOn();
    }else{
      digitalWrite(PUMP_ON_LED, LOW);
    }
/*fonte de calor*/
  if (airTemp < COLD_TEMP && soilHum < WET_SOIL || LAMPstate == HIGH) 
  {
   turnLampOn();
  }else{
    digitalWrite(LAMP_ON_LED, LOW);
  }
}

void turnPumpOn()
{
  digitalWrite(PUMP_ON_LED, HIGH);
  delay (TIME_PUMP_ON*1000);
  digitalWrite(PUMP_ON_LED, LOW);
}
void turnLampOn()
{
  digitalWrite(LAMP_ON_LED, HIGH);
  delay (TIME_LAMP_ON*1000);
  digitalWrite(LAMP_ON_LED, LOW);
}
