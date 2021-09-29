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

float alpha = 0.5f;
float beta  = 0.5f;
float raio  = 7.0f;

Vetor vetorTranslacao(0.0f, 0.0f, 0.0f);
Vetor vetorRotacao(0.0f, 0.0f, 0.0f);
Vetor vetorEscala(1.0f, 1.0f, 1.0f);

bool botaoEsqCarregado = false;
bool botaoDirCarregado = false;
int posXRato = 0, posYRato = 0;


Scene cena;

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

//Funcao responsavel por desenhar no ecra
void renderScene(void) {

	// clear buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// set the camera
	glLoadIdentity();
	float pos1 = sin(beta) * cos(alpha) * raio;
	float pos2 = sin(alpha) * raio;
	float pos3 = cos(beta) * cos(alpha) * raio;
	gluLookAt(pos1, pos2, pos3,
		0.0, 0.0, 0.0,
		0.0f, 1.0f, 0.0f);

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
	glBegin(GL_TRIANGLES);
		for (list<Modelo>::iterator it = (cena).modelos.begin(); it != (cena).modelos.end(); it++) {
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

	// End of frame
	glutSwapBuffers();
}

// Funcao responsavel por receber eventos do teclado (Teclas Normais)
void getNormalKey(unsigned char key, int x, int y) {
	switch (key)
	{
	case 'a':{
		*translacao -= 0.1;

		glutPostRedisplay();
		break;
	}
	case 'd': {
		*translacao += 0.1;

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
	case 'm': {
		glFrontFace(GL_CCW);

		glutPostRedisplay();
		break;
	}
	case 'n': {
		glFrontFace(GL_CW);

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
	case GLUT_KEY_RIGHT :{
		*angle += 1;

		glutPostRedisplay();
		break;
	}
	case GLUT_KEY_LEFT: {
		*angle -= 1;

		glutPostRedisplay();
		break;
	}
	case GLUT_KEY_UP: {
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
	case GLUT_KEY_DOWN: {
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
	float teste;
	if (botaoEsqCarregado && !botaoDirCarregado) {
		if (posXRato != x) {
			beta = (posXRato - x) * 0.007;
		}
		
		if (posYRato != y) {
			teste = (posYRato - y) * 0.01;
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

//Funcao MAIN
int main(int argc, char **argv) {

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
	glutInitDisplayMode(GLUT_DEPTH|GLUT_DOUBLE|GLUT_RGBA);
	glutInitWindowPosition(100,100);
	glutInitWindowSize(800,800);
	
	glutCreateWindow("Projeto2021_Fase1");
		
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
