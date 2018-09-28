#include <Adafruit_VC0706.h>
#define CamSerial     Serial1

void cam_power_off(void);
void cam_hi_baud_rate(void);
void cam_low_baud_rate(void);
void cam_get_size(void);
void camera_init(void);
void cam_snapshot(void);
void take_photo(void);
void cam_open_file(char*);
void transfer_photo_2_sd(void);
void cam_chg_comp(byte);
void trans_pic(void);

#if USE_CAMERA
    Adafruit_VC0706 cam = Adafruit_VC0706(&CamSerial);
#endif

#if USE_CAMERA

boolean photo_ready = false;
char filename[13];
uint16_t jpglen;
int32_t pic_time;
byte wCount;
File imgFile;
byte pic_count = 0;

//Σβήσε την κάμερα για να μην καταναλώνει ενέργεια
void cam_power_off()
{
 digitalWrite(CAM_MUX_PIN, LOW);
 digitalWrite(CAM_EN_PIN, HIGH);
 digitalWrite(GPS_MUX_PIN, HIGH);     //Diodes AND gate, High = Pass data - Πέρνα δεδομένα GPS
 GPSSerial.begin(9600);     //Αποκατάσταση bitrate GPS
}

void cam_hi_baud_rate()
{
 cam.setBaud115200();
 delay(200);
 CamSerial.end();
 delay(100);
 CamSerial.begin(115200);
}

void cam_low_baud_rate()
{
 cam.setBaud38400();
 delay(200);
 CamSerial.end();
 delay(100);
 CamSerial.begin(38400);
}

void cam_get_size()
{
 uint8_t imgsize = cam.getImageSize();
 Serial.print("Image size: ");
 if (imgsize == VC0706_640x480) Serial.println(F("640x480"));
 if (imgsize == VC0706_320x240) Serial.println(F("320x240"));
 if (imgsize == VC0706_160x120) Serial.println(F("160x120"));
}

void camera_init()
{
  digitalWrite(GPS_MUX_PIN, LOW);     //Diodes AND gate, High = Pass data - Κόψε δεδοένα από GPS
  digitalWrite(CAM_MUX_PIN, HIGH);    //Diodes AND gate, High = Pass data - Πέρνα δεδομένα κάμερας
  delay(100);
  digitalWrite(CAM_EN_PIN, LOW); //Ενεργοποίησε κάμερα
  delay(500);
  // Ανίχνευση κάμερας
  if (cam.begin())
     {
      Serial.println(F("Camera Found:"));
      bitSet(hw_status, CAMERA);
     }
  else
     {
      Serial.println(F("No camera found?"));
      return;
     }
  // Εμφάνισε την έκδοση της κάμερας
  char *reply = cam.getVersion();
  if (reply == 0) 
      Serial.print(F("Failed to get version"));
  else 
     {
      Serial.println(F("-----------------"));
      Serial.print(reply);
      Serial.println(F("-----------------"));
     }
  delay(200);
  // Διάβασε το μέγεθος της εικόνας
  cam_get_size();
}

void cam_snapshot()
{
 cam_hi_baud_rate(); //Ανέβασε ρυθμό 115200 για γρήγορη μεταφορά εικόνας
 delay(100);
 if (! cam.takePicture()) //Πάγωσε εικόνα στο Frame Buffer
     Serial.println(F("Failed to snap!"));
  else 
     Serial.println(F("Picture taken!"));
}

void take_photo()
{ 
  cam_hi_baud_rate(); //Ανέβασε ρυθμό 115200 για γρήγορη μεταφορά εικόνας
  delay(100);
  if (! cam.takePicture()) //Πάγωσε εικόνα στο Frame Buffer
    Serial.println(F("Failed to snap!"));
  else 
    Serial.println(F("Picture taken!"));
  // Δημιουργία ονόματος αρχείου IMAGExx.JPG 8 χαρακτήρες για FAT16
  char filename[13];
  strcpy(filename, "IMAGE00.JPG"); //Αρχικά φτιάξε αυτό το όνομα
  for (int i = 0; i < 100; i++)
      {
       filename[5] = '0' + i/10;  //Τροποποίησε τον αριθμό από 00 έως 99
       filename[6] = '0' + i%10;
       //Φτιάξε το αρχείο αν δεν υπάρχει, διαφορετικά δοκίμασε το επόμενο
       if (! SD.exists(filename))
           break;
      }
  //Άνοιξε το αρχείο για εγγραφή
  File imgFile = SD.open(filename, FILE_WRITE);
  //Πάρε το μέγεθος της εικόνας που τράβηξες  
  uint16_t jpglen = cam.frameLength();
  Serial.print(F("Storing "));
  Serial.print(jpglen, DEC);
  Serial.print(F(" byte image."));
  //Ξεκίνα χρονομετρητή
  int32_t time = millis();
  //Διάβασε όλα τα δεδομένα μέχρι να φτάσεις το μέγεθος της frame buffer
  byte wCount = 0; //Καταμέτρηση του αριθμού των εγγραφών
  //Όσο υπάρχουν δεδομένα στη Frame Buffer
  while (jpglen > 0) 
        {
         // Διάβασε 64 bytes τη φορά;
         uint8_t *buffer;
         uint8_t bytesToRead = min(64, jpglen); //64 κανονικά αλλά το ασφαλές είναι 32 και είναι πιο αργό. Το μικρότερο αν είναι < 64
         buffer = cam.readPicture(bytesToRead); //Διάβασε τα δεδομένα στο buffer
         imgFile.write(buffer, bytesToRead); //Αποθήκευσε τα δεδομένα στην SD
         //Κάθε 2Kbyte δείξε την τρέχουσα κατάσταση
         if(++wCount >= 64) 
           {
            Serial.print('.');
            wCount = 0;
           }
         jpglen -= bytesToRead; //Μείωσε τα υπολοιπόμενα bytes
        }
  imgFile.close(); //Κλείσε το αρχείο
  time = millis() - time; //Πόσος χρόνος πέρασε;
  Serial.println(F("done!"));
  Serial.print(time); Serial.println(F(" ms elapsed")); //Εμφάνισε τον χρόνο
  cam.resumeVideo();  //Ξεκίνα πάλι την λήψη εικόνας video
  cam_low_baud_rate(); //Γύρισε σε ταχύτητα 38400
}


