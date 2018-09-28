#define MAX_MSG_SIZE    62    //Το μέγιστο μήκος της εντολής μαζί με παραμέτρους
#define PROMPT           0
#define SER_CMD_LEN     30

typedef void (*Func)(int argc, char **argv);  //Δομή της συνάρτησης προς κλήση

Func execFunc;

//Δομή του κάθε κόμβου της συνδεδεμένης λίστας
typedef struct _L_cmd
         {
          const char *cmd;
          const Func ExecSub;
          const struct _L_cmd *next;
         } L_cmd;

//Καθολικές μεταβλητές του module
const char cmd_unrecog[] PROGMEM = "CMD: Command not recognized."; //Μήνυμα στην Flash για μη έγκυρη εντολή
char ser_cmd[SER_CMD_LEN]; //Πίνακας χαρακτήρων (buffer) που κρατάει την εντολή που έστειλε ο χρήστης από την σειριακή
byte ser_i = 0; //Μετρητής χαρακτήρων
char msg[MAX_MSG_SIZE];   //Buffer με την εντολή προς διερμηνία από το radio modem

//Πρωτότυποι ορισμοί συναρτήσεων που ακολουθούν παρακάτω
void chk_for_ser_cmd(void);
void cmd_handler(void);
void cmd_parse(char*);
void cmd_display(void);
void init_commands(void);
void args_error(void);
//Ορισμοί εντολών για χρήση στην λίστα
void sampint(int, char**);
void rel(int, char**);
void fd(int, char**);
void bk(int, char**);
void rt(int, char**);
void lt(int, char**);
void frt(int, char**);
void flt(int, char**);
void pic(int, char**);
void piccomp(int, char**);
void getpicsz(int, char**);
void setpicsz(int, char**);
void campwr(int, char**);
void gpspwr(int, char**);
void alarm(int, char**);
void rssitest(int, char**);
void ping(int, char**);
void txpic(int, char**);
void snap(int, char**);
void pitchtrim(int, char**);
void fp(int, char**);
void txlog(int, char**);
void txphoto(int, char**);

//Συνδεδεμένη λίστα με τις εντολές στην Program Memory
L_cmd *cmd_tbl;
//Οι εντολές που αναγνωρίζονται από τον δορυφόρο
const char PROGMEM \
   //        1         |           2        |         3              |         4            |          5          |
   ct01[] = "sampint"  , ct02[] = "rel"     , ct03[] = "fd"          , ct04[] = "bk"        , ct05[] = "rt"       , 
   ct06[] = "lt"       , ct07[] = "frt"     , ct08[] = "flt"         , ct09[] = "pic"       , ct10[] = "piccomp"  ,
   ct11[] = "getpicsz" , ct12[] = "setpicsz", ct13[] = "campwr"      , ct14[] = "gpspwr"    , ct15[] = "alarm"    ,
   ct16[] = "rssitest" , ct17[] = "ping"    , ct18[] = "txpic"       , ct19[] = "snap"      , ct20[] = "pitchtrim", 
   ct21[] = "fp"       , ct22[] = "txlog"   , ct23[] = "txphoto";    
   
