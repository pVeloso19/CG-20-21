#ifndef _Grupo
#define _Grupo

#include "Modelo.h"

#include <stdio.h>
#include <list>
#include <map>

using namespace std;

class OperacoesGeometricas {
public:

	char tipo;

	float angle;

	float x;
	float y;
	float z;

	OperacoesGeometricas() {	}

	OperacoesGeometricas(char tipo1, float angle1, float x1, float x2, float x3) {
		
		this->tipo = tipo1;

		this->angle = angle1;
		
		this->x = x1;
		this->y = x2;
		this->z = x3;
	}
};


class Group{
	public: 
	
	list<OperacoesGeometricas> operacoes;

	list<Modelo> modelos;

	list<Group> subGrupos;
	
	Group() {	}
	
	Group(list<Modelo> m, list<Group> g, list<OperacoesGeometricas> ops){
		
		this->operacoes = ops;

		this->modelos = m;

		this->subGrupos = g;
	}
} ; 

#endif 
