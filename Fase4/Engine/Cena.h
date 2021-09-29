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
	void loadModelo(string ficheiro, list<Triangulo>* m, list<Ponto>* n, list<Ponto>* text){
		
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
				Ponto n1;
				Ponto t1;
				getline(f,line);
				sscanf(line.c_str(), "%f %f %f %f %f %f %f %f\n", &(p1.x), &(p1.y), &(p1.z), &(n1.x), &(n1.y), &(n1.z), &(t1.x), &(t1.y));

				Ponto p2;
				Ponto n2;
				Ponto t2;
				getline(f,line);
				sscanf(line.c_str(), "%f %f %f %f %f %f %f %f\n", &(p2.x), &(p2.y), &(p2.z), &(n2.x), &(n2.y), &(n2.z), &(t2.x), &(t2.y));

				Ponto p3;
				Ponto n3;
				Ponto t3;
				getline(f,line);
				sscanf(line.c_str(), "%f %f %f %f %f %f %f %f\n", &(p3.x), &(p3.y), &(p3.z), &(n3.x), &(n3.y), &(n3.z), &(t3.x), &(t3.y));

				Triangulo t(p1,p2,p3);

				(*m).push_back(t);
				(*n).push_back(n1);
				(*n).push_back(n2);
				(*n).push_back(n3);

				(*text).push_back(t1);
				(*text).push_back(t2);
				(*text).push_back(t3);

				i += 3;
			}
			f.close() ;
		}else {
	        printf("Ficheiro %s não encontrado\n", ficheiro.c_str());
	    } 
	}

	Group loadGrupo(XMLElement *group){
		
		list<OperacoesGeometricas> ops;
		list<Luz> luz;
		RGB a;
		RGB d;
		RGB s;
		RGB e;

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

			for (XMLElement *elem = modelos->FirstChildElement(); elem != NULL; elem = elem->NextSiblingElement()) {
	            string ficheiro = elem->Attribute("file");
				const char* texturasNomeT = elem->Attribute("texture");

				const char* ambR = elem->Attribute("ambR"), *ambG = elem->Attribute("ambG"), *ambB = elem->Attribute("ambB");
				if (ambR != NULL) a.r = atof(ambR);
				if (ambG != NULL) a.g = atof(ambG);
				if (ambB != NULL) a.b = atof(ambB);

				const char* diffR = elem->Attribute("diffR"), *diffG = elem->Attribute("diffG"), *diffB = elem->Attribute("diffB");
				if (diffR != NULL) d.r = atof(diffR);
				if (diffG != NULL) d.g = atof(diffG);
				if (diffB != NULL) d.b = atof(diffB);

				const char* specR = elem->Attribute("specR"), *specG = elem->Attribute("specG"), *specB = elem->Attribute("specB");
				if (specR != NULL) s.r = atof(specR); else s.r = 1.0f;
				if (specG != NULL) s.g = atof(specG); else s.g = 1.0f;
				if (specB != NULL) s.b = atof(specB); else s.b = 1.0f;

				const char* emiR = elem->Attribute("emiR"), *emiG = elem->Attribute("emiG"), *emiB = elem->Attribute("emiB");
				if (emiR != NULL) e.r = atof(emiR);
				if (emiG != NULL) e.g = atof(emiG);
				if (emiB != NULL) e.b = atof(emiB);

				int code = -1;
				const char* cod = elem->Attribute("code");
				if (cod != NULL) code = atoi(cod);

				const char* nomeModelo = elem->Attribute("nome");
				
				list<Triangulo> m;
				list<Ponto> n;
				list<Ponto> text;

	            loadModelo(ficheiro, &m, &n, &text);
				
				if(!m.empty()){
					if (texturasNomeT != NULL) {
						string texturasNome(texturasNomeT);
						if (nomeModelo != NULL) {
							Modelo m(m, n, text, texturasNome, a, d, s, e, code, nomeModelo);
							lm.push_back(m);
						}
						else {
							Modelo m(m, n, text, texturasNome, a, d, s, e, -1, "");
							lm.push_back(m);
						}
					}
					else {
						if (nomeModelo != NULL) {
							Modelo m(m, n, text, "", a, d, s, e, code, nomeModelo);
							lm.push_back(m);
						}
						else {
							Modelo m(m, n, text, "", a, d, s, e, -1, "");
							lm.push_back(m);
						}
					}
					
					
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
	list<Luz> luz;

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

			if (XMLElement *luzes = scene->FirstChildElement("lights")) {
				for (XMLElement* elem = luzes->FirstChildElement(); elem != NULL; elem = elem->NextSiblingElement()) {
					if (strcmp(elem->Value(), "light") == 0) {
						char type = 'P';
						float x = 0.0f;
						float y = 0.0f;
						float z = 0.0f;
						float angle = 180.0f;

						const char* stringT = elem->Attribute("type");
						const char* stringX = elem->Attribute("posX");
						const char* stringY = elem->Attribute("posY");
						const char* stringZ = elem->Attribute("posZ");
						const char* stringAngle = elem->Attribute("angle");

						if (stringX != NULL)
							x = atof(stringX);
						if (stringY != NULL)
							y = atof(stringY);
						if (stringZ != NULL)
							z = atof(stringZ);
						if (stringAngle != NULL)
							angle = atof(stringAngle);
						if (stringT != NULL) {
							if (strcmp(stringT, "DIRECTIONAL") == 0) {
								type = 'D';
							}
							else {
								if (strcmp(stringT, "SPOT") == 0) {
									type = 'S';
								}
							}
						}

						Luz l(type, x, y, z,angle);	
						this->luz.push_back(l);
					}
				}
			}

	    } else {
	        printf("Ficheiro XML não encontrado\n");
	    } 
	}
};

#endif 
