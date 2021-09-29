#include "Cena.h"

#ifdef __APPLE__
#include <GLUT/glut.h>
#include <GL/glew.h>
#else
#include <GL/glew.h>
#include <GL/glut.h>
#endif

#define _USE_MATH_DEFINES // always before the include
#include <math.h>
#include <stdio.h>

////////////////////////////////////////////////////////////////////////

class ModeloVBO {
public:

	map<Cor, GLuint> triangulos;
	map<Cor, int> tam;

	ModeloVBO() {
		this->triangulos = map<Cor, GLuint>();
	}

	ModeloVBO(map<Cor, GLuint> triangulos, map<Cor, int> tam) {
		this->triangulos = triangulos;
		this->tam = tam;
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
////////////////////////////////////////////////////////////////////////


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

GroupVBO vbos;

//Funcao responsavel por receber um evento de mudar o tamanho da janela
void changeSize(int w, int h) {

	// Prevent a divide by zero, when window is too short
	// (you cant make a window with zero width).
	if(h == 0)
		h = 1;

	// compute window's aspect ratio 
	float ratio = w * 1.0 / h;

	// Set the projection matrix as current
	glMatrixMode(GL_PROJECTION);
	// Load Identity Matrix
	glLoadIdentity();
	
	// Set the viewport to be the entire window
    glViewport(0, 0, w, h);

	// Set perspective
	gluPerspective(45.0f ,ratio, 1.0f ,1000.0f);

	// return to the model view matrix mode
	glMatrixMode(GL_MODELVIEW);
}

//Funcao que desenha os eixos do X,Y,Z
void desenhaEixos() {
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
}


//Desenha a orbita dos planetas
void desenhaOrbita(float raio) {

	glColor3f(1.0f, 1.0f, 1.0f);

	glBegin(GL_LINE_LOOP);
		for (int i = 0; i <= 360; i++) {
			float angle = (i * M_PI) / 180;
			glVertex3f(cos(angle) * raio, 0, sin(angle) * raio);
		}
	glEnd();
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

//Noemaliza um vetor
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
	
	float pA[3];
	float ignored[3];

	int POINT_COUNT = (int)p1.size();
	
	glColor3f(1.0f, 1.0f, 1.0f);

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
				
				//printf("%f %f %f\n", pos[0], pos[1], pos[2]);

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
		int j = 1;
		for (map<Cor, GLuint>::iterator it2 = it->triangulos.begin(); it2 != it->triangulos.end(); it2++) {
			Cor c(it2->first.r, it2->first.g, it2->first.b);
			glColor3f(c.r, c.g, c.b);
			int t = (it->tam)[c];
			glBindBuffer(GL_ARRAY_BUFFER, it2->second);
			glVertexPointer(3, GL_FLOAT, 0, 0);
			glDrawArrays(GL_TRIANGLES, 0, t);
		}
	}

	if (!g.subGrupos.empty()) {
		for (list<GroupVBO>::iterator it = g.subGrupos.begin(); it != g.subGrupos.end(); it++)
			desenha(*it);
	}

	glPopMatrix();
}

//Funcao responsavel por desenhar no ecra
void renderScene(void) {

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

	// Desenho dos triangulos
	desenha(vbos);
	
	char tit[35];
	int tempo = ceil(glutGet(GLUT_ELAPSED_TIME) / 1000);
	sprintf(tit, "Projeto2021_Fase3 | Tempo: %d", tempo);
	glutSetWindowTitle(tit);

	// End of frame
	glutSwapBuffers();
}

// Funcao responsavel por receber eventos do teclado (Teclas Normais)
void getNormalKey(unsigned char key, int x, int y) {
	switch (key)
	{
	case 'a': {
		*translacao -= 2;

		//glutPostRedisplay();
		break;
	}
	case 'd': {
		*translacao += 2;

		//glutPostRedisplay();
		break;
	}
	case 'w': {
		*angle += 2;

		//glutPostRedisplay();
		break;
	}
	case 's': {
		*angle -= 2;

		//glutPostRedisplay();
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

		//glutPostRedisplay();
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

		//glutPostRedisplay();
		break;
	}
	case 'i': {
		tipoDesenho = GL_LINE;

		//glutPostRedisplay();
		break;
	}
	case 'o': {
		tipoDesenho = GL_POINT;

		//glutPostRedisplay();
		break;
	}
	case 'p': {
		tipoDesenho = GL_FILL;

		//glutPostRedisplay();
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

		//glutPostRedisplay();
		break;
	}
	case 'z': {
		angle = &vetorRotacao.z;
		escala = &vetorEscala.z;
		translacao = &vetorTranslacao.z;

		//glutPostRedisplay();
		break;
	}
	case 't': {
		orbitas = !orbitas;

		//glutPostRedisplay();
		break;
	}
	case 'r': {
		eixos = !eixos;

		//glutPostRedisplay();
		break;
	}
	case '0': {
		tipoExplorador = !tipoExplorador;

		//glutPostRedisplay();
		break;
	}
	case '1': {
		raio = 15.0f;
		//glutPostRedisplay();
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
			px -= lz * 1;
			pz += lx * 1;

			//glutPostRedisplay();
		}
		break;
	}
	case GLUT_KEY_LEFT: {
		if (!tipoExplorador) {
			px += lz * 1;
			pz -= lx * 1;

			//glutPostRedisplay();
		}
		break;
	}
	case GLUT_KEY_UP: {
		if (!tipoExplorador) {
			px += lx * 1;
			py += ly * 1;
			pz += lz * 1;

			//glutPostRedisplay();
		}
		break;
	}
	case GLUT_KEY_DOWN: {
		if (!tipoExplorador) {
			px -= lx * 1;
			py -= ly * 1;
			pz -= lz * 1;

			//glutPostRedisplay();
		}
		break;
	}
	default:
		break;
	}
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

