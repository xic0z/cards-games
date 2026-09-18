#include <stdio.h>
#include <math.h>

void raizes(float a, float b, float c) {
	float delta = b * b - 4 * a * c;

	float r1 = (- b - sqrt(delta)) / (2 * a);
	float r2 = (- b + sqrt(delta)) / (2 * a);
	printf("r1 = %f\nr2 = %f\n", r1, r2);
}
