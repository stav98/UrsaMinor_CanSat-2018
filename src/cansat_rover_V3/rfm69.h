#include <RFM69.h>
//#include <RFM69_ATC.h>    

#define TxI   0 //Δεν εκπέμπει κάτι
#define TxS   1 //Πακέτο με θερμοκρασία, πίεση, υγρασία
#define TxG   2 //Θέση GPS


void radio_init(void);
//void Tx_Packet(char*, byte, const __FlashStringHelper*, int); //Ορίζεται το πρωτότυπο στο functions.h γιατί χρησιμοποιείται εκεί
void Rx_Packet(void);
//void tx_status(void); //Ορίζεται το πρωτότυπο στο functions.h γιατί χρησιμοποιείται εκεί


/* Για MSK (Minimum Shift Keying) ο δείκτης διαμόρφωσης είνα 0,5. MI=(2xFdev)/BR => MI=(2x5000)/19200 => MI=10000/19200 = 0,52
 * Το εύρος καναλιού είναι Bw = BR + 2xFdev => Bw = 19200 + 2x5000 => Bw = 29200Hz ή ?? (19200/2)+2*5000 = 9600 + 10000 = 19600Hz
 * Βάζω φίλτρο διέλευσης δέκτη 31,3KHz και φροντίζω να ισχύει (BR < 2 * RxBw) 19200 < 60000.
 * Για GMSK βάζω Gaussian φίλτρο στα σειριακά δεδεομένα πριν αυτά διαμορφωθούν με BT 1.0 ή 0.5 ή 0.3 (Έχω μικρότερο εύρος αλλά μεγαλύτερο BER)
 * 
 * Δοκιμή με BR=19200 και Fdev = 35000. Ο δείκτης διαμόρφωσης MI=(2x35000)/19200=3,65. Το εύρος καναλιού του δέκτη είναι RxBw >= Fdev + (BR/2)  
 * = 35000 + 9600 ~= 45000. Αν βάλω 83KHz έχω P.E.R = 1%. (Packet Error Rate με πακέτα 60bytes και 4 πακέτα / sec = 240bytes x 8 = 1920bps - Με 2
 * φορές προσπάθεια επανεκπομπής (tik=250msec).
 * Αν υποθέσουμε ότι ο κρύσταλλος έχει τσούλιμα +-20ppm τότε στα 433MHZ έχω df=(433000000*20)/10^6 = 8660Hz. Επομένως για να κλειδώσει το AFC το
 * RxBwAFC >= Fdev + (BR/2) + LOoffset = 45000 + 8660 ~= 55KHz και δοκιμάζω 50KHz και έχω PER = 3-5%. Με shaping 1.0 έχω PER = 2-5%. Με 0.5 -> 1-2%
 * ->(tik=50msec) Επίσης με BR=38400 έχω PER = 1-2%. Αν αυξήσω τον ρυθμό σε 20 πακέτα / sec  έχω PER = 2-3%.
 * ->(tik=25msec) 40 πακέτα. Μεταδίδω εικόνα 640x480 grey scale σε 2.2 min. Αν βάλω Ack retry=3 φορές τότε το PER 
 * πέφτει στο 1%.
 * [2-1-17]
 * Με διαμόρφωση GMSK shaping 0.5 και Fdev = 15000 και BR = 38400 βάζω εύρος καναλιού 41.7KHz. Επίσης εφαρμόζω και μηχανισμό DC-Free data whitening και
 * όχι Manchester γιατί διπλασιάζει το εύρος του bitrate. Το κάθε πακέτο έχει συνολικό μήκος 72bytes και διαρκεί 15msec. Με επιτυχές ACK είναι +5msec δηλ. 
 * 20msec. Αν κάνει αποτυχημένο ACK θέλει +40 για κάθε ένα δηλ. 45. Εφόσον έχω 3 ζωές δηλ. 4 επαναλήψεις θέλω 20x4 + 4x45 = 260msec. Δηλαδή σε κάθε sec 
 * μπορώ να στείλω μέχρι 4 πακέτα δηλ. 4x60 = 240bytes ή 1920bps. Με σήμα -80dbm έχω PER < 1% για 4 πακέτα το δευτερόλεπτο.
 * [2-1-17]
 * Χωρίς shaping και Fdev = 35000 και εύρος καναλιού 50 KHz και σήμα -70dbm μειώνω το Ack roundtrip από 40 σε 20ms και βάζω 6 retry και έχω PER = 0,03%.
 * -------------------------------------------------------------------------
 * [2-1-17][Το καλύτερο]
 * Με shaping 0.3 και Fdev = 15000 και εύρος καναλιού 41.7KHz και σήμα -75dbm Ack roundtrip 20ms και 6 retry, μεταδίδω με PER=0% 1000 πακέτα των 60bytes
 * σε 80sec δηλ. πραγματικό bit rate 6Kbps δηλαδή η εικόνα 640x480 gray scale = 307200bytes θα μεταδοθεί σε 7 λεπτά ενώ 320x240 = 76800bytes σε
 * (76800x8=614400 / 6000 = 102,4sec) ~= 1,8 λεπτά. 
 * Για μετάδοση μετρήσεων στέλνω 240 πακέτα το λεπτό δηλ. 4/sec δηλ. πραγματικό bit rate = 60x4x8= 1920 bps.
 * Το σήμα να μην πέσει κάτω από -85 dbm γιατί αρχίζει τα λάθη. Με -92 έχει PER < 1%.
 */

