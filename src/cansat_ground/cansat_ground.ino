#include "Wire.h"
#include <SPI.h>
#include <LowPower.h>
#define RADIO_LED   9

#include "rfm69.h"

char tx_buffer[61];
byte buf_cnt = 0;
uint32_t rx_time;
long int pkt_count;
boolean cmdtx_flag = false, blink_ack = false;
String sat_data, data1, data2;
int idx1;
word pkt_cnt, pkt_cnt_old = 65535;

void setup() 
{
 pinMode(RADIO_LED, OUTPUT);
 Serial.begin(115200);
 Serial.print(F("Program Started\n"));
 delay(10);
 radio_init();
}

void loop() 
{
 //Έλεγξε αν υπάρχει ολοκληρωμένο πακέτο προς επεξεργασία
 if (radio.receiveDone())
    {
     rx_time = millis();
     sat_data = String((char*)radio.DATA);
     idx1 = sat_data.indexOf(';');
     data1 = sat_data.substring(0, idx1);
     pkt_cnt = data1.toInt();
     //Serial.print((char*)radio.DATA); //Εμφανίζει την απάντηση από τον δορυφόρο
     if (pkt_cnt != pkt_cnt_old)
        {
         Serial.print(sat_data);
         pkt_cnt_old = pkt_cnt;
        }
     else
         Serial.println("---------------------- Duplicate ----------------");
         
     //Serial.println(pkt_cnt);
     Serial.print(F("L>RSSI: "));
     Serial.print(radio.RSSI);
     Serial.print(" #");
     Serial.println(pkt_count); //Debug
     //Αν έλαβε πακέτο Ping Request τότε γύρισέ το πίσω στον δορυφόρο
     //if (strcmp_P(radio.DATA, PSTR("PngRq:0123456789abcdefghijklmnopqrsruvwxyz!@#$%^&*-+.ABCDEFG")) == 0)
     if (strlen((char*)radio.DATA) == 60 && radio.DATA[0]=='P' && radio.DATA[1]=='n' && radio.DATA[2]=='g')
        {
         //Serial.println("OK match"); //Debug
         radio.sendWithRetry(SENDER, radio.DATA, strlen(radio.DATA), 1, 20);
        }
     pkt_count++;
     //Έλεγξε μήπως ο δορυφόρος ζήτησε επιβεβαίωση λήψης (ACK)
     if (radio.ACKRequested())
         radio.sendACK();
    }
 //LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_ON); //sleep Moteino in low power mode (to save battery)
 //Αν υπάρχουν δεδομένα στη σειριακή
 if (Serial.available())
    {
     char c = Serial.read();
     if (c != '\r')
         tx_buffer[buf_cnt++] = c;
     else
        {
         tx_buffer[buf_cnt] = '\0';
         buf_cnt = 0;
         cmdtx_flag = true;
        }
    }
 if (cmdtx_flag)
    {
     if (millis() - rx_time > 2000) rx_time = millis();
     if ((millis() - rx_time > 25) && (millis() - rx_time < 75)) //100-150
        {
         if (radio.sendWithRetry(SENDER, tx_buffer, strlen(tx_buffer), 1, 20)) //3 Αν το έστειλε και πήρε απάντηση (προσπαθεί 3 φορές), τότε να εμφανίσει μήνυμα για debug. 
            {
             Serial.println(F("L>Command send and verified")); 
             cmdtx_flag = false;
             blink_ack = true;
            }
         else
             Serial.println(F("L>Command not acknoweleged"));
         //cmdtx_flag = false;
        }
    }
 Blink(RADIO_LED);
}

void Blink(byte PIN)
{ 
 static uint32_t ms_now;
 if (blink_ack)
    {
     if (!digitalRead(PIN))
        {
         ms_now = millis();
         digitalWrite(PIN, HIGH);
        }
     else
        {
         if (millis() - ms_now > 200)
            {
             digitalWrite(PIN, LOW);
             blink_ack = false;
            }
        }
    }
}
