#include <iostream>
#include <thread>
#include <vector>

int contador = 0;

void incrementar(int repeticiones) {
    for (int i = 0; i < repeticiones; ++i) {
        contador++;
    }
}

int main() {
    const int cantidad_hilos = 4;
    const int repeticiones = 100000;
    std::vector<std::thread> hilos;

    for (int i = 0; i < cantidad_hilos; ++i) {
        hilos.emplace_back(incrementar, repeticiones);
    }

    for (auto& hilo: hilos) {
        hilo.join();
    }

    std::cout << "Valor esperado: " 
              << cantidad_hilos * repeticiones 
              << std::endl;
    
    std::cout << "Valor obtenido: " 
              << contador 
              << std::endl;
              
    return 0;
}