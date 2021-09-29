#include <stdlib.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <iostream>
#include <fstream>
#include <string.h>

using namespace std;

void plane(float x, float z, float partition, char* file) {

	ofstream write(file);

	float number_vertices = 12 * (partition * partition);

	write << number_vertices << endl;

	float movimentoX = x / partition;
	float movimentoZ = z / partition;

	for (int i = 0; i < partition; i++) {

		float myZ = -z / 2 + (movimentoZ * i);

		for (int j = 0; j < partition; j++) {

			float myX = -x / 2 + (movimentoX * j);

			//triangulos de cima
			float p1_x = myX;
			float p1_z = myZ;

			float p2_x = myX + movimentoX;
			float p2_z = myZ + movimentoZ;

			float p3_x = myX + movimentoX;
			float p3_z = myZ;

			float p4_x = myX;
			float p4_z = myZ + movimentoZ;

			write << p1_x << " " << 0.0f << " " << p1_z << endl;
			write << p2_x << " " << 0.0f << " " << p2_z << endl;
			write << p3_x << " " << 0.0f << " " << p3_z << endl;

			write << p1_x << " " << 0.0f << " " << p1_z << endl;
			write << p4_x << " " << 0.0f << " " << p4_z << endl;
			write << p2_x << " " << 0.0f << " " << p2_z << endl;

			//triangulos de baixo

			write << p1_x << " " << 0.0f << " " << p1_z << endl;
			write << p3_x << " " << 0.0f << " " << p3_z << endl;
			write << p2_x << " " << 0.0f << " " << p2_z << endl;

			write << p1_x << " " << 0.0f << " " << p1_z << endl;
			write << p2_x << " " << 0.0f << " " << p2_z << endl;
			write << p4_x << " " << 0.0f << " " << p4_z << endl;

		}
	}

	write.close();
}

