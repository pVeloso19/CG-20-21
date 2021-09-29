#ifndef _Cena
#define _Cena

#include "tinyxml2-8.0.0/tinyxml2.h"
#include "Cor.h"
#include "Triangulo.h"
#include "Modelo.h"
#include "Ponto.h"

#include <stdio.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <string>
#include <list>

using namespace std;
using namespace tinyxml2;

class Scene {
private:
	map<Cor, list<Triangulo>> load(string ficheiro, Cor *c){
		
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
    			}
    			else{
    				list<Triangulo> l;
    				l.push_back(t);
					m.insert(pair<Cor,list<Triangulo>>(c[corUt],l));
    			}
				
				corUt = 1 - corUt;
				i += 3;
			}
			f.close() ;

		}else {
	        printf("Ficheiro %s não encontrado\n", ficheiro);
	    } 

		return m;
	}
public:
	list<Modelo> modelos;

	Scene() { 
		this->modelos = list<Modelo>(); 
	}

	Scene(list<Modelo> modelos) { 
		this->modelos = modelos; 
	}

	void load (const char* pFilename) { 
		this->modelos = list<Modelo>(); 

		XMLDocument doc;
	    XMLElement *root;

		Cor c1(0.039f, 0.933f, 0.941f), c2(0.870f, 0.909f, 0.909f);

		Cor c[2];
		c[0] = c1;
		c[1] = c2;

	    if (!(doc.LoadFile(pFilename))) {
	        root = doc.FirstChildElement();
	        for (XMLElement *elem = root->FirstChildElement(); elem != NULL; elem = elem->NextSiblingElement()) {
	            string ficheiro = elem->Attribute("file");
	            
	            map<Cor, list<Triangulo>> lista = load(ficheiro,c);
				
				if(!lista.empty()){
					Modelo m(lista);
					this->modelos.push_back(m);
				}
	        }
	    } else {
	        printf("Ficheiro XML não encontrado\n");
	    } 
	}
};

#endif 
