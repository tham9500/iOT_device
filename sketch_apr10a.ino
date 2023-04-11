#include <OneWire.h>
#include <DallasTemperature.h>
#include <PZEM004Tv30.h>
#include "UUID.h"
#include <ArduinoJson.h>
#include <ArduinoUniqueID.h>

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
String tempsensor1;
String tempsensor2;
String tempsensor3;
String tempsensor4;
String tempsensor5;
String watt;
String volt;
String amp;
String ener;
String frequen;
String powerfactor;
String uu_id;
String water;
StaticJsonDocument<500> data_req;

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
  data_req.clear();
  JsonObject doc = data_req.createNestedObject();
  Serial.print("UUID: ");
  Serial.print(uuid);
  water_flow();
  Serial.println("");
  temp_get();
  Serial.println("");
  pzem_get();
  Serial.println("");
  doc["uuid"] = uuid;
  doc["sensor1"] = tempsensor1;
  doc["sensor2"] = tempsensor2;
  doc["sensor3"] = tempsensor3;
  doc["sensor4"] = tempsensor4;
  doc["sensor5"] = tempsensor5;
  doc["voltage"] = volt == " NAN" ? "0" : volt;
  doc["current"] = amp== " NAN" ? "0" : amp;
  doc["power"] = watt == " NAN" ? "0" : watt;
  doc["energy"] = ener == " NAN" ? "0" : ener;
  doc["frequency"] = frequen == " NAN" ? "0" : frequen;
  doc["pf"] = powerfactor == " NAN" ? "0" : powerfactor;
  doc["water"] = water;
  serializeJson(data_req, Serial);
  Serial.println("");
  delay(36000);
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
    if (i == 0)
    {
      tempsensor1 = tempC;
    }
    else if (i == 1)
    {
      tempsensor2 = tempC;
    }
    else if (i == 2)
    {
      tempsensor3 = tempC;
    }
    else if (i == 3)
    {
      tempsensor4 = tempC;
    }
    else if (i == 4)
    {
      tempsensor5 = tempC;
    }
  }
}

void water_flow()
{
  uint32_t pulse = pulseIn(flow_sensor, HIGH);
  if (pulse < 1)
  {
    digitalWrite(LED_R, HIGH);
    digitalWrite(LED_G, LOW);
    water ="0";
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
    water = String(flow / 60);
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
    volt = String(voltage);
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
    amp = String(current);
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
    watt = String(power);
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
    ener = String(energy);
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
    frequen = String(frequency);
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
    powerfactor = String(pf);
  }
  else
  {
    Serial.println("Error reading power factor");
  }

  Serial.println();
}