void box(float x, float z, float y, float partition, char* file) {

	ofstream write(file);

	float number_vertices = 6 * (6 * partition * partition);

	write << number_vertices << endl;

	float movimentoX = x / partition;
	float movimentoY = y / partition;
	float movimentoZ = z / partition;

	//face frente
	float myZ = z / 2;

	for (int i = 0; i < partition; i++) {

		float myY = -y / 2 + (movimentoY * i);

		for (int j = 0; j < partition; j++) {

			float myX = -x / 2 + (movimentoX * j);

			float p1_x = myX;
			float p1_y = myY;

			float p2_x = myX + movimentoX;
			float p2_y = myY;

			float p3_x = myX;
			float p3_y = myY + movimentoY;

			float p4_x = myX + movimentoX;
			float p4_y = myY + movimentoY;

			write << p1_x << " " << p1_y << " " << myZ << endl;
			write << p2_x << " " << p2_y << " " << myZ << endl;
			write << p3_x << " " << p3_y << " " << myZ << endl;

			write << p2_x << " " << p2_y << " " << myZ << endl;
			write << p4_x << " " << p4_y << " " << myZ << endl;
			write << p3_x << " " << p3_y << " " << myZ << endl;
		}
	}

	//face tras
	myZ = -z / 2;

	for (int i = 0; i < partition; i++) {

		float myY = -y / 2 + (movimentoY * i);

		for (int j = 0; j < partition; j++) {

			float myX = -x / 2 + (movimentoX * j);

			float p1_x = myX;
			float p1_y = myY;

			float p2_x = myX;
			float p2_y = myY + movimentoY;

			float p3_x = myX + movimentoX;
			float p3_y = myY;

			float p4_x = myX + movimentoX;
			float p4_y = myY + movimentoY;

			write << p1_x << " " << p1_y << " " << myZ << endl;
			write << p2_x << " " << p2_y << " " << myZ << endl;
			write << p3_x << " " << p3_y << " " << myZ << endl;

			write << p2_x << " " << p2_y << " " << myZ << endl;
			write << p4_x << " " << p4_y << " " << myZ << endl;
			write << p3_x << " " << p3_y << " " << myZ << endl;
		}
	}

	//face direita
	float myX = x / 2;

	for (int i = 0; i < partition; i++) {

		float myY = -y / 2 + (movimentoY * i);

		for (int j = 0; j < partition; j++) {

			float myZ = -z / 2 + (movimentoZ * j);

			float p1_y = myY;
			float p1_z = myZ;

			float p2_y = myY + movimentoY;
			float p2_z = myZ + movimentoZ;

			float p3_y = myY;
			float p3_z = myZ + movimentoZ;

			float p4_y = myY + movimentoY;
			float p4_z = myZ;

			write << myX << " " << p1_y << " " << p1_z << endl;
			write << myX << " " << p2_y << " " << p2_z << endl;
			write << myX << " " << p3_y << " " << p3_z << endl;

			write << myX << " " << p1_y << " " << p1_z << endl;
			write << myX << " " << p4_y << " " << p4_z << endl;
			write << myX << " " << p2_y << " " << p2_z << endl;
		}
	}

	//face esquerda
	myX = -x / 2;

	for (int i = 0; i < partition; i++) {

		float myY = -y / 2 + (movimentoY * i);

		for (int j = 0; j < partition; j++) {

			float myZ = -z / 2 + (movimentoZ * j);

			float p1_y = myY;
			float p1_z = myZ;

			float p2_y = myY;
			float p2_z = myZ + movimentoZ;

			float p3_y = myY + movimentoY;
			float p3_z = myZ + movimentoZ;

			float p4_y = myY + movimentoY;
			float p4_z = myZ;

			write << myX << " " << p1_y << " " << p1_z << endl;
			write << myX << " " << p2_y << " " << p2_z << endl;
			write << myX << " " << p3_y << " " << p3_z << endl;

			write << myX << " " << p1_y << " " << p1_z << endl;
			write << myX << " " << p3_y << " " << p3_z << endl;
			write << myX << " " << p4_y << " " << p4_z << endl;
		}
	}

	//face baixo
	float myY = -y / 2;

	for (int i = 0; i < partition; i++) {

		float myZ = -z / 2 + (movimentoZ * i);

		for (int j = 0; j < partition; j++) {

			float myX = -x / 2 + (movimentoX * j);

			float p1_x = myX;
			float p1_z = myZ;

			float p2_x = myX + movimentoX;
			float p2_z = myZ + movimentoZ;

			float p3_x = myX;
			float p3_z = myZ + movimentoZ;

			float p4_x = myX + movimentoX;
			float p4_z = myZ;

			write << p1_x << " " << myY << " " << p1_z << endl;
			write << p2_x << " " << myY << " " << p2_z << endl;
			write << p3_x << " " << myY << " " << p3_z << endl;

			write << p1_x << " " << myY << " " << p1_z << endl;
			write << p4_x << " " << myY << " " << p4_z << endl;
			write << p2_x << " " << myY << " " << p2_z << endl;
		}
	}

	//face cima
	myY = y / 2;

	for (int i = 0; i < partition; i++) {

		float myZ = -z / 2 + (movimentoZ * i);

		for (int j = 0; j < partition; j++) {

			float myX = -x / 2 + (movimentoX * j);

			float p1_x = myX;
			float p1_z = myZ;

			float p2_x = myX;
			float p2_z = myZ + movimentoZ;

			float p3_x = myX + movimentoX;
			float p3_z = myZ + movimentoZ;

			float p4_x = myX + movimentoX;
			float p4_z = myZ;

			write << p1_x << " " << myY << " " << p1_z << endl;
			write << p2_x << " " << myY << " " << p2_z << endl;
			write << p3_x << " " << myY << " " << p3_z << endl;

			write << p1_x << " " << myY << " " << p1_z << endl;
			write << p3_x << " " << myY << " " << p3_z << endl;
			write << p4_x << " " << myY << " " << p4_z << endl;
		}
	}

	write.close();
}


void sphere(float radius, float slices, float stacks, char* file) {

	float alpha = (2 * M_PI) / slices;
	float beta = M_PI / stacks;

	ofstream write(file);

	float number_triangles = ((6 * slices) * stacks);

	write << number_triangles << endl;

	for (int i = 0; i < slices; i += 1) {

		float newAlpha = alpha * i;

		for (int j = 0; j < stacks; j += 1) {

			float newBeta = (-M_PI / 2) + beta * j;

			float p1_x = radius * sin(newAlpha) * cos(newBeta);
			float p1_y = radius * sin(newBeta);
			float p1_z = radius * cos(newAlpha) * cos(newBeta);

			float p2_x = radius * sin(newAlpha + alpha) * cos(newBeta);
			float p2_y = radius * sin(newBeta);
			float p2_z = radius * cos(newAlpha + alpha) * cos(newBeta);

			float p3_x = radius * sin(newAlpha) * cos(newBeta + beta);
			float p3_y = radius * sin(newBeta + beta);
			float p3_z = radius * cos(newAlpha) * cos(newBeta + beta);

			float p4_x = radius * sin(newAlpha + alpha) * cos(newBeta + beta);
			float p4_y = radius * sin(newBeta + beta);
			float p4_z = radius * cos(newAlpha + alpha) * cos(newBeta + beta);

			write << p1_x << " " << p1_y << " " << p1_z << endl;
			write << p2_x << " " << p2_y << " " << p2_z << endl;
			write << p3_x << " " << p3_y << " " << p3_z << endl;

			write << p2_x << " " << p2_y << " " << p2_z << endl;
			write << p4_x << " " << p4_y << " " << p4_z << endl;
			write << p3_x << " " << p3_y << " " << p3_z << endl;
		}
	}

	write.close();
}


