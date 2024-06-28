#include <arduino-timer.h>
#include <LiquidCrystal_I2C.h>

#define Safety_cutoff 3

#define set_b 2 // set menu button (INTERRUPT)

#define pwm_const 0.019607843

#define clk 11 // encoder pin
#define dt  12
#define psb 3   // INTERRUPT

#define SD1 4   // Shutdown pin
#define SD2 13
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
int page;      
uint8_t i=0;
bool increment=false;
bool decrement=false;

volatile byte state = LOW;   // button state
volatile byte edit = LOW;    // edit menu flag
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

bool calc(){// calculations

  iv1 = int(trunc(i1/pwm_const));
  v_1 = analogRead(vp1);
  v1 = v_1*(5/1024.0);
  
  iv2 = int(trunc(i2/pwm_const));
  v_2 = analogRead(vp2);
  v2 = v_2*(5/1024.0);
  
  iv3 = int(trunc(i3/pwm_const));
  v_3 = analogRead(vp3);
  v3 = v_3*(5/1024.0);
  
  iv4 = int(trunc(i4/pwm_const));
  v_4 = analogRead(vp4);
  v4 = v_4*(5/1024.0);

  if(CH1){
    
    s1++;
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
    analogWrite(ip1,iv1);     // write the value
  }
  else{
    digitalWrite(SD1,HIGH);  // mos on
    analogWrite(ip1,0);     // write the value
  }

  if(CH2){
    digitalWrite(SD2,LOW);   // mos off
    analogWrite(ip2,iv2);     // write the value
  }
  else{
    digitalWrite(SD2,HIGH);  // mos on
    analogWrite(ip2,0);     // write the value
  }

  if(CH3){
    digitalWrite(SD3,LOW);   // mos off
    analogWrite(ip3,iv3);     // write the value
  }
  else{
    digitalWrite(SD3,HIGH);  // mos on
    analogWrite(ip3,0);     // write the value
  }

  if(CH4){
    digitalWrite(SD4,LOW);   // mos off
    analogWrite(ip4,iv4);     // write the value
  }
  else{
    digitalWrite(SD4,HIGH);  // mos on
    analogWrite(ip4,0);     // write the value
  }
}

/////////////////////////////////////

bool display(){// display refresh

  if(set){  // if we are in the set menu

    switch(page){

      case 0:            
      lcd.clear();
      lcd.print("CH1 SET ");  
      lcd.setCursor(0,1);
      lcd.print("I:");    
      lcd.print(i1);
      break;

      case 1:            
      lcd.clear();
      lcd.print("CH2 SET "); 
      lcd.setCursor(0,1); 
      lcd.print("I:");    
      lcd.print(i2);
      break;

      case 2:            
      lcd.clear();
      lcd.print("CH3 SET ");  
      lcd.setCursor(0,1);
      lcd.print("I:");    
      lcd.print(i3);
      break;

      case 3:            
      lcd.clear();
      lcd.print("CH4 SET ");
      lcd.setCursor(0,1);
      lcd.print("I:");    
      lcd.print(i4);
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

  if(!set){          // if we are not in the set menu toggle channel on off 

    state = !state;     //change state
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
  }                  
  else{              // otherwise if we are in the set menu, toggle edit flag
    edit = !edit;
  }
}        

/////////////////////////////////////

void set_m(){// set button interrupt handler
           
  if(!set){         // if the set button is pressed and we are not in the set menu
    set=true;       // enter set menu
  }

  else{             
    set=false;      // exit from the set menu and if edit mode is still on turn it off otherwise when not in set/edit mode the user can still
    if(edit){       // change the current value by accident
      edit=LOW;
    }            
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

  if (cclk != pclk) { 
    if (cdt == cclk){  // counter clockwise
      if(i>=2){       // one time (2 pulse per tick)

        if(!edit){    // and we are not in the in the edit ch mode (only in info or set display)
          page --;    // decrement page number                                             

          if(page<0){
            page=3;
          }
        }

        else{              //otherwise we are in the edit menu so use the encoder to decrement the value
            switch(page){  // select the correct page and
          
              case 0:
                if(i1>0.101){ // if the value is above the minimum
                  i1=i1-istep;;   // decrement the value
                }
              break;

              case 1:
                if(i2>0.101){ // if the value is above the minimum
                  i2=i2-istep;;   // decrement the value
                }
              break;

              case 2:
                if(i3>0.101){ // if the value is above the minimum
                  i3=i3-istep;;   // decrement the value
                }
              break;

              case 3:
                if(i4>0.101){ // if the value is above the minimum
                  i4=i4-istep;;   // decrement the value
                }
              break;
            }
        }
        i=0;
      }
    }
    
    else{              // if encoder is rotated clockwise 
       if(i>=2){       // one time (2 pulse per tick)
        if(!edit){     // and we are not in the in the edit ch mode (only in info or set display)
          page ++;     // increment page number                                             

          if(page>3){
            page=0;
          }
        }

        else{              //otherwise we are in the edit menu so use the encoder to decrement the value
            switch(page){  // select the correct page and
          
              case 0:
                if(i1<=4.900){ // if the value is below the maximum
                  i1=i1+istep;;   // decrement the value
                }
              break;

              case 1:
                if(i2<=4.900){ 
                  i2=i2+istep;;   
                }
              break;

              case 2:
                if(i3<=4.900){ 
                  i3=i3+istep;  
                }
              break;

              case 3:
                if(i4<=4.900){ 
                  i4=i4+istep;
                }
              break;
            }
        }
        i=0;
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
