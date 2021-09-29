#ifndef _Grupo
#define _Grupo

#include "Modelo.h"

#include <stdio.h>
#include <list>
#include <map>
#include <vector>

using namespace std;

class OperacoesGeometricas {
public:

	char tipo;

	float angle;

	float x;
	float y;
	float z;

	float time;

	vector<float> p1;
	vector<float> p2;
	vector<float> p3;

	float yy1 = 0;
	float yy2 = 1;
	float yy3 = 0;

	OperacoesGeometricas() {	}

	OperacoesGeometricas(char tipo1, float angle1, float x1, float x2, float x3, float t, vector<float> p1T, vector<float> p2T, vector<float> p3T) {
		
		this->tipo = tipo1;

		this->angle = angle1;
		
		this->x = x1;
		this->y = x2;
		this->z = x3;

		this->time = t;

		this->p1 = p1T;
		this->p2 = p2T;
		this->p3 = p3T;
	}
};


class Luz {
public:

	char tipoLuz;

	float posLuzX;
	float posLuzY;
	float posLuzZ;

	float angleLuz;

	Luz() {	
		this->tipoLuz = 'P';

		this->posLuzX = 0;
		this->posLuzY = 10;
		this->posLuzZ = 0;
	}

	Luz(char tipo1, float x1, float x2, float x3, float a) {

		this->tipoLuz = tipo1;

		this->posLuzX = x1;
		this->posLuzY = x2;
		this->posLuzZ = x3;

		this->angleLuz = a;
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
