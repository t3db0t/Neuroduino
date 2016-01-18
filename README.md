# Neuroduino
An artificial neural network library for Arduino

Neuroduino is a two-layer perceptron network with a simplified API suitable for Arduino projects that need to flexibly associate an input pattern with an output pattern.  See [Neuroduino Example](https://github.com/t3db0t/Neuroduino/blob/master/examples/Neuroduino_Example/Neuroduino_Example.pde) for a functioning example.  This library was developed in 2010 at the Interactive Telecommunications Program for my thesis, [The Dawn Chorus](http://log.liminastudio.com/itp/the-dawn-chorus).

Example uses:
- Simple clustering and classification (but [not XOR](https://www.quora.com/Why-cant-the-XOR-problem-be-solved-by-a-one-layer-perceptron) until backpropagation is finished!)
- 'Natural' language emergence (as in my thesis)
- Conceivably, simple stabilization of servos or motors

Pull requests for bug fixes, improvements, documentation, etc are absolutely welcome!

## Basic Usage

### Setup
```C++
Neuroduino myNet;
int netArray[NUM_LAYERS] = {8,8};
int inputArray[] = {1, -1, 1, -1, -1, 1, -1, 1};    // The input to be trained to
int trainArray[] = {-1, 1, -1, -1, 1, -1, 1, -1};   // What you want the network to output when it gets the inputArray
myNet = Neuroduino(netArray, NUM_LAYERS, ETA, THETA, DEBUG);
```

- `NUM_LAYERS` is currently fixed to 2. If anyone wants to finish implementing backpropagation, I would welcome your pull requests!
- `ETA` is the learning rate: the amount each value changes on each iteration. Making this larger makes learning much faster, but can result in oscillations. Making it smaller does the opposite. 0.1 by default.
- `THETA` is the activation threshold. This is 0.0 by default.
- `DEBUG` turns on extra debugging output. False by default.

### Set initial weights
```C++
srand(analogRead(0)); (or use a fixed integer for reproducible results)
myNet.randomizeWeights();
```

### Run some iterations
`printArray(myNet.simulate(inputArray), netArray[1]);`

Eventually, the network should output `trainArray` when it gets `inputArray`.  Yay! Why go through all this trouble? The association between input and output is flexible, so small 'imperfections' in the input should still produce the desired output.
