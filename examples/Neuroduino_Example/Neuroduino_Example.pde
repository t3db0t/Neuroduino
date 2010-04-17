#include <Neuroduino.h>

#define NUM_LAYERS 2

// used for get_free_memory()
extern int __bss_end;
extern void *__brkval;

int netArray[NUM_LAYERS] = {3,3};
int inputArray[] = {1, -1, 1};
int trainArray[] = {-1, 1, -1};

int pinState;
int lastRead = HIGH;

//Neuroduino myNet(netArray, NUM_LAYERS, 0.05, 0.0, true);
//Neuroduino myNet = Neuroduino();

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
  //Serial.begin(9600);
  int seed = analogRead(0);
  Serial.println(seed);
  srand(seed);  // random seed
  //srand(4711);  // for testing
  
  pinMode(8, INPUT);
  digitalWrite(8, HIGH);
  
  int netMem;
  netMem = get_free_memory();
  
  // Neuroduino params: (network array, number of layers, Eta, Theta, debug)
  myNet.init(netArray, NUM_LAYERS, 0.05, 0.0, true);
  
  netMem = netMem - get_free_memory();
  Serial.print("Net size: ");
  Serial.print(netMem);
  Serial.print(" / Free memory: ");
  Serial.println(get_free_memory());
  
  // Neuroduino wants all activations expressed as 1 or -1,
  // so we must first convert to this from binary
  
  
  
  // Test activation of random weights
  myNet.printNet();
  printArray(myNet.simulate(inputArray), 3);    
}

void loop(){
  pinState = digitalRead(8);
  if(pinState == LOW  && lastRead == HIGH){
    // switch just closed
    Serial.println("beep!");
    
    myNet.train(inputArray, trainArray);
    myNet.printNet();
    printArray(myNet.simulate(inputArray), 3);
  }
  lastRead = pinState;
}
