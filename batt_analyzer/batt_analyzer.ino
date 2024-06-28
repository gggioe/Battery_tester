#include <arduino-timer.h>
#include <LiquidCrystal_I2C.h>

#define Safety_cutoff -1

#define set_b 2 // set menu button (INTERRUPT)

#define clk 12  // encoder pin
#define dt  13
#define psb 3   // INTERRUPT

#define SD1 4   // Shutdown pin
#define SD2 11
#define SD3 7
#define SD4 8

#define vp1 A0  // voltage pin
#define vp2 A1
#define vp3 A2
#define vp4 A3

#define ip1 5    // pwm pin
#define ip2 6 
#define ip3 9
#define ip4 10 

#define istep 0.1  // by default the current is incremented/decremented by 0.1A

Timer<4, millis> timer;

LiquidCrystal_I2C lcd(0x27, 16, 2);

unsigned int pclk;   //Encoder variables
unsigned int pdt;
unsigned int cclk;
unsigned int cdt;
uint8_t page;      
uint8_t i=0;
bool increment=false;
bool decrement=false;

volatile byte state = LOW;   // button state
bool set = false;


bool CH1 = false;  // Channel status, false= off , default off
bool CH2 = false;
bool CH3 = false;
bool CH4 = false;

uint8_t s1 = 0;   //sec
uint8_t s2 = 0;
uint8_t s3 = 0;
uint8_t s4 = 0;

uint8_t m1 = 0;   // min
uint8_t m2 = 0;
uint8_t m3 = 0;
uint8_t m4 = 0;

uint8_t h1 = 0;   // hour
uint8_t h2 = 0;
uint8_t h3 = 0;
uint8_t h4 = 0;

float Ah1 = 0.0;   // Ampere hour value
float Ah2 = 0.0;
float Ah3 = 0.0;
float Ah4 = 0.0;

float i1 = 1.0;    // default discharge current 1A
float i2 = 1.0;
float i3 = 1.0;
float i4 = 1.0;

uint8_t iv1;      // current value in 8 bit format
uint8_t iv2;
uint8_t iv3;
uint8_t iv4;

float v1;         // real voltage value
float v2;
float v3;
float v4;

uint16_t v_1;    // voltage value in 10 bit 
uint16_t v_2;
uint16_t v_3;
uint16_t v_4;

/////////////////////////////////////

bool calc(){

  v_1 = analogRead(vp1);
  v1 = v_1*(5/1024.0);
  
  v_2 = analogRead(vp2);
  v2 = v_2*(5/1024.0);
  
  v_3 = analogRead(vp3);
  v3 = v_3*(5/1024.0);
  
  v_4 = analogRead(vp4);
  v4 = v_4*(5/1024.0);

  if(CH1){
    
    s1++;
    iv1=(i1/(5/255));
    Ah1=Ah1+(i1/3600);

    if(s1>=60){            // if seconds >60
          s1=s1/60;
          m1++;             // add a minute
        }

      if(m1>=60){   
        m1=m1/60;
        h1++;
      }
  }

  if(CH2){
    s2++;
    iv2=(i2/(5/255));
    Ah2=Ah2+(i2/3600);

    if(s2>=60){            // if seconds >60
          s2=s2/60;
          m2++;             // add a minute
        }

      if(m2>=60){   
        m2=m2/60;
        h2++;
      }
  }

  if(CH3){
    s3++;
    iv3=(i3/(5/255));
    Ah3=Ah3+(i3/3600);

    if(s3>=60){            // if seconds >60
          s3=s3/60;
          m3++;             // add a minute
        }

      if(m3>=60){   
        m3=m3/60;
        h3++;
      }
  }

  if(CH4){
    s4++;
    iv4=(i4/(5/255));
    Ah4=Ah4+(i4/3600);

    if(s4>=60){            // if seconds >60
          s4=s4/60;
          m4++;             // add a minute
        }

      if(m4>=60){   
        m4=m4/60;
        h4++;
      }
  }

  return true;
}

/////////////////////////////////////

void out_set(){// set output on/off

  if(CH1){                   // if we want the channel to be on
    digitalWrite(SD1,LOW);   // mos off
  }
  else{
    digitalWrite(SD1,HIGH);  // mos on
  }

  if(CH2){
    digitalWrite(SD2,LOW);   // mos off
  }
  else{
    digitalWrite(SD2,HIGH);  // mos on
  }

  if(CH3){
    digitalWrite(SD3,LOW);   // mos off
  }
  else{
    digitalWrite(SD3,HIGH);  // mos on
  }

  if(CH4){
    digitalWrite(SD4,LOW);   // mos off
  }
  else{
    digitalWrite(SD4,HIGH);  // mos on
  }
}

