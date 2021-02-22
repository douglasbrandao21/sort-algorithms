#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#define INPUT_SIZE 500000

double *vector;
int size = 0;

struct timespec initialSortTime, finalSortTime;
struct timespec initialSearchTime, finalSearchTime;

struct timeRegistry {
  long int seconds;
  long int nanoseconds;
};

struct timeRegistry getTimeRegistry(struct timespec initial, struct timespec final) {
  struct timeRegistry registry;

  if (final.tv_nsec < initial.tv_nsec) {
    final.tv_nsec += 1000000000;
    final.tv_sec--;
  }

  registry.seconds = (long)(final.tv_sec - initial.tv_sec);
  registry.nanoseconds = final.tv_nsec - initial.tv_nsec;

  return registry;
}

void printTimes(int printSortTime, int printSearchTime) {
  struct timeRegistry searchRegistry;
  struct timeRegistry sortRegistry;

  if(printSortTime == 1) {
    sortRegistry = getTimeRegistry(initialSortTime, finalSortTime);

    printf("Tempo de ordenação: %ld.%09ld\n", sortRegistry.seconds, sortRegistry.nanoseconds);
  }

  if(printSearchTime == 1) {
    searchRegistry = getTimeRegistry(initialSearchTime, finalSearchTime);

    printf("Tempo de busca: %ld.%09ld\n", searchRegistry.seconds, searchRegistry.nanoseconds);
  }

  if(printSearchTime == 1 && printSortTime == 1)
    printf("Tempo total: %ld.%09ld\n", 
      searchRegistry.seconds+sortRegistry.seconds, searchRegistry.nanoseconds+sortRegistry.nanoseconds);
}

void generateInput() {
  FILE *input = fopen("./vetor.dat", "w");
  FILE *search = fopen("./busca.dat", "w");

  srand((unsigned)time(NULL));

  for(int index = 0; index < INPUT_SIZE; index++) {
    double randomNumber = ((double) rand()/RAND_MAX) * 100;

    if(index == 2 || index == 4 || index == 10 || index == 20)
      fprintf(search, "%lf\n", randomNumber);

    fprintf(input, "%lf\n", randomNumber);
  }

  fclose(search);
  fclose(input);
}

void *getInputVector() {
  FILE *input = fopen("./vetor.dat", "r");

  vector = (double *) malloc(INPUT_SIZE * sizeof(double));

  while(fscanf(input, "%lf", &vector[size]) != EOF) size++;

  fclose(input);
}

int linearSearch(double target) {
  for(int index = 0; index < size; index++) {
    double difference = target > vector[index] ? target - vector[index] : vector[index] - target;

    if(difference < 0.0001) return index;
  }

  return -1;
}

int binarySearch(double left, double right, double target) {
    if (right >= left) { 
        int middle = left + (right - left) / 2;

        double difference = target > vector[middle] ? 
          target - vector[middle] : vector[middle] - target;
  
        if (difference < 0.0001) return middle; 
  
        if (vector[middle] > target) 
            return binarySearch(left, middle - 1, target); 

        return binarySearch(middle+1, right, target); 
    }
  
    return -1;
} 

void search(int useLinearSearch, int useBinarySearch) {
  FILE *searchFile = fopen("./busca.dat", "r");
  FILE *outputFile = fopen("./resultado.dat", "w");

  double target;
  int targetIndex;

  if(useLinearSearch == 1 && useBinarySearch == 0) {
    while(fscanf(searchFile, "%lf", &target) != EOF) {
      clock_gettime(CLOCK_REALTIME, &initialSearchTime);

      targetIndex = linearSearch(target);

      clock_gettime(CLOCK_REALTIME, &finalSearchTime);

      if(targetIndex == -1)
        fprintf(outputFile, "[-1] [0.0] [%lf]\n", target);
      else
        fprintf(outputFile, "[%d] [%lf] [%lf]\n", targetIndex, vector[targetIndex], target);
    };
  }
  else if(useBinarySearch == 1 && useLinearSearch == 0) {
    while(fscanf(searchFile, "%lf", &target) != EOF) {
      clock_gettime(CLOCK_REALTIME, &initialSearchTime);
      
      targetIndex = binarySearch(0, size - 1, target);

      clock_gettime(CLOCK_REALTIME, &finalSearchTime);

      if(targetIndex == -1)
        fprintf(outputFile, "[-1] [0.0] [%lf]\n", target);
      else
        fprintf(outputFile, "[%d] [%lf] [%lf]\n", targetIndex, vector[targetIndex], target);
    };
  }

  fclose(searchFile);
  fclose(outputFile);
}


int partition (int low, int high) {
  int index = (low - 1);
  double temporary = 0;
  double pivot = vector[high];

  for (int j = low; j <= high - 1; j++) {
    if (vector[j] < pivot) {
      index++;

      temporary = vector[index];
      vector[index] = vector[j];
      vector[j] = temporary;
    }
  }

  temporary = vector[index + 1];
  vector[index + 1] = vector[high];
  vector[high] = temporary;

  return (index + 1);
}

void quickSort(int low, int high) {
  if (low < high) {
      int partitionIndex = partition(low, high); 

      quickSort(low, partitionIndex - 1);
      quickSort(partitionIndex + 1, high);
  } 
}

void shellSort() {
  clock_gettime(CLOCK_REALTIME, &initialSortTime);

  for (int gap = size/2; gap > 0; gap /= 2) { 
    for (int i = gap; i < size; i += 1) {
      double temporary = vector[i]; 
      
      int j;
      for (j = i; j >= gap && vector[j - gap] > temporary; j -= gap) 
        vector[j] = vector[j - gap];
          
      vector[j] = temporary; 
    } 
  }

  clock_gettime(CLOCK_REALTIME, &finalSortTime);
}

void insertionSort(){
    clock_gettime(CLOCK_REALTIME, &initialSortTime);
    
    int i, j;
    double key;

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

void print() {
  for(int index = 0; index < size; index++)
    printf("%lf\n", vector[index]);

  printf("\n");
}

int main() {
  int option;

  printf("1. Busca sem ordenação\n");
  printf("2. Ordenação por insertion e busca sequencial\n");
  printf("3. Ordenação por shell sort e busca sequencial\n");
  printf("4. Ordenação por quicksort e busca sequencial\n");
  printf("5. Ordenação por insertion e busca binária\n");
  printf("6. Ordenação por shell sort e busca binária\n");
  printf("7. Ordenação por quicksort e busca binária\n");

  printf("Operação: ");
  scanf("%d", &option);

  getInputVector();

  switch (option) {
    case 1:
      search(1, 0); // Sequential Search

      printTimes(0, 1); // Sequential Search
      break;

    case 2:
      insertionSort();
      print();

      search(1,0); // Sequential Search
      
      printTimes(1, 1);
      break;

    case 3:
      shellSort();

      search(1,0); //Sequential Search;
      
      printTimes(1, 1);
      break;

    case 4:
      quickSort(0, size - 1);

      search(1,0); //Sequential Search;
      
      printTimes(1, 1);
      break;

    case 5:
      insertionSort();

      search(0,1); //Binary Search

      printTimes(1, 1);
      break;

    case 6:
      shellSort();

      search(0,1); //Binary Search

      printTimes(1, 1);
      break;

    case 7:
      quickSort(0, size - 1);
      

      search(0,1); //Binary Search

      printTimes(1, 1);
      break;

    default:
      printf("Escolha uma opção válida.");
  }
  return 0;
}