/*
 *  Neuroduino.c
 *  
 *  Arduino Neural Network Library
 *  Simplified from:
 *  http://www.neural-networks-at-your-fingertips.com/bpn.html
 *
 *  Implements a simple feedforward perceptron network
 * 
 *  Created by Ted Hayes on 4/15/10.
 *  Copyright 2010 Limina.Studio. All rights reserved.
 *
 */

#include "WProgram.h"
#include "Neuroduino.h"

Neuroduino::Neuroduino(int nodeArray[], int numLayers, double eta = 0.1, double theta = 0.0, boolean debug = false) {
	// Constructor
	_debug = debug;
	trace("Neuroduino::Neuroduino\n");
	
	_numLayers = numLayers;
	
	// allocate structs & initialize network
	
	int i, j;
	
	_net.Layer = (LAYER**) calloc(_numLayers, sizeof(LAYER*));
	
	for (i=0; i < _numLayers; i++) {
		// 2D array of the network's layers
		_net.Layer[i] = (LAYER*) malloc(sizeof(LAYER));
		_net.Layer[i]->Units = nodeArray[i];		
		_net.Layer[i]->Output = (int*) calloc(nodeArray[i], sizeof(int));
		
		// 2D array of weights for each pair of nodes between layers
		_net.Layer[i]->Weight = (VAL**) calloc(nodeArray[i]+1, sizeof(VAL*));
		
		if (i!=0) {
			for (j=0; j<nodeArray[i]; j++) {
				// init array for incoming connections to node j of layer i
				_net.Layer[i]->Weight[j] = (VAL*) calloc(nodeArray[i-1], sizeof(VAL));
			}
		}
	}
	
	_net.InputLayer = _net.Layer[0];
	_net.OutputLayer = _net.Layer[_numLayers-1];
	_net.Eta = eta;		// learning rate
	_net.Theta = theta;	// threshold
	
	randomizeWeights();
	printNet();
}

/********* UTILITIES *********/

//just for testing alternative ways of storing the values... (for now is the same: DOUBLE)
VAL Neuroduino::doubleToVal(double dValue) {
	return (VAL) dValue;
}

double Neuroduino::randomEqualDouble(double Low, double High) {
	//trace("randomEqualDouble\n");
	return ((double) rand() / RAND_MAX) * (High-Low) + Low;
}

void Neuroduino::printNet(){
	if (_debug) {
		int l,i,j;
		
		//Serial.print("weights of layer 0 test: ");
		//Serial.println(_net.Layer[0]->Weight[0][0], DEC);
		
		for (l=1; l<_numLayers; l++) {
			Serial.print("Layer[");
			Serial.print(l, DEC);
			Serial.println("]:");
			Serial.print("\t");
			for (i=0; i<_net.Layer[l]->Units; i++) {
				Serial.print(i,DEC);
				Serial.print("\t");
			}
			Serial.println();
			for (i=0; i<_net.Layer[l]->Units; i++) {
				// cycle through units of current layer
				//Serial.print("  Unit ");
				Serial.print(i,DEC);
				Serial.print(":\t");
				for (j=0; j<_net.Layer[l-1]->Units; j++) {
					// cycle through units of previous layer
					Serial.print(_net.Layer[l]->Weight[i][j], 3);
					trace("\t");
				}
				trace("\n");
			}
			trace("\n");
		}
		trace("\n");
	}
}

// free memory check
// from: http://forum.pololu.com/viewtopic.php?f=10&t=989#p4218
int Neuroduino::get_free_memory(){
	int free_memory;
	if((int)__brkval == 0)
		free_memory = ((int)&free_memory) - ((int)&__bss_end);
	else
		free_memory = ((int)&free_memory) - ((int)__brkval);
	
	return free_memory;
}

/********* PRIVATE *********/

void Neuroduino::randomizeWeights() {
	int l,i,j;
	double temp;
	
	for (l=1; l<_numLayers; l++) {
		for (i=0; i<_net.Layer[l]->Units; i++) {
			for (j=0; j<_net.Layer[l-1]->Units; j++) {
				_net.Layer[l]->Weight[i][j] = doubleToVal(randomEqualDouble(-0.5, 0.5));
			}
		}
	}
}

