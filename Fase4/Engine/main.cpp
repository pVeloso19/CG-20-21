#include "Cena.h"

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glew.h>
#include <GL/glut.h>
#endif

#include <IL/il.h>

#define _USE_MATH_DEFINES // always before the include
#include <math.h>
#include <stdio.h>

////////////////////////////////////////////////////////////////////////

class ModeloVBO {
public:

	GLuint triangulos;
	GLuint normais;
	GLuint texturas;
	int tam;
	int texturaId;

	RGB amb;
	RGB diff;
	RGB spec;
	RGB emi;

	int code;

	ModeloVBO(GLuint p, GLuint n, GLuint t, int tam, int tID, RGB a, RGB d, RGB s, RGB e, int cod) {
		this->triangulos = p;
		this->normais = n;
		this->texturas = t;
		
		this->tam = tam;
		
		this->texturaId = tID;
		
		this->amb = a;
		this->diff = d;
		this->spec = s;
		this->emi = e;

		this->code = cod;
	}
};

class GroupVBO {
public:

	list<OperacoesGeometricas> operacoes;

	list<ModeloVBO> modelos;

	list<GroupVBO> subGrupos;

	GroupVBO() {	}

	GroupVBO(list<ModeloVBO> m, list<GroupVBO> g, list<OperacoesGeometricas> ops) {

		this->operacoes = ops;

		this->modelos = m;

		this->subGrupos = g;
	}
};

list<Luz> luz;

map<int, char*> nomes;
////////////////////////////////////////////////////////////////////////

int w, h;

int tipoDesenho = GL_FILL;

float* angle;
float* escala;
float* translacao;

Vetor vetorTranslacao(0.0f, 0.0f, 0.0f);
Vetor vetorRotacao(0.0f, 0.0f, 0.0f);
Vetor vetorEscala(1.0f, 1.0f, 1.0f);

bool botaoEsqCarregado = false;
bool botaoDirCarregado = false;
int posXRato = 0, posYRato = 0;

bool orbitas = true;
bool eixos = false;

//Define o modo
bool tipoExplorador = true;

//Para camara em modo explorador
float alpha = 0.5f;
float beta = -0.75f;
float raio = 315.0f;

// Para camara em FPS
float lx = 1.0f;
float ly = 0.0f;
float lz = -0.3;

float px = -33.0f;
float py = -7.0f;
float pz = 20;

float alphaFPS = 0;
float betaFPS = -5000;

int timebase = 0, frame = 0;

unsigned int picked = 0;

GroupVBO vbos;

//Funcao responsavel por receber um evento de mudar o tamanho da janela
void changeSize(int ww, int hh) {

	float ratio;

	w = ww;
	h = hh;
	// Prevent a divide by zero, when window is too short
	// (you cant make a window of zero width).
	if (h == 0)
		h = 1;

	ratio = 1.0f * w / h;

	// Reset the coordinate system before modifying
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// Set the viewport to be the entire window
	glViewport(0, 0, w, h);

	// Set the clipping volume
	gluPerspective(45, ratio, 1, 1000);
	glMatrixMode(GL_MODELVIEW);
}

//Funcao que desenha os eixos do X,Y,Z
void desenhaEixos() {
	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);

	glBegin(GL_LINES);
		// X axis in red
		glColor3f(1.0f, 0.0f, 0.0f);
		glVertex3f(-100.0f, 0.0f, 0.0f);
		glVertex3f(100.0f, 0.0f, 0.0f);
		// Y Axis in Green
		glColor3f(0.0f, 1.0f, 0.0f);
		glVertex3f(0.0f, -100.0f, 0.0f);
		glVertex3f(0.0f, 100.0f, 0.0f);
		// Z Axis in Blue
		glColor3f(0.0f, 0.0f, 1.0f);
		glVertex3f(0.0f, 0.0f, -100.0f);
		glVertex3f(0.0f, 0.0f, 100.0f);
	glEnd();

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_LIGHTING);
}

