#include <stdio.h>
#include <stdlib.h>

#define INPUT_SIZE 20

void generateInput() {
  FILE *input = fopen("./vetor.dat", "w");

  for(int index = 0; index < INPUT_SIZE; index++) {
    float randomNumber = ((float)rand()/RAND_MAX) * 100;

    fprintf(input, "%f\n", randomNumber);
  }

  fclose(input);
}

float *getInputVector() {
  FILE *input = fopen("./vetor.dat", "r");
  float *vector;
  int index = 0;

  vector = (float *) malloc(500000 * sizeof(float));

  while(fscanf(input, "%f", &vector[index]) != EOF) index++;
  
  return vector;
}

int main() {
  generateInput();

  float *vector = getInputVector();

  return 0;
}