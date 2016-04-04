/*
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Description:
Pot sets required temperature on 7-segment led display.
Temperature probe checks temperature. 
Relay switches heater coil off when temperature is reached. 
There is a three second delay before the process repeats.
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Pins:
Aref  connect to 5v
A0    temperature set pot wiper - the other pins (1&3) go to Gnd & 5v respectively
D2    LED display CLK
D3    LED Digital i/o (DIO)     (Digital i/o)

D8    LED Still heating indicator  (use red 220r)
D9    LED temperature probe error indicator (use blue 220r)

D10   Temperature probe CS pin  (chip select)
D11   Relay activate pin
D12   Temperature probe SO pin  (slave out)
D13   Temperature probe SCK pin (serial clock)
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
*/
#include <Arduino.h>
#include <MAX6675.h>
#include <TM1637Display.h>

#define CLK 2
#define DIO 3

TM1637Display display(CLK, DIO);
uint8_t data[] = { 0xff, 0xff, 0xff, 0xff };

int temppot = 0;                // temp value pot pin
int relay = 11;                 // relay pin
int temperature_set = 0;        // value from 100 to 250 degrees C
int LED1 = 8;                   // Still heating LED Pin (use red)
int LED2 = 9;                   // Probe error LED Pin (use blue)
int CS = 10;                    // CS pin on MAX6675
int SO = 12;                    // SO pin of MAX6675
//int SCK = 13;                 // SCK pin of MAX6675 (already defined in the library)
int units = 1;                  // Units to readout temp (0 = raw, 1 = C, 2 = F)
float temperature = 0.0;        // Temperature output variable
MAX6675 temp(CS,SO,SCK,units);  // Initialize the MAX6675 Library for our chip
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void setup() {
  Serial.begin(9600);
  pinMode(LED1, OUTPUT);
  pinMode(relay, OUTPUT);
  digitalWrite(relay, LOW);
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void loop() {
  SetTemperature();                                       // Set the required temperature via the pot
  display.setBrightness(0x0f);                            // Need to set the brightness before we can display the reading
  display.showNumberDec(temperature_set, false,4,0);      // Show temperature reading on 4 digit 7 segment LED display
  ReadTemperature();                                      // Read temperature probe
  if (temperature <= temperature_set){                    // Switch the relay on while the temperature probe reads less than the set temperature
    digitalWrite(relay, HIGH);                            // Switch the heater relay on
    digitalWrite(LED2, HIGH);                             // Switch 'heating' led on
  }else{
    digitalWrite(relay, LOW);                             // The temperature is above the set value so switch the heater relay off
    digitalWrite(LED2, LOW);                              // Switch 'heating' led off
  } 
  delay(3000);                                            // Wait three second before reading again	                                 
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void SetTemperature(){
// sets the temperature via the pot  
  int mapTemp;
  temperature_set = analogRead(temppot);
  mapTemp = map(temperature_set, 0, 1023, 120, 210);      // 160 degrees C is about the right temperature for bending acrylic
  temperature_set = mapTemp;
  Serial.print("Temperature_set: "); Serial.println(temperature_set);
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ReadTemperature(){
// Read the temperature from the MAX6675 probe
  temperature = temp.read_temp();
  if(temperature < 0) {                                   // If there is an error with the TC, temperature will be < 0              
    digitalWrite(LED2, HIGH);                             // The 'temperature probe has got a problem' led on  
    Serial.print("Thermocouple Error on CS: "); Serial.println( temperature );
  } else {
    digitalWrite(LED2, LOW);                              // The 'temperature probe has got a problem' led off, so normal operation
    Serial.print("Current Temperature: "); Serial.println( temperature );
  }
}