//Desenha a orbita dos planetas
void desenhaOrbita(float raio) {
	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);

	glColor3f(0.2f, 0.2f, 0.2f);

	glBegin(GL_LINE_LOOP);
		for (int i = 0; i <= 360; i++) {
			float angle = (i * M_PI) / 180;
			glVertex3f(cos(angle) * raio, 0, sin(angle) * raio);
		}
	glEnd();

	glColor3f(1.0f, 1.0f, 1.0f);

	glEnable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);
	
}

// Constroi a matriz de rotacao
void buildRotMatrix(float* x, float* y, float* z, float* m) {

	m[0] = x[0]; m[1] = x[1]; m[2] = x[2]; m[3] = 0;
	m[4] = y[0]; m[5] = y[1]; m[6] = y[2]; m[7] = 0;
	m[8] = z[0]; m[9] = z[1]; m[10] = z[2]; m[11] = 0;
	m[12] = 0; m[13] = 0; m[14] = 0; m[15] = 1;
}

//Realiza o produto de 2 vetores
void cross(float* a, float* b, float* res) {

	res[0] = a[1] * b[2] - a[2] * b[1];
	res[1] = a[2] * b[0] - a[0] * b[2];
	res[2] = a[0] * b[1] - a[1] * b[0];
}

//Normaliza um vetor
void normalize(float* a) {

	float l = sqrt(a[0] * a[0] + a[1] * a[1] + a[2] * a[2]);
	a[0] = a[0] / l;
	a[1] = a[1] / l;
	a[2] = a[2] / l;
}

float length(float* v) {

	float res = sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
	return res;

}

//Multiplica uma matriz por um vetor
void multMatrixVector(float* m, float* v, float* res) {

	for (int j = 0; j < 4; ++j) {
		res[j] = 0;
		for (int k = 0; k < 4; ++k) {
			res[j] += v[k] * m[j * 4 + k];
		}
	}

}

//Da a posição na cruva, para um dado t e para os pontos de controlo certos
void getCatmullRomPoint(float t, float* p0, float* p1, float* p2, float* p3, float* pos, float* deriv) {

	// catmull-rom matrix
	float m[4][4] = { {-0.5f,  1.5f, -1.5f,  0.5f},
						{ 1.0f, -2.5f,  2.0f, -0.5f},
						{-0.5f,  0.0f,  0.5f,  0.0f},
						{ 0.0f,  1.0f,  0.0f,  0.0f} };

	for (size_t i = 0; i < 3; i++)
	{
		// Compute A = M * P
		float pA[4];
		pA[0] = p0[i];
		pA[1] = p1[i];
		pA[2] = p2[i];
		pA[3] = p3[i];

		float a[4];
		multMatrixVector((float*)m, pA, a);

		// Compute pos = T * A
		float T[4];
		T[0] = pow(t, 3);
		T[1] = pow(t, 2);
		T[2] = t;
		T[3] = 1;

		pos[i] = T[0] * a[0] + T[1] * a[1] + T[2] * a[2] + T[3] * a[3];

		// compute deriv = T' * A

		T[0] = 3 * pow(t, 2);
		T[1] = 2 * t;
		T[2] = 1;
		T[3] = 0;

		deriv[i] = T[0] * a[0] + T[1] * a[1] + T[2] * a[2] + T[3] * a[3];
	}
}


// retorna os pontos da curva, contudo descobre os pontos de controlo a usar
void getAtualCatmullRomPoint(float time, vector<float> p1, vector<float> p2, vector<float> p3, float* pos, float* deriv) {

	int POINT_COUNT = (int) p1.size();
	
	float tempo = glutGet(GLUT_ELAPSED_TIME) % (int)(time * 1000);
	float a = tempo / (time * 1000);
	float t = a * POINT_COUNT;
	int index = floor(t);
	t = t - index;

	// indices store the points
	int indices[4];
	indices[0] = (index + POINT_COUNT - 1) % POINT_COUNT;
	indices[1] = (indices[0] + 1) % POINT_COUNT;
	indices[2] = (indices[1] + 1) % POINT_COUNT;
	indices[3] = (indices[2] + 1) % POINT_COUNT;
	
	float p[4][3];
	p[0][0] = p1[indices[0]];
	p[0][1] = p2[indices[0]];
	p[0][2] = p3[indices[0]];

	p[1][0] = p1[indices[1]];
	p[1][1] = p2[indices[1]];
	p[1][2] = p3[indices[1]];

	p[2][0] = p1[indices[2]];
	p[2][1] = p2[indices[2]];
	p[2][2] = p3[indices[2]];

	p[3][0] = p1[indices[3]];
	p[3][1] = p2[indices[3]];
	p[3][2] = p3[indices[3]];

	getCatmullRomPoint(t, p[0], p[1], p[2], p[3], pos, deriv);
}

