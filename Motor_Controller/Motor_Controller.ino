int enA = 7;
int in1 = 2;
int in2 = 3;
int input = A1;
int output = A0;
float r = 0;
float y = 0;
float e = 0;
float MAPr = 0;
float MAPy = 0;
float MAPe =0;
int motorOutput = 0;
float Kp = 9;
float Ki = 0;
float Kd = 0;
double drerror = 0;

double elapsedTime;
double cumError;
double currentTime;
double previousTime;



void setup() {
  // put your setup code here, to run once:
  pinMode(enA, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(input, INPUT);
  pinMode(output, INPUT);
  Serial.begin(9600);
  cli();//stop interrupts

  //set timer1 interrupt at 1Hz
  TCCR1A = 0;// set entire TCCR1A register to 0
  TCCR1B = 0;// same for TCCR1B
  TCNT1  = 0;//initialize counter value to 0
  // set compare match register for 1hz increments
  OCR1A = 1000;// = (16*10^6) / (1*1024) - 1 (must be <65536)
  // turn on CTC mode
  TCCR1B |= (1 << WGM12);
  // Set CS12 and CS10 bits for 1024 prescaler
  TCCR1B |= (1 << CS12) | (1 << CS10); 
  // enable timer compare interrupt
  TIMSK1 |= (1 << OCIE1A);


  sei();//allow interrupts
  
}
// Interrupt routine

void motorControl(float error)
{
  if(error == 0.0)
  {
    analogWrite(enA , motorOutput);
  }
  else if(error < 0.0)
  {
      digitalWrite(in1, HIGH);
      digitalWrite(in2, LOW);
      analogWrite(enA ,motorOutput);
  }
  else
  {
     digitalWrite(in1, LOW);
     digitalWrite(in2, HIGH);
     analogWrite(enA ,motorOutput);
  }
}
ISR(TIMER1_COMPA_vect)
{
  r = analogRead(input);//*12/500;
  y = analogRead(output);
  MAPr = map(r , 0, 1023 , 0, 255);
  MAPy = map(y , 0, 1023 , 0, 255);
  //Serial.println("r = ");
  //Serial.println(MAPr);
  //Serial.println("y = ");
  //Serial.println(MAPy);
  e = r-y;
  MAPe = map(e , 0, 1023 , 0, 255);
  //Serial.println("e = ");
  //Serial.println(MAPe);
  //Serial.println(MAPy);
  currentTime = millis();
  elapsedTime = (currentTime - previousTime);
  
  cumError += MAPe * elapsedTime;
  drerror = e / elapsedTime;
  
  motorOutput = (Kp*MAPe) + (Ki*cumError)+ (Kd*drerror);
    if (motorOutput < 0)
    {
      motorOutput = -motorOutput;
    }
    //Serial.println("Input = ");
    Serial.print(MAPr);
    Serial.print(",");
    Serial.print(MAPy);
    //Serial.println("Error = ");
    //Serial.println(MAPe);
  }


void loop() {
  motorControl(MAPe);
}
