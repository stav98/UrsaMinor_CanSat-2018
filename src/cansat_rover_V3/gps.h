//#include <SoftwareSerial.h>
//SoftwareSerial gpsSerial(12, 13); // RX, TX (TX not used)
#define GPSSerial     Serial1
#define NMEA_LEN      75   //68

//3857.14758;N;02044.63346;E

void gps_init(void);
void gps_test(void);
boolean gps_getdata(void);
void displayGPS(int);
void getField(char*, int, int);

char nmeaSentence[NMEA_LEN], last_status = 'V';
int hour, minutes, secs;

//char nmeaSentence[] = {"$GPRMC,093144.00,A,3857.14335,N,02044.63255,E,0.133,,171217,,,A*75"};

void gps_init()
{
  #if USE_GPS
      digitalWrite(GPS_MUX_PIN, HIGH);     //Diodes AND gate, High = Pass data - Πέρνα δεδομένα GPS
      digitalWrite(CAM_MUX_PIN, LOW);      //Diodes AND gate, High = Pass data - Κόψε δεδομένα κάμερας
      delay(100);
      digitalWrite(GPS_EN_PIN, LOW);  //Ενεργοποίησε GPS
      delay(200);
      GPSSerial.begin(9600);
  #endif
}

void gps_power_off()
{
 digitalWrite(GPS_MUX_PIN, LOW);  
 digitalWrite(GPS_EN_PIN, HIGH);  //Απενεργοποίησε GPS
}

//Έλεγχος αν λειτουργεί το GPS
void gps_test()
{
  uint32_t start = millis(); //Πάρε τρέχων χρόνο
  boolean a;
  do{
     a = gps_getdata();   //Διάβασε δεδομένα GPS
     delay(5);
    } while (millis() < (start + 200)); //Αν πέρασαν 200 msec φύγε
  if (a)  //Αν υπήρχε '$' μέσα στα δεδομένα τότε υπάρχει GPS
     {
      bitSet(hw_status, GPS);
      Serial.println(F("GPS OK"));
     }
  else
      Serial.println(F("GPS not initialized"));  
}

boolean gps_getdata()
{
 static int pos = 0;
 static boolean found = false;
 //$GPRMC,093144.00,A,3857.14335,N,02044.63255,E,0.133,,171217,,,A*75
 //$GPRMC,093142.00,A,3857.14339,N,02044.63303,E,0.077,,171217,,,A*7C

 if (!GPSSerial.available()) 
     return(found);

 while (GPSSerial.available()) //Υπάρχουν χαρακτήρες στο Receive Buffer ;
       {
        char c = GPSSerial.read();  //Διάβασε ένα-ένα τους χαρακτήρες
        //Serial.print(c);   //Debug
        if (c == '$') //Αρχή της πρότασης 
           {
            pos = 0; //Η πρόταση NMEA ξεκινάει με το '$'
            found = true;
           }
        if (c != '\n' && pos < NMEA_LEN)
            nmeaSentence[pos++] = c;
        else
           {
            nmeaSentence[pos] = '\0';
            pos = 0;
            //Serial.println();
            //Serial.print("----");
            //Serial.println(nmeaSentence);
            displayGPS(strlen(nmeaSentence));
            //memset(nmeaSentence, '\0', NMEA_LEN);
           }
       }
 return(found);
}

/*String getklTime(String s)
{
  int hour = s.substring(0,2).toInt();
  int minute = s.substring(2,4).toInt();
  int second = s.substring(4,6).toInt();

  hour += 8;

  if(hour > 24)
    hour -= 24;
  s = String(hour) +":"+String(minute) +":"+ String(second);
  return s;
}
*/
//Parse GPRMC NMEA sentence data from String
//String must be GPRMC or no data will be parsed
//Return Latitude
/*String parseGprmcLat(String s)
{
  int pLoc = 0; //paramater location pointer
  int lEndLoc = 0; //lat parameter end location
  int dEndLoc = 0; //direction parameter end location
  String lat;
  /*make sure that we are parsing the GPRMC string. 
   Found that setting s.substring(0,5) == "GPRMC" caused a FALSE.
   There seemed to be a 0x0D and 0x00 character at the end. */