//Desenha a curva 
void renderCatmullRomCurve(vector<float> p1, vector<float> p2, vector<float> p3) {
	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);

	float pA[3];
	float ignored[3];

	int POINT_COUNT = (int)p1.size();
	
	glColor3f(0.2f, 0.2f, 0.2f);

	// draw curve using line segments with GL_LINE_LOOP
	glBegin(GL_LINE_LOOP);
	for (float i = 0; i < 1; i += 0.01) {

		float t = i * POINT_COUNT; // this is the real global t
		int index = floor(t);  // which segment
		t = t - index; // where within  the segment

		// indices store the points
		int indices[4];
		indices[0] = (index + POINT_COUNT - 1) % POINT_COUNT;
		indices[1] = (indices[0] + 1) % POINT_COUNT;
		indices[2] = (indices[1] + 1) % POINT_COUNT;
		indices[3] = (indices[2] + 1) % POINT_COUNT;

		float p[4][3];
		p[0][0] = p1[indices[0]];
		p[0][1] = p2[indices[0]];
		p[0][2] = p3[indices[0]];

		p[1][0] = p1[indices[1]];
		p[1][1] = p2[indices[1]];
		p[1][2] = p3[indices[1]];

		p[2][0] = p1[indices[2]];
		p[2][1] = p2[indices[2]];
		p[2][2] = p3[indices[2]];

		p[3][0] = p1[indices[3]];
		p[3][1] = p2[indices[3]];
		p[3][2] = p3[indices[3]];

		getCatmullRomPoint(t, p[0], p[1], p[2], p[3], pA, ignored);
		
		glVertex3f(pA[0], pA[1], pA[2]);
	}
	glEnd();

	glEnable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);
}

