//In vitro cercaria detection model
//Read temperature and conditionally provide voltage to heating element via relay
//__________________________________________________
//Libraries (included in Arduino IDE)

//Standard library
#include <stdlib.h>

//SD card library
#include <SD.h>

//__________________________________________________
//Definitions

//Reference voltage
#define aref_voltage 3.3

//Number of readings
int NumReadings = 20;

//Temperature array for holding data (placeholder arrays of 20 elements)
int AllReadings[21];

//All average temperature readings (placeholder array)
float AllAvg[6];

//Delay between average readings (milliseconds)
int AverageDelay = 20;

//Delay between individual readings (milliseconds)
int IndividualDelay = 10;

//Data logger file
File LogFile;

//Device name
char DeviceName;

//Time reading (milliseconds since program start)
unsigned long Time;

char filename[] = "logger00.csv";

//__________________________________________________
//Pin definitions

//SD card library requires digital pins 10, 11, 12, 13
//SD detector pin
int SdPresencePin = 10;

//Analog pins for temperature readings
//These definitions are not used in the code
//TempPinA = A0;
//TempPinB = A1;
//TempPinC = A2;
//TempPinD = A3;
//WaterTempPin = A4;

//Digital pins for heater control
int HeaterA = 9;
int HeaterB = 7;
int HeaterC = 5;
int HeaterD = 3;

//Digital pins for LEDs
int ledA = 8;
int ledB = 6;
int ledC = 4;
int ledD = 2;

//__________________________________________________
//Setup and initialization

void setup()
{

  //Set led pins to output
  pinMode(ledA, OUTPUT);
  pinMode(ledB, OUTPUT);
  pinMode(ledC, OUTPUT);
  pinMode(ledD, OUTPUT);

  //Set led pins to on or off
  //**
  //**
  //change to HIGH (on) or LOW (off)
  digitalWrite(ledA, HIGH);
  digitalWrite(ledB, HIGH);
  digitalWrite(ledC, HIGH);
  digitalWrite(ledD, HIGH);
  //**
  //**
  
  //Set heater control pins to output
  pinMode(HeaterA, OUTPUT);
  pinMode(HeaterB, OUTPUT);
  pinMode(HeaterC, OUTPUT);
  pinMode(HeaterD, OUTPUT);
  
  //Begin serial communication though USB (default 9600 baud)
  Serial.begin(9600);
  
  //Set reference voltage to 3.3V instead of 5V
  analogReference(EXTERNAL);
  
  //SD card initialization
  Serial.println("Initializing SD card...");
  pinMode(SdPresencePin, OUTPUT);
  
  //SD card detection
  if (!SD.begin(SdPresencePin)){
    Serial.println("No SD Card");
    delay(100);
  }
  
  //Print confirmation
  Serial.println("Ready.");
  
  //Create new log file
  //char filename[] = "logger00.csv";
  
  //Loop to create multiple new files
  for (uint8_t i = 0; i < 100; i++){
    filename[6] = i/10 + '0';
    filename[7] = i%10 + '0';
    
    //Only open a new file if it does not exist
    if (!SD.exists(filename)){
      LogFile = SD.open(filename, FILE_WRITE);
      break;
    }
  }
  if (!LogFile){
    Serial.println("Could not create file.");
  }
  
  Serial.print("Logging to: ");
  Serial.println(filename);
  
  //Create new header in file
  if (LogFile){
    String header = "Device,Millis,Analog,Voltage,Temp";
    LogFile.println(header);
  }
  LogFile.close();
  //Delay before main loop (milliseconds)
  delay(100);
  
}

//__________________________________________________
//Main Loop

void loop()
{
  //Loop that takes 20 readings at a delay of 10ms, and inserts them into an array for each device + surrounding water to construct a matrix of temperature readings
  LogFile= SD.open(filename,FILE_WRITE);
  
  //Loop over each analog pin
  for (int thisPin = 0; thisPin < 4; thisPin++){
    
    //Switch to determine which device is reading
    switch (thisPin){
      case 0:
        DeviceName = 'a';
        break;
      case 1:
        DeviceName = 'b';
        break;
      case 2:
        DeviceName = 'c';
        break;
      case 3:
        DeviceName = 'd';
        break;
    }
    
    //Time of reading
    Time = millis();
    
    //Print to display and write to SD
    Serial.print(DeviceName);
    Serial.print(",");
    Serial.print(Time);
    Serial.print(",");
    
    LogFile.print(DeviceName);
    LogFile.print(",");
    LogFile.print(Time);
    LogFile.print(",");
    
    //Loop over each element in the array and take a reading
    for (int reading = 0; reading < NumReadings; reading ++){
      AllReadings[reading] = analogRead(thisPin);
      delay(IndividualDelay);
    }
    
    //Throw out first two to account for current fluctuations during switching and average the readings
    for (int place = 2; place < NumReadings; place++){
      AllAvg[thisPin] += AllReadings[place];
    }
    AllAvg[thisPin] /= NumReadings-2;
    
    //Print to display and write to SD
    Serial.print(AllAvg[thisPin]);
    Serial.print(",");
    LogFile.print(AllAvg[thisPin]);
    LogFile.print(",");

    //Calculate voltage from analog reading
    AllAvg[thisPin] = AllAvg[thisPin] * aref_voltage;
    AllAvg[thisPin] /= 1023.0;
    
    //Print to display and write to SD
    Serial.print(AllAvg[thisPin]);
    Serial.print(",");
    LogFile.print(AllAvg[thisPin]);
    LogFile.print(",");
    
    //Calculate temperature in Celsius
    AllAvg[thisPin] = (AllAvg[thisPin]-0.5)*100;
    
    //Print to display and write to SD
    Serial.println(AllAvg[thisPin]);
    LogFile.println(AllAvg[thisPin]);
    
    //Heater control based on temperature !!replace LOW and HIGH temps!!
    switch (thisPin) {
      case 0:
        //Turn heater on is temp chamber is < LOW Celsius
        if (AllAvg[thisPin] < 39.5){
          digitalWrite(HeaterA, HIGH);
        } else
        //Turn heater off if temp chamber is > HIGH Celsius
        if (AllAvg[thisPin] > 40.5){
          digitalWrite(HeaterA, LOW);
          break;
        }
      case 1:
        if (AllAvg[thisPin] < 39.5){
          digitalWrite(HeaterB, HIGH);
          } else
        if (AllAvg[thisPin] > 40.5){
          digitalWrite(HeaterB, LOW);
          break;
        }
      case 2:
        if (AllAvg[thisPin] < 39.5){
          digitalWrite(HeaterC, HIGH);
          } else
        if (AllAvg[thisPin] > 40.5){
          digitalWrite(HeaterC, LOW);
          break;
        }       
      case 3:
        if (AllAvg[thisPin] < 39.5){
          digitalWrite(HeaterD, HIGH);
          } else
        if (AllAvg[thisPin] > 40.5){
          digitalWrite(HeaterD, LOW);
          break;
        }
    }
    delay(1000);
  
  }
  LogFile.close();
}

