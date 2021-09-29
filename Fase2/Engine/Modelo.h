#ifndef _Modelo
#define _Modelo

#include "Triangulo.h"
#include "Cor.h"

#include <stdio.h>
#include <list>
#include <map>

using namespace std;

class Modelo{
	public: 
	
	map<Cor, list<Triangulo>> triangulos;
	
	Modelo(){
		this->triangulos = map<Cor, list<Triangulo>>() ; 
	}

	Modelo(map<Cor, list<Triangulo>> triangulos){
		this->triangulos = triangulos ; 
	}
} ; 

#endif 
