
void tx_status(void);  //Η συνάρτηση βρίσκεται στο rfm69.h
void Tx_Packet(char*, byte, const __FlashStringHelper*, int);
void start_timer(int);
void Blink(byte);
void blink_pat(byte, byte);
void beep(byte, word);
void beep_update(void);
void tx_response(const char*);

void start_timer(int ms)
{
 timer1_event = timer1.every(ms, tx_status); //Κάθε 250ms στείλε την κατάσταση (ορίζεται στο functions.h). Η εικόνα κάθε 25ms για πραγματικό br=6000bps
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

void blink_pat(byte PIN, byte p)
{ 
 static uint32_t ms_now = millis();
 word del_H, del_L;
 switch (p)
   {
    case 1:
         del_H = 500;
         del_L = 500;
         break;
    case 2:
         del_H = 200;
         del_L = 2000;
         break;
   }
     if (!digitalRead(PIN))
        {
         if (millis() - ms_now > del_L)
             {
              digitalWrite(PIN, HIGH);
              ms_now = millis();
             }
        }
     else
        {
         if (millis() - ms_now > del_H)
            {
             digitalWrite(PIN, LOW);
             ms_now = millis();
            }
        }
    
}

byte beep_times;
word beep_intrvl;

void beep(byte times, word interval)
{
  beep_times = times;
  beep_intrvl = interval;
}

void beep_update()
{
  static uint32_t ms_now = 0;
  static boolean state = false;
  if (beep_times > 0)
     {
      if (ms_now == 0)
         {
          ms_now = millis();
          if (!state)
             {
              digitalWrite(BUZ_PIN, HIGH);
              state = true;
             }
          else
             {
              digitalWrite(BUZ_PIN, LOW);
              state = false;
              --beep_times;
             }
         }
      if (millis() > (ms_now + beep_intrvl))
         {
          ms_now = 0;
         }
     }
  else
     state = false;
}

void tx_response(const char* r)
//void tx_response(char* r)
{
 char cmd_resp1[20], cmd_resp2[35]; 
 sprintf_P(cmd_resp1, r);
 sprintf_P(cmd_resp2, PSTR("%u;Re:%s\n"), pkt_cnt++, cmd_resp1);
 Tx_Packet(cmd_resp2, 6, F("Response Packet N:"), 1);
}

void tx_response_r(float r)
{
 char cmd_resp1[20], buf[11];
 dtostrf(r, 8, 2, buf);
 sprintf_P(cmd_resp1, PSTR("Re:%s\n"), buf);
 Tx_Packet(cmd_resp1, 6, F("Response Packet N:"), 1);
}