void Neuroduino::setInput(int inputs[]){
	int i;
	for (i=0; i<_net.InputLayer->Units; i++) {
		_net.InputLayer->Output[i] = inputs[i];
	}
}
/*
void Neuroduino::getOutput(){
	// copy output layer values to _output
	int i;
	for (i=0; i<_net.OutputLayer->Units; i++) {
		_output[i] = _net.OutputLayer->Output[i];
	}
}
 */

int Neuroduino::signThreshold(double sum){
	Serial.print("signThreshold ");
	Serial.print("Units: ");
	Serial.println(_net.Layer[1]->Units);
	
	if (sum >= _net.Theta) {
		Serial.println("--> returning 1");
		return 1;
	} else {
		Serial.println("--> returning -1");
		return -1;
	}
}

double Neuroduino::weightedSum(int l, int node){
	// calculates input activation for a particular neuron
	int i;
	double currentWeight, sum = 0.0;
	//sum = 0.0;
	
	//Serial.print("weightedSum Units: ");
	//Serial.println(_net.Layer[1]->Units);
	
	for (i=0; i<_net.Layer[l-1]->Units; i++) {
		currentWeight = this->_net.Layer[l-1]->Weight[node][i];
		Serial.print("wS cW: ");
		Serial.print(currentWeight, 3);
		Serial.print(" input: ");
		Serial.println(_net.Layer[l-1]->Output[i]);
		
		sum += currentWeight * _net.Layer[l-1]->Output[i];
		
		//Serial.print("for loop Units: ");
		//Serial.println(_net.Layer[1]->Units);
	}
	
	return sum;
}

void Neuroduino::adjustWeights(int trainArray[]){
	int l,i,j;
	int in,out, error;
	int activation;	// for each "rightmost" node
	double delta;
	
	for (l=1; l<_numLayers; l++) {
		// cycle through each pair of nodes
		Serial.print("adjustWeights: Layer[l]->Units: ");
		Serial.println(_net.Layer[l]->Units);
		for (i=0; i<_net.Layer[l]->Units; i++) {
			// "rightmost" layer
			// calculate current activation of this output node
			activation = signThreshold(weightedSum(l,i));
			out = trainArray[i];	// correct activation
			error = out - activation;	// -2, 2, or 0
			
			for (j=0; j<_net.Layer[l-1]->Units; j++) {
				// "leftmost" layer
				
				in = _net.Layer[l-1]->Output[j];
				
				delta = _net.Eta * in * error;
				_net.Layer[l]->Weight[j][i] += delta;
			}
		}
	}
}

void Neuroduino::simulateNetwork(){
	/*****
	 Calculate activations of each output node
	 *****/
	//Serial.println("simulateNetwork");
	int num, l,j, activation;
	
	//Serial.print("simulateNetwork: Layer[1]->Units: ");
	//Serial.println(_net.Layer[1]->Units);
	num = _net.Layer[1]->Units;
	Serial.print("simulateNetwork: num: ");
	Serial.println(num);
	//l = 1;
	//for (l=_numLayers; l<=0; l--) {
		//Serial.print("sim l: ");
		//Serial.println(l);
		// step backwards through layers
		// TODO: this will only work for _numLayers = 2!
		for (j=0; j < num; j++) {
			Serial.print("free mem: ");
			Serial.print(get_free_memory());
			Serial.print("  > j: ");
			Serial.println(j);
			//Serial.print(" / Units: ");
			//Serial.println(_net.Layer[1]->Units);
			// rightmost layer
			//activation = signThreshold(weightedSum(l, j));
			//Serial.print(activation, DEC);
			//Serial.print(" ");
			_output[j] = signThreshold(weightedSum(1, j));
		}
	//}
	 
	Serial.println("Exiting simulateNetwork");
}

/********* PUBLIC *********/

void Neuroduino::train(int inputArray[], int trainArray[]) {
	trace("Neuroduino::train\n");
	
	setInput(inputArray);
	adjustWeights(trainArray);
	
}

void Neuroduino::simulate(int inputArray[]) {
	// introduce an input stimulus, simulate the network,
	// and return an output array
	trace("Neuroduino::simulate\n");
	setInput(inputArray);
	simulateNetwork();
	
	//getOutput();	// sets _output public array
	
	//return _output;
}

void Neuroduino::trace(char *message) {
	if(_debug){
		Serial.print(message);
	}
}