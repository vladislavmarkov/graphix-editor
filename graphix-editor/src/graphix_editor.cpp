#include <iostream>
#include <gfx/graphix.h>

int main(){
    auto window = gfx::window::create("Graphix Editor");

    return gfx::run(*window);
}