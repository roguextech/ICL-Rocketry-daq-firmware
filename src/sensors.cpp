#include "sensors.h"
#include <SD.h>
#include <SPI.h>

SPIClass vspi(VSPI);

Adafruit_MAX31855 thermocouple1(TC_CS1, &vspi);
Adafruit_MAX31855 thermocouple2(TC_CS2, &vspi);
Adafruit_MAX31855 thermocouple3(TC_CS3, &vspi);
Adafruit_MAX31855 thermocouple4(TC_CS4, &vspi);

struct sensors {
    double thermo1ambient;
    double thermo1celsius;
    double thermo2celsius;
    double thermo3celsius;
    double thermo4celsius;
    double ptap1;
    double ptap2;
    double ptap3;
    double ptap4;
    double ptap5;
};

sensors sensorData;

File dataFile;

void setupSensors() {
    vspi.setClockDivider(SPI_CLOCK_DIV8);
    vspi.begin();

    thermocouple1.begin();
    thermocouple2.begin();
    thermocouple3.begin();
    thermocouple4.begin();
    pinMode(TC_CS1, OUTPUT);
    pinMode(TC_CS2, OUTPUT);
    pinMode(TC_CS3, OUTPUT);
    pinMode(TC_CS4, OUTPUT);
    pinMode(ADC_CS, OUTPUT);
    digitalWrite(TC_CS1, HIGH);
    digitalWrite(TC_CS2, HIGH);
    digitalWrite(TC_CS3, HIGH);
    digitalWrite(TC_CS4, HIGH);
    digitalWrite(ADC_CS, HIGH);

}

void setupSD(){
    Serial.begin(9600);
    Serial.print("Initializing SD card...");
    pinMode(13, OUTPUT);

    if (!SD.begin()) {
        Serial.println("Card failed, or not present");
        return;
    }
    Serial.println("Card initialized.");
    dataFile = SD.open("sensors_data.txt", FILE_WRITE);
}

double readThermocoupleInternal(){
    return thermocouple1.readInternal();
}

double readThermocoupleCelsius(uint8_t i){
    switch(i){
        case 1:
        return thermocouple1.readCelsius();
        break;

        case 2:
        return thermocouple2.readCelsius();
        break;

        case 3:
        return thermocouple3.readCelsius();
        break;

        case 4:
        return thermocouple4.readCelsius();
        break;

        default:
        return -1;
    }
}

double readPtap(uint16_t i){
    switch(i){
        case 1:
        return analogRead(PTAP1);
        break;

        case 2:
        return analogRead(PTAP2);
        break;

        case 3:
        return analogRead(PTAP3);
        break;

        case 4:
        return analogRead(PTAP4);
        break;

        case 5:
        return analogRead(PTAP5);
        break;

        default:
        return -1;
    }
}

void dataLoop() {
    sensorData.thermo1ambient = thermocouple1.readInternal();    
    sensorData.thermo1celsius = thermocouple1.readCelsius();
    sensorData.thermo2celsius = thermocouple2.readCelsius();
    sensorData.thermo3celsius = thermocouple3.readCelsius();
    sensorData.thermo4celsius = thermocouple4.readCelsius();
    sensorData.ptap1 = analogRead(PTAP1);
    sensorData.ptap2 = analogRead(PTAP2);
    sensorData.ptap3 = analogRead(PTAP3);
    sensorData.ptap4 = analogRead(PTAP4);
    sensorData.ptap5 = analogRead(PTAP5);   
    dataFile.write((const uint8_t *)&sensorData, sizeof(sensorData));
}