//Funcao responsavel por desenhar as figuras nas suas respetivas posicoes
void desenha(GroupVBO g) {

	glPushMatrix();

	for (list<OperacoesGeometricas>::iterator it = g.operacoes.begin(); it != g.operacoes.end(); it++) {
		switch (it->tipo)
		{
		case 'R': {
			if (it->time == 0) {
				glRotatef(it->angle, it->x, it->y, it->z);
			}
			else {
				int t = glutGet(GLUT_ELAPSED_TIME);
				float angle = 360 / (it->time * 1000);
				glRotatef(t * angle, it->x, it->y, it->z);
			}
			break;
		}
		case 'T': {
			
			if (it->time == 0) {
				if (orbitas && it->x != 0)
					desenhaOrbita(it->x);

				glTranslatef(it->x, it->y, it->z);
			}
			else {
				if (orbitas)
					renderCatmullRomCurve(it->p1, it->p2, it->p3);

				float pos[3];
				float xx[3];

				getAtualCatmullRomPoint(it->time, it->p1, it->p2, it->p3, pos, xx);
				
				float zz[3];
				float yy[3];

				yy[0] = it->yy1;
				yy[1] = it->yy2;
				yy[2] = it->yy3;
				
				cross(xx, yy, zz);
				cross(zz, xx, yy);
				
				normalize(yy);
				normalize(xx);
				normalize(zz);

				float rot[16];
				buildRotMatrix(xx, yy, zz, rot);
				
				glTranslated(pos[0], pos[1], pos[2]);

				glMultMatrixf(rot);

				it->yy1 = yy[0];
				it->yy2 = yy[1];
				it->yy3 = yy[2];
			}
			break;
		}
		case 'S': {
			glScalef(it->x, it->y, it->z);
			break;
		}
		}
	}

	for (list<ModeloVBO>::iterator it = g.modelos.begin(); it != g.modelos.end(); it++) {
		
		/////////////

		if(it->texturaId>0)
			glBindTexture(GL_TEXTURE_2D, it->texturaId);
		
		////////

		GLfloat amb[3] = { it->amb.r,it->amb.g,it->amb.b };
		glMaterialfv(GL_FRONT, GL_AMBIENT, amb);

		GLfloat diff[3] = { it->diff.r,it->diff.g,it->diff.b };
		glMaterialfv(GL_FRONT, GL_DIFFUSE, diff);

		GLfloat spec[3] = { it->spec.r,it->spec.g,it->spec.b };
		glMaterialfv(GL_FRONT, GL_SPECULAR, spec);

		glLightfv(GL_LIGHT0, GL_SPECULAR, spec);
		glMaterialf(GL_FRONT, GL_SHININESS, 128);

		GLfloat emi[3] = { it->emi.r,it->emi.g,it->emi.b };
		glMaterialfv(GL_FRONT, GL_EMISSION, emi);

		////////

		glBindBuffer(GL_ARRAY_BUFFER, it->triangulos);
		glVertexPointer(3, GL_FLOAT, 0, 0);

		glBindBuffer(GL_ARRAY_BUFFER, it->normais);
		glNormalPointer(GL_FLOAT, 0, 0);

		glBindBuffer(GL_ARRAY_BUFFER, it->texturas);
		glTexCoordPointer(2, GL_FLOAT, 0, 0);

		glDrawArrays(GL_TRIANGLES, 0, it->tam);

		/////////

		if (it->texturaId > 0)
			glBindTexture(GL_TEXTURE_2D, 0);
	}

	if (!g.subGrupos.empty()) {
		for (list<GroupVBO>::iterator it = g.subGrupos.begin(); it != g.subGrupos.end(); it++)
			desenha(*it);
	}

	glPopMatrix();
}

void desenha2(GroupVBO g) {

	glPushMatrix();

	for (list<OperacoesGeometricas>::iterator it = g.operacoes.begin(); it != g.operacoes.end(); it++) {
		switch (it->tipo)
		{
		case 'R': {
			if (it->time == 0) {
				glRotatef(it->angle, it->x, it->y, it->z);
			}
			else {
				int t = glutGet(GLUT_ELAPSED_TIME);
				float angle = 360 / (it->time * 1000);
				glRotatef(t * angle, it->x, it->y, it->z);
			}
			break;
		}
		case 'T': {

			if (it->time == 0) {
				glTranslatef(it->x, it->y, it->z);
			}
			else {
				float pos[3];
				float xx[3];

				getAtualCatmullRomPoint(it->time, it->p1, it->p2, it->p3, pos, xx);

				float zz[3];
				float yy[3];

				yy[0] = it->yy1;
				yy[1] = it->yy2;
				yy[2] = it->yy3;

				cross(xx, yy, zz);
				cross(zz, xx, yy);

				normalize(yy);
				normalize(xx);
				normalize(zz);

				float rot[16];
				buildRotMatrix(xx, yy, zz, rot);

				glTranslated(pos[0], pos[1], pos[2]);

				glMultMatrixf(rot);
			}
			break;
		}
		case 'S': {
			glScalef(it->x, it->y, it->z);
			break;
		}
		}
	}

	for (list<ModeloVBO>::iterator it = g.modelos.begin(); it != g.modelos.end(); it++) {
		
		if (it->code > 0) {
			
			float color = it->code / 255.0f;

			glColor3f(color, color, color);

			glBindBuffer(GL_ARRAY_BUFFER, it->triangulos);
			glVertexPointer(3, GL_FLOAT, 0, 0);

			glDrawArrays(GL_TRIANGLES, 0, it->tam);
		}
	}

	if (!g.subGrupos.empty()) {
		for (list<GroupVBO>::iterator it = g.subGrupos.begin(); it != g.subGrupos.end(); it++)
			desenha2(*it);
	}

	glPopMatrix();
}


