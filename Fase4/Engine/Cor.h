#ifndef _Cor
#define _Cor

#include <stdio.h>

class RGB {
public:
	float r;
	float g;
	float b;
	
	RGB() {
		this->r = 0.0f;
		this->g = 0.0f;
		this->b = 0.0f;
	}

	RGB(float r2, float g2, float b2) {
		this->r = r2;
		this->g = g2;
		this->b = b2;
	}

	bool operator <(const RGB& c) const
    {
        return r < c.r 
          || (r == c.r) && (g < c.g)
          || (r == c.r) && (g == c.g) && b < c.b;
    }
};

#endif 