/////////////////////////////////////

bool display(){// display refresh

  if(set){  // if the user press the set button display the set menu

    switch(page){

      case 0:            
      lcd.clear();
      lcd.print("CH1 SET ");  
      lcd.setCursor(0,1);
      lcd.print("I:");    
      lcd.print(i1);

      if(state){    // if the encoder button is pressed, edit the current channel
    
        if(increment){        // if an increment is detected
          i1=i1+istep;;       // increment i by a defined step
          increment=false;    // reset the variable when an increment is done
        }

        if(decrement){
          i1=i1-istep;;       // decrement i by a defined step
          decrement=false;    // reset the variable when a decrement is done
        }
      analogWrite(ip1,i1);    // write the set value
      }
      break;

      case 1:            
      lcd.clear();
      lcd.print("CH2 SET "); 
      lcd.setCursor(0,1); 
      lcd.print("I:");    
      lcd.print(i2);

      if(state){    // if the encoder button is pressed, edit the current channel
    
        if(increment){        // if an increment is detected
          i2=i2+istep;;       // increment i by a defined step
          increment=false;    // reset the variable when an increment is done
        }

        if(decrement){
          i2=i2-istep;;       // decrement i by a defined step
          decrement=false;    // reset the variable when a decrement is done
        }
      }
      analogWrite(ip2,i2);    // write the set value
      break;

      case 2:            
      lcd.clear();
      lcd.print("CH3 SET ");  
      lcd.setCursor(0,1);
      lcd.print("I:");    
      lcd.print(i3);

      if(state){    // if the encoder button is pressed, edit the current channel
    
        if(increment){        // if an increment is detected
          i3=i3+istep;;       // increment i by a defined step
          increment=false;    // reset the variable when an increment is done
        }

        if(decrement){
          i3=i3-istep;;       // decrement i by a defined step
          decrement=false;    // reset the variable when a decrement is done
        }
      }
      analogWrite(ip3,i3);    // write the set value
      break;

      case 3:            
      lcd.clear();
      lcd.print("CH4 SET ");
      lcd.setCursor(0,1);
      lcd.print("I:");    
      lcd.print(i4);

      if(state){    // if the encoder button is pressed, edit the current channel
    
        if(increment){        // if an increment is detected
          i4=i4+istep;;       // increment i by a defined step
          increment=false;    // reset the variable when an increment is done
        }

        if(decrement){
          i4=i4-istep;;       // decrement i by a defined step
          decrement=false;    // reset the variable when a decrement is done
        }
      }
      analogWrite(ip4,i4);    // write the set value
      break;
    }
  }

  else{     // otherwise show the info menu
    switch(page){

      case 0:
      lcd.clear();
      lcd.print("CH1 ");  // print ch number
      lcd.print("V:");    // print voltage
      lcd.print(v1);

      if(CH1){
        lcd.print(" ON");  // print status
      }
      else{
      lcd.print(" OFF");
      }
      
      lcd.setCursor(0,1);
      lcd.print("Ah:");      // print capacity
      lcd.print(Ah1,3);
      lcd.print(" ");

      if(h1>0){               //to save space in the lcd print hours only if >o
        lcd.print(h1); 
        lcd.print(":");
      }
      lcd.print(m1);        // print minutes

      if(h1<10){            // if the test runs for more than 10 hours in order to save space does not print seconds

        lcd.print(":");
        lcd.print(s1);        // print seconds
      }
      break;

      case 1:
      lcd.clear();
      lcd.print("CH2 ");  //print ch number
      lcd.print("V:");    // print voltage
      lcd.print(v2);

      if(CH2){
        lcd.print(" ON");  // print status
      }
      else{
      lcd.print(" OFF");
      }

      lcd.setCursor(0,1);
      lcd.print("Ah:");      // print capacity
      lcd.print(Ah2,3);
      lcd.print(" ");

      if(h2>0){               //to save space in the lcd print hours only if >o
        lcd.print(h2); 
        lcd.print(":");
      }
      lcd.print(m2);        // print minutes

      if(h2<10){            // if the test runs for more than 10 hours in order to save space does not print seconds

        lcd.print(":");
        lcd.print(s2);        // print seconds
      }
      break;

      case 2:
      lcd.clear();
      lcd.print("CH3 ");  //print ch number
      lcd.print("V:");    // print voltage
      lcd.print(v3);

      if(CH3){
        lcd.print(" ON");  // print status
      }
      else{
      lcd.print(" OFF");
      }

      lcd.setCursor(0,1);
      lcd.print("Ah:");      // print capacity
      lcd.print(Ah3,3);
      lcd.print(" ");

      if(h3>0){               //to save space in the lcd print hours only if >o
        lcd.print(h3); 
        lcd.print(":");
      }
      lcd.print(m3);        // print minutes

      if(h3<10){            // if the test runs for more than 10 hours in order to save space does not print seconds

        lcd.print(":");
        lcd.print(s3);        // print seconds
      }
      break;

      case 3:
      lcd.clear();
      lcd.print("CH4 ");  //print ch number
      lcd.print("V:");    // print voltage
      lcd.print(v4);

      if(CH4){
        lcd.print(" ON");  // print status
      }
      else{
      lcd.print(" OFF");
      }

      lcd.setCursor(0,1);
      lcd.print("Ah:");      // print capacity
      lcd.print(Ah4,3);
      lcd.print(" ");

      if(h4>0){               //to save space in the lcd print hours only if >o
        lcd.print(h4); 
        lcd.print(":");
      }
      lcd.print(m4);        // print minutes

      if(h4<10){            // if the test runs for more than 10 hours in order to save space does not print seconds

        lcd.print(":");
        lcd.print(s4);        // print seconds
      }
      break;
    }
  }

  return true;
}

