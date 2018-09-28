#define NO                0
#define YES               1
#define USE_GPS           YES
#define USE_CAMERA        YES
#define USE_SER_CMDS      YES

#include "Wire.h"
//#include <LowPower.h>
#include <Timer.h>
#include <EEPROM.h>
#include <Base64.h>

#define BUZ_PIN           19
#define CAM_EN_PIN        15
#define CAM_MUX_PIN       13
#define GPS_EN_PIN        22
#define GPS_MUX_PIN       12
#define LANDER_REL_PIN    23
#define RED_LED           LED_BUILTIN
#define GREEN_LED         1
#define BUTTON_PIN        18

#define BME280SENS        0
#define GYROSCOPE         1
#define RADIO             2
#define SDCARD            3
#define CAMERA            4
#define GPS               5

//Αρχική διεύθυνση χρήσης της EEPROM (μέγεθος 4K ή 0x01000  [0 - 0x0FFF]) 
#define     EEPROM_TOP      0x0000
#define     EEPROM_END      0x03ff

//Περιοχή αποθήκευσης EEPROM
#define     EE_PRESSURE       (EEPROM_TOP + 0)    //4 byte
#define     EE_FILE_ID        (EE_PRESSURE + 4)   //2 byte

void sd_Click(void);

//----- Καθολικές μεταβλητές -----
//Το Max DataLen του FIFO buffer είναι 66-5=61 bytes
char tx_buffer1[61], gps_buffer[60], tx_buffer2[61], img_buffer[61];
float bat_voltage;
unsigned int  pkt_cnt;
byte hw_status = 0, blnk_pat = 1;

Timer timer1;
int timer1_event;
int timer1_int = 1000;
boolean stop_job = false, blink_ack = false, full_power = false;
uint32_t ping_time;
long cur_pressure, start_pressure = -1;
unsigned int filenum;

#include "functions.h"
#include "sdcard.h"
#include "bme280.h"
#include "motion.h"
#include "gps.h"
#include "camera.h"
// Αυτό γίνεται μέσα στο rfm69.h #include "cmd.h"
#include "gyroscope.h"
#include "rfm69.h"
#include "FWB.h"

FWB but_sd;

uint32_t now;

void setup() 
{
 pinMode(RED_LED, OUTPUT);
 pinMode(GREEN_LED, OUTPUT);
 pinMode(BUZ_PIN, OUTPUT);
 pinMode(CAM_EN_PIN, OUTPUT);
 pinMode(CAM_MUX_PIN, OUTPUT);
 pinMode(GPS_EN_PIN, OUTPUT);
 pinMode(GPS_MUX_PIN, OUTPUT);
 pinMode(LANDER_REL_PIN, OUTPUT);
 digitalWrite(CAM_EN_PIN, HIGH);     //PNP High = Disable - Αρχικά απενεργοποίησε κάμερα
 digitalWrite(CAM_MUX_PIN, LOW);    //Diodes AND gate, High = Pass data - Κόψε τα δεδομένα της κάμερας
 digitalWrite(GPS_EN_PIN, HIGH);    //PNP High = Disable - Αρχικά απενεργοποίησε GPS
 digitalWrite(GPS_MUX_PIN, LOW);    //Diodes AND gate, High = Pass data - Κόψε τα δεδομένα του GPS
 digitalWrite(LANDER_REL_PIN, LOW); //NPN Low = Disable
 Serial.begin(115200);
 Serial.print(F("***** Ursa Minor CanSat (c)2018 1o EPAL PREVEZAS V1.0 *****\n"));  //Ένα μήνυμα στο τερματικό
 delay(100);
 analogReference(INTERNAL1V1);  //Τάση αναφοράς για ADC 1,1 Volt

 but_sd.Configure(BUTTON_PIN);
 but_sd.OnClick = sd_Click;
 
 bme280_init();  //Αρχικοποίηση του BME280
 mpu_init();     //Αρχικοποίηση γυροσκοπίου
 radio_init();   //Αρχικοποίηση ασύρματου πομποδέκτη
 //bme280_status(); //Εμάνισε την κατάσταση του bme280
 sd_init();      //Αρχικοποίηση κάρτας SD
 delay(100);
 #if USE_CAMERA
     camera_init();
     delay(100);
     cam_power_off();
 #endif
 #if USE_GPS
     gps_init();     //Αρχικοποίηση επικοινωνίας GPS
     delay(100);
     gps_test();
 #endif
 //disp_sd_file(); //Debug για έλεγχο κάρτας SD. Πρέπει να έχω γράψει κάτι στο αρχείο LOGO.TXT από PC.
 init_commands();
 //timer1_event = timer1.every(5000, tx_status); //Κάθε 250ms στείλε την κατάσταση (ορίζεται στο functions.h). Η εικόνα κάθε 25ms για πραγματικό br=6000bps
 //EEPROM.put(EE_PRESSURE, 101406); 
 EEPROM.get(EE_PRESSURE, start_pressure);
 if (hw_status == 63)
    {
     Serial.print(F("All devices working !!! Status: "));
     beep(2, 100);
    }
 else
    {
     Serial.print(F("Device malfunction: Status: "));
     beep(5, 500);
    }
 Serial.println(hw_status);

//Calibration();
 start_timer(1000); //1000
}

void loop() 
{
 timer1.update();
 but_sd.CheckBP();
 get_orientation();
 #if USE_GPS
    gps_getdata();
 #endif
 Rx_Packet();
 #if USE_SER_CMDS
    chk_for_ser_cmd();
 #endif
 mv_servos_update();
 beep_update();
 Blink(RED_LED);
 blink_pat(GREEN_LED, blnk_pat);
 transfer_photo_2_sd();
}

void sd_Click()
{
  timer1.stop(timer1_event);
  myFile.close();
  Serial.println(F("Ready to remove SD"));
  beep(5, 50);
}

