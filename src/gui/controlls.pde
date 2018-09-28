// ==============================
// Κλάσεις Button & ImageButtons
// ==============================
class Button
{
  int x, y;
  int w, h;
  color basecolor, highlightcolor;
  color currentcolor;
  boolean over = false;
  boolean pressed = false;   
  boolean k_pressed = false;
  
  void pressed() 
  {
   if(over && mousePressed) 
      pressed = true;
   else if (k_pressed)
      pressed = true;
   else 
      pressed = false;  
  }
  
  boolean overRect(int x, int y, int width, int height) 
  {
   if (mouseX >= x && mouseX <= x+width && mouseY >= y && mouseY <= y+height) 
       return true;
   else 
       return false;
  }
}

//Κλάση Κουμπιά με εικόνες
class ImageButtons extends Button 
{
  PImage base;
  PImage roll;
  PImage down;
  PImage currentimage;
  String text = "";
  
  //Κατασκευαστής
  ImageButtons(int ix, int iy, int iw, int ih, PImage ibase, PImage iroll, PImage idown, String itext) 
  {
    x = ix;
    y = iy;
    w = iw;
    h = ih;
    base = ibase;
    roll = iroll;
    down = idown;
    currentimage = base;
    text = itext;
  }
  
  void update() 
  {
    //println(mouseX);
   over();
   pressed();
   if(pressed) 
     {
      currentimage = down;
      fill(255,0,0);
     }
   else if (over)
      currentimage = roll;
   else 
      {
       currentimage = base;
        fill(255);
      }
  }
  
  void over() 
  {
   if( overRect(x, y, w, h) ) 
      over = true;
   else
      over = false;
  }
  
  void display() 
  {
    image(currentimage, x, y);
    textSize(15); 
    //fill(255); 
    textAlign(CENTER, CENTER);
    text(text, x+16, y+12); 
  }
}

class Label
{
 int x, y, w, h;
 String Text;
 //Κατασκευαστής
 Label(int ix, int iy, int iw, int ih, String is)
   {
    x = ix;
    y = iy;
    w = iw;
    h = ih;
    Text = is;
   }
 void display() 
   {
    textSize(15); 
    fill(90); 
    strokeWeight(1);
    stroke(255, 255, 255);
    rect(x, y, w, h); 
    fill(255); 
    text(Text, x+5, y+3, w, h); 
   }
}

class Radio
{
  int x, y;
  int d;
  boolean selected;
  color basecolor, highlightcolor;
  color currentcolor;
  boolean over = false;
  boolean pressed = false;   
  boolean k_pressed = false;
  
  void pressed() 
  {
   if(over && mousePressed && !selected) 
      pressed = true;
   //else if (k_pressed)
   //   pressed = true;
   else if (!selected)
      pressed = false;  
  }
  
  boolean overCircle(int x, int y, int diameter) 
  {
   float disX = x+ (diameter / 2) - mouseX;
   float disY = y + (diameter / 2) - mouseY;
   if (sqrt(sq(disX) + sq(disY)) < diameter/2 ) 
       return true;
   else 
       return false;
   }  
}

//Κλάση Κουμπιά με εικόνες
class ImageRadio extends Radio 
{
  PImage base;
  PImage roll;
  PImage down;
  PImage currentimage;
  String text = "";
  //Κατασκευαστής
  ImageRadio(int ix, int iy, int id, PImage ibase, PImage iroll, PImage idown, String itext) 
  {
    x = ix;
    y = iy;
    d = id;
    base = ibase;
    roll = iroll;
    down = idown;
    currentimage = base;
    text = itext;
  }
  
  void update() 
  {
    //println(mouseX);
   over();
   pressed();
   if(pressed) 
      currentimage = down;
   else if (over)
      currentimage = roll;
   else 
      currentimage = base;
  }
  
  void over() 
  {
   if( overCircle(x, y, d) ) 
      over = true;
   else
      over = false;
  }
  
  void display() 
  {
    image(currentimage, x, y);
    textSize(15); 
    fill(255); 
    textAlign(CENTER, CENTER);
    text(text, x+16, y+12); 
  }
}

//Έλεγχος πλήκτρων πληκτρολογίου
boolean Ctrl_pressed = false;
void keyPressed() 
{ //Βελάκια
  if (key == CODED) //Αν πατήθηκαν ειδικά πλήκτρα π.χ. βελάκια ή Home, End κλπ.
     {
      //println(keyCode);
      if (keyCode == LEFT) 
         if (Ctrl_pressed)
            {
             Ctrl_pressed = false;
             left_full_Button.k_pressed = true;
            }
         else
            left_arrow_Button.k_pressed = true;
      else if (keyCode == RIGHT) 
         if (Ctrl_pressed)
            {
             Ctrl_pressed = false;
             right_full_Button.k_pressed = true;
            }
         else
            right_arrow_Button.k_pressed = true;
      else if (keyCode == UP) 
         if (Ctrl_pressed)
            {
             Ctrl_pressed = false;
             pitch_up_Button.k_pressed = true;
            }
         else
          up_arrow_Button.k_pressed = true;
      else if (keyCode == DOWN)
         if (Ctrl_pressed)
            {
             Ctrl_pressed = false;
             pitch_down_Button.k_pressed = true;
            }
         else
          down_arrow_Button.k_pressed = true;
      else if (keyCode == 17)
          Ctrl_pressed = true;
     } 
  else //Αν πατήθηκε πλήκτρο με χαρακτήρα ASCII
     {
      if (key == '+')
         {
          zoom += 1;
          //map_must_update = true;
          map_update();
         }
      else if (key == '-')
         {
          zoom -= 1;
          //map_must_update = true;
          map_update();
         }
       else if (key == 'p' || key == 'P')
         {
          snap_Button.k_pressed = true;
         }
      else if (key == 'x')
         {
          cansat_data.flush();
          cansat_data.close();
          position_data.flush();
          position_data.close();
          exit();
         }
     }
}

//Αφήθηκε το πλήκτρο
void keyReleased() 
{
  left_arrow_Button.k_pressed = false;
  right_arrow_Button.k_pressed = false;
  up_arrow_Button.k_pressed = false;
  down_arrow_Button.k_pressed = false;
  left_full_Button.k_pressed = false;
  right_full_Button.k_pressed = false;
  pitch_up_Button.k_pressed = false;
  pitch_down_Button.k_pressed = false;
  snap_Button.k_pressed = false;
}