//Δομή συνδεδεμένης λίστας
const L_cmd PROGMEM cmd23 = {(const char*)ct23, txphoto,       NULL};
const L_cmd PROGMEM cmd22 = {(const char*)ct22, txlog,       &cmd23};
const L_cmd PROGMEM cmd21 = {(const char*)ct21, fp,          &cmd22};
const L_cmd PROGMEM cmd20 = {(const char*)ct20, pitchtrim,   &cmd21};
const L_cmd PROGMEM cmd19 = {(const char*)ct19, snap,        &cmd20};
const L_cmd PROGMEM cmd18 = {(const char*)ct18, txpic,       &cmd19};
const L_cmd PROGMEM cmd17 = {(const char*)ct17, ping,        &cmd18};
const L_cmd PROGMEM cmd16 = {(const char*)ct16, rssitest,    &cmd17};
const L_cmd PROGMEM cmd15 = {(const char*)ct15, alarm,       &cmd16};
const L_cmd PROGMEM cmd14 = {(const char*)ct14, gpspwr,      &cmd15};
const L_cmd PROGMEM cmd13 = {(const char*)ct13, campwr,      &cmd14};
const L_cmd PROGMEM cmd12 = {(const char*)ct12, setpicsz,    &cmd13};
const L_cmd PROGMEM cmd11 = {(const char*)ct11, getpicsz,    &cmd12};
const L_cmd PROGMEM cmd10 = {(const char*)ct10, piccomp,     &cmd11};
const L_cmd PROGMEM cmd09 = {(const char*)ct09, pic,         &cmd10};
const L_cmd PROGMEM cmd08 = {(const char*)ct08, flt,         &cmd09};
const L_cmd PROGMEM cmd07 = {(const char*)ct07, frt,         &cmd08};
const L_cmd PROGMEM cmd06 = {(const char*)ct06, lt,          &cmd07};
const L_cmd PROGMEM cmd05 = {(const char*)ct05, rt,          &cmd06};
const L_cmd PROGMEM cmd04 = {(const char*)ct04, bk,          &cmd05};
const L_cmd PROGMEM cmd03 = {(const char*)ct03, fd,          &cmd04};
const L_cmd PROGMEM cmd02 = {(const char*)ct02, rel,         &cmd03};
const L_cmd PROGMEM cmd01 = {(const char*)ct01, sampint,     &cmd02};

//Έλεγχος για εντολή από την σειριακή κονσόλα (Μόνο για Debugging)
void chk_for_ser_cmd()
{
 while (Serial.available()) //Αν υπάρχουν χαρακτήρες στο buffer της σειριακής
    {
     char c = Serial.read(); //Διάβασε ένα - ένα τους χαρακτήρες και βάλτους στη c
     //Έλεγξε τι είναι το c
     switch (c)
            {
             case '\r': //Πατήθηκε <enter>
                 Serial.print("\r\n");
                 ser_cmd[ser_i] = '\0'; //Βάλε χαρακτήρα τερματισμού του string
                 ser_i = 0; //Δείξε στην αρχή του buffer για την επόμενη εντολή
                 //Serial.println(ser_cmd); //Τύπωσε την εντολή στην οθόνη του χρήστη (Debug)
                 cmd_parse((char*)ser_cmd);  //Κάλεσε συνάρτηση ελέγχου της εντολής
                 break;
             default:    //Βάλε τον χαρακτήρα στο buffer
                 Serial.print(c);
                 ser_cmd[ser_i++] = c; //Αποθήκευσε τον χαρακτήρα στην τρέχουσα θέση του buffer
                                       //Δείξε στην επόμενη θέση
            }
    }
}

void cmd_handler()
{
 strcpy(msg, (char*)radio.DATA); //Αν υπάρχουν δεδομένα αντέγραψέ τα στο msg για έλεγχο
 //Serial.println(msg);  //Debug
 cmd_parse((char*)msg);  //Κάλεσε συνάρτηση ελέγχου της εντολής
}

void cmd_parse(char *cmd)
{
 byte argc, i = 0;
 char *argv[10];
 char buf[40];
 L_cmd *cmd_entry;
 // Διάβασε το cmd και χώρισε τα ορίσματα αν υπάρχουν κενά και βάλτα στον πίνακα argv[]
 argv[i] = strtok(cmd, " ");
 do
   {
    argv[++i] = strtok(NULL, " ");
   } while ((i < 10) && (argv[i] != NULL));
    
 //Αποθήκευσε τον αριθμό των ορισμάτων
 argc = i;
  
 //Αν δεν πατήθηκε απλώς enter
 if (argv[0] != NULL)
    {
     //Serial.println(argv[0]); //Debug
     //Ψάξε στο λεξικό να βρεις την εντολή που δόθηκε στο prompt. Το argv[0] έχει την εντολή
     for (cmd_entry = cmd_tbl; cmd_entry != NULL; cmd_entry = (L_cmd*)pgm_read_word(&cmd_entry->next))
         {
          if (!strcmp_P(argv[0], (char*)pgm_read_word(&cmd_entry->cmd)))
             {
              execFunc = (Func)pgm_read_word(&cmd_entry->ExecSub);
              execFunc(argc, argv);
              return;
             }
          }
      //Η εντολή δεν αναγνωρίστηκε. Βγάλε μήνυμα λάθους
      strcpy_P(buf, cmd_unrecog);
      Serial.println(buf);
    }
}

