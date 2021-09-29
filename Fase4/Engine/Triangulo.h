#ifndef _Triangulo
#define _Triangulo

#include "Cor.h"
#include "Ponto.h"

#include <stdio.h>

class Triangulo {
public:
	Ponto p1;
	Ponto p2;
	Ponto p3;

	Triangulo(Ponto p11, Ponto p22, Ponto p33) {
		this->p1 = p11;
		this->p2 = p22;
		this->p3 = p33;
	}
};

#endif 
