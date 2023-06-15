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
#define DHTPIN 4  
#define DHTTYPE DHT11 

/* Buttons */
#define PUMP_ON_BUTTON 19         //push-button PUMP (blue)
#define PUMP_ON_LED 33            //LED BLUE
#define LAMP_ON_BUTTON 18        //push-button LAMP (yellow)
#define LAMP_ON_LED 32          //LED YELLOW
//#define SENSORS_READ_BUTTON 13   //push-button SENSOR (red)

// Atuadores
boolean pumpStatus = 0;
boolean lampStatus = 0;
int timePumpOn = 10; // turn Pump On in minutes

/* Automatic Control Parameters Definition */
#define DRY_SOIL      40
#define WET_SOIL      70
#define COLD_TEMP     12
#define HOT_TEMP      25
#define TIME_PUMP_ON  10
#define TIME_LAMP_ON  10
int PUMPstate =0;
int LAMPstate=0;
/*-------- Token de Autenticação ----------- */
char auth[] = "Qyvl_WEZVA4TEx0aOWgYQxdTLvplmwY2";
/*-------- Configurações de Wi-Fi ----------- */
char ssid[] = "connect-izaias";
char pass[] = "244466666"; 

BlynkTimer timer;
Adafruit_SSD1306 oled(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

/*Air Temperature & Humidity*/
DHT dht(DHTPIN, DHTTYPE);
int airHum = 0;
int airTemp = 0;
/* Soil Humidity */
#define soilHumPIN 39
int soilHum = 0;
/* rain sensor */
#define rainPIN 36
int rainSensor = 0;

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
  Blynk.virtualWrite(V4,rainSensor);
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
  //pinMode(SENSORS_READ, INPUT_PULLUP); // Button
  dht.begin();
  oledStart();
  delay(1000);
}

void loop()
{
  // Runs all Blynk stuff
  Blynk.run(); 
  timer.run(); 

  // elapsedTime = millis()-startTiming;   // Start timer for measurements
  readSensors();
  displayData();
  //readLocalCmd(); //Read local button status
  autoControlPlantation();

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

/*void readLocalCmd() 
{  
  int digiValue = debounce(PUMP_ON_BUTTON);
  if (!digiValue) 
  {
    pumpStatus = !pumpStatus;
    delay (TIME_PUMP_ON*1000);
    //showDataLCD();
    aplyCmd();
  }

  digiValue = debounce(LAMP_ON_BUTTON);
  if (!digiValue) 
  {
    lampStatus = !lampStatus;
    delay (TIME_LAMP_ON*1000);
    //showDataLCD();
    aplyCmd();
  }
  /*
  digiValue = debounce(SENSORS_READ);
  if (!digiValue) 
  {
    digitalWrite(YELLOW_LED, HIGH); 
    lcd.setCursor (0,0);
    lcd.print("< Updating Sensors >");
    readSensors();
    digitalWrite(YELLOW_LED, LOW); 
  }

}
/*
/*Aplicar Comandos nos atuadores*/
void aplyCmd()
{
    if (pumpStatus == 1) digitalWrite(PUMP_ON_LED, HIGH);
    if (pumpStatus == 0) digitalWrite(PUMP_ON_LED, LOW);
  
    if (lampStatus == 1) digitalWrite(LAMP_ON_LED, HIGH);
    if (lampStatus == 0) digitalWrite(LAMP_ON_LED, LOW);
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
  pumpStatus = 1;
  aplyCmd();
  delay (TIME_PUMP_ON*1000);
  pumpStatus = 0;
  aplyCmd();
}
void turnLampOn()
{
  lampStatus = 1;
  aplyCmd();
  delay (TIME_LAMP_ON*1000);
  lampStatus = 0;
  aplyCmd();
}
