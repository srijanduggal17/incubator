#include <OneWire.h> 
#include <DallasTemperature.h>

// Data wire is plugged into digital pin 2 on the Arduino

#define ONE_WIRE_BUS 2

// Setup a oneWire instance to communicate with any OneWire device

OneWire oneWire(ONE_WIRE_BUS);     
// Pass oneWire reference to DallasTemperature library

DallasTemperature sensors(&oneWire);

float set_point = 98.6; //F

int tempDeviceCount = 0;

float tempF; 
bool wasHigh = false;

float current_readings[4];

int current_pins[4] = {A5, A6, A10, A3};

float initial_current[4];

float temp_readings[14];

int time_step = 2 ;//seconds

unsigned long timer = 0;
 
float current_reading;

int current_raw_reading;

bool digital_heat_on;
bool analog_heat_on;

float analog_temp_reading;

float set_point_pot;

float avg_bed_temp = 0;

void setup(){ 
  
  sensors.begin();             // Start up the library

  Serial.begin(9600);
  pinMode(5, OUTPUT);
  pinMode(7, INPUT);
  pinMode(A14, INPUT);
  pinMode(A13, INPUT);
  digitalWrite(5, HIGH); // start with heating off

  tempDeviceCount = sensors.getDeviceCount();

  digital_heat_on = false;

  Serial.print("Time, Digital Heat, Analog Heat, Set Point, Analog Temp, C1, C2, C3, C4, D1, D2, D3, D4, D5, D6, D7, D8, D8, D9, D10, D11, D12, D13, D14\n");

   for (int i = 0; i < 4; i++)

   {
     current_raw_reading = analogRead(current_pins[i]);
     initial_current[i] = ((float) current_raw_reading/1023)*36.7 - 18.3;
   } 
}

void loop() {

  // acquire current reading

  for (int i = 0; i < 4; i++)
  {
  current_raw_reading = analogRead(current_pins[i]);
  current_readings[i] = ((float) current_raw_reading/1023)*36.7 - 18.3 - initial_current[i]; 
  }
  
  // acquire temp readings
  sensors.requestTemperatures();

  for (int i = 0; i < tempDeviceCount; i++)
  {
    temp_readings[i] = DallasTemperature::toFahrenheit(sensors.getTempCByIndex(i));
  }

  analog_temp_reading = DallasTemperature::toFahrenheit((float) analogRead(A14) * 100.0 * 5.0 / 1023.0);

  set_point_pot = DallasTemperature::toFahrenheit((float) analogRead(A13) * 100.0 * 5.0 / 1023.0);
  
  //set_point_pot = (float) 5 * analogRead(A13) / 1023;

  

  // get avg bed heat
  avg_bed_temp = (temp_readings[4] + temp_readings[5] ) / 2;

  // state machine transitions for heating
  if (avg_bed_temp > set_point + 1 && digital_heat_on == true)
  {
    //digitalWrite(5, LOW); // turn off
    digital_heat_on = false;
  } 
  else if (avg_bed_temp < set_point - 1 && digital_heat_on == false)
  {
    //digitalWrite(5, HIGH); // turn on
    digital_heat_on = true;
  }

  analog_heat_on = digitalRead(7);

  // print time
  Serial.print(timer);
  Serial.print(", ");

  // print digital heater status
  Serial.print(digital_heat_on);
  Serial.print(", ");

  // print analog heater status
  Serial.print(!analog_heat_on);
  Serial.print(", ");
  
  Serial.print(set_point_pot);

  // print analog temperature
  Serial.print(", ");
  Serial.print(analog_temp_reading);
  
  // print current readings
  for (int i = 0; i < 4; i++)
  {
    Serial.print(", ");
    Serial.print(current_readings[i]);
  }

  
  // print temp
  for (int i = 0; i < tempDeviceCount; i++)
  {
    Serial.print(", ");
    Serial.print(temp_readings[i]);
  }

  Serial.print(", \n");

  delay(time_step * 1000);

  timer += time_step;


}
