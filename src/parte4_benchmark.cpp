#include <iostream>
#include <vector>
#include <thread>
#include <chrono>

long long suma_secuencial(const std::vector<int>& datos) {
    long long suma = 0;
    for (int valor: datos) {
        suma += valor;
    }
    return suma;
}

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
    const size_t cantidad_elementos = 1000000000;
    const int cantidad_hilos = 4;
    std::vector<int> datos(cantidad_elementos, 1);

    auto inicio_sec = std::chrono::high_resolution_clock::now();
    long long resultado_sec = suma_secuencial(datos);
    auto fin_sec = std::chrono::high_resolution_clock::now();
    auto duracion_sec = std::chrono::duration_cast<std::chrono::milliseconds>(
        fin_sec - inicio_sec
    ).count();

    auto inicio_par = std::chrono::high_resolution_clock::now();
    long long resultado_par = suma_paralela(datos, cantidad_hilos);
    auto fin_par = std::chrono::high_resolution_clock::now();
    auto duracion_par = std::chrono::duration_cast<std::chrono::milliseconds>(
        fin_par - inicio_par
    ).count();

    std::cout << "Resultado secuencial: " << resultado_sec << std::endl;
    std::cout << "Tiempo secuencial: " << duracion_sec << " ms" << std::endl;
    std::cout << "Resultado paralelo: " << resultado_par << std::endl;
    std::cout << "Tiempo paralelo: " << duracion_par << " ms" << std::endl;

    return 0;
}