void renderText() {

	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	// set projection so that coordinates match window pixels
	gluOrtho2D(0, w, h, 0);
	glMatrixMode(GL_MODELVIEW);

	glDisable(GL_DEPTH_TEST);
	glPushMatrix();
	glLoadIdentity();
	glRasterPos2d(10, 20); // text position in pixels

	//glColor3f(0.192f, 0.827f, 0.741f);
	glColor3f(1.0f, 1.0f, 1.0f);
	
	if (picked) {
		char text[64];
		char* nomePlaneta = nomes[picked];
		
		sprintf(text, "%s", nomePlaneta);
		for (char* c = text; *c != '\0'; c++) {
			glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, *c);
		}
	}
	else {
		char text[64];
		sprintf(text, " ");
		for (char* c = text; *c != '\0'; c++) {
			glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, *c);
		}
	}

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	glEnable(GL_DEPTH_TEST);

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_LIGHTING);
}


//Funcao responsavel por desenhar no ecra
void renderScene(void) {
		
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	// clear buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// set the camera
	glLoadIdentity();
	float pos1;
	float pos2;
	float pos3;

	float pos4;
	float pos5;
	float pos6;

	if (tipoExplorador) {
		pos1 = sin(beta) * cos(alpha) * raio;
		pos2 = sin(alpha) * raio;
		pos3 = cos(beta) * cos(alpha) * raio;

		pos4 = 0.0f;
		pos5 = 0.0f;
		pos6 = 0.0f;
	}
	else {
		pos1 = px;
		pos2 = py;
		pos3 = pz;

		pos4 = px + lx;
		pos5 = py + ly;
		pos6 = pz + lz;
	}
	
	gluLookAt(pos1, pos2, pos3,
		pos4, pos5, pos6,
		0.0f, 1.0f, 0.0f);

	if(eixos)
		desenhaEixos();

	// Funcoes responsaveis por Rotacoes/Translacoes/Escalas 
	glRotatef(vetorRotacao.x, 1.0f, 0.0f, 0.0f);
	glRotatef(vetorRotacao.y, 0.0f, 1.0f, 0.0f);
	glRotatef(vetorRotacao.z, 0.0f, 0.0f, 1.0f);

	glTranslatef(vetorTranslacao.x, vetorTranslacao.y, vetorTranslacao.z);

	glScalef(vetorEscala.x, vetorEscala.y, vetorEscala.z);

	//Funcao responsavel por defenir o tipo de desenho (Pontos/Linhas/Solido)
	glPolygonMode(GL_FRONT_AND_BACK, tipoDesenho);

	int numLuz = 0;
	for (list<Luz>::iterator it = luz.begin(); (it != luz.end()) && numLuz < 8; it++) {

		switch (it->tipoLuz)
		{
			case 'P': {
				float pos[4];
				pos[0] = it->posLuzX;
				pos[1] = it->posLuzY;
				pos[2] = it->posLuzZ;
				pos[3] = 1.0f;
				glLightfv(GL_LIGHT0 + numLuz, GL_POSITION, pos);
				break;
			}
			case 'D': {
				float pos[4];
				pos[0] = it->posLuzX;
				pos[1] = it->posLuzY;
				pos[2] = it->posLuzZ;
				pos[3] = 0.0f;
				glLightfv(GL_LIGHT0 + numLuz, GL_POSITION, pos);
				break;
			}
			case 'S': {
				GLfloat spot_direction[] = { it->posLuzX, it->posLuzY, it->posLuzZ };
				glLightf(GL_LIGHT0 + numLuz, GL_SPOT_CUTOFF, it->angleLuz);
				glLightfv(GL_LIGHT0 + numLuz, GL_SPOT_DIRECTION, spot_direction);
				break;
			}
		}

		numLuz++;
	}

	// Desenho dos triangulos
	desenha(vbos);
	
	char tit[128];
	float fps;
	int time;

	frame++;
	time = glutGet(GLUT_ELAPSED_TIME);
	if (time - timebase > 1000) {
		fps = frame * 1000.0 / (time - timebase);
		timebase = time;
		frame = 0;

		int tempo = ceil(glutGet(GLUT_ELAPSED_TIME) / 1000);
		sprintf(tit, "Projeto2021_Fase4 | Tempo: %d | FPS: %f6.2", tempo, fps);

		glutSetWindowTitle(tit);
	}

	renderText();

	// End of frame
	glutSwapBuffers();
}