/*  if(s.substring(0,4) == "GPRM")
  {
    //Serial.println(s);
    for(int i = 0; i < 5; i++)
    {
      if(i < 3) 
      {
        pLoc = s.indexOf(',', pLoc+1);
        /*Serial.print("i < 3, pLoc: ");
         Serial.print(pLoc);
         Serial.print(", ");
         Serial.println(i);*/
 /*     }
      if(i == 3)
      {
        lEndLoc = s.indexOf(',', pLoc+1);
        lat = s.substring(pLoc+1, lEndLoc);
        /*Serial.print("i = 3, pLoc: ");
         Serial.println(pLoc);
         Serial.print("lEndLoc: ");
         Serial.println(lEndLoc);*/
  /*    }
      else
      {
        dEndLoc = s.indexOf(',', lEndLoc+1);
        lat = lat + " " + s.substring(lEndLoc+1, dEndLoc);
        /*Serial.print("i = 4, lEndLoc: ");
         Serial.println(lEndLoc);
         Serial.print("dEndLoc: ");
         Serial.println(dEndLoc);*/
  /*    }
    }
    return lat; 
  }
  //}
  //}
}

//Parse GPRMC NMEA sentence data from String
//String must be GPRMC or no data will be parsed
//Return Longitude
String parseGprmcLon(String s)
{
  int pLoc = 0; //paramater location pointer
  int lEndLoc = 0; //lat parameter end location
  int dEndLoc = 0; //direction parameter end location
  String lon;

  /*make sure that we are parsing the GPRMC string. 
   Found that setting s.substring(0,5) == "GPRMC" caused a FALSE.
   There seemed to be a 0x0D and 0x00 character at the end. */
 /* if(s.substring(0,4) == "GPRM")
  {
    //Serial.println(s);
    for(int i = 0; i < 7; i++)
    {
      if(i < 5) 
      {
        pLoc = s.indexOf(',', pLoc+1);
        /*Serial.print("i < 3, pLoc: ");
         Serial.print(pLoc);
         Serial.print(", ");
         Serial.println(i);*/
  /*    }
      if(i == 5)
      {
        lEndLoc = s.indexOf(',', pLoc+1);
        lon = s.substring(pLoc+1, lEndLoc);
        /*Serial.print("i = 3, pLoc: ");
         Serial.println(pLoc);
         Serial.print("lEndLoc: ");
         Serial.println(lEndLoc);*/
  /*    }
      else
      {
        dEndLoc = s.indexOf(',', lEndLoc+1);
        lon = lon + " " + s.substring(lEndLoc+1, dEndLoc);
        /*Serial.print("i = 4, lEndLoc: ");
         Serial.println(lEndLoc);
         Serial.print("dEndLoc: ");
         Serial.println(dEndLoc);*/
  /*    }
    }
    return lon; 
  }
}

//Parse GPRMC NMEA sentence data from String
//String must be GPRMC or no data will be parsed
//Return Longitude
String parseGprmcSpeed(String s)
{
  int pLoc = 0; //paramater location pointer
  int lEndLoc = 0; //lat parameter end location
  int dEndLoc = 0; //direction parameter end location
  String lndSpeed;

  /*make sure that we are parsing the GPRMC string. 
   Found that setting s.substring(0,5) == "GPRMC" caused a FALSE.
   There seemed to be a 0x0D and 0x00 character at the end. */
 /* if(s.substring(0,4) == "GPRM")
  {
    //Serial.println(s);
    for(int i = 0; i < 8; i++)
    {
      if(i < 7) 
      {
        pLoc = s.indexOf(',', pLoc+1);
        /*Serial.print("i < 8, pLoc: ");
         Serial.print(pLoc);
         Serial.print(", ");
         Serial.println(i);*/
