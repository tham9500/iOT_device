#include <OneWire.h>
#include <DallasTemperature.h>
#include <PZEM004Tv30.h>
#include "UUID.h"

#define ONE_WIRE_BUS 2  // Define Pin 2 Data tempsensor
#define flow_sensor 3   // Define Pin 3 Data water flow
#define LED_G 13        // Define Pin 13 Status Water have flow
#define LED_R 12        // Define Pin 12 Status Water Stop flow
PZEM004Tv30 pzem(4, 5); // Define TK RK input Code data Pzem Power meter
UUID uuid;

// Setup a oneWire instance to communicate with any OneWire device
OneWire oneWire(ONE_WIRE_BUS);

// Pass oneWire reference to DallasTemperature library
DallasTemperature sensors(&oneWire);

int deviceCount = 0;
float tempC;

void setup(void)
{
  sensors.begin(); // Start up the library
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

void loop(void)
{
  Serial.print("UUID: ");
  Serial.print(uuid);
  water_flow();
  Serial.println("");
  temp_get();
  Serial.println("");
  pzem_get();
  Serial.println("");
  delay(1000);
}

void temp_get()
{
  sensors.requestTemperatures();
  // Display temperature from each sensor
  for (int i = 0; i < deviceCount; i++)
  {
    Serial.print("Sensor ");
    Serial.print(i + 1);
    Serial.print(" : ");
    tempC = sensors.getTempCByIndex(i);
    Serial.print(tempC);
    Serial.print(" ํC | ");
  }
}

void water_flow()
{
  uint32_t pulse = pulseIn(flow_sensor, HIGH);
  if (pulse < 1)
  {
    digitalWrite(LED_R, HIGH);
    digitalWrite(LED_G, LOW);
  }

  else
  {
    float Hz = 1 / (2 * pulse * pow(10, -6));
    float flow = 7.2725 * (float)Hz + 3.2094;
    float PSI = 0.5 * (flow / 60 * flow / 60) / 100;
    Serial.print(Hz);
    Serial.print("Hz\t");
    Serial.print(flow / 60);
    Serial.println(" L/minute");
    Serial.println(" ");
    Serial.print(PSI);
    Serial.println(" PSI");
    digitalWrite(LED_R, LOW);
    digitalWrite(LED_G, HIGH);
  }
}

void pzem_get()
{
  float voltage = pzem.voltage();
  if (voltage != NAN)
  {
    Serial.print("Voltage: ");
    Serial.print(voltage);
    Serial.println("V");
  }
  else
  {
    Serial.println("Error reading voltage");
  }

  float current = pzem.current();
  if (current != NAN)
  {
    Serial.print("Current: ");
    Serial.print(current);
    Serial.println("A");
  }
  else
  {
    Serial.println("Error reading current");
  }

  float power = pzem.power();
  if (current != NAN)
  {
    Serial.print("Power: ");
    Serial.print(power);
    Serial.println("W");
  }
  else
  {
    Serial.println("Error reading power");
  }

  float energy = pzem.energy();
  if (current != NAN)
  {
    Serial.print("Energy: ");
    Serial.print(energy, 3);
    Serial.println("kWh");
  }
  else
  {
    Serial.println("Error reading energy");
  }

  float frequency = pzem.frequency();
  if (current != NAN)
  {
    Serial.print("Frequency: ");
    Serial.print(frequency, 1);
    Serial.println("Hz");
  }
  else
  {
    Serial.println("Error reading frequency");
  }

  float pf = pzem.pf();
  if (current != NAN)
  {
    Serial.print("PF: ");
    Serial.println(pf);
  }
  else
  {
    Serial.println("Error reading power factor");
  }

  Serial.println();
}