			//glutPostRedisplay();
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

			//glutPostRedisplay();
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

			//glutPostRedisplay();
		}
	}
}

// Funcao responsavel por careegar a estrutura de dados usado para desenhar os modelos com vbos
GroupVBO carregaVBO(Group g) {

	list<OperacoesGeometricas> ops = g.operacoes;
	list<ModeloVBO> l;
	for (list<Modelo>::iterator it = g.modelos.begin(); it != g.modelos.end(); it++) {
		vector<float> p;
		map<Cor, GLuint> m;
		map<Cor, int> t;
		for (map<Cor, list<Triangulo>>::iterator it2 = it->triangulos.begin(); it2 != it->triangulos.end(); it2++) {
			Cor c(it2->first.r, it2->first.g, it2->first.b);
			for (list<Triangulo>::iterator it3 = it2->second.begin(); it3 != it2->second.end(); it3++) {
				
				p.push_back(it3->p1.x);
				p.push_back(it3->p1.y);
				p.push_back(it3->p1.z);

				p.push_back(it3->p2.x);
				p.push_back(it3->p2.y);
				p.push_back(it3->p2.z);

				p.push_back(it3->p3.x);
				p.push_back(it3->p3.y);
				p.push_back(it3->p3.z);
			}
			GLuint vertices;
			glGenBuffers(1, &vertices);
			glBindBuffer(GL_ARRAY_BUFFER, vertices);
			glBufferData(
				GL_ARRAY_BUFFER, // tipo do buffer, só é relevante na altura do desenho
				sizeof(float) * p.size(), // tamanho do vector em bytes
				p.data(), // os dados do array associado ao vector
				GL_STATIC_DRAW); // indicativo da utilização (estático e para desenho)
			m.insert(pair<Cor, GLuint>(c, vertices));
			t.insert(pair<Cor, int>(c, p.size()));
		}
		ModeloVBO model(m, t);
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
	glEnableClientState(GL_VERTEX_ARRAY);
	vbos = carregaVBO(cena.g);
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

	glutCreateWindow("Projeto2021_Fase3");

	// Required callback registry 
	glutDisplayFunc(renderScene);
	glutIdleFunc(renderScene);
	glutReshapeFunc(changeSize);
	glutMouseFunc(apertaRato);
	glutMotionFunc(movimentoRato);


	// put here the registration of the keyboard callbacks
	glutKeyboardFunc(getNormalKey);
	glutSpecialFunc(getSpecialKey);

	//  OpenGL settings
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	//Carrega os dados para os VBOs
	carregaDados(argv[1]);
	
	// enter GLUT's main cycle
	glutMainLoop();

	return 1;
}
