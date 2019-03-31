/**
 * Acquire a GPS position and display it on Serial
 */

#include <SIM808.h>
#include <ArduinoLog.h>
#include <SoftwareSerial.h>

#define SIM_RST		5	///< SIM808 RESET
#define SIM_RX		6	///< SIM808 RXD
#define SIM_TX		7	///< SIM808 TXD
#define SIM_PWR		9	///< SIM808 PWRKEY
#define SIM_STATUS	8	///< SIM808 STATUS

#define SIM808_BAUDRATE 4800    ///< Control the baudrate use to communicate with the SIM808 module
#define SERIAL_BAUDRATE 38400   ///< Controls the serial baudrate between the arduino and the computer
#define NO_FIX_GPS_DELAY 3000   ///< Delay between each GPS read when no fix is acquired
#define FIX_GPS_DELAY  10000    ///< Delay between each GPS read when a fix is acquired

#define POSITION_SIZE   128     ///< Size of the position buffer
#define NL  "\n"

SoftwareSerial simSerial = SoftwareSerial(SIM_TX, SIM_RX);
SIM808 sim808 = SIM808(SIM_RST, SIM_PWR, SIM_STATUS);
char position[POSITION_SIZE];

void setup() {
    Serial.begin(SERIAL_BAUDRATE);
    Log.begin(LOG_LEVEL_NOTICE, &Serial);

    simSerial.begin(SIM808_BAUDRATE);
    sim808.begin(simSerial);

    Log.notice(F("Powering on SIM808..." NL));
    sim808.powerOnOff(true);
    sim808.init();

    Log.notice(F("Powering on SIM808's GPS..." NL));
	sim808.powerOnOffGps(true);
}

void loop() {
    SIM808_GPS_STATUS status = sim808.getGpsStatus(position, POSITION_SIZE);
    
    if(status < SIM808_GPS_STATUS::FIX) {
        Log.notice(F("No fix yet..." NL));
        delay(NO_FIX_GPS_DELAY);
        return;
    }

    uint16_t sattelites;
    float lat, lon;
    __FlashStringHelper * state;

    if(status == SIM808_GPS_STATUS::FIX) state = S_F("Normal");
    else state = S_F("Accurate");

    sim808.getGpsField(position, SIM808_GPS_FIELD::GNSS_USED, &sattelites);
    sim808.getGpsField(position, SIM808_GPS_FIELD::LATITUDE, &lat);
    sim808.getGpsField(position, SIM808_GPS_FIELD::LONGITUDE, &lon);

    Log.notice(F("%s" NL), position);
    Log.notice(F("Fix type: %S" NL), state);
    Log.notice(F("Sattelites used : %d" NL), sattelites);
    Log.notice(F("Latitude : %F" NL), lat);
    Log.notice(F("Longitude : %F" NL), lon);
    
    delay(FIX_GPS_DELAY);
}