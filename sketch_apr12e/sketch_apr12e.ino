#include "Magellan_SIM7020E.h"

#include <OneWire.h>
#include <DallasTemperature.h>
#include <PZEM004Tv30.h>
#include <GPS.h>
#include "UUID.h"
// #include <HardwareSerial.h>
// #include <SoftwareSerial.h>
#define ONE_WIRE_BUS 23  //กำหนดขาที่จะเชื่อมต่อ Sensor
#define flow_sensor 22
#if defined(ESP32)
PZEM004Tv30 pzem(Serial2, 21, 19);
#else
PZEM004Tv30 pzem(Serial2);
#endif
String payload;
String payload1;
String payload2;
String payload3;
String payload4;
String payload5;
// SoftwareSerial pzemSWSerial(21, 19);
Magellan_SIM7020E magel;
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

int deviceCount = 0;
UUID uuid;
String UUID = "";
String uniqueID = "deviotest-001";
float tempC;
String waterflow = "0";
String waterpressure = "0";
String highpressuretemp= "0";
String lowpressuretemp= "0";
String watertemp= "0";
String airflowtemp= "0";
String coolertemp= "0";
String currentpower= "0";
String currentamp= "0";
String currentvolt= "0";
String currentfrequency= "0";
String currentenergy= "0";
String currentPF= "0";


void setup(void) {
  Serial.begin(19200);
  magel.begin();
  sensors.begin();
  Serial.println(uuid);
  pinMode(flow_sensor, INPUT);
  Serial.print("Locating devices...");
  Serial.print("Found ");
  deviceCount = sensors.getDeviceCount();
  Serial.print(deviceCount, DEC);
  Serial.println(" devices.");
  Serial.println("");
}

void loop(void) {
  temp_get();
  Serial.println("");
  water_flow();
  pzem_get();
  Serial.println("");
  
  delay(5000);
  request_data();
  
}

void temp_get() {
  sensors.requestTemperatures();
  // Display temperature from each sensor
  for (int i = 0; i < deviceCount; i++) {
    Serial.print("Sensor ");
    Serial.print(i + 1);
    Serial.print(" : ");
    tempC = sensors.getTempCByIndex(i);
    Serial.print(tempC);
    Serial.print(" C | ");
    if (i == 0) {
      highpressuretemp = tempC;
    } else if (i == 1) {
      lowpressuretemp = String(tempC);
    } else if (i == 2) {
      watertemp = String(tempC);
    } else if (i == 3) {
      airflowtemp = String(tempC);
    } else if (i == 4) {
      coolertemp = String(tempC);
    }
  }
}

void water_flow() {
  uint32_t pulse = pulseIn(flow_sensor, HIGH);
  if (pulse < 1) {
    Serial.println("0.00 L/minute");

  }

  else {

    float Hz = 1 / (2 * pulse * pow(10, -6));
    float flow = 7.2725 * (float)Hz + 3.2094;
    float PSI = 0.5 * (flow / 60 * flow / 60) / 100;
    waterflow = String(flow);
    waterpressure = String(PSI);
    // Serial.print(Hz);
    // Serial.print("Hz\t");
    Serial.print(flow / 60);
    Serial.println(" L/minute");
    Serial.println(" ");
    Serial.print(PSI);
    Serial.println(" PSI");

    // water = String(flow / 60);
  }
}

void pzem_get() {


  // Read the data from the sensor
  float voltage = pzem.voltage();
  float current = pzem.current();
  float power = pzem.power();
  float energy = pzem.energy();
  float frequency = pzem.frequency();
  float pf = pzem.pf();

  // Check if the data is valid
  if (isnan(voltage)) {
    Serial.println("Error reading voltage");
  } else if (isnan(current)) {
    Serial.println("Error reading current");
  } else if (isnan(power)) {
    Serial.println("Error reading power");
  } else if (isnan(energy)) {
    Serial.println("Error reading energy");
  } else if (isnan(frequency)) {
    Serial.println("Error reading frequency");
  } else if (isnan(pf)) {
    Serial.println("Error reading power factor");
  } else {
    // float currentpower = 0.00;
    // float currentamp = 0.00;
    // float currentvolt = 0.00;
    // float currentfrequency = 0.00;
    // float currentenergy = 0.00;
    // float currentPF = 0.00;

    // Print the values to the Serial console
    currentpower = String(power);
    currentamp = String(current);
    currentvolt = String(voltage);
    currentfrequency = String(frequency);
    currentenergy = String((energy, 3));
    currentPF = String(pf);
    Serial.print("Voltage: ");
    Serial.print(voltage);
    Serial.println("V");
    Serial.print("Current: ");
    Serial.print(current);
    Serial.println("A");
    Serial.print("Power: ");
    Serial.print(power);
    Serial.println("W");
    Serial.print("Energy: ");
    Serial.print(energy, 3);
    Serial.println("kWh");
    Serial.print("Frequency: ");
    Serial.print(frequency, 1);
    Serial.println("Hz");
    Serial.print("PF: ");
    Serial.println(pf);
  }
}

void request_data() {

  // UUID uuid;
  // String UUID = "";
  // String uniqueID = "deviotest-001";
  // float tempC;
  // float waterflow = 0.00;
  // float waterpressure = 0.00;
  // float highpressuretemp = 0.00;
  // float lowpressuretemp = 0.00;
  // float watertemp = 0.00;
  // float airflowtemp = 0.00;
  // float coolertemp = 0.00;
  // float currentpower = 0.00;
  // float currentamp = 0.00;
  // float currentvolt = 0.00;
  // float currentfrequency = 0.00;
  // float currentenergy = 0.00;
  // float currentPF = 0.00;
  payload = "{\"highpressure\":"+highpressuretemp+",\"lowpressure\":"+lowpressuretemp+",\"airflow\":"+airflowtemp+",\"cooler\":"+coolertemp+",\"currentpower\":"+currentpower+",\"currentamp\":"+currentamp+",\"currentvolt\":"+ currentvolt+",\"currentfrequency\":"+currentfrequency+",\"currentenergy\":"+currentenergy+",\"currentPF\":"+currentPF+",\"waterflow\":"+waterflow+",\"waterpressure\":"+waterpressure+",\"watertemp\":"+watertemp+"}";
  // payload1="{\"highpressure\":"+highpressuretemp+",\"lowpressure\":"+lowpressuretemp+",\"airflow\":"+airflowtemp+"}";
  // magel.report(payload1);
  // delay(1000);   
  // payload2="{\"cooler\":"+coolertemp+",\"currentpower\":"+currentpower+",\"currentamp\":"+currentamp+"}";
  // magel.report(payload2);
  // delay(1000);   
  // payload3="{\"currentvolt\":"+currentvolt+",\"currentfrequency\":"+currentfrequency+",\"currentenergy\":"+currentenergy+"}";
  // magel.report(payload3);
  // delay(1000); 
  // payload4="{\"currentPF\":"+currentPF+",\"waterflow\":"+waterflow+",\"waterpressure\":"+waterpressure+"}";
  // magel.report(payload4);
  // delay(1000); 
  // payload5="{\"watertemp\":"+watertemp+"}";
  // magel.report(payload5);  //Report sensor data
  magel.report(payload);
  delay(5000);  
}
