#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <functional>

void suma_parcial(
    const std::vector<int>& datos,
    size_t inicio,
    size_t fin,
    long long& resultado
) {
    long long suma = 0;
    for (size_t i = inicio; i < fin; ++i) {
        suma += datos[i];
    }
    resultado = suma;
}

long long suma_paralela(const std::vector<int>& datos, int cantidad_hilos) {
    std::vector<std::thread> hilos;
    std::vector<long long> resultados(cantidad_hilos, 0);
    size_t tamano = datos.size();
    size_t bloque = tamano / cantidad_hilos;

    for (int i = 0; i < cantidad_hilos; ++i) {
        size_t inicio = i * bloque;
        size_t fin = (i == cantidad_hilos - 1) ? tamano : inicio + bloque;
        
        hilos.emplace_back(
            suma_parcial,
            std::cref(datos),
            inicio,
            fin,
            std::ref(resultados[i])
        );
    }

    for (auto& hilo: hilos) {
        hilo.join();
    }

    long long suma_total = 0;
    for (long long parcial: resultados) {
        suma_total += parcial;
    }
    return suma_total;
}

int main() {
    const size_t cantidad_elementos = 100000000; // 100 millones según el lab
    std::vector<int> datos(cantidad_elementos, 1);
    std::vector<int> pruebas = {1, 2, 4, 8, 16};

    for (int cantidad_hilos : pruebas) {
        auto inicio = std::chrono::high_resolution_clock::now();
        long long resultado = suma_paralela(datos, cantidad_hilos);
        auto fin = std::chrono::high_resolution_clock::now();
        auto duracion = std::chrono::duration_cast<std::chrono::milliseconds>(
            fin - inicio
        ).count();

        std::cout << "Hilos: " << cantidad_hilos 
                  << " | Resultado: " << resultado 
                  << " | Tiempo: " << duracion << " ms" 
                  << std::endl;
    }

    return 0;
}