#define NETWORKID     100  //Ίδιο για όλους του κόμβους
#define RECEIVER      1
#define SENDER        2
#define NODEID        SENDER  //Το όχημα είναι ο πομπός
#define FREQUENCY     RF69_433MHZ   //Μπάντα 433MHz για Ελλάδα 433.05 - 434.79 MHz
//#define ENCRYPTKEY    "ABCDEFGHIJKLMNOP" //16 χαρακτήρες, ίδιο για όλους τους κόμβους

//*****************************************************************************************************************************
//#define ENABLE_ATC      //comment out this line to disable AUTO TRANSMISSION CONTROL
#define ATC_RSSI        -75
//*********************************************************************************************

#ifdef ENABLE_ATC
  RFM69_ATC radio;
#else
  RFM69 radio;
#endif

int ok_packet, nok_packet;
int ack_rssi; //Είναι το σήμα που μου φέρνει ο σταθμός βάσης όταν στέλνει πακέτα ACK.

void radio_init(void)
{
  if (radio.initialize(FREQUENCY, NODEID, NETWORKID))
     {
      Serial.println(F("Radio tranceiver OK")); 
      bitSet(hw_status, RADIO);
      radio.setHighPower(true); //Αυτό μόνο αν το module είναι HW ή HCW δηλ. 100mW
      radio.setPowerLevel(31); //0-31 = 5-20dbm
      //radio.encrypt(ENCRYPTKEY); //Σχόλιο αν δεν θέλω encryption
      #ifdef ENABLE_ATC
         radio.enableAutoPower(ATC_RSSI);
      #endif
      radio.setFrequency(434000000);  //για Ελλάδα 433.05 - 434.79 MHz
      Serial.print("Listening at ");
      Serial.println(radio.getFrequency());
      Serial.flush();
     }
   else
     {
      Serial.println(F("Radio not initialized")); 
     }
}

#include "cmd.h"

//Στέλνει ένα πακέτο στον δέκτη
void Tx_Packet(char* tx_buffer, byte times, const __FlashStringHelper* debug_msg, int id)
{
 //Serial.print(debug_msg);
 //Serial.print(id);

 if (radio.sendWithRetry(RECEIVER, tx_buffer, strlen(tx_buffer), times, 20)) //Αν το έστειλε και πήρε απάντηση (προσπαθεί times φορές), τότε να εμφανίσει μήνυμα για debug. 
    {
     //Serial.println(F(" send and verified")); 
     ok_packet++;
     ack_rssi = radio.RSSI;
     blink_ack = true;
    }
 else
    {
     //Serial.println(F(" not acknoweleged"));
     nok_packet++;
    }
 //Serial.print(ok_packet);Serial.print("---");Serial.println(nok_packet);
}

//Ελέγχει συνεχώς μήπως έλαβε κάποιο πακέτο από το δίκτυο.
void Rx_Packet()
{
 if (radio.receiveDone())
    {
     //Serial.print((char*)radio.DATA); //Debug
     //Serial.print(F("RSSI: "));
     //Serial.println(radio.RSSI);
     //check if sender wanted an ACK
     if (radio.ACKRequested()) //Ελέγχει το CTL byte για να δει αν ζητήθηκε επιβεβαίωση.
        {
         radio.sendACK();
         //Serial.println(" - ACK sent"); //Debug
        }
     //if (strcmp_P((char*)radio.DATA, PSTR("PngRq:0123456789abcdefghijklmnopqrsruvwxyz!@#$%^&*-+.ABCDEFG")) == 0)
     if (strlen((char*)radio.DATA) == 60 && radio.DATA[0]=='P' && radio.DATA[1]=='n' && radio.DATA[2]=='g')
        {
         //Serial.println("Ping Reply from base");  //Debug
         int d = millis() - ping_time;
         char cmd_resp[35];
         sprintf_P(cmd_resp, PSTR("Re:Tot time: %d ms\n"), d);
         Tx_Packet(cmd_resp, 6, F("Response Ping Packet N:"), 1);
        }
     else 
        cmd_handler(); //Έλεγξε αν είναι κάποια γνωστή εντολή στο cmd.h
    }
}

