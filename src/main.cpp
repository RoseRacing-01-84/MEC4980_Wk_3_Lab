#include <Arduino.h>
#include <stdint.h>
#include <SparkFun_Qwiic_OLED.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <cstring>

#define MICRO
#define SEALEVELPRESSURE_HPA (1013.25)

Adafruit_BME280 bme;

QwiicMicroOLED myOLED;
const char * deviceName = "Micro OLED";

int yoffset;
float targetTemperature = 20.0;
float targetTF = 68.0;
char degreeSystem[]="C";

int pinButton = 10;
int pinButton1 = 11;
int pinButton2 = 12;
int LEDHeater = 5;
bool prevpress = false;
bool prev1 = false;
bool prev2 = false;

enum MachineStates{
  DisplayTemps, // 0
  SetTemp,      // 1
  ChooseSystem  // 2
};

char sysC[] = "C";
char sysF[] = "F";

MachineStates currentState;

void setup()
{
  currentState = DisplayTemps;
  pinMode(pinButton, INPUT_PULLDOWN);
  pinMode(pinButton1, INPUT_PULLDOWN);
  pinMode(pinButton2, INPUT_PULLDOWN);
  pinMode(LEDHeater, OUTPUT);
  Serial.begin(9600);
  delay(3000);
  Serial.println("Testing BME Sensor");
  if (! bme.begin(0x77, &Wire)) {
        Serial.println("Could not find a valid BME280 sensor, check wiring!");
        while (1);
    }

    delay(500);   //Give display time to power on
    
    Serial.println("\n\r-----------------------------------");

    Serial.print("Running Test #5 on: ");
    Serial.println(String(deviceName));

    if(!myOLED.begin()){

        Serial.println("- Device Begin Failed");
        while(1);
    }

    yoffset = (myOLED.getHeight() - myOLED.getFont()->height)/2;

    delay(1000);
}

/*typedef void (*testFn)(void);
typedef struct _testRoutines{
    void (*testFn)(void);
    const char *title;
}testRoutine;*/


float ctoF(float degC){
  return degC * 1.8 + 32.0;
}

void loop()
{

  float temp = bme.readTemperature();
  float tempF = ctoF(temp);

  if(digitalRead(pinButton) == HIGH && !prevpress) {
    currentState = MachineStates(((int)currentState + 1) % 3);
  }
    
  prevpress = digitalRead(pinButton);

  char myNewText [50];
  if(currentState == DisplayTemps){

    
    if(strcmp(degreeSystem, sysC) == 0){
        myOLED.erase();
        myOLED.text(3, yoffset, myNewText);

        sprintf(myNewText, "Tt: %.1f C", targetTemperature);
        myOLED.text(3, yoffset + 13, myNewText);
        myOLED.display();
        sprintf(myNewText, "Tc: %.1f C", temp);
      }
      else{
        myOLED.erase();
        myOLED.text(3, yoffset, myNewText);

        sprintf(myNewText, "Tt: %.1f F", targetTF);
        myOLED.text(3, yoffset + 13, myNewText);
        myOLED.display();
        sprintf(myNewText, "Tc: %.1f F", tempF);
      }
    

 prev1 = digitalRead(pinButton1);
 prev2 = digitalRead(pinButton2);
    
  } else if(currentState == SetTemp){
    
    if(digitalRead(pinButton1) == HIGH && !prev1){
      targetTemperature--;
      targetTF = ctoF(targetTemperature);
    }
    else if(digitalRead(pinButton2) == HIGH && !prev2){
      targetTemperature++;
      targetTF = ctoF(targetTemperature);
    }
        
    prev1 = digitalRead(pinButton1);
    prev2 = digitalRead(pinButton2);
    
    if(strcmp(degreeSystem, sysC) == 0){
        sprintf(myNewText, "Tt: %.1f C", targetTemperature);
        myOLED.erase();
        myOLED.text(3, yoffset , myNewText);
        myOLED.display();
      }
      else{
        sprintf(myNewText, "Tt: %.1f F", targetTF);
        myOLED.erase();
        myOLED.text(3, yoffset , myNewText);
        myOLED.display();
      }
    
    
  } else if(currentState == ChooseSystem){
    sprintf(myNewText, "System: %s", degreeSystem);
    myOLED.erase();
    myOLED.text(3, yoffset, myNewText);
    myOLED.display();
    if((digitalRead(pinButton1) == HIGH && !prev1)||(digitalRead(pinButton2) == HIGH && !prev2)){
      if(strcmp(degreeSystem, sysC) == 0){
        strcpy(degreeSystem, sysF);
      }
      else{
        strcpy(degreeSystem, sysC);
      }
    }
    
  prev1 = digitalRead(pinButton1);
  prev2 = digitalRead(pinButton2);
  }

  if(temp < targetTemperature){
    digitalWrite(LEDHeater, HIGH);
  } else{
    digitalWrite(LEDHeater, LOW);
  }

}