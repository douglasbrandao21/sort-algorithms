#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#define INPUT_SIZE 500

float *vector;
int size = 0;

struct timespec initialSortTime, finalSortTime;

void printTime(struct timespec initial, struct timespec final) {
  if (final.tv_nsec < initial.tv_nsec) {
        final.tv_nsec += 1000000000;
        final.tv_sec--;
  }

  printf("%ld.%09ld\n", (long)(final.tv_sec - initial.tv_sec),
        final.tv_nsec - initial.tv_nsec);
}

void generateInput() {
  FILE *input = fopen("./vetor.dat", "w");

  srand((unsigned)time(NULL));

  for(int index = 0; index < INPUT_SIZE; index++) {
    float randomNumber = ((float) rand()/RAND_MAX) * 100;

    fprintf(input, "%f\n", randomNumber);
  }

  fclose(input);
}

void *getInputVector() {
  FILE *input = fopen("./vetor.dat", "r");

  vector = (float *) malloc(INPUT_SIZE * sizeof(float));

  while(fscanf(input, "%f", &vector[size]) != EOF) size++;

  fclose(input);
}

void insertionSort(){
    clock_gettime(CLOCK_REALTIME, &initialSortTime);
    
    int i, j;
    float key;

    for (i = 1; i < size; i++) {
        key = vector[i];
        j = i - 1;

        while (j >= 0 && vector[j] > key) {
            vector[j + 1] = vector[j];
            j = j - 1;
        }
        vector[j + 1] = key;
    }

    clock_gettime(CLOCK_REALTIME, &finalSortTime);
}

int main() {
  clock_gettime(CLOCK_REALTIME, &initialSortTime);

  // generateInput();

  // getInputVector();

  // insertionSort();

  sleep(5);

  clock_gettime(CLOCK_REALTIME, &finalSortTime);

  printTime(initialSortTime, finalSortTime);

  return 0;
}