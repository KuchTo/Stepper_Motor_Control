/*  Einfache Schrittmotor Steuerung 
 *  von Tobias Kuch 2020 https://github.com/kuchto tobias.kuch@googlemail.com
 *  
 *  Version: 1.0 
 *   
 *  Funktion: 
 *  Steuert die Geschwindigkeit eines Schrittmotor im Full Step Modus im Frequenzbereich von 241 bis 980 Hz.
 *  Bei einem Schrittmotor mit einem Schrittgrad von 1,8 Grad und von 200 Schritten pro Umdrehung ergibt sich eine
 *  Geschwindigkeitseinstellbarkeit von min. 72 U/Min bis max. 294 U/Min
 *  
 *  Verwendete Hardware: 
 *    - Arduino Nano V3 
 *    - Potentiometer (10KOhm) 
 *    - externe Spannungsversorgung 5V und 12 Volt für Steppermotor
 *    - Steppermotor
 *    - Stepper Motor IC
 *    
 *  Verwendete Bibliotheken: 
 *    - keine
 *   
 */ 


#define MOTORDIRPIN 7 
#define MOTORSPEED 8 
#define MOTORENABLE 6 
#define A4988MS1 10
#define A4988MS2 11 
#define A4988MS3 12 

#define SPEEDCONTROL A0

byte valuea = 0;
byte oldvaluea = 0;


ISR(TIMER1_COMPA_vect){ //Timer1 interrupt. Schaltet Pin 8 um.
 // Weitere Infos auf: https://www.mikrocontroller.net/articles/AVR-Tutorial:_Timer
 digitalWrite(MOTORSPEED,!(digitalRead(MOTORSPEED)));// Aufruffrequenz maximal: 2.604 Khz 
 //Frequenz an Pin 8(Hz) = (Arduino clock speed 16,000,000Hz) / (prescaler * (compare match register + 1)) / 2
}

void setup() {
 pinMode(MOTORSPEED, OUTPUT);
 pinMode(MOTORDIRPIN, OUTPUT);
 pinMode(MOTORENABLE, OUTPUT);
 pinMode(A4988MS1, OUTPUT);
 pinMode(A4988MS2, OUTPUT);
 pinMode(A4988MS3, OUTPUT);
 digitalWrite(A4988MS1,LOW); // FullStep 
 digitalWrite(A4988MS2,LOW); // FullStep 
 digitalWrite(A4988MS3,LOW); // FullStep 
 digitalWrite(MOTORENABLE,LOW);
 cli();                                //stoppe alle Interrupts
 TCCR1A = 0;                           // set entire TCCR1A register to 0 TCCR - Timer/Counter Control Register
 TCCR1B = 0;                           // Setze Timer/Counter Control Register TCCR1B auf 0
 TCCR1B |= (1 << WGM12);               // Schalte Clear Timer on Compare (CTC) Modus ein
 // TCCR1B |= (1 << CS12) | (1 << CS10);  // Setze CS10 und CS12 Bit auf 1 für den 1024 Prescaler. Maximalfrequenz: 7.812 Khz
 TCCR1B |= (1 << CS12);                 // Setze CS12 Bit auf 1 für den 256 Prescaler. 
 TCNT1  = 0;                           // Initialisiere Zähler/Zeitgeber Register Wert auf 0
 OCR1A = 130;//  Aufruffrequenz Timer 1  241 Hz * 2
 TIMSK1 |= (1 << OCIE1A); // Erlaube Timer compare interrupt TIMSK - Timer/Counter Interrupt Mask Register
 sei();//allow interrupts
}

void loop() { // Hauptschleife - Abfragen des Potentiometers
 int Gb = analogRead(SPEEDCONTROL);
 byte valuea = map(Gb,0,1023,31,130);
 if (valuea != oldvaluea)
  {
  oldvaluea = valuea ;
  cli();//stop interrupts
  OCR1A = valuea;
  if ( TCNT1 > OCR1A )
    {
    TCNT1 = OCR1A -1;//initialize counter value to 0
    }
  sei();//allow interrupts
  }
}
