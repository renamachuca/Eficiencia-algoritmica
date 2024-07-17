#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <chrono>
#include <sys/resource.h>
#include <omp.h>

using namespace std;


void intercambiar(int& a, int& b) {
    int temp = a;
    a = b;
    b = temp;
}

int particion(vector<int>& arr, int bajo, int alto) {
    int pivotIndex = bajo + rand() % (alto - bajo + 1);
    intercambiar(arr[pivotIndex], arr[alto]);
    int pivote = arr[alto];
    int i = bajo - 1;
    for (int j = bajo; j <= alto - 1; j++) {
        if (arr[j] <= pivote) {
            i++;
            intercambiar(arr[i], arr[j]);
        }
    }
    intercambiar(arr[i + 1], arr[alto]);
    return (i + 1);
}

void insertionSort(vector<int>& arr, int bajo, int alto) {
    for (int i = bajo + 1; i <= alto; i++) {
        int key = arr[i];
        int j = i - 1;
        while (j >= bajo && arr[j] > key) {
            arr[j + 1] = arr[j];
            j--;
        }
        arr[j + 1] = key;
    }
}

void quickSortHelper(vector<int>& arr, int bajo, int alto) {
    while (bajo < alto) {
        if (alto - bajo < 10) { 
            insertionSort(arr, bajo, alto);
            break;
        } else {
            int pi = particion(arr, bajo, alto);
            #pragma omp task
            {
                quickSortHelper(arr, bajo, pi - 1);
            }
            bajo = pi + 1;
        }
    }
}

void quickSort(vector<int>& arr, int bajo, int alto) {
    #pragma omp parallel
    {
        #pragma omp single nowait
        {
            quickSortHelper(arr, bajo, alto);
        }
    }
}

vector<int> generarArrayAleatorio(int n) {
    vector<int> arr(n);
    for (int i = 0; i < n; i++) {
        arr[i] = rand() % 1000;
    }
    return arr;
}

void imprimirArray(const vector<int>& arr) {
    for (int num : arr) {
        cout << num << " ";
    }
    cout << "\n";
}

bool estaOrdenado(const vector<int>& arr) {
    for (size_t i = 1; i < arr.size(); ++i) {
        if (arr[i - 1] > arr[i]) {
            return false;
        }
    }
    return true;
}

long getMemoryUsage() {
    struct rusage r_usage;
    getrusage(RUSAGE_SELF, &r_usage);
    return r_usage.ru_maxrss;
}

int main() {
    srand(static_cast<unsigned int>(time(0)));

    vector<int> tamanos = {100, 300, 500};
    for (int tamano : tamanos) {
        vector<int> arr = generarArrayAleatorio(tamano);

        long memoriaAntes = getMemoryUsage();

        auto inicio = chrono::high_resolution_clock::now();
        quickSort(arr, 0, arr.size() - 1);
        auto fin = chrono::high_resolution_clock::now();
        chrono::duration<double, milli> duracion = fin - inicio;

        long memoriaDespues = getMemoryUsage();

        bool correcto = estaOrdenado(arr);

        cout << "Conjunto de " << tamano << " elementos:\n";
        cout << "Tiempo de Ejecución: " << duracion.count() << " ms\n";
        cout << "Uso de Memoria: " << (memoriaDespues - memoriaAntes) << " KB\n";
        cout << "Correctitud: " << (correcto ? "Correcto" : "Incorrecto") << "\n";
        cout << "\n";
    }

    return 0;
}