#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int arraySize = 0;

struct timespec initialSortTime, finalSortTime;
struct timespec initialSearchTime, finalSearchTime;


/**
 * @brief struct that will be used to store the difference of times.
 */
struct timeRegistry {
  long int seconds;
  long int nanoseconds;
};



/**
  * @brief Function that get the time difference between two timespecs.
  * 
  * @param initial: struct timespec containing the initial time.
  * @param final: struct timespec containing the final time.
  * 
  * @return struct timeRegistry containing the difference of time between the two input timespecs.
*/
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



/**
  * @brief Function that prints the processing times (sorting and/or searching)
  * 
  * @param printSortTime: flag that indicates to print the sorting time.
*/
void printTimes(int printSortTime) {
  struct timeRegistry searchRegistry;
  struct timeRegistry sortRegistry;

  if(printSortTime == 1) {
    sortRegistry = getTimeRegistry(initialSortTime, finalSortTime);

    printf("Tempo de ordenação: %ld.%09ld\n", sortRegistry.seconds, sortRegistry.nanoseconds);
  }

  searchRegistry = getTimeRegistry(initialSearchTime, finalSearchTime);

  printf("Tempo de busca: %ld.%09ld\n", searchRegistry.seconds, searchRegistry.nanoseconds);

  if(printSortTime == 1)
    printf("Tempo total: %ld.%09ld\n", 
      searchRegistry.seconds+sortRegistry.seconds, searchRegistry.nanoseconds+sortRegistry.nanoseconds);
}



/**
  * @brief Function that create an array with the input values of vetor.dat
  * 
  * @return an array of double, containing all elements presents in vetor.dat (in same order).
*/
double *getInputVector() {
  FILE *input = fopen("./vetor.dat", "r");

  double readedNumber = 0;

  double *array = (double *) malloc(0 * sizeof(double));

  while(fscanf(input, "%lf", &readedNumber) != EOF) {
    arraySize++;

    array = realloc(array, arraySize * sizeof(double));

    array[arraySize-1] = readedNumber;
  }

  fclose(input);

  return array;
}



/**
  * @brief Function that checks if an element is close enought of another element to be considered equal.
  * 
  * @param a: firsts element of comparision
  * @param b: second element of comparision

  * @return 1, if the two elements are close enought, or 0, if not.
*/
int isCloseEnought(double a, double b) {
  if(a < 0 && b < 0) {
    a = a * -1;
    b = b * -1;
  }

  double difference = a > b ? 
          a - b : b - a;

  if(difference < 0.0001) return 1;

  return 0;
}



/**
  * @brief Function that implements linear search
  * 
  * @param array: array where the elements of busca.dat will be searched.
  * @param target: element to be found.
  * @param isSorted: flag that indicates if the passed array is sorted. 

  * @return index of target element (or -1, if that element was not found).
*/
int linearSearch(double *array, double target, int isSorted) {
  if(isSorted == 1)
    for(int index = 0; index < arraySize; index++) {
      if(isCloseEnought(target, array[index]) == 1)
        return index;

      if(array[index] > target)
        return -1;
    }

  else
    for(int index = 0; index < arraySize; index++)
        if(isCloseEnought(target, array[index]) == 1)
          return index;
  
    
  return -1;
}



/**
  * @brief Function that implements binary search
  * 
  * @param array: array where the elements of busca.dat will be searched.
  * @param left: index of element in left most position.
  * @param right: index of element in right most position.
  * @param target: element to be found.
  * 
  * @return index of target element (or -1, if that element was not found).
*/
int binarySearch(double *array, double left, double right, double target) {
    if (right >= left) { 
        int middle = left + (right - left) / 2;

        if(isCloseEnought(target, array[middle]) == 1) return middle;
  
        if (array[middle] > target) 
            return binarySearch(array, left, middle - 1, target); 

        return binarySearch(array, middle+1, right, target); 
    }
  
    return -1;
}