void init_commands(void)
{
 cmd_tbl = (L_cmd*) &cmd01;
}

void args_error(void)
{
 Serial.println(F("Argument missing"));
}


//======================= Λίστα εντολών για εκτέλεση ===========================================================
//Αλλάζει την ταχύτητα μετρήσεων σε ms. π.χ. sampint 500 θα κάνει μέτρηση κάθε 0,5 sec. 
void sampint(int arg_cnt, char **args)
{
 if (arg_cnt > 1)
    {
     char cmd_resp[35];
     timer1.stop(timer1_event);
     timer1_int = atoi(args[1]);
     sprintf_P(cmd_resp, PSTR("%u;Re:OK sampint=%d ms\n"), pkt_cnt++, timer1_int);
     Tx_Packet(cmd_resp, 6, F("Response Packet N:"), 1);
     start_timer(timer1_int);
    }
 else
     args_error();
}

//Δίνει τάση στην αντίσταση λίγο χρόνο
//Με τάση μπαταρίας 7,4V αρκεί μισό λεπτό, αν πέσει 6,2V θέλω 1 λεπτό, δηλ. στέλνω 2 rel
//Ένταση ρεύματος 450mA
void rel(int arg_cnt, char **args)
{
 digitalWrite(LANDER_REL_PIN, HIGH);
 tx_response(PSTR("LND:Try to Release"));
 delay(20000);  //30 δευτερόλεπτα
 digitalWrite(LANDER_REL_PIN, LOW);
 tx_response(PSTR("LND:Released"));
}

//---------------------------- Εντολές κίνησης του rover -----------------------------
void fd(int arg_cnt, char **args)
{
 byte a;
 if (arg_cnt > 1)
     a = atoi(args[1]);
 else
     a = 1;
 tx_response(PSTR("Moving Forward"));
 rover_fd(a);
}

void bk(int arg_cnt, char **args)
{
 tx_response(PSTR("Moving Back"));
 rover_bk();
}

void rt(int arg_cnt, char **args)
{
 tx_response(PSTR("Moving Right"));
 rover_rt();
}

void lt(int arg_cnt, char **args)
{
 tx_response(PSTR("Moving Left"));
 rover_lt();
}

void frt(int arg_cnt, char **args)
{
 word deg;
 if (arg_cnt > 1)
     deg = atoi(args[1]);
 else 
     deg = 90;
 tx_response(PSTR("Moving Full Right"));
 rover_turn(deg, false);
}

void flt(int arg_cnt, char **args)
{
 word deg;
 if (arg_cnt > 1)
     deg = atoi(args[1]);
 else 
     deg = 90;
 tx_response(PSTR("Moving Full Left"));
 rover_turn(deg, true);
}

void pitchtrim(int arg_cnt, char **args)
{
 if (arg_cnt > 1)
    {
     if(strcmp_P(args[1], PSTR("up")) == 0)
        rover_pitch_up();
     else if (strcmp_P(args[1], PSTR("down")) == 0)
        rover_pitch_down();
    }
 else
    args_error();
}

void fp(int arg_cnt, char **args)
{
 if (arg_cnt > 1)
    {
     if(strcmp_P(args[1], PSTR("on")) == 0) 
        full_power = true;
     else if(strcmp_P(args[1], PSTR("off")) == 0)
        full_power = false;
    }
 else
     args_error();
}

//---------------------------- Εντολές διαχείρισης κάμερας ---------------------------
void pic(int arg_cnt, char **args)
{
 #if USE_CAMERA
 /*camera_init();
 take_photo();
 delay(100);
 cam_power_off();*/

 camera_init(); //Γυρίζει τον MUX στην κάμερα, ανάβει την κάμερα
 cam_snapshot(); //Παγώνει εικόνα στην RAM της κάμερας
 cam_open_file(""); //Ανοίγει αρχείο εικόνας της κάμερας
 #endif
}

void snap(int arg_cnt, char **args)
{
 #if USE_CAMERA
 camera_init(); //Γυρίζει τον MUX στην κάμερα, ανάβει την κάμερα
 cam_snapshot(); //Παγώνει εικόνα στην RAM της κάμερας
 cam_open_file("SNAP.JPG"); //Ανοίγει αρχείο εικόνας της κάμερας
 #endif
}

