#include <gfx/graphix.h>

int main(){
    auto window = gfx::window::create("Graphix Editor");

    window->set_key_reaction(
        [](gfx::window &wnd, gfx::key::code key, gfx::key::state state){
        if (key == gfx::key::code::escape &&
            state == gfx::key::state::release
        ){
            wnd.close();
        }
    });

    return gfx::run(*window);
}
