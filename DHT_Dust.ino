#include <Wire.h>
#include <String.h>
#include <LiquidCrystal_I2C.h>
#include "DHT.h"
#define DHTPIN 2
#define DHTTYPE DHT11
DHT dht(DHTPIN,DHTTYPE);

LiquidCrystal_I2C lcd(0x27,16,2);

byte buff[11];
uint32_t duration;
uint32_t starttime;
uint32_t endtime;
uint32_t sampletime_ms=30000;
uint32_t lowpulseoccupancy=0;

float ratio=0;
long concentration =0;
float pcsPerCF=0;
float ugm3=0;

int i=0;
int pin=3;

void setup() {
  Serial.begin(9600);
  dht.begin();
  lcd.begin();
  lcd.backlight();
  initDsm501A();
}

void loop() {
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();
  Serial.print((int)temperature);
  Serial.print("C , ");
  Serial.print((int)humidity);
  Serial.println(" % ");

  String humi=(String)humidity;
  humi+="% / ";

  String temp = (String)temperature;
  temp+="C";

  lcd.setCursor(0,0);
  lcd.print(humi);

  lcd.setCursor(9,0);
  lcd.print(temp);
  dsm501A_read();

  delay(1000);
}

void initDsm501A(){
  pinMode(pin,INPUT);
  starttime=millis();
}

void dsm501A_read(){
  duration = pulseIn(pin,LOW);
  lowpulseoccupancy+=duration;
  endtime=millis();

  if((endtime-starttime)>=sampletime_ms){
    ratio=lowpulseoccupancy/(sampletime_ms*10.0);
    concentration = 1.1*pow(ratio,3)-3.8*pow(ratio,2)+520*ratio+0.62;
    pcsPerCF=concentration*100;
    ugm3+=pcsPerCF/1300;

    lowpulseoccupancy=0;
    starttime=millis();
  }
  Serial.println(ugm3);
  String dust="Dust : ";
  dust+=(String)ugm3;

  lcd.setCursor(0,1);
  lcd.print(dust);
}
