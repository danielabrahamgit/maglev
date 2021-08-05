#include "Arduino.h"
#include "LowPass.h"

LowPass::LowPass() {
	LowPass(1);
}

LowPass::LowPass(int n) {
	_n = n;
	_arr = new int[n];
	for (int i = 0; i < n; i++) {
		_arr[i] = 0;
	}
}

int LowPass::getNext(int nextVal) {
	int total = nextVal;
	for (int i = 0; i < _n - 1 ; i++) {
		_arr[i] = _arr[i + 1];
		total += _arr[i];
	}
	_arr[_n - 1] = nextVal;
	return total / _n;
}
