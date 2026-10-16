#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>

std::mutex recurso1;
std::mutex recurso2;

void hilo_a() {
    std::cout << "Hilo A intentando tomar recurso 1..." << std::endl;
    recurso1.lock();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    std::cout << "Hilo A intentando tomar recurso 2..." << std::endl;
    recurso2.lock();
    
    std::cout << "Hilo A obtuvo ambos recursos." << std::endl;
    recurso2.unlock();
    recurso1.unlock();
}

void hilo_b() {
    std::cout << "Hilo B intentando tomar recurso 2..." << std::endl;
    recurso2.lock();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    std::cout << "Hilo B intentando tomar recurso 1..." << std::endl;
    recurso1.lock();
    
    std::cout << "Hilo B obtuvo ambos recursos." << std::endl;
    recurso1.unlock();
    recurso2.unlock();
}

int main() {
    std::thread a(hilo_a);
    std::thread b(hilo_b);

    a.join();
    b.join();

    return 0;
}