// Funcao responsavel por receber eventos do teclado (Teclas Normais)
void getNormalKey(unsigned char key, int x, int y) {
	switch (key)
	{
	case 'a': {
		*translacao -= 2;
		break;
	}
	case 'd': {
		*translacao += 2;
		break;
	}
	case 'w': {
		*angle += 2;
		break;
	}
	case 's': {
		*angle -= 2;
		break;
	}
	case 'q': {
		*escala += 0.1;

		if (*escala > 0 && (*escala - 0.2 < 0)) {
			int num = vetorEscala.numCoordNegativas();

			if (num == 0) {
				glFrontFace(GL_CCW);
			}
			else {
				if (num == 2)
					glFrontFace(GL_CCW);
				else
					glFrontFace(GL_CW);
			}
		}
		break;
	}
	case 'e': {
		*escala -= 0.1;

		if (*escala < 0 && (*escala + 0.2 > 0)) {
			int num = vetorEscala.numCoordNegativas();

			if (num == 0) {
				glFrontFace(GL_CCW);
			}
			else {
				if (num == 2)
					glFrontFace(GL_CCW);
				else
					glFrontFace(GL_CW);
			}
		}
		break;
	}
	case 'i': {
		tipoDesenho = GL_LINE;
		break;
	}
	case 'o': {
		tipoDesenho = GL_POINT;
		break;
	}
	case 'p': {
		tipoDesenho = GL_FILL;
		break;
	}
	case 'x': {
		angle = &vetorRotacao.x;
		escala = &vetorEscala.x;
		translacao = &vetorTranslacao.x;
		break;
	}
	case 'y': {
		angle = &vetorRotacao.y;
		escala = &vetorEscala.y;
		translacao = &vetorTranslacao.y;
		break;
	}
	case 'z': {
		angle = &vetorRotacao.z;
		escala = &vetorEscala.z;
		translacao = &vetorTranslacao.z;
		break;
	}
	case 't': {
		orbitas = !orbitas;
		break;
	}
	case 'r': {
		eixos = !eixos;
		break;
	}
	case '0': {
		tipoExplorador = !tipoExplorador;
		break;
	}
	case '1': {
		raio = 15.0f;
		break;
	}
	default:
		break;
	}
}

// Funcao responsavel por receber eventos do teclado (Teclas Especiais)
void getSpecialKey(int key_code, int x, int y) {
	switch (key_code)
	{
	case GLUT_KEY_RIGHT: {
		if (!tipoExplorador) {
			px -= lz;
			pz += lx;
		}
		break;
	}
	case GLUT_KEY_LEFT: {
		if (!tipoExplorador) {
			px += lz;
			pz -= lx;
		}
		break;
	}
	case GLUT_KEY_UP: {
		if (!tipoExplorador) {
			px += lx;
			py += ly;
			pz += lz;
		}
		break;
	}
	case GLUT_KEY_DOWN: {
		if (!tipoExplorador) {
			px -= lx;
			py -= ly;
			pz -= lz;
		}
		break;
	}
	default:
		break;
	}
}

unsigned char  picking(int x, int y) {

	unsigned char res[4];

	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);

	glClear(GL_COLOR_BUFFER_BIT);
	glLoadIdentity();

	float pos1;
	float pos2;
	float pos3;

	float pos4;
	float pos5;
	float pos6;
	if (tipoExplorador) {
		pos1 = sin(beta) * cos(alpha) * raio;
		pos2 = sin(alpha) * raio;
		pos3 = cos(beta) * cos(alpha) * raio;

		pos4 = 0.0f;
		pos5 = 0.0f;
		pos6 = 0.0f;
	}
	else {
		pos1 = px;
		pos2 = py;
		pos3 = pz;

		pos4 = px + lx;
		pos5 = py + ly;
		pos6 = pz + lz;
	}

	gluLookAt(pos1, pos2, pos3,
		pos4, pos5, pos6,
		0.0f, 1.0f, 0.0f);


	glDepthFunc(GL_LEQUAL);
	desenha2(vbos);
	glDepthFunc(GL_LESS);

	GLint viewport[4];

	glGetIntegerv(GL_VIEWPORT, viewport);
	glReadPixels(x, viewport[3] - y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, res);

	glEnable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);

	return res[0];
}

