#include <SPI.h>
#include <SD.h>
#define SD_ChipSelectPin  3 //9 για mini

void sd_init(void);
void disp_sd_file(void);
void write_sd_data(void);

File myFile;
char log_csv_line[128];

void sd_init()
{
 if (!SD.begin(SD_ChipSelectPin))  //Ενεργοποίηση κάρτας Micro SD
     Serial.println(F("SD fail"));  
 else
    {
     Serial.println(F("SD OK !!!"));  
     bitSet(hw_status, SDCARD);
    }
}

void disp_sd_file()
{
 //Άνοιγμα αρχείου για ανάγνωση
 myFile = SD.open("LOGO.TXT");
 if (myFile) //Αν το αρχείο υπάρχει 
    {
     Serial.println("LOGO.TXT:");
     //Διάβασε το αρχείο μέχρι το τέλος
     while (myFile.available())
            Serial.write(myFile.read());
     //Κλείσε το αρχείο
     myFile.close();
    } 
 else 
    {
     //Αλλιώς αν το αρχείο δεν ανοίγει εμφάνισε μήνυμα λάθους
     Serial.println(F("Error opening file"));
    }
}

void write_sd_data()
{
 static boolean once = false;
 //uint32_t tt = 86400000;
 if (!once)
    {
     //myFile = SD.open("DATA1.TXT", O_WRITE | O_TRUNC);
     myFile = SD.open("DATA1.TXT", O_WRITE);
     if (myFile)
        {
         myFile.println(F("-------------------------------------------------"));
         myFile.close();
         once = true;
        }
    }
 myFile = SD.open("DATA1.TXT", FILE_WRITE);
 if (myFile) 
    {
     sprintf(log_csv_line, "%lu;%s;%s\n", millis(), tx_buffer1, gps_buffer);
     myFile.println(log_csv_line);
     myFile.close();
    }
}

void trans_log()
{
 myFile.flush();
 myFile.close();
 char bd[43], encoded[60];
 //Άνοιξε το αρχείο για εγγραφή
 File snapFile = SD.open("DATA1.TXT", FILE_READ);
 if (snapFile) //Αν το αρχείο υπάρχει 
    {
     word bytes_cnt = 0, img_pkt_cnt = 0;
     word file_len = snapFile.size();
     Serial.println(F("Sending ..."));
     while (file_len > 0)
           {
            byte bytesToRead = min(39, file_len); //42
            snapFile.read((uint8_t *)&bd, bytesToRead);
            base64_encode(encoded, bd, bytesToRead); 
            sprintf(img_buffer, "%u;$;%s\n", img_pkt_cnt++, encoded);
            Tx_Packet(img_buffer, 6, F("Image Packet N:"), 33);
            //Serial.print(img_buffer);
            file_len -= bytesToRead; //Μείωσε τα υπολοιπόμενα bytes
            bytes_cnt += bytesToRead;
           }
     snapFile.close();
     Serial.print(F("Complete :"));
     Serial.print(bytes_cnt);
     Serial.println(F(" bytes"));
     tx_response(PSTR("LOG:Trans"));
    }
 else
    Serial.println(F("Error opening file"));
 start_timer(timer1_int);
}
