#ifndef _Cena
#define _Cena

#include "tinyxml2-8.0.0/tinyxml2.h"
#include "Cor.h"
#include "Triangulo.h"
#include "Modelo.h"
#include "Ponto.h"
#include "Grupo.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string>
#include <list>

using namespace std;
using namespace tinyxml2;

class Scene {
private:
	map<Cor, list<Triangulo>> loadModelo(string ficheiro, Cor *c){
		
		map<Cor, list<Triangulo>> m;

		fstream f ; 
		f.open(ficheiro); 

		if(f.is_open()){
			
			string line;
			int numPontos = 0;
			int corUt = 0;

			getline(f, line);
			
			sscanf(line.c_str(), "%d\n", &numPontos);
			
			int i = 0;
			while(i<numPontos){

				Ponto p1;
				getline(f,line);
				sscanf(line.c_str(), "%f %f %f\n", &(p1.x), &(p1.y), &(p1.z));

				Ponto p2;
				getline(f,line);
				sscanf(line.c_str(), "%f %f %f\n", &(p2.x), &(p2.y), &(p2.z));

				Ponto p3;
				getline(f,line);
				sscanf(line.c_str(), "%f %f %f\n", &(p3.x),&(p3.y),&(p3.z));

				Triangulo t(p1,p2,p3);

				map<Cor, list<Triangulo>>::iterator it = m.find(c[corUt]);
				if (it != m.end()){
					it->second.push_back(t);
    			}else{
    				list<Triangulo> l;
    				l.push_back(t);
					m.insert(pair<Cor,list<Triangulo>>(c[corUt],l));
    			}
				
				corUt = 1 - corUt;
				i += 3;
			}
			f.close() ;
		}else {
	        printf("Ficheiro %s não encontrado\n", ficheiro.c_str());
	    } 

		return m;
	}

	Group loadGrupo(XMLElement *group){
		
		list<OperacoesGeometricas> ops;

		for (XMLElement* elem = group->FirstChildElement(); elem != NULL; elem = elem->NextSiblingElement()) {
			if (strcmp(elem->Value(), "translate") == 0) {
				const char* stringX = elem->Attribute("X");
				const char* stringY = elem->Attribute("Y");
				const char* stringZ = elem->Attribute("Z");
				const char* stringT = elem->Attribute("time");
				
				float x = 0.0f;
				float y = 0.0f;
				float z = 0.0f;
				float time = 0;
				
				vector<float> p1;
				vector<float> p2;
				vector<float> p3;

				if (stringT != NULL) {
					time = atof(stringT);
					for (XMLElement* elem2 = elem->FirstChildElement(); elem2 != NULL; elem2 = elem2->NextSiblingElement()) {
						
						if (strcmp(elem2->Value(), "point") == 0) {
							stringX = elem2->Attribute("X");
							stringY = elem2->Attribute("Y");
							stringZ = elem2->Attribute("Z");

							Ponto pTemp;
							pTemp.x = 0;
							pTemp.y = 0;
							pTemp.z = 0;

							if (stringX != NULL)
								pTemp.x = atof(stringX);
							if (stringY != NULL)
								pTemp.y = atof(stringY);
							if (stringZ != NULL)
								pTemp.z = atof(stringZ);
							
							p1.push_back(pTemp.x);
							p2.push_back(pTemp.y);
							p3.push_back(pTemp.z);
						}
					}
				}
				else {
					if (stringX != NULL)
						x = atof(stringX);
					if (stringY != NULL)
						y = atof(stringY);
					if (stringZ != NULL)
						z = atof(stringZ);
				}

				OperacoesGeometricas o('T', 0.0f, x, y, z, time, p1, p2, p3);
				ops.push_back(o);
			}
			if (strcmp(elem->Value(), "scale") == 0) {
				const char* stringX = elem->Attribute("X");
				const char* stringY = elem->Attribute("Y");
				const char* stringZ = elem->Attribute("Z");

				float x = 0.0f;
				float y = 0.0f;
				float z = 0.0f;

				if (stringX != NULL)
					x = atof(stringX);
				if (stringY != NULL)
					y = atof(stringY);
				if (stringZ != NULL)
					z = atof(stringZ);
				
				vector<float> p1;

				OperacoesGeometricas o('S', 0.0f, x, y, z, 0, p1, p1, p1);
				ops.push_back(o);
			}
			if (strcmp(elem->Value(), "rotate") == 0) {
				float angle = 0.0f;
				float x = 0.0f;
				float y = 0.0f;
				float z = 0.0f;
				float time = 0;
				
				const char* stringT = elem->Attribute("time");
				const char* stringAngle = elem->Attribute("angle");
				const char* stringX = elem->Attribute("axisX");
				const char* stringY = elem->Attribute("axisY");
				const char* stringZ = elem->Attribute("axisZ");

				if (stringX != NULL)
					x = atof(stringX);
				if (stringY != NULL)
					y = atof(stringY);
				if (stringZ != NULL)
					z = atof(stringZ);
				if (stringAngle != NULL)
					angle = atof(stringAngle);
				if (stringT != NULL)
					time = atof(stringT);

				vector<float> p1;

				OperacoesGeometricas o('R', angle, x, y, z, time, p1, p1, p1);
				ops.push_back(o);
			}
		}

		XMLElement *modelos = group-> FirstChildElement("models");

		list<Modelo> lm = list<Modelo>();

		if(modelos!=NULL){

			Cor c1(0.039f, 0.933f, 0.941f), c2(0.870f, 0.909f, 0.909f);
			Cor c[2]; c[0] = c1; c[1] = c2;

		    for (XMLElement *elem = modelos->FirstChildElement(); elem != NULL; elem = elem->NextSiblingElement()) {
	            string ficheiro = elem->Attribute("file");

	            map<Cor, list<Triangulo>> lista = loadModelo(ficheiro,c);
				
				if(!lista.empty()){
					Modelo m(lista);
					lm.push_back(m);
				}
	        }
		}

		list<Group> lg = list<Group>();

		for (XMLElement* subgrupos = group->FirstChildElement("group"); subgrupos != NULL; subgrupos = subgrupos->NextSiblingElement("group")) {
			Group g = loadGrupo(subgrupos);
			lg.push_back(g);
		}

		Group grupo(lm,lg, ops);

		return grupo;
	}

public:
	Group g;

	Scene() { 	}

	Scene(Group g1) { 
		this->g = g1; 
	}

	void load (const char* pFilename) { 

		XMLDocument doc;

		if (!(doc.LoadFile(pFilename))) {
			
	        XMLElement * scene = doc.FirstChildElement("scene");
            XMLElement * group = scene-> FirstChildElement("group");
	        
            g = loadGrupo(group);

	    } else {
	        printf("Ficheiro XML não encontrado\n");
	    } 
	}
};

#endif 
