#ifndef _Modelo
#define _Modelo

#include "Triangulo.h"
#include "Cor.h"

#include <stdio.h>
#include <list>
#include <map>
#include <string>

using namespace std;

class Modelo{
	public: 
	
	int code;
	char nome[100];

	list<Triangulo> triangulos;
	list<Ponto> normais;
	list<Ponto> texturas;
	string nomeTextura;

	RGB amb;
	RGB diff;
	RGB spec;
	RGB emi;
	
	Modelo(){
		this->triangulos = list<Triangulo>() ; 
		this->normais = list<Ponto>();
		this->texturas = list<Ponto>();
	}

	Modelo(list<Triangulo> triangulos, list<Ponto> n, list<Ponto> t, string nt, RGB a, RGB d, RGB s, RGB e, int cod, const char* nom){
		this->triangulos = triangulos ;
		this->normais = n;
		this->texturas = t;
		this->nomeTextura = nt;

		this->amb = a;
		this->diff = d;
		this->spec = s;
		this->emi = e;

		this->code = cod;
		
		strcpy(this->nome, nom);
	}
} ; 

#endif 
