#include <OneWire.h>
#include <DallasTemperature.h>
#include <PZEM004Tv30.h>

// Data wire is plugged into digital pin 2 on the Arduino
#define ONE_WIRE_BUS 2
#define flow_sensor 3
#define LED_G 13
#define LED_R 12 
PZEM004Tv30 pzem(4, 5);

// Setup a oneWire instance to communicate with any OneWire device
OneWire oneWire(ONE_WIRE_BUS);

// Pass oneWire reference to DallasTemperature library
DallasTemperature sensors(&oneWire);

int deviceCount = 0;
float tempC;

void setup(void) {
  sensors.begin();  // Start up the library
  Serial.begin(115200);

  // locate devices on the bus
  pinMode(LED_G, OUTPUT);
  pinMode(LED_R, OUTPUT);
  pinMode(flow_sensor, INPUT);
  Serial.print("Locating devices...");
  Serial.print("Found ");
  deviceCount = sensors.getDeviceCount();
  Serial.print(deviceCount, DEC);
  Serial.println(" devices.");
  Serial.println("");
}

void loop(void) {
  float voltage = pzem.voltage();
  // Send command to all the sensors for temperature conversion
  sensors.requestTemperatures();
  uint32_t pulse = pulseIn(flow_sensor,HIGH);
  if(pulse < 1){
    digitalWrite(LED_R, HIGH);
    digitalWrite(LED_G, LOW);
  }

  else{
    float Hz = 1/(2*pulse*pow(10,-6));
    float flow = 7.2725*(float)Hz + 3.2094;
    Serial.print(Hz);
    Serial.print("Hz\t");
    Serial.print(flow/60);
    Serial.println(" L/minute");
    digitalWrite(LED_R, LOW);
    digitalWrite(LED_G, HIGH);
  }

  

  // Display temperature from each sensor
  for (int i = 0; i < deviceCount; i++) {
    Serial.print("Sensor ");
    Serial.print(i + 1);
    Serial.print(" : ");
    tempC = sensors.getTempCByIndex(i);
    Serial.print(tempC);
    Serial.print(" ํC | ");
  }

  Serial.println("");
  delay(500);
}