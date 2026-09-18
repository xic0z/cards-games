
#include <stdio.h>
int main(int argc, char **argv) {
  float a = 2.0, b = 2.75, c = -(a + b);
  
  float  i = 0.1 + 0.1;
  double j = 0.1 + 0.1;
  printf("i: %f\n", i);
  printf("j: %f\n", j);  

  int variavel;  
  variavel = argv[0][0];     
  return variavel; // ATENÇÃO: isto não se faz!
}
