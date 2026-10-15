#include <iostream>
#include <thread>
#include <vector>
#include <mutex>

int contador = 0;
std::mutex mtx;

void incrementar(int repeticiones) {
    for (int i = 0; i < repeticiones; ++i) {
        std::lock_guard<std::mutex> lock(mtx);
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