/////////////////////////////////////

void button(){// encoder button interrupt handler

  state = !state;     //change state and

  if(!set){          // if we are not in the set menu 

    switch(page){        // check the page and then change the state of the channel if the voltage is above the safety threshold

    case 0:
      if(state && (v1 > Safety_cutoff)){
        CH1 = true;          
      }
      else{
        CH1 = false;           
      }
    break;

    case 1:
      if(state && (v2 > Safety_cutoff)){
        CH2 = true;          
      }
      else{
        CH2 = false;           
      }
    break;

    case 2:
      if(state && (v3 > Safety_cutoff)){
        CH3 = true;          
      }
      else{
        CH3 = false;           
      }
    break;

    case 3:
      if(state && (v4 > Safety_cutoff)){
        CH4 = true;          
      }
      else{
        CH4 = false;          
      }
    break;
    }
  }                  // otherwise if we are in the set menu just toggle the button state
}        

/////////////////////////////////////

void set_m(){// set button interrupt handler
           
  if(!set){         // if the set button is pressed and we are not in the set menu
    set=true;       // enter set menu
  }

  else{            
    set=false;      // otherwise exit from the set menu
  }
}

/////////////////////////////////////

void setup() {

  Serial.begin(9600);
  lcd.init();          //LCD init
  lcd.backlight();
  lcd.home();

  pinMode(SD1,OUTPUT); // Shutdown channel
  pinMode(SD2,OUTPUT);
  pinMode(SD3,OUTPUT);
  pinMode(SD4,OUTPUT);

  pinMode(vp1, INPUT); // voltage pin
  pinMode(vp2, INPUT);
  pinMode(vp3, INPUT);
  pinMode(vp4, INPUT);

  pinMode(clk,INPUT);  // Encoder init
  pinMode(dt,INPUT);
  pinMode(psb,INPUT);

  pclk = digitalRead(clk);  
  pdt = digitalRead(dt);
  page = 0;

  timer.every(200, out_set);
  timer.every(500, display);
  timer.every(100,calc);
  
  pinMode(psb, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(psb), button, FALLING);

  pinMode(set_b, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(set_b), set_m, FALLING);

}

void loop() {

  timer.tick();
  
  cclk = digitalRead(clk);
  cdt = digitalRead(dt);

  if (cclk != pclk) {  // encoder and page control
    if (cdt == cclk){
      if(i>=2){         // if the encoder is rotaded counter clockwise

        if(!state){     // and we are not in the edit ch mode 
           page --;     // switch page                                               

          if(page>3){
            page=3;
          }
          i=0;
        }

        else{        //otherwise we are in the edit menu, then use the encoder to decrement the value
          decrement=true;
        }
      }
    } 

    else { 
      if(i>=2){     

        if(!state){    // if we are not in the edit ch mode do the usual page stuff
          page ++;     
          if(page>3){
            page=0;
          }
          i=0;


        }
        else{
          increment=true;  //othewise use the encoder to increment the value
        }
      }
    }
    pclk = cclk;
    pdt = cdt;
    i++;
  }
}

/*TODO:

        4) aggiungere algoritmo di calcolo Wh ecc
        
        5) aggiungere pulsante di reset, quando ci troviamo in una pagina, se premuto resetta le variabili

*/       
