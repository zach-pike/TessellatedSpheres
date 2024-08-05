#include "Tessellation.hpp"

#include <functional>

void Tessellation::_gameThread() {
    init();
    loop();
    cleanup();
}

Tessellation::Tessellation() {

}

Tessellation::~Tessellation() {

}

void Tessellation::start() {
    gameThread = std::thread(std::bind(&Tessellation::_gameThread, this));
}

void Tessellation::join() {
    gameThread.join();
}