void tx_status()
{
 if (!stop_job)
 {
 static byte pat_idx = 0;
 long pressure_offs;
 byte pat[] = {TxS, TxS, TxS, TxG};
 //byte pat[] = {TxS, TxI, TxS, TxI, TxG, TxI};
 //byte pat[] = {TxS, TxS, TxS, TxS};
 static int packet_id = 1;
 char temp_buffer[8];
 char pres_buffer[12];
 //char alt_buffer[8];
 char hum_buffer[8];
 char bat_buffer[5];
 bat_voltage = (float) analogRead(A0) / 123;
 dtostrf(mySensor.readTempC(), 5, 1, temp_buffer); //5,2 - Μέγιστο μήκος μαζί με '-' και '.' δηλ. -12.5 και μετά 2 δεκαδικά ψηφία
 //dtostrf(mySensor.readFloatPressure(), 9, 2, pres_buffer); //Σε Pa, παρακάτω το στέλνω ως long και όχι float
 //dtostrf(mySensor.readFloatAltitudeMeters(), 6, 1, alt_buffer); //7,2-Σε μέτρα
 cur_pressure = (long)mySensor.readFloatPressure();
// if (start_pressure == -1)
//     pressure_offs = 102325;
// else
     pressure_offs = start_pressure;
 //Υπολογισμός ύψους από πίεση. Δεν το θέλω θα το υπολογίζω στο τερματικό
 //dtostrf((((float)-45846.2)*(pow(((float)mySensor.readFloatPressure()/(float)pressure_offs), 0.190263) - (float)1)), 6, 1, alt_buffer); //7,2-Σε μέτρα
 dtostrf(mySensor.readFloatHumidity(), 4, 1, hum_buffer); //Επί τοις %
 dtostrf(bat_voltage, 4, 2, bat_buffer); //Σε Volts
 //sprintf(tx_buffer1, "St%s;%ld;%s;%s;%s;%s\n", temp_buffer, cur_pressure, alt_buffer, hum_buffer, bat_buffer, Orientation);  //Ένωσε τα strings
 sprintf(tx_buffer1, "S:;%s;%ld;%s;%s;%s\n", temp_buffer, cur_pressure, hum_buffer, bat_buffer, Orientation);  //Ένωσε τα strings
 write_sd_data(); //Αποθήκευσε στην μνήμη SD
 
 //---- Ότι ακολουθεί είναι για debuging. Το πακέτο έχει μήκος 60 bytes ----
 //Serial.print(tx_buffer1); //debug
 //Serial.print("--->");
 //Serial.println(strlen(tx_buffer1)); //Τύπωσε το μήκος του πακέτου. Δεν πρέπει να ξεπερνά τα 61 bytes.
//// Serial.print(gps_buffer);
 //Serial.print("--->");
 //Serial.println(strlen(gps_buffer)); //Τύπωσε το μήκος του πακέτου. Δεν πρέπει να ξεπερνά τα 61 bytes.
 //Serial.print(hour);
 //Serial.print(':');
 //Serial.print(minutes);
 //Serial.print(':');
 //Serial.println(secs);
//// Serial.println(ack_rssi);
 //--------------------------------------------------------------------------
 switch (pat[pat_idx])
        {
         case TxS:
            //Tx_Packet(tx_buffer1, 6, F("Status Packet N:"), packet_id++);
            sprintf(tx_buffer2, "%u;%s", pkt_cnt, tx_buffer1);
            break;
         case TxG:
            #if USE_GPS
                //Tx_Packet(gps_buffer, 2, F("Position Packet N:"), packet_id++);
                sprintf(tx_buffer2, "%u;%s", pkt_cnt, gps_buffer);
            #endif
            break;
         default:
            __asm__("nop");  
        }
 Tx_Packet(tx_buffer2, 6, F("Status Packet N:"), packet_id++);
 Serial.print(tx_buffer2); //debug
 ++pkt_cnt;
 if (pat_idx >= sizeof(pat) - 1)
     pat_idx = 0;
 else
     pat_idx++;
 }
}


