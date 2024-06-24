#include <LiquidCrystal_I2C.h>
#include <arduino-timer.h>

auto timer = timer_create_default();

LiquidCrystal_I2C lcd(0x27, 16, 2);

#define v1 A0  // voltage readback pin def
#define v2 A1
#define v3 A2
#define v4 A3

#define i1 3 // Current set pin def
#define i2 5
#define i3 6
#define i4 9

#define SD1 2 // Shutdown pin def
#define SD2 4
#define SD3 7
#define SD4 8

#define start 10  // Misc def
#define stop 11
#define Safety_cutoff 2.5

bool start_b = false;   //Var init
bool stop_b = false;

float v_1, v_2, v_3, v_4;

bool ch1 = false;    // at startup all channel off (false)
bool ch2 = false;
bool ch3 = false;
bool ch4 = false;


void setup() {

  Serial.begin(9600);
  
  pinMode(v1,INPUT);   // Voltage readback
  pinMode(v2,INPUT);
  pinMode(v3,INPUT);
  pinMode(v4,INPUT);
  
  pinMode(i1,OUTPUT);  // Current set
  pinMode(i2,OUTPUT);
  pinMode(i3,OUTPUT);
  pinMode(i4,OUTPUT);

  pinMode(SD1,OUTPUT); // Shutdown channel
  pinMode(SD2,OUTPUT);
  pinMode(SD3,OUTPUT);
  pinMode(SD4,OUTPUT);

  pinMode(start,INPUT); // Misc pin 
  pinMode(stop,INPUT);

  lcd.init();          //LCD init
  lcd.backlight();
  lcd.home();

  timer.every(5, v_read); //Voltage Read
  timer.every(10, calculate); //Calculate the required parameter     aggiustare i tempi di read e calc in modo da poter fare una media di 3/4 valori letti
  timer.every(20, set_out); //Output on/off
  timer.every(200, display); //Display refresh
  timer.every(100, i_o_acq); //IO acquisition         
}


void v_read(){ // read the value and convert it into a voltage  

  v_1 = analogRead(v1)*(5/1024);  
  v_2 = analogRead(v2)*(5/1024);     //aggiungere magari un filtro sw che fa una media temporale dei valori?
  v_3 = analogRead(v3)*(5/1024);     //   pls
  v_4 = analogRead(v4)*(5/1024);

  if(v_1 < Safety_cutoff){     //if readed voltage is below the safety threshold turn off the channel

    ch1=false;
  }
  if(v_2 < Safety_cutoff){
    
    ch2=false;
  }
  if(v_3 < Safety_cutoff){
    
    ch3=false;
  }
  if(v_4 < Safety_cutoff){
    
    ch4=false;
  }
}


void set_out(bool ch1, bool ch2, bool ch3, bool ch4){   //periodically refresh the status of the output

  if (ch1){
    digitalWrite(SD1, LOW);
  }
    else{
      digitalWrite(SD1, HIGH);
    }

  if (ch2){
    digitalWrite(SD2, LOW);
  }
    else{
      digitalWrite(SD2, HIGH);
    }

  if (ch3){
    digitalWrite(SD3, LOW);
  }
    else{
      digitalWrite(SD3, HIGH);
    }

  if (ch4){
    digitalWrite(SD4, LOW);
  }
    else{
      digitalWrite(SD4, HIGH);
    }
}

void display(){


}

void i_o_acq(){


}





void loop() {

  timer.tick();  
}

  // task 3 acquisisce pulsanti + pulsante encoder

  // solo quando nel menu set, setta correnti
  // solo quando start, starta ch x
  // solo quando stop, starta ch x

  // con encoder scorre le pagine/cursore dei canali e con il pulsante seleziona pagina/impostazione es: varia tensione/corrente, 
  