void cam_open_file(char* name)
{
  // Δημιουργία ονόματος αρχείου IMAGExx.JPG 8 χαρακτήρες για FAT16
  if (strlen(name) == 0)
  {
  strcpy(filename, "IMAGE00.JPG"); //Αρχικά φτιάξε αυτό το όνομα
  for (int i = 0; i < 100; i++)
      {
       filename[5] = '0' + i/10;  //Τροποποίησε τον αριθμό από 00 έως 99
       filename[6] = '0' + i%10;
       //Φτιάξε το αρχείο αν δεν υπάρχει, διαφορετικά δοκίμασε το επόμενο
       if (! SD.exists(filename))
           break;
      }
  }
  else
     strcpy(filename, name);
  //Άνοιξε το αρχείο για εγγραφή
  imgFile = SD.open(filename, O_WRITE | O_CREAT | O_TRUNC); //   FILE_WRITE
  //Πάρε το μέγεθος της εικόνας που τράβηξες  
  jpglen = cam.frameLength();
  Serial.print(F("Storing "));
  Serial.print(jpglen, DEC);
  Serial.print(F(" byte image."));
  //Ξεκίνα χρονομετρητή
  pic_time = millis();
  //Διάβασε όλα τα δεδομένα μέχρι να φτάσεις το μέγεθος της frame buffer
  wCount = 0; //Καταμέτρηση του αριθμού των εγγραφών
  photo_ready = true;
}

void transfer_photo_2_sd()
{
  if (photo_ready)
     {
      if (jpglen > 0) 
         {
          // Διάβασε 64 bytes τη φορά;
          uint8_t *buffer;
          uint8_t bytesToRead = min(64, jpglen); //64 κανονικά αλλά το ασφαλές είναι 32 και είναι πιο αργό. Το μικρότερο αν είναι < 64
          buffer = cam.readPicture(bytesToRead); //Διάβασε τα δεδομένα στο buffer
          imgFile.write(buffer, bytesToRead); //Αποθήκευσε τα δεδομένα στην SD
          //Κάθε 2Kbyte δείξε την τρέχουσα κατάσταση
          if(++wCount >= 64) 
            {
             Serial.print('.');
             wCount = 0;
            }
          jpglen -= bytesToRead; //Μείωσε τα υπολοιπόμενα bytes
         }
      else
         {
          imgFile.close(); //Κλείσε το αρχείο
          pic_time = millis() - pic_time; //Πόσος χρόνος πέρασε;
          Serial.println(F("done!"));
          tx_response(PSTR("IMG:Save"));
          Serial.print(pic_time); Serial.println(F(" ms elapsed")); //Εμφάνισε τον χρόνο
          cam.resumeVideo();  //Ξεκίνα πάλι την λήψη εικόνας video
          cam_low_baud_rate(); //Γύρισε σε ταχύτητα 38400
          delay(100);
          cam_power_off();
          photo_ready = false;
         }
     }
}

void cam_chg_comp(byte a)
{
 cam.setCompression(a); //0x35, 0xC0
 delay(100); 
}

void trans_pic()
{
 //char filename[13];
 char bd[43], encoded[60];
 //Άνοιξε το αρχείο για εγγραφή
 File snapFile = SD.open(filename, FILE_READ);
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
            //for (int i=0; i<42; ++i)
            //     Serial.print(bd[i], HEX);
            //Serial.println();
            //Serial.println(encoded);
            //byte decodedLen = base64_dec_len(encoded, 56);
            //base64_decode(decoded, encoded, 56);
            //Serial.println(decodedLen);
            //for (int i=0; i<42; ++i)
            //     Serial.print(decoded[i], HEX);
            //Serial.println();
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
     tx_response(PSTR("IMG:Trans"));
    }
 else
    Serial.println(F("Error opening file"));
 start_timer(timer1_int);
}


void trans_pic_hq()
{
 char filename[13];
 char bd[43], encoded[60];
 //Άνοιξε το αρχείο για εγγραφή
 strcpy(filename, "IMAGE00.JPG");
 filename[5] = '0' + pic_count/10;  //Τροποποίησε τον αριθμό από 00 έως 99
 filename[6] = '0' + pic_count%10;
 File snapFile = SD.open(filename, FILE_READ);
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
            //for (int i=0; i<42; ++i)
            //     Serial.print(bd[i], HEX);
            //Serial.println();
            //Serial.println(encoded);
            //byte decodedLen = base64_dec_len(encoded, 56);
            //base64_decode(decoded, encoded, 56);
            //Serial.println(decodedLen);
            //for (int i=0; i<42; ++i)
            //     Serial.print(decoded[i], HEX);
            //Serial.println();
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
     tx_response(PSTR("IMG:Photo"));
     ++pic_count;
    }
 else
    Serial.println(F("Error opening file"));
 start_timer(timer1_int);
}

#endif



