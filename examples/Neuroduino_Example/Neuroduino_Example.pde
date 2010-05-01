#include <Neuroduino.h>

#define NUM_LAYERS 2
#define ETA 0.01
#define THETA 0.0
#define DEBUG false

// used for get_free_memory()
extern int __bss_end;
extern void *__brkval;

int netArray[NUM_LAYERS] = {8,8};
int inputArray[] = {1, -1, 1, -1, -1, 1, -1, 1};
int trainArray[] = {-1, 1, -1, -1, 1, -1, 1, -1};

int pinState;
int lastRead = HIGH;

int netMem;

Neuroduino myNet;

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

byte arrayToByte(int arr[], int len){
  // Convert -1 to 0 and pack the array into a byte
  int i;
  byte result = 0;
  
  for(i=len-1; i>=0; i--){
    if(arr[i] == -1){
      result &= ~(0 << i);
    } else {
      result |= (1 << i);
    }
  }
  return result;
}

void byteToArray(byte in, int out[]){
	int i, temp;//, out[8];
  
  for(i=0; i<8; i++){
    temp = (in >> i) & 1;
    if(temp == 0) temp = -1;
    out[i] = temp;
  }
}

void printArray(int arr[], int len){
  int i;
  Serial.print("[");
  for(i=0; i<len; i++){
	if (arr[i] == 1) {
		Serial.print(" ");	// Leading space to maintain alignment
	}
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
  srand(analogRead(0));
  //srand(4711);  // for testing
	
  Serial.begin(9600);
  netMem = get_free_memory();
  checkMem();
  
  // Neuroduino params: (network array, number of layers, Eta, Theta, debug)
  myNet = Neuroduino(netArray, NUM_LAYERS, ETA, THETA, DEBUG);
  
  netMem = netMem - get_free_memory();
  Serial.print("Net size: ");
  Serial.print(netMem);
  Serial.print(" / Free memory: ");
  Serial.println(get_free_memory());
  
  myNet.randomizeWeights();
  
  pinMode(8, INPUT);
  digitalWrite(8, HIGH);
  
  printArray(inputArray, netArray[0]);
  byte temp = arrayToByte(inputArray, netArray[0]);
  Serial.println(temp, BIN);
  
  int outputArray[8];
  byteToArray(temp, outputArray);
  printArray(outputArray, 8);
  
  // Test activation of initial (randomized) weights

  Serial.print("Current activation: ");
  printArray(myNet.simulate(inputArray), netArray[1]);
  Serial.print("Target activation:  ");
  printArray(trainArray, netArray[1]);
  Serial.println();
}

void loop(){
  pinState = digitalRead(8);
  if(pinState == LOW  && lastRead == HIGH){
    // switch just closed
    
    myNet.train(inputArray, trainArray);
    myNet.printNet();
    
    Serial.print("Current activation: ");
    printArray(myNet.simulate(inputArray), netArray[1]);
    Serial.print("Target activation:  ");
    printArray(trainArray, netArray[1]);
	Serial.println();
  }
  lastRead = pinState;
}