/*      }
      else
      {
        lEndLoc = s.indexOf(',', pLoc+1);
        lndSpeed = s.substring(pLoc+1, lEndLoc);
        /*Serial.print("i = 8, pLoc: ");
         Serial.println(pLoc);
         Serial.print("lEndLoc: ");
         Serial.println(lEndLoc);*/
  /*    }
    }
    return lndSpeed; 
  }
}


//Parse GPRMC NMEA sentence data from String
//String must be GPRMC or no data will be parsed
//Return Longitude
void parseGprmcTime(String s)
{
  int pLoc = 0; //paramater location pointer
  int lEndLoc = 0; //lat parameter end location
  int dEndLoc = 0; //direction parameter end location
  String gpsTime;

  /*make sure that we are parsing the GPRMC string. 
   Found that setting s.substring(0,5) == "GPRMC" caused a FALSE.
   There seemed to be a 0x0D and 0x00 character at the end. */
 /* if(s.substring(0,4) == "GPRM")
    {
     //Serial.println(s);
     for(int i = 0; i < 2; i++)
        {
         if(i < 1) 
           {
            pLoc = s.indexOf(',', pLoc+1);
            /*Serial.print("i < 8, pLoc: ");
            Serial.print(pLoc);
            Serial.print(", ");
            Serial.println(i);*/
 /*          }
         else
           {
            lEndLoc = s.indexOf(',', pLoc+1);
            gpsTime = s.substring(pLoc+1, lEndLoc);
            /*Serial.print("i = 8, pLoc: ");
            Serial.println(pLoc);
            Serial.print("lEndLoc: ");
            Serial.println(lEndLoc);*/
 /*          }
        }
     gpsTime.toCharArray(buf, 10);
    }
}

// Turn char[] array into String object
String charToString(char *c)
{

  String val = "";

  for(int i = 0; i <= sizeof(c); i++) 
  {
    val = val + c[i];
  }

  return val;
}
*/

void displayGPS(int tot_len)
{
  char time_field[10], date_field[7], lat_field[12], latNS_field[2], lon_field[12], lonEW_field[2], val_field[2];
  char tmp_str[3];
  if (strncmp(nmeaSentence, "$GPRMC", 6) == 0)
  {
    //Serial.println(nmeaSentence);  //debug  
    getField(time_field, 1, tot_len);
    getField(val_field, 2, tot_len);
    getField(lat_field, 3, tot_len);
    getField(latNS_field, 4, tot_len);
    getField(lon_field, 5, tot_len);
    getField(lonEW_field, 6, tot_len);
    getField(date_field, 9, tot_len);
    sprintf(gps_buffer, "G:;%s;%s;%s;%s;%s;%s;%s\n", time_field, date_field, lat_field, latNS_field, lon_field, lonEW_field, val_field);
    tmp_str[0] = time_field[0];
    tmp_str[1] = time_field[1];
    tmp_str[2] = '\0';
    hour = atoi(tmp_str);
    hour += 2;
    tmp_str[0] = time_field[2];
    tmp_str[1] = time_field[3];
    tmp_str[2] = '\0';
    minutes = atoi(tmp_str);
    tmp_str[0] = time_field[4];
    tmp_str[1] = time_field[5];
    tmp_str[2] = '\0';
    secs = atoi(tmp_str);
    char gps_status = val_field[0];
    if (gps_status != last_status)
       {
        last_status = gps_status;
        if (gps_status == 'A')
           {
            beep(3, 100);
            blnk_pat = 2;
           }
        else
            blnk_pat = 1;
       }
  }
}

void getField(char* buffer, int index, int tot_len)
{
 int sentencePos = 0;
 int fieldPos = 0;
 int commaCount = 0;
 while (sentencePos < tot_len)
       {
        if (nmeaSentence[sentencePos] == ',')
           {
            commaCount++;
            sentencePos++;
           }
        if (commaCount == index)
           {  
            if (nmeaSentence[sentencePos] != ',') //Αν ακολουθεί και άλλο ',' να μη το αντιγράψει
               {
                buffer[fieldPos] = nmeaSentence[sentencePos];
                fieldPos++;
               }
           }
        if (nmeaSentence[sentencePos] != ',') //Αν δεν είναι ',' να δείξει στο επόμενο.
            sentencePos++;
       }
 buffer[fieldPos] = '\0';
} 