void cone(float radius, float height, float slices, float stacks, char* file) {

	ofstream write(file);

	float alpha = (2 * M_PI) / slices;
	float horizontalSpace = height / stacks;
	float newHeight = -height / 2;

	int number_triangles = (3 * slices) + ((6 * slices) * stacks);
	write << number_triangles << endl;

	// base
	for (int i = 0; i < slices; i++) {

		float newAlpha = alpha * i;

		float p1_x = 0;
		float p1_y = newHeight;
		float p1_z = 0;

		float p2_x = radius * sin(newAlpha + alpha);
		float p2_y = newHeight;
		float p2_z = radius * cos(newAlpha + alpha);

		float p3_x = radius * sin(newAlpha);
		float p3_y = newHeight;
		float p3_z = radius * cos(newAlpha);

		write << p1_x << " " << p1_y << " " << p1_z << endl;
		write << p2_x << " " << p2_y << " " << p2_z << endl;
		write << p3_x << " " << p3_y << " " << p3_z << endl;

	}

	//face lateral (camada a camada)
	for (int i = 0; i < stacks; i++) {

		float fb = newHeight + (i * horizontalSpace);
		float fc = newHeight + ((i + 1) * horizontalSpace);

		float rb = radius - ((radius / stacks) * i);
		float rc = radius - ((radius / stacks) * (i + 1));

		for (int j = 0; j < slices; j++) {

			height = alpha * j;

			float t1_p1_x = rb * sin(height);
			float t1_p1_y = fb;
			float t1_p1_z = rb * cos(height);

			float t1_p2_x = rc * sin(height + alpha);
			float t1_p2_y = fc;
			float t1_p2_z = rc * cos(height + alpha);

			float t1_p3_x = rc * sin(height);
			float t1_p3_y = fc;
			float t1_p3_z = rc * cos(height);

			write << t1_p1_x << " " << t1_p1_y << " " << t1_p1_z << endl;
			write << t1_p2_x << " " << t1_p2_y << " " << t1_p2_z << endl;
			write << t1_p3_x << " " << t1_p3_y << " " << t1_p3_z << endl;

			float t2_p1_x = rb * sin(height);
			float t2_p1_y = fb;
			float t2_p1_z = rb * cos(height);

			float t2_p2_x = rb * sin(height + alpha);
			float t2_p2_y = fb;
			float t2_p2_z = rb * cos(height + alpha);

			float t2_p3_x = rc * sin(height + alpha);
			float t2_p3_y = fc;
			float t2_p3_z = rc * cos(height + alpha);

			write << t2_p1_x << " " << t2_p1_y << " " << t2_p1_z << endl;
			write << t2_p2_x << " " << t2_p2_y << " " << t2_p2_z << endl;
			write << t2_p3_x << " " << t2_p3_y << " " << t2_p3_z << endl;
		}

	}
	write.close();
}

int main(int argc, char **argv) {

	if ((argc >= 5) && (strcmp(argv[1], "plane") == 0)) {

		int offset = 0;

		float t_size_x = atof(argv[2]);
		float t_size_z = atof(argv[3]);
		float partition = 1; // Como default reparte um plano em 2 triangulos
		if(argc==6){
			partition = atof(argv[4]);
			offset++;
		}
		char* file = argv[4+offset];

		plane(t_size_x, t_size_z, partition, file);

		return 0;
	}

	if ((argc >= 6) && (strcmp(argv[1], "box") == 0)) {

		int offset = 0;

		float t_size_x = atof(argv[2]);
		float t_size_z = atof(argv[4]);
		float t_size_y = atof(argv[3]);
		float partition = 1; // Como default reparte cada face em 2 triangulos
		if(argc==7){
			partition = atof(argv[5]);
			offset++;
		}
		char* file = argv[5+offset];

		box(t_size_x, t_size_z, t_size_y, partition, file);

		return 0;
	}

	if ((argc == 6) && (strcmp(argv[1], "sphere") == 0)) {

		float radius = atof(argv[2]);
		float slices = atof(argv[3]);
		float stacks = atof(argv[4]);
		char* file = argv[5];

		sphere(radius, slices, stacks, file);

		return 0;
	}

	if ((argc == 7) && (strcmp(argv[1], "cone") == 0)) {

		float radius = atof(argv[2]);
		float height = atof(argv[3]);
		float slices = atof(argv[4]);
		float stacks = atof(argv[5]);
		char* file = argv[6];

		cone(radius, height, slices, stacks, file);

		return 0;
	}

	return 1;
}
