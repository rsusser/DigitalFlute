//mic info
const int sampleWindow = 50; // Sample window width in mS (50 mS = 20Hz)
unsigned int sample;

//key mechanism
const int numKeys = 9;
const int byt = 8;
const int numByt = numKeys/byt + (1 * (numKeys % byt));
unsigned char state[numKeys];
int buttons[numKeys];

//together
byte send[numByt+1];//two bytes for keys, one for amplitude
boolean toPrint = false;

//unsigned int maxYet = 0;
unsigned int ampl = 0;
 
void setup() 
{
   Serial.begin(9600);
   
     //Set up the pin for each key; key 0 = pin 10
  for(int count = 0; count < numKeys; count++){
    int curr = numKeys + 1 - count; //set the pin for the current key (lowest is 2)
    buttons[count] = curr;
    
    pinMode(curr, INPUT_PULLUP);
    digitalWrite(curr, HIGH);
  }
  for(int count = 0; count < numKeys; count++){//unused bits are "0"
    state[count] = 0;
  }
}
 
 
void loop() 
{
  getState();
   unsigned long startMillis= millis();  // Start of sample window
   unsigned int peakToPeak = 0;   // peak-to-peak level
 
   unsigned int signalMax = 0;
   unsigned int signalMin = 1024;
 
   // collect data for 50 mS
   while (millis() - startMillis < sampleWindow)
   {
     getState();
     
      sample = analogRead(0);
      if (sample < 1024) { // toss out spurious readings
         if (sample > signalMax){
            signalMax = sample;  // save just the max levels
            //if(signalMax > maxYet){
            //  maxYet = signalMax;
            //}
         }
         else if (sample < signalMin){
            signalMin = sample;  // save just the min levels
         }
      }
   }
   
   peakToPeak = signalMax - signalMin;  // max - min = peak-peak amplitude
   double volts = (peakToPeak * 3.3) / 1024;  // convert to volts
   //ampl = map(volts, 0, maxYet, 0, 255); //map voltage from 0 to 255
   ampl = (volts*200);
   
   if(ampl != send[2]){
     send[2] = ampl;
     printS();
     delay(5);
   }
   delay(1);
}

//checks the state of the keys and plays note at most recent amplite if something
//has changed since last reading
void getState(){
  toPrint = false;
  
  //Check the state of each button
  for(int count = 0; count < numKeys; count++){
    char current = !digitalRead(buttons[count]);//checked - ! works
    if(state[count]!=current){
      state[count] = current;
      toPrint = true;
    }
  }
 
 //low bit
  unsigned char snd = 0;
  for(int i = 0; i < byt; i++){
    snd |= state[8-i] << i;
  }
  send[1] = snd;
  
  //high bit
  snd = 0;
  snd |= state[0];
  send[0] = snd;
  
  //amp bit
  send[2] = ampl;
  
  if(toPrint){
      printS();
      delay(1);
  }
  delay(1);
}



void printS(){
  Serial.write(send, 3);
}
