#include <stdio.h>
#include <stdlib.h>
#define _USE_MATH_DEFINES
#include <math.h>

int main(int argc, char const *argv[])
{
	
	int slices = 40;
	
	float radius = 4.0f;
	float radius2 = 2.7f;

	float alpha = (2 * M_PI) / slices;

	int tam = 18 * slices;

	printf("%i\n", tam);

	for (int i = 0; i < slices; i++) {

		float newAlpha = alpha * i;

		float p1_x = 0;
		float p1_y = 0;
		float p1_z = 0;

		float p2_x = radius * sin(newAlpha + alpha);
		float p2_y = 0;
		float p2_z = radius * cos(newAlpha + alpha);

		float p3_x = radius * sin(newAlpha);
		float p3_y = 0;
		float p3_z = radius * cos(newAlpha);

		float p_x = radius2 * sin(newAlpha + alpha);
		float p_y = 0;
		float p_z = radius2 * cos(newAlpha + alpha);

		float o_x = radius2 * sin(newAlpha);
		float o_y = 0;
		float o_z = radius2 * cos(newAlpha);

		float m_x = radius2 * sin(newAlpha + (alpha/2));
		float m_y = 0;
		float m_z = radius2 * cos(newAlpha + (alpha/2));

		printf("%f 0 %f\n", p_x, p_z);
		printf("%f 0 %f\n", m_x, m_z);
		printf("%f 0 %f\n", p2_x, p2_z);

		printf("%f 0 %f\n", o_x, o_z);
		printf("%f 0 %f\n", p3_x, p3_z);
		printf("%f 0 %f\n", m_x, m_z);

		printf("%f 0 %f\n", m_x, m_z);
		printf("%f 0 %f\n", p3_x, p3_z);
		printf("%f 0 %f\n", p2_x, p2_z);

		printf("%f 0 %f\n", p_x, p_z);
		printf("%f 0 %f\n", p2_x, p2_z);
		printf("%f 0 %f\n", m_x, m_z);

		printf("%f 0 %f\n", o_x, o_z);
		printf("%f 0 %f\n", m_x, m_z);
		printf("%f 0 %f\n", p3_x, p3_z);

		printf("%f 0 %f\n", m_x, m_z);
		printf("%f 0 %f\n", p2_x, p2_z);
		printf("%f 0 %f\n", p3_x, p3_z);

	}
	return 0;
}