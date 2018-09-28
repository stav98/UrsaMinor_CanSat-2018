#include <RFM69.h>
//#include <RFM69_ATC.h>    

void radio_init(void);

#define NETWORKID     100  //Ίδιο για όλους του κόμβους
#define RECEIVER      1
#define SENDER        2
#define NODEID        RECEIVER  //Ο σταθμός εδάφους είναι δέκτης
#define FREQUENCY     RF69_433MHZ   //Μπάντα 433MHz
//#define ENCRYPTKEY    "ABCDEFGHIJKLMNOP" //16 χαρακτήρες, ίδιο για όλους τους κόμβους

//*****************************************************************************************************************************
//#define ENABLE_ATC      //comment out this line to disable AUTO TRANSMISSION CONTROL
#define ATC_RSSI        -75
//*********************************************************************************************

#define LED           9 // Moteinos have LEDs on D9
#define BUTTON_INT    1 //user button on interrupt 1 (D3)
#define BUTTON_PIN    3 //user button on interrupt 1 (D3)

#define LED_GREEN       4 //GREEN LED on the SENDER
#define LED_RED         5 //RED LED on the SENDER
#define RX_TOGGLE_PIN   7 //GPIO to toggle on the RECEIVER

#ifdef ENABLE_ATC
  RFM69_ATC radio;
#else
  RFM69 radio;
#endif

void radio_init(void)
{
  radio.initialize(FREQUENCY, NODEID, NETWORKID);
  radio.setHighPower(true); //Αυτό μόνο αν το module είναι HW ή HCW δηλ. 100mW
  radio.setPowerLevel(31);  //0-31 = 5-20dbm
  //radio.encrypt(ENCRYPTKEY); 
  #ifdef ENABLE_ATC
     radio.enableAutoPower(ATC_RSSI);
  #endif
  radio.setFrequency(434000000); //για Ελλάδα 433.05 - 434.79 MHz
  Serial.print("Listening at ");
  Serial.println(radio.getFrequency());
  Serial.flush();
  //pinMode(BUTTON_PIN, INPUT_PULLUP);
  //pinMode(LED, OUTPUT);
  //attachInterrupt(BUTTON_INT, handleButton, FALLING);
}


