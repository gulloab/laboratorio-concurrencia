#include <iostream>
#include <thread>
#include <vector>

void saludar(int id) {
    std::cout << "Hola desde el hilo " << id << std::endl;
}

int main() {
    std::vector<std::thread> hilos;
    for (int i = 0; i < 5; ++i) {
        hilos.emplace_back(saludar, i);
    }
    
    for (auto& hilo: hilos) {
        hilo.join();
    }
    
    std::cout << "Todos los hilos finalizaron." << std::endl;
    return 0;
}