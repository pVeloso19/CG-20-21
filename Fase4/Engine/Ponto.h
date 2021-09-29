#ifndef _Ponto
#define _Ponto

#include <stdio.h>

class Ponto {
public:
	float x;
	float y;
	float z;
};

class Vetor {
public:
	float x;
	float y;
	float z;

	Vetor(float x1, float x2, float x3) {
		this->x = x1;
		this->y = x2;
		this->z = x3;
	}

	int numCoordNegativas() {
		int res = 0;
		
		if (this->x < 0){
			res += 1;
		}
		if (this->y < 0) {
			res += 1;
		}
		if (this->z < 0) {
			res += 1;
		}

		return res;
	}
};

#endif 
