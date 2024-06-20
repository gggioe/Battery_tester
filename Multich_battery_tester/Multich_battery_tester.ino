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

bool start = FALSE;
bool start = FALSE;

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

  digitalWrite(SD1, HIGH);  // At startup, all channel off
  digitalWrite(SD2, HIGH);
  digitalWrite(SD3, HIGH);
  digitalWrite(SD4, HIGH);
}

void loop() {

  // task 1 acquisisce tensioni ogni x ms
  // task 2 aggiorna schermo
  // task 3 acquisisce pulsanti + pulsante encoder

  // solo quando nel menu set, setta correnti
  // solo quando start, starta ch x
  // solo quando stop, starta ch x

  // con encoder scorre le pagine/cursore dei canali e con il pulsante seleziona pagina/impostazione es: varia tensione/corrente, 
  

  
}
