// Image button. 
// Loading images and using them to create a button. 
//int buttonSize = 34;
int buttonX = 46;
int buttonY = 42;
ImageButtons left_arrow_Button;
ImageButtons right_arrow_Button;
ImageButtons up_arrow_Button;
ImageButtons down_arrow_Button;
ImageButtons left_full_Button;
ImageButtons right_full_Button;
ImageButtons pitch_up_Button;
ImageButtons pitch_down_Button;
ImageButtons alarm_Button;
ImageButtons snap_Button;
ImageButtons txlog_Button;

ImageButtons sz1_Button;
ImageButtons sz2_Button;
ImageButtons photo_Button;
ImageButtons txphoto_Button;

ImageRadio dist1m_radio;
ImageRadio dist2m_radio;
ImageRadio dist3m_radio;
ImageRadio dist5m_radio;

ImageRadio sampInt05_radio;
ImageRadio sampInt1_radio;
ImageRadio sampInt2_radio;
ImageRadio sampInt4_radio;

ImageRadio chute_radio;

Label label1, label2, label3, label4, label5, label6, label7;

PImage left_arrow_Norm;
PImage left_arrow_Over;
PImage left_arrow_Select;
PImage right_arrow_Norm;
PImage right_arrow_Over;
PImage right_arrow_Select;
PImage up_arrow_Norm;
PImage up_arrow_Over;
PImage up_arrow_Select;
PImage down_arrow_Norm;
PImage down_arrow_Over;
PImage down_arrow_Select;
PImage radio_Norm;
PImage radio_Over;
PImage radio_Select;
PImage rect_radio_Norm;
PImage rect_radio_Over;
PImage rect_radio_Select;
PImage snap_Norm;
PImage snap_Over;
PImage snap_Select;
PImage blank_Norm;
PImage blank_Over;
PImage chute_radio_Norm;
PImage chute_radio_Over;
PImage chute_radio_Select;
PImage alarm_Norm;
PImage alarm_Over;
PImage alarm_Select;

