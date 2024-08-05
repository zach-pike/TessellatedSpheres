#include "Tessellation/Tessellation.hpp"

int main() {
    auto game = std::make_unique<Tessellation>();
    game->start();
    game->join();

    return 0;
}