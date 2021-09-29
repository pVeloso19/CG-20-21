#ifndef _Cor
#define _Cor

#include <stdio.h>

class Cor {
public:
	float r;
	float g;
	float b;
	
	Cor() {
		this->r = 1.0f;
		this->g = 1.0f;
		this->b = 1.0f;
	}

	Cor(float r2, float g2, float b2) {
		this->r = r2;
		this->g = g2;
		this->b = b2;
	}

	bool operator <(const Cor& c) const
    {
        return r < c.r 
          || (r == c.r) && (g < c.g)
          || (r == c.r) && (g == c.g) && b < c.b;
    }
};

#endif 