void CreateUI()
{
 left_arrow_Norm = loadImage("left.png");
 left_arrow_Over = loadImage("leftOver.png");
 left_arrow_Select = loadImage("leftSel.png");
 right_arrow_Norm = loadImage("right.png");
 right_arrow_Over = loadImage("rightOver.png");
 right_arrow_Select = loadImage("rightSel.png");
 up_arrow_Norm = loadImage("up.png");
 up_arrow_Over = loadImage("upOver.png");
 up_arrow_Select = loadImage("upSel.png");
 down_arrow_Norm = loadImage("down.png");
 down_arrow_Over = loadImage("downOver.png");
 down_arrow_Select = loadImage("downSel.png");
 blank_Norm = loadImage("blank.png");
 blank_Over = loadImage("blankOver.png");
 //radio_Norm = loadImage("radio.png");
 //radio_Over = loadImage("radioOver.png");
 //radio_Select = loadImage("radioSel.png");
 radio_Norm = loadImage("rectRadio.png");
 radio_Over = loadImage("rectRadioOver.png");
 radio_Select = loadImage("rectRadioSel.png");
 rect_radio_Norm = loadImage("rectRadio.png");
 rect_radio_Over = loadImage("rectRadioOver.png");
 rect_radio_Select = loadImage("rectRadioSel.png");
 snap_Norm = loadImage("snap.png");
 snap_Over = loadImage("snapOver.png");
 snap_Select = loadImage("snapSel.png");
 chute_radio_Norm = loadImage("chute.png");
 chute_radio_Over = loadImage("chuteOver.png");
 chute_radio_Select = loadImage("chuteSelect.png");
 alarm_Norm = loadImage("alarm.png");
 alarm_Over = loadImage("alarmOver.png");
 alarm_Select = loadImage("alarmSel.png");
  
 int left_offs = 615; int top_offs = 410; 
 
 left_arrow_Button = new ImageButtons(left_offs + 0, top_offs + 50, buttonX, buttonY, left_arrow_Norm, left_arrow_Over, left_arrow_Select, "");
 right_arrow_Button = new ImageButtons(left_offs + 120,top_offs + 50, buttonX, buttonY, right_arrow_Norm, right_arrow_Over, right_arrow_Select, "");
 up_arrow_Button = new ImageButtons(left_offs + 60, top_offs + 0, buttonX, buttonY, up_arrow_Norm, up_arrow_Over, up_arrow_Select, "");
 down_arrow_Button = new ImageButtons(left_offs + 60, top_offs + 100, buttonX, buttonY, down_arrow_Norm, down_arrow_Over, down_arrow_Select, "");
 left_full_Button = new ImageButtons(left_offs + 49, top_offs + 57, 30, 28, blank_Norm, blank_Over, blank_Over, "<<");
 right_full_Button = new ImageButtons(left_offs + 87, top_offs + 57, 30, 28, blank_Norm, blank_Over, blank_Over, ">>");
 pitch_up_Button = new ImageButtons(left_offs + 126, top_offs + 6, 30, 28, blank_Norm, blank_Over, blank_Over, "Up");
 pitch_down_Button = new ImageButtons(left_offs + 126, top_offs + 106, 30, 28, blank_Norm, blank_Over, blank_Over, "Dn");
 alarm_Button = new ImageButtons(left_offs + 176, top_offs + 50, 60, 55, alarm_Norm, alarm_Over, alarm_Select, "");
 txlog_Button = new ImageButtons(left_offs + -240, top_offs + 130, 30, 28, blank_Norm, blank_Over, blank_Over, "Tx");
 
 
 sz1_Button = new ImageButtons(left_offs + -210, top_offs - 260, 30, 28, blank_Norm, blank_Over, blank_Over, "640");
 sz2_Button = new ImageButtons(left_offs + -210, top_offs - 220, 30, 28, blank_Norm, blank_Over, blank_Over, "160");
 photo_Button = new ImageButtons(left_offs + -210, top_offs - 300, 30, 28, blank_Norm, blank_Over, blank_Over, "Ph");
 txphoto_Button = new ImageButtons(left_offs + -210, top_offs - 360, 30, 28, blank_Norm, blank_Over, blank_Over, "Tx");
 
 
 dist1m_radio = new ImageRadio(left_offs + 10, top_offs - 50, 32, radio_Norm, radio_Over, radio_Select, "1m");
 dist2m_radio = new ImageRadio(left_offs + 50, top_offs - 50, 32, radio_Norm, radio_Over, radio_Select, "2m");
 dist3m_radio = new ImageRadio(left_offs + 90, top_offs - 50, 32, radio_Norm, radio_Over, radio_Select, "3m");
 dist5m_radio = new ImageRadio(left_offs + 130, top_offs - 50, 32, radio_Norm, radio_Over, radio_Select, "5m");
 dist1m_radio.selected = true;
 dist1m_radio.pressed = true;
 
 sampInt05_radio = new ImageRadio(left_offs - 400, top_offs + 128, 32, rect_radio_Norm, rect_radio_Over, rect_radio_Select, "0,5");
 sampInt1_radio = new ImageRadio(left_offs - 365, top_offs + 128, 32, rect_radio_Norm, rect_radio_Over, rect_radio_Select, "1");
 sampInt2_radio = new ImageRadio(left_offs - 330, top_offs + 128, 32, rect_radio_Norm, rect_radio_Over, rect_radio_Select, "2");
 sampInt4_radio = new ImageRadio(left_offs - 295, top_offs + 128, 32, rect_radio_Norm, rect_radio_Over, rect_radio_Select, "4");
 sampInt1_radio.selected = true;
 sampInt1_radio.pressed = true;
 
 chute_radio = new ImageRadio(left_offs + 176, top_offs - 12, 58, chute_radio_Norm, chute_radio_Over, chute_radio_Select, "");
 
 label1 = new Label(5, 10, 225, 25, "Θερμοκρασία 1 :");
 label2 = new Label(5, 38, 225, 25, "Πίεση: ");
 label3 = new Label(5, 66, 225, 25, "Υψόμετρο :");
 label4 = new Label(5, 94, 225, 25, "Υγρασία :");
 label5 = new Label(5, 122, 225, 25, "Θερμοκρασία 2 :");
 label6 = new Label(275, 20, 55, 25, "V");
 label7 = new Label(240, 88, 75, 25, "dbm");
 
 left_offs = 440; top_offs = 220; 
 snap_Button = new ImageButtons(left_offs + 00, top_offs + 5, buttonX, buttonY, snap_Norm, snap_Over, snap_Select, "");
}

