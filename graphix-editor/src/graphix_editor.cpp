#include <gfx/graphix.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <iostream>

int main(){
    auto window = gfx::window::create("Graphix Editor");

    auto camera = gfx::camera::create(
        glm::vec3(6.0f, -2.0f, 4.0f),
        glm::normalize(-glm::vec3(6.0f, -2.0f, 4.0f)),
        glm::vec3(0.0f, 0.0f, 1.0f)
    );

    auto scene = gfx::scene::load(
        90.0f, window->get_width(), window->get_height(),
        0.1f, 100.0f, camera.get(), glm::vec4(0.0f, 0.0f, 0.0f, 1.0f),
        "/path/to/model.blend"
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

    window->set_resize_reaction(
        [](gfx::window&, int, int){}
    );

    return gfx::run(*window);
}
