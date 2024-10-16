#include <stdbool.h>  // what the actual fuck
#include <stdio.h>
#include <stdlib.h>
#include <time.h>  // Per time()
#include <unistd.h>

void shuffle(int arr[], int size) {
    for (int i = size - 1; i > 0; i--) {
        // Genera un indice casuale tra 0 e i
        int j = rand() % (i + 1);

        // Scambia arr[i] con arr[j]
        int temp = arr[i];
        arr[i] = arr[j];
        arr[j] = temp;
    }
}

void fillAndShuffle(int arr[], int n) {
    // Riempie il vettore con i numeri da 1 a n
    for (int i = 0; i < n; i++)
        arr[i] = i + 1;

    // Mescola il vettore
    shuffle(arr, n);
}
void printArr(const int arr[], int size) {
    if (!arr)
        return;
    for (int i = 0; i < size; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");
}
#define SIZE 5
int main() {
    srand(time(NULL));
    int arr[SIZE];
    fillAndShuffle(arr, SIZE);
    printf("%d) ", getpid());
    printArr(arr, SIZE);
}