//Συντελεστής συμπίεσης 0x35 ή 53 δεκαδικό είναι το προκαθορισμένο
//0 -> χωρίς συμπίεση, 128, 200 (50%)
void piccomp(int arg_cnt, char **args)
{
 #if USE_CAMERA
 if (arg_cnt > 1)
     cam_chg_comp((byte)atoi(args[1]));
 else
     args_error();
 #endif
}

void getpicsz(int arg_cnt, char **args)
{
 #if USE_CAMERA
 cam_get_size();
 #endif
}

void setpicsz(int arg_cnt, char **args)
{
 #if USE_CAMERA
 if (arg_cnt > 1)
    {
     byte a = atoi(args[1]);
     camera_init(); //Γυρίζει τον MUX στην κάμερα, ανάβει την κάμερα
     switch (a)
       {
        case 1:
          cam.setImageSize(VC0706_640x480);
          break;
        case 2:
          cam.setImageSize(VC0706_320x240);
          break;
        case 3:
          cam.setImageSize(VC0706_160x120);
          break;
       }
      delay(200);
      cam_power_off();
      //delay(100);
      //camera_init();
    }
 else
     args_error();
 #endif
}

void campwr(int arg_cnt, char **args)
{
 #if USE_CAMERA
 if (arg_cnt > 1)
    {
     if(strcmp_P(args[1], PSTR("on")) == 0) 
        camera_init();
     else if(strcmp_P(args[1], PSTR("off")) == 0)
        cam_power_off();
    }
 else
     args_error();
 #endif
}

//--------------------------------- Εντολές GPS  ---------------------------------
void gpspwr(int arg_cnt, char **args)
{
 #if USE_GPS
 if (arg_cnt > 1)
    {
     if(strcmp_P(args[1], PSTR("on")) == 0) 
        gps_init();
     else if(strcmp_P(args[1], PSTR("off")) == 0)
        gps_power_off();
    }
 else
     args_error();
 #endif
}

//------------------------------ Εντολές Ανάκτησης  ------------------------------
void alarm(int arg_cnt, char **args)
{
 if (arg_cnt > 1)
    {
     byte n = atoi(args[1]);
     beep(n, 300);
    }
 else
     args_error();
}

//------------------------------ Εντολές ελέγχου επικοινωνίας ----------------------------
void rssitest(int arg_cnt, char **args)
{
 timer1.stop(timer1_event);
 int sat_sig;
 char cmd_resp[35];
 sat_sig = radio.RSSI;
 sprintf_P(cmd_resp, PSTR("Re:RSSI Ground -> Sat: %d dbm\n"), sat_sig);
 Tx_Packet(cmd_resp, 6, F("Response Packet N:"), 1);
 //radio.send(RECEIVER, cmd_resp, strlen(cmd_resp), false); //3 Αν το έστειλε και πήρε απάντηση (προσπαθεί 3 φορές), τότε να εμφανίσει μήνυμα για debug. 
 start_timer(timer1_int);
}

//Η εντολή στέλνει ένα πακέτο από τον δορυφόρο στη βάση και η βάση το στέλνει πίσω στον δορυφόρο. Εμφανίζει τον χρόνο σε ms
void ping(int arg_cnt, char **args)
{
 timer1.stop(timer1_event);
 //stop_job = true;
 //--------------------         111111111122222222223333333333444444444455555555556
 //------------------- 123456789012345678901234567890123456789012345678901234567890
 char ping_packet[] = "PngRq:0123456789abcdefghijklmnopqrsruvwxyz!@#$%^&*-+.ABCDEF\n";
 ping_time = millis();
 Tx_Packet(ping_packet, 1, F("Ping Request N:"), 1);
 //start_timer(timer1_int);
}

// sprintf_P(cmd_resp, PSTR("Re:Set pressure %ld as altitude 0 meters\n"), cur_pressure);
// Tx_Packet(cmd_resp, 6, F("Response Packet N:"), 1);

void txpic(int arg_cnt, char **args)
{
 timer1.stop(timer1_event);
 trans_pic();
}

void txlog(int arg_cnt, char **args)
{
 timer1.stop(timer1_event);
 trans_log();
}

void txphoto(int arg_cnt, char **args)
{
 timer1.stop(timer1_event);
 trans_pic_hq();
}


