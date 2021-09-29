#include "Cena.h"

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#define _USE_MATH_DEFINES // always before the include
#include <math.h>
#include <stdio.h>

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

Scene cena;

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
		glVertex3f(
			-100.0f, 0.0f, 0.0f);
		glVertex3f(100.0f, 0.0f, 0.0f);
		// Y Axis in Green
		glColor3f(0.0f, 1.0f, 0.0f);
		glVertex3f(0.0f,
			-100.0f, 0.0f);
		glVertex3f(0.0f, 100.0f, 0.0f);
		// Z Axis in Blue
		glColor3f(0.0f, 0.0f, 1.0f);
		glVertex3f(0.0f, 0.0f,
			-100.0f);
		glVertex3f(0.0f, 0.0f, 100.0f);
	glEnd();
}


// --- Draw Eliptic Orbit  --- //
void desenhaOrbita(float raio) {

	glColor3f(1.0f, 1.0f, 1.0f);

	glBegin(GL_LINE_STRIP);
		for (int i = 0; i <= 360; i++) {
			float angle = (i * M_PI) / 180;
			glVertex3f(cos(angle) * raio, 0, sin(angle) * raio);
		}
	glEnd();
}

//Funcao responsavel por desenhar as figuras nas suas respetivas posicoes
void desenha(Group g) {
	
	glPushMatrix();
	
	for (list<OperacoesGeometricas>::iterator it = g.operacoes.begin(); it != g.operacoes.end(); it++) {
		switch (it->tipo)
		{
			case 'R': {
				glRotatef(it->angle, it->x, it->y, it->z);
				break;
			}
			case 'T': {
				if (orbitas && it->x != 0)
					desenhaOrbita(it->x);

				glTranslatef(it->x, it->y, it->z);
				break;
			}
			case 'S': {
				glScalef(it->x, it->y, it->z);
				break;
			}
		}
	}
	
	glBegin(GL_TRIANGLES);
		for (list<Modelo>::iterator it = g.modelos.begin(); it != g.modelos.end(); it++) {
			int j = 1;
			for (map<Cor, list<Triangulo>>::iterator it2 = it->triangulos.begin(); it2 != it->triangulos.end(); it2++) {
				glColor3f(it2->first.r, it2->first.g, it2->first.b);
				for (list<Triangulo>::iterator it3 = it2->second.begin(); it3 != it2->second.end(); it3++) {
						glVertex3f(it3->p1.x, it3->p1.y, it3->p1.z);
						glVertex3f(it3->p2.x, it3->p2.y, it3->p2.z);
						glVertex3f(it3->p3.x, it3->p3.y, it3->p3.z);
				}
			}
		}
	glEnd();

	if (!g.subGrupos.empty()) {
		for (list<Group>::iterator it = g.subGrupos.begin(); it != g.subGrupos.end(); it++)
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
	desenha(cena.g);
	
	// End of frame
	glutSwapBuffers();
}

// Funcao responsavel por receber eventos do teclado (Teclas Normais)
void getNormalKey(unsigned char key, int x, int y) {
	switch (key)
	{
	case 'a': {
		*translacao -= 2;

		glutPostRedisplay();
		break;
	}
	case 'd': {
		*translacao += 2;

		glutPostRedisplay();
		break;
	}
	case 'w': {
		*angle += 2;

		glutPostRedisplay();
		break;
	}
	case 's': {
		*angle -= 2;

		glutPostRedisplay();
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

		glutPostRedisplay();
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

		glutPostRedisplay();
		break;
	}
	case 'i': {
		tipoDesenho = GL_LINE;

		glutPostRedisplay();
		break;
	}
	case 'o': {
		tipoDesenho = GL_POINT;

		glutPostRedisplay();
		break;
	}
	case 'p': {
		tipoDesenho = GL_FILL;

		glutPostRedisplay();
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

		glutPostRedisplay();
		break;
	}
	case 'z': {
		angle = &vetorRotacao.z;
		escala = &vetorEscala.z;
		translacao = &vetorTranslacao.z;

		glutPostRedisplay();
		break;
	}
	case 't': {
		orbitas = !orbitas;

		glutPostRedisplay();
		break;
	}
	case 'r': {
		eixos = !eixos;

		glutPostRedisplay();
		break;
	}
	case '0': {
		tipoExplorador = !tipoExplorador;

		glutPostRedisplay();
		break;
	}
	case '1': {
		raio = 15.0f;
		glutPostRedisplay();
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

			glutPostRedisplay();
		}
		break;
	}
	case GLUT_KEY_LEFT: {
		if (!tipoExplorador) {
			px += lz * 1;
			pz -= lx * 1;

			glutPostRedisplay();
		}
		break;
	}
	case GLUT_KEY_UP: {
		if (!tipoExplorador) {
			px += lx * 1;
			py += ly * 1;
			pz += lz * 1;

			glutPostRedisplay();
		}
		break;
	}
	case GLUT_KEY_DOWN: {
		if (!tipoExplorador) {
			px -= lx * 1;
			py -= ly * 1;
			pz -= lz * 1;

			glutPostRedisplay();
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

			glutPostRedisplay();
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

			glutPostRedisplay();
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

			glutPostRedisplay();
		}
	}
}

//Funcao MAIN
int main(int argc, char** argv) {

	if (argc < 2) {
		//Faltou introduzir o nome do ficheiro XML na linha de comandos
		printf("Nenhum ficheiro XML indicado\n\n");
		return 1;
	}

	cena.load(argv[1]);

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

	glutCreateWindow("Projeto2021_Fase2");

	// Required callback registry 
	glutDisplayFunc(renderScene);
	glutReshapeFunc(changeSize);
	glutMouseFunc(apertaRato);
	glutMotionFunc(movimentoRato);


	// put here the registration of the keyboard callbacks
	glutKeyboardFunc(getNormalKey);
	glutSpecialFunc(getSpecialKey);

	//  OpenGL settings
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	// enter GLUT's main cycle
	glutMainLoop();

	return 1;
}
