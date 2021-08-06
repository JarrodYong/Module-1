const int TRIG=9;//These are the trigger and echo pins on the sensor
const int ECHO=10;//I have put them on pins 9 and 10
int LED=13;
int maxRange=30;//This determines the distance that the LED will
                //turn off. Smaller numbers will work better.
float duration, distance;
int getdistance(int trig, int echo)//This is the function that will use sensor output at a distance
{
  long duration;                    
  digitalWrite(trig, LOW);
  delayMicroseconds(2);
  digitalWrite(trig, HIGH);
  delayMicroseconds(20);
  digitalWrite(trig, LOW);
  duration = pulseIn(echo, HIGH);
  duration = duration / 59;//This line appropriately scales the output to be in cm
  if ((duration < 2) || (duration > maxRange)) return false;
  return duration;                                    
}


void setup()
{
  pinMode(TRIG,OUTPUT);
  pinMode(ECHO,INPUT);
  pinMode(LED,OUTPUT);
  digitalWrite(TRIG,LOW);
  Serial.begin(9600);
}

void loop()
{
  int x=getdistance(TRIG,ECHO);
  int y=maxRange-x;
  int fade=map(y,0,maxRange,0,255);//Maps the brightness to be lowest just before reaching maxRange
  if (x!=0){
    analogWrite(LED,fade); //Dimming part
  }
  else {//If the distance is outside of maxRange, the LED will turn off.
    digitalWrite(LED,LOW);
  }
  digitalWrite(TRIG, LOW);
  delayMicroseconds(10);
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);
  duration = pulseIn(ECHO, HIGH);
  distance = (duration*.0343)/2;
  Serial.print("Distance: ");
  Serial.println(distance);
  Serial.print("LED Status: "); //Use serial.print to label the data in the monitor. 
  Serial.println(digitalRead(13));
  delay(500);
}