/**
  * @brief Function that search all elements in busca.dat, according with the selected search method.
  * 
  * @param array: array where the elements of busca.dat will be searched.
  * @param useLinearSearch: flag that indicates to use linear search method.
  * @param useBinarySearch: flag that indicates to use binary search method.
  * @param isSorted: flag that indicates if the passed array is sorted.
*/
void search(double *array, int useLinearSearch, int useBinarySearch, int isSorted) {
  FILE *searchFile = fopen("./busca.dat", "r");
  FILE *outputFile = fopen("./resultado.dat", "w");

  double target;
  int targetIndex;

  if(useLinearSearch == 1 && useBinarySearch == 0) {
    clock_gettime(CLOCK_REALTIME, &initialSearchTime);
    
    while(fscanf(searchFile, "%lf", &target) != EOF) {
      targetIndex = linearSearch(array, target, isSorted);

      if(targetIndex == -1)
        fprintf(outputFile, "-1 0.0 %lf\n", target);
      else
        fprintf(outputFile, "%d %lf %lf\n", targetIndex, array[targetIndex], target);
    };

    clock_gettime(CLOCK_REALTIME, &finalSearchTime);
  }
  else if(useBinarySearch == 1 && useLinearSearch == 0) {
    clock_gettime(CLOCK_REALTIME, &initialSearchTime);
    
    while(fscanf(searchFile, "%lf", &target) != EOF) {  
      targetIndex = binarySearch(array, 0, arraySize - 1, target);


      if(targetIndex == -1)
        fprintf(outputFile, "-1 0.0 %lf\n", target);
      else
        fprintf(outputFile, "%d %lf %lf\n", targetIndex, array[targetIndex], target);
    };

    clock_gettime(CLOCK_REALTIME, &finalSearchTime);
  }

  fclose(searchFile);
  fclose(outputFile);
}



/**
  * @brief Function that takes last element as pivot, places the pivot element at its correct position in sorted 
    array, and places all smaller (smaller than pivot) to left of pivot and all greater elements to right of pivot
  * 
  * @param array: array containing elements of vetor.dat to be ordered.
  * @param low: initial index
  * @param high: final index
  * 
  * @return index of partition
*/
int partition (double *array, int low, int high) {
  int index = (low - 1);
  double temporary = 0;
  double pivot = array[high];

  for (int j = low; j <= high - 1; j++) {
    if (array[j] < pivot) {
      index++;

      temporary = array[index];
      array[index] = array[j];
      array[j] = temporary;
    }
  }

  temporary = array[index + 1];
  array[index + 1] = array[high];
  array[high] = temporary;

  return (index + 1);
}



/**
  * @brief Function that implements Quick Sort method.
  * 
  * @param array: array containing elements of vetor.dat to be ordered.
  * @param low: initial index
  * @param high: final index
*/
void quickSort(double *array, int low, int high) {
  if (low < high) {
      int partitionIndex = partition(array, low, high); 

      quickSort(array, low, partitionIndex - 1);
      quickSort(array, partitionIndex + 1, high);
  } 
}



/**
  * @brief Function that implements Shell Sort method.
  * 
  * @param array: array containing elements of vetor.dat to be ordered.
*/
void shellSort(double *array) {
  clock_gettime(CLOCK_REALTIME, &initialSortTime);

  for (int gap = arraySize/2; gap > 0; gap /= 2) { 
    for (int i = gap; i < arraySize; i += 1) {
      double temporary = array[i]; 
      
      int j;
      for (j = i; j >= gap && array[j - gap] > temporary; j -= gap) 
        array[j] = array[j - gap];
          
      array[j] = temporary; 
    } 
  }

  clock_gettime(CLOCK_REALTIME, &finalSortTime);
}



/**
  * @brief Function that implements Insertion Sort method.
  * 
  * @param array: array containing elements of vetor.dat to be ordered.
*/
void insertionSort(double *array){
    clock_gettime(CLOCK_REALTIME, &initialSortTime);
    
    int i, j;
    double key;

    for (i = 1; i < arraySize; i++) {
        key = array[i];
        j = i - 1;

        while (j >= 0 && array[j] > key) {
            array[j + 1] = array[j];
            j = j - 1;
        }
        array[j + 1] = key;
    }

    clock_gettime(CLOCK_REALTIME, &finalSortTime);
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

  double *array = getInputVector();

  switch (option) {
    case 1:
      search(array, 1, 0, 0); // Sequential Search

      printTimes(0); // Sequential Search
      break;

    case 2:
      insertionSort(array);

      search(array, 1,0,1); // Sequential Search
      
      printTimes(1);

      
      break;

    case 3:
      shellSort(array);
      
      search(array, 1,0,1); //Sequential Search;
      
      printTimes(1);

      break;

    case 4:
      clock_gettime(CLOCK_REALTIME, &initialSortTime);
      quickSort(array, 0, arraySize - 1);
      clock_gettime(CLOCK_REALTIME, &finalSortTime);

      search(array, 1,0,1); //Sequential Search;
      
      printTimes(1);
      
      break;

    case 5:
      insertionSort(array);

      search(array, 0,1,1); //Binary Search

      printTimes(1);
      break;

    case 6:
      shellSort(array);

      search(array, 0,1,1); //Binary Search

      printTimes(1);
      break;

    case 7:
      clock_gettime(CLOCK_REALTIME, &initialSortTime);
      quickSort(array, 0, arraySize - 1);
      clock_gettime(CLOCK_REALTIME, &finalSortTime);
      
      search(array, 0,1,1); //Binary Search

      printTimes(1);
      break;

    default:
      printf("Escolha uma opção válida.");
  }

  return 0;
}