void UpdateControls()
{
  fill(#F7A641);
  stroke(255);
  rect(610, 395, 174, 170, 10);
  buttons_events();
  distance_control();  
  sampint_control();
  chute_control();
}

void buttons_events()
{
  left_arrow_Button.update();
  left_arrow_Button.display();
  if(left_arrow_Button.over)
    {
     //println("lover");
    }
  if(left_arrow_Button.pressed)
    {
     port.write("lt\r");
     delay(200);
    }
    
  right_arrow_Button.update();
  right_arrow_Button.display();
  if(right_arrow_Button.over)
    {
     //println("rover");
    }
  if(right_arrow_Button.pressed)
    {
     port.write("rt\r");
     delay(200);
    }
    
  up_arrow_Button.update();
  up_arrow_Button.display();
  if(up_arrow_Button.over)
    {
     //println("urover");
    }
  if(up_arrow_Button.pressed)
    {
     port.write("fd " + fd_step + "\r");
     delay(200);
     dist1m_radio.selected = true;
     dist1m_radio.pressed = true;
    }
    
  down_arrow_Button.update();
  down_arrow_Button.display();
  if(down_arrow_Button.over)
    {
     //println("down-over");
    }
  if(down_arrow_Button.pressed)
    {
     port.write("bk\r");
     delay(200);
    }
  
  left_full_Button.update();
  left_full_Button.display();
  if(left_full_Button.over)
    {
     //println("down-over");
    }
  if(left_full_Button.pressed)
    {
     port.write("flt 90\r");
     delay(300);
    }
    
  right_full_Button.update();
  right_full_Button.display();
  if(right_full_Button.over)
    {
     //println("down-over");
    }
  if(right_full_Button.pressed)
    {
     port.write("frt 90\r");
     delay(300);
    }
    
  pitch_up_Button.update();
  pitch_up_Button.display();
  if(pitch_up_Button.over)
    {
     //println("down-over");
    }
  if(pitch_up_Button.pressed)
    {
     port.write("pitchtrim up\r");
     delay(300);
    }
    
  pitch_down_Button.update();
  pitch_down_Button.display();
  if(pitch_down_Button.over)
    {
     //println("down-over");
    }
  if(pitch_down_Button.pressed)
    {
     port.write("pitchtrim down\r");
     delay(300);
    }
    
  alarm_Button.update();
  alarm_Button.display();
  if(alarm_Button.over)
    {
     //println("lover");
    }
  if(alarm_Button.pressed)
    {
     port.write("alarm 10\r");
     delay(300);
    }
    
  txlog_Button.update();
  txlog_Button.display();
  if(txlog_Button.over)
    {
     //println("lover");
    }
  if(txlog_Button.pressed)
    {
     img_data = "";
     port.write("txlog\r");
     txlogState = 1;
     delay(300);
    }
    
  snap_Button.update();
  snap_Button.display();
  if(snap_Button.over)
    {
     //println("snap-over");
    }
  if(snap_Button.pressed)
    {
     img_data = "";
     port.write("snap\r");
     snapState = 1;
     delay(300);
    }
    
  sz1_Button.update();
  sz1_Button.display();
  if(sz1_Button.over)
    {
     //println("snap-over");
    }
  if(sz1_Button.pressed)
    {
     port.write("setpicsz 1\r");
     delay(300);
    }
    
  sz2_Button.update();
  sz2_Button.display();
  if(sz2_Button.over)
    {
     //println("snap-over");
    }
  if(sz2_Button.pressed)
    {
     port.write("setpicsz 3\r");
     delay(300);
    }
    
  photo_Button.update();
  photo_Button.display();
  if(photo_Button.over)
    {
     //println("snap-over");
    }
  if(photo_Button.pressed)
    {
     port.write("pic\r");
     delay(300);
    }
    
  txphoto_Button.update();
  txphoto_Button.display();
  if(txphoto_Button.over)
    {
     //println("snap-over");
    }
  if(txphoto_Button.pressed)
    {
     img_data = "";
     port.write("txphoto\r");
     photoState = 1;
     delay(300);
    }
}

void distance_control()
{
 //Radio buttons  
  dist1m_radio.update();
  dist1m_radio.display();
 
  if(dist1m_radio.pressed)
    {
     fd_step = 1;
     dist1m_radio.selected = true;
     dist2m_radio.selected = false;
     dist3m_radio.selected = false;
     dist5m_radio.selected = false;
    }
    
  dist2m_radio.update();
  dist2m_radio.display();
  if(dist2m_radio.pressed)
    {
     fd_step = 2;
     dist1m_radio.selected = false;
     dist2m_radio.selected = true;
     dist3m_radio.selected = false;
     dist5m_radio.selected = false;
    }
    
  dist3m_radio.update();
  dist3m_radio.display();
  if(dist3m_radio.pressed)
    {
      fd_step = 3;
     dist1m_radio.selected = false;
     dist2m_radio.selected = false;
     dist3m_radio.selected = true;
     dist5m_radio.selected = false;
    }
    
  dist5m_radio.update();
  dist5m_radio.display();
  if(dist5m_radio.pressed)
    {
     fd_step = 5;
     dist1m_radio.selected = false;
     dist2m_radio.selected = false;
     dist3m_radio.selected = false;
     dist5m_radio.selected = true;
    }
}

byte cur_sampInt = 2, last_sampInt = 2;
void sampint_control()
{
  if (cur_sampInt != last_sampInt)
     {
      sampintOnce = false;
      last_sampInt = cur_sampInt;
     }
  sampInt05_radio.update();
  sampInt05_radio.display();
  if(sampInt05_radio.pressed)
    {
     if (!sampintOnce)
        {
         port.write("sampint 2000\r");
         sampintOnce = true;
        }
     sampInt05_radio.selected = true;
     sampInt1_radio.selected = false;
     sampInt2_radio.selected = false;
     sampInt4_radio.selected = false;
     cur_sampInt = 1;
    }
  
  sampInt1_radio.update();
  sampInt1_radio.display();
  if(sampInt1_radio.pressed)
    {
     if (!sampintOnce)
        {
         port.write("sampint 1000\r");
         sampintOnce = true;
        }
     sampInt05_radio.selected = false;
     sampInt1_radio.selected = true;
     sampInt2_radio.selected = false;
     sampInt4_radio.selected = false;
     cur_sampInt = 2;
    }
    
  sampInt2_radio.update();
  sampInt2_radio.display();
  if(sampInt2_radio.pressed)
    {
     if (!sampintOnce)
        {
         port.write("sampint 500\r");
         sampintOnce = true;
        }
     sampInt05_radio.selected = false;
     sampInt1_radio.selected = false;
     sampInt2_radio.selected = true;
     sampInt4_radio.selected = false;
     cur_sampInt = 3;
    }
    
  sampInt4_radio.update();
  sampInt4_radio.display();
  if(sampInt4_radio.pressed)
    {
     if (!sampintOnce)
        {
         port.write("sampint 250\r");
         sampintOnce = true;
        }
     sampInt05_radio.selected = false;
     sampInt1_radio.selected = false;
     sampInt2_radio.selected = false;
     sampInt4_radio.selected = true;
     cur_sampInt = 4;
    }
}

void chute_control()
{
  chute_radio.update();
  chute_radio.display();
 
  if(chute_radio.pressed)
    {
     if (!chuteOnce)
        {
         port.write("rel\r");
         chuteOnce = true;
        }
     chute_radio.selected = true;
    }
}