#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <float.h>
#include <time.h>

/* fungsi yang digunakan :

1. menghitung kemungkinan urutan perjalanan (permutasi)
2. menghitung jarak antar 2 kota (dengan formula haversine)
3. menghitung total jarak masing-masing kemungkinan
4. memilih jarak yang paling efisien

*/

#define MAX_LENGTH 100
#define PI 3.14159265358979323846
#define r 6371

// fungsi untuk melakukan konversi latitude dan longitude dari derajat ke radian
double konversiRadian(double derajat) {
    return derajat * (PI / 180.0);
}

// fungsi untuk menghitung jarak antarkota melalui formula haversine
double fungsiHaversine(double lat1, double lon1, double lat2, double lon2) {
    double dLat = konversiRadian(lat2 - lat1);
    double dLon = konversiRadian(lon2 - lon1);
    lat1 = konversiRadian(lat1);
    lat2 = konversiRadian(lat2);

    double a = sin(dLat / 2) * sin(dLat / 2) +
               cos(lat1) * cos(lat2) * sin(dLon / 2) * sin(dLon / 2);
    double d = 2 * r * asin(sqrt(a));
    return d;
}

// fungsi untuk menghitung totalJarak dari suatu rute
double calculateRouteDistance(int route[], int n, double distanceMatrix[][n]) {
    double totalDistance = 0;
    for (int i = 0; i < n - 1; i++) {
        totalDistance += distanceMatrix[route[i]][route[i + 1]];
    }
    totalDistance += distanceMatrix[route[n - 1]][route[0]];
    return totalDistance;
}

// fungsi swap untuk mempermudah permutasi
void swap(int *a, int *b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

// fungsi permutasi untuk menentukan rute terbaik
void permute(int cities[], int start, int end, int n, double distanceMatrix[][n], double *minDistance, int bestRoute[]) {
    if (start == end) {
        double currentDistance = calculateRouteDistance(cities, n, distanceMatrix);
        if (currentDistance < *minDistance) {
            *minDistance = currentDistance;
            for (int i = 0; i < n; i++) {
                bestRoute[i] = cities[i];
            }
        }
    } else {
        for (int i = start; i <= end; i++) {
            swap(&cities[start], &cities[i]);
            permute(cities, start + 1, end, n, distanceMatrix, minDistance, bestRoute);
            swap(&cities[start], &cities[i]); // backtrack
        }
    }
}

int main() {
    char fileName[MAX_LENGTH];
    char startingPoint[MAX_LENGTH];
    
    // masukkan nama file
    printf("Enter list of cities file name: ");
    scanf("%s", fileName);

    //buka file
    FILE *file = fopen(fileName, "r");
    if (file == NULL) {
        printf("Can't open file %s\n", fileName);
        return 1;
    } 
    else {
        printf("File %s opened successfully\n", fileName);
    }

    // masukkan kota awal
    printf("Enter starting point: ");
    scanf("%s", startingPoint);

    // Membaca file
    int jumlahKota = 0;
    char line[MAX_LENGTH];
    while (fgets(line, sizeof(line), file)) {
        jumlahKota++;
    }
    rewind(file);
    
    char cityNames[jumlahKota][MAX_LENGTH];
    double latitudes[jumlahKota];
    double longitudes[jumlahKota];

    int index = 0;
    while (fgets(line, sizeof(line), file)) {
        sscanf(line, "%[^,],%lf,%lf", cityNames[index], &latitudes[index], &longitudes[index]);
        index++;
    }

    // inisialisasi matriks jarak
    double distanceMatrix[jumlahKota][jumlahKota];
    for (int i = 0; i < jumlahKota; i++) {
        for (int j = 0; j < jumlahKota; j++) {
            if (i != j) {
                distanceMatrix[i][j] = fungsiHaversine(latitudes[i], longitudes[i], latitudes[j], longitudes[j]);
            } else {
                distanceMatrix[i][j] = 0;
            }
        }
    }

    // inisialisasi array kota
    int cities[jumlahKota];
    for (int i = 0; i < jumlahKota; i++) {
        cities[i] = i;
    }

    // memeriksa keberadaan starting city
    int startIndex = -1;
    for (int i = 0; i < jumlahKota; i++) {
        if (strcasecmp(cityNames[i], startingPoint) == 0) {
            startIndex = i;
            break;
        }
    }

    if (startIndex == -1) {
        fprintf(stderr, "Starting city not found\n");
        return 1;
    }

    
    swap(&cities[0], &cities[startIndex]);

    int bestRoute[jumlahKota];
    double minDistance = DBL_MAX;

    clock_t startTime = clock();

    permute(cities, 1, jumlahKota - 1, jumlahKota, distanceMatrix, &minDistance, bestRoute);

    clock_t endTime = clock();
    double timeElapsed = (double)(endTime - startTime) / CLOCKS_PER_SEC;

    // menampilkan rute terbaik, jarak tempuh rute terbaik, dan 
    printf("Best route found:\n");
    printf("%s", cityNames[bestRoute[0]]);
    for (int i = 1; i < jumlahKota; i++) {
        printf(" -> %s", cityNames[bestRoute[i]]);
    }
    printf(" -> %s\n", cityNames[bestRoute[0]]);
    printf("Best route distance: %.8f km\n", minDistance);
    printf("Time elapsed: %.10f s\n", timeElapsed);

    return 0;
}