//Funcao responsavel por receber eventos de carregar no rato
void apertaRato(int botao, int estado, int x, int y) {
	if (botao == GLUT_LEFT_BUTTON) {
		if (estado == GLUT_DOWN) {
			botaoEsqCarregado = true;
			posXRato = x;
			posYRato = y;
		}
		else {
			botaoEsqCarregado = false;
		}
	}
	if (botao == GLUT_RIGHT_BUTTON) {
		if (estado == GLUT_DOWN) {
			botaoDirCarregado = true;
			posXRato = x;
			posYRato = y;
		}
		else {
			botaoDirCarregado = false;
		}
	}
	if (botao == GLUT_MIDDLE_BUTTON) {
		picked = picking(x, y);
	}
}

//Funcao responsavel por receber eventos de movimento do rato
void movimentoRato(int x, int y) {

	if (tipoExplorador) {
		if (botaoEsqCarregado && !botaoDirCarregado) {
			if (posXRato != x) {
				beta = (posXRato - x) * 0.007;
			}

			if (posYRato != y) {
				float teste = (posYRato - y) * 0.01;
				if (teste > -M_PI_2 && teste < M_PI_2)
					alpha = teste;
			}
		}

		if (botaoDirCarregado && !botaoEsqCarregado) {
			if (posYRato < y) {
				raio += 0.05;
			}
			else {
				if (raio > 0 && posYRato > y) {
					raio -= 0.05;
				}
			}
		}
	}
	else {
		if (botaoEsqCarregado) {
			int dx = ((posXRato - x) * 0.07) * -1;
			int dy = (posYRato - y) * 0.07;

			alphaFPS = alphaFPS + dx * 0.0005;
			betaFPS = betaFPS + dy * 0.0005;

			lx = sin(betaFPS) * sin(alphaFPS);
			ly = -cos(betaFPS);
			lz = -sin(betaFPS) * cos(alphaFPS);
		}
	}
}

int loadTexture(string s) {

	unsigned int t, tw, th;
	unsigned char* texData;
	unsigned int texID;

	ilInit();
	ilEnable(IL_ORIGIN_SET);
	ilOriginFunc(IL_ORIGIN_LOWER_LEFT);
	ilGenImages(1, &t);
	ilBindImage(t);
	ilLoadImage((ILstring)s.c_str());
	tw = ilGetInteger(IL_IMAGE_WIDTH);
	th = ilGetInteger(IL_IMAGE_HEIGHT);
	ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);
	texData = ilGetData();

	glGenTextures(1, &texID);

	glBindTexture(GL_TEXTURE_2D, texID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tw, th, 0, GL_RGBA, GL_UNSIGNED_BYTE, texData);
	glGenerateMipmap(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, 0);

	return texID;
}

