#include <arduino-timer.h>
#include <LiquidCrystal_I2C.h>

#define Safety_cutoff 2.8

#define clk 10  // encoder pin
#define dt 11
#define psb 3

#define SD1 2   // Shutdown pin
#define SD2 4
#define SD3 7
#define SD4 8

#define vp1 A0  // voltage pin
#define vp2 A1
#define vp3 A2
#define vp4 A3

#define SD1 2   // Shutdown pin
#define SD2 4
#define SD3 7
#define SD4 8

Timer<4, millis> timer;

LiquidCrystal_I2C lcd(0x27, 16, 2);

unsigned int pclk;   //Encoder variables
unsigned int pdt;
unsigned int cclk;
unsigned int cdt;
uint8_t page;       
uint8_t i=0;

volatile byte state = LOW;   // button state

bool CH1 = false;  // Channel status, false= off , default off
bool CH2 = false;
bool CH3 = false;
bool CH4 = false;

uint32_t t1 = 0;   // time value in sec
uint32_t t2 = 0;
uint32_t t3 = 0;
uint32_t t4 = 0;

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
  timer.every(1000,calc);
  
  pinMode(psb, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(psb), button, FALLING);

}

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
    
    t1++;
    iv1=(i1/(5/255));
    Ah1=Ah1+(i1/3600);
  }

  if(CH2){
    t2++;
    iv2=(i2/(5/255));
    Ah2=Ah2+(i2/3600);
  }

  if(CH3){
    t3++;
    iv3=(i3/(5/255));
    Ah3=Ah3+(i3/3600);
  }

  if(CH4){
    t4++;
    iv4=(i4/(5/255));
    Ah4=Ah4+(i4/3600);
  }

  return true;
}

/////////////////////////////////////

void display(){// display refresh

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
      lcd.print(" T:");      // print time
      lcd.print(t1);
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
      lcd.print(" T:");      // print time
      lcd.print(t2);
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
      lcd.print(" T:");      // print time
      lcd.print(t3);
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
      lcd.print(" T:");      // print time
      lcd.print(t4);
    break;
  }
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

void button(){// button interrupt handler

  state = !state;     //change state

  switch(page){        // check the page and then change the state of it if the voltage is above the safety threshold

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

/////////////////////////////////////


void loop() {

  timer.tick();
  
  cclk = digitalRead(clk);
  cdt = digitalRead(dt);

  if (cclk != pclk) {  // encoder and page control
    if (cdt == cclk){
      if(i>=2){

        page --;

        if(page>3){
          page=3;
        }
        i=0;
      }
    } 

    else {
      if(i>=2){
        page ++;
        if(page>3){
          page=0;
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


        6) implementare menù set -> per ogni canale se premi a lungo puoi settare i parametri e cliccando torni indietro

        3) nel menù, quando si imposta una corrente la setta in automatico

        4) aggiungere algoritmo di calcolo Ah, Wh ecc
        
        5) aggiungere pulsante di reset, quando ci troviamo in una pagina, se premuto resetta le variabili

        7) cambiare display tempo 



*/       
