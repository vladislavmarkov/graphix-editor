#include <gfx/graphix.h>

int main(){
    auto window = gfx::window::create("Graphix Editor");

    auto camera = gfx::camera::create(
        glm::vec3(0.0f, 0.0f,-3.0f),
        glm::vec3(0.0f, 0.0f, 1.0f),
        glm::vec3(0.0f, 1.0f, 0.0f)
    );

    auto scene = gfx::scene::create(
        90.0f, window->get_width(), window->get_height(),
        0.1f, 100.0f, camera.get(), glm::vec4(0.6f, 0.6f, 0.6f, 1.0f)
    );

    window->add(scene.get());

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