// Funcao responsavel por careegar a estrutura de dados usado para desenhar os modelos com vbos
GroupVBO carregaVBO(Group g) {

	list<OperacoesGeometricas> ops = g.operacoes;
	list<ModeloVBO> l;
	for (list<Modelo>::iterator it = g.modelos.begin(); it != g.modelos.end(); it++) {
		vector<float> p;
		vector<float> n;
		vector<float> t;
		for (list<Triangulo>::iterator it2 = it->triangulos.begin(); it2 != it->triangulos.end(); it2++) {
			p.push_back(it2->p1.x);
			p.push_back(it2->p1.y);
			p.push_back(it2->p1.z);

			p.push_back(it2->p2.x);
			p.push_back(it2->p2.y);
			p.push_back(it2->p2.z);

			p.push_back(it2->p3.x);
			p.push_back(it2->p3.y);
			p.push_back(it2->p3.z);
		}

		for (list<Ponto>::iterator it2 = it->normais.begin(); it2 != it->normais.end(); it2++) {
			n.push_back(it2->x);
			n.push_back(it2->y);
			n.push_back(it2->z);
		}
		
		for (list<Ponto>::iterator it2 = it->texturas.begin(); it2 != it->texturas.end(); it2++) {
			t.push_back(it2->x);
			t.push_back(it2->y);
		}

		GLuint vertices;
		GLuint normais;
		GLuint texturas;
		glGenBuffers(1, &vertices);
		glGenBuffers(1, &normais);
		glGenBuffers(1, &texturas);

		glBindBuffer(GL_ARRAY_BUFFER, vertices);
		glBufferData(
			GL_ARRAY_BUFFER, // tipo do buffer, só é relevante na altura do desenho
			sizeof(float) * p.size(), // tamanho do vector em bytes
			p.data(), // os dados do array associado ao vector
			GL_STATIC_DRAW); // indicativo da utilização (estático e para desenho)

		glBindBuffer(GL_ARRAY_BUFFER, normais);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * n.size(), n.data(), GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, texturas);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * t.size(), t.data(), GL_STATIC_DRAW);

		int texturaID = -1;
		if (!it->nomeTextura.empty()) {
			texturaID = loadTexture(it->nomeTextura);
		}

		char* temp = (char*)malloc(sizeof(char) * 20);
		strcpy(temp, it->nome);

		nomes.insert(pair<int, char*>(it->code, temp));

		ModeloVBO model(vertices, normais, texturas, p.size(), texturaID, it->amb, it->diff, it->spec, it->emi, it->code);
		l.push_back(model);
		
	}

	list<GroupVBO> l2;
	if (!g.subGrupos.empty()) {
		for (list<Group>::iterator it = g.subGrupos.begin(); it != g.subGrupos.end(); it++)
			l2.push_back(carregaVBO(*it));
	}

	GroupVBO gVBO(l,l2,ops);
	return gVBO;
}

void carregaDados(char *nome) {
	Scene cena;
	cena.load(nome);

	glewInit();
	
	glEnable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnable(GL_RESCALE_NORMAL);

	vbos = carregaVBO(cena.g);
	luz = cena.luz;

	int numLuz = 0;
	for (list<Luz>::iterator it = luz.begin(); (it != luz.end()) && numLuz<8; it++) {
		GLfloat light_ambient[] = { 0.1, 0.1, 0.1, 1.0 };
		GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
		GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };

		glEnable(GL_LIGHT0 + numLuz);
			
		glLightfv(GL_LIGHT0 + numLuz, GL_AMBIENT, light_ambient);
		glLightfv(GL_LIGHT0 + numLuz, GL_DIFFUSE, light_diffuse);
		glLightfv(GL_LIGHT0 + numLuz, GL_SPECULAR, light_specular);

		numLuz++;
	}
}


//Funcao MAIN
int main(int argc, char** argv) {

	if (argc < 2) {
		//Faltou introduzir o nome do ficheiro XML na linha de comandos
		printf("Nenhum ficheiro XML indicado\n\n");
		return 1;
	}

	// Inicia a rotação em torno do eixo do X
	angle = &vetorRotacao.x;

	// Inicia a escala a aumentar em X
	escala = &vetorEscala.x;

	// Inicia a translacao em torno do eixo do X
	translacao = &vetorTranslacao.x;

	// init GLUT and the window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(800, 800);

	glutCreateWindow("Projeto2021_Fase4");

	// Required callback registry 
	glutDisplayFunc(renderScene);
	glutIdleFunc(renderScene);
	glutReshapeFunc(changeSize);
	glutMouseFunc(apertaRato);
	glutMotionFunc(movimentoRato);


	// put here the registration of the keyboard callbacks
	glutKeyboardFunc(getNormalKey);
	glutSpecialFunc(getSpecialKey);

	//Carrega os dados para os VBOs
	carregaDados(argv[1]);
	
	// enter GLUT's main cycle
	glutMainLoop();

	return 1;
}
