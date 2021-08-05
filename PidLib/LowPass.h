#include "Arduino.h"

class LowPass {
public:
	int *_arr;
	int _n;
 
	LowPass(int);
	LowPass();
	int getNext(int);
};
