#include <Neuroduino.h>

#define NUM_LAYERS 2
#define ETA 0.01
#define THETA 0.0
#define DEBUG true

// used for get_free_memory()
extern int __bss_end;
extern void *__brkval;

int netArray[NUM_LAYERS] = {8,8};
int inputArray[] = {1, -1, 1, -1, -1, 1, -1, 1};
int trainArray[] = {-1, 1, -1, -1, 1, -1, 1, -1};

int pinState;
int lastRead = HIGH;

int netMem = get_free_memory();

// Neuroduino params: (network array, number of layers, Eta, Theta, debug)
Neuroduino myNet(netArray, NUM_LAYERS, ETA, THETA, DEBUG);

// free memory check
// from: http://forum.pololu.com/viewtopic.php?f=10&t=989#p4218
int get_free_memory(){
  int free_memory;
  if((int)__brkval == 0)
    free_memory = ((int)&free_memory) - ((int)&__bss_end);
  else
    free_memory = ((int)&free_memory) - ((int)__brkval);

  return free_memory;
}

void printArray(int arr[], int len){
  int i;
  Serial.print("[");
  for(i=0; i<len; i++){
    Serial.print(arr[i], DEC);
    if(i != len-1) Serial.print(", ");
  }
  Serial.println("]");
}

int checkMem(){
  Serial.print("Free memory: ");
  Serial.println(get_free_memory(), DEC);
}

void setup(){
  netMem = netMem - get_free_memory();
  Serial.begin(9600);
  srand(analogRead(0));
  //srand(4711);  // for testing
  
  myNet.randomizeWeights();
  
  pinMode(8, INPUT);
  digitalWrite(8, HIGH);
  
  Serial.print("Net size: ");
  Serial.print(netMem);
  Serial.print(" / Free memory: ");
  Serial.println(get_free_memory());
  
  // Neuroduino wants all activations expressed as 1 or -1,
  // so we must first convert to this from binary
  
  
  
  // Test activation of random weights
  myNet.printNet();
  printArray(myNet.simulate(inputArray), netArray[1]);    
}

void loop(){
  pinState = digitalRead(8);
  if(pinState == LOW  && lastRead == HIGH){
    // switch just closed
    Serial.println("beep!");
    
    myNet.train(inputArray, trainArray);
    myNet.printNet();
    printArray(myNet.simulate(inputArray), netArray[1]);
  }
  lastRead = pinState;
}
