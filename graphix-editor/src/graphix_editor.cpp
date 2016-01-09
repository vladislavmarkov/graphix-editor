#include <chrono>
#include <gfx/graphix.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/transform.hpp>
#include <iomanip>
#include <iostream>
#include <memory>
#include <vector>

#include "wasd_movement.h"

using std::vector;

int main(){
    auto window = gfx::window::create("Graphix Editor");

    // create camera
    glm::vec3
        cam_pos = glm::vec3(3.0f, 4.0f, 5.0f),
        cam_dir = glm::normalize(-glm::vec3(3.0f, 4.0f, 5.0f)),
        cam_right = glm::normalize(
            glm::cross(cam_dir, glm::vec3(0.0f, 0.0f, 1.0f))
        ),
        cam_up = glm::normalize(glm::cross(cam_right, cam_dir));

    auto camera = gfx::camera::create(glm::lookAt(cam_pos, cam_dir, cam_up));

    // load scene from file
    auto scene = gfx::scene::load(
        90.0f, window->get_width(), window->get_height(),
        0.1f, 100.0f, camera.get(), glm::vec4(0.0f, 0.0f, 0.0f, 1.0f),
        "/home/vlad/Downloads/audi_r8.blend"
    );

    gfxe::wasd_movement wasd(camera);
    auto wasd_movement = gfx::movement::create();
    wasd_movement->set_move_callback([&wasd, &scene, &camera](){
        static const float speed = 0.2f;

        static auto prev = std::chrono::high_resolution_clock::now();

        static std::chrono::high_resolution_clock::time_point current; current =
            std::chrono::high_resolution_clock::now();

        static std::chrono::duration<float> delta_time; delta_time =
            std::chrono::duration_cast<std::chrono::duration<float>>(
                prev - current
            );

        prev = current;

        static bool was_in_motion = false;
        static bool was_immobile = true;

        if (wasd.is_in_motion()){
            if (was_immobile){
                was_in_motion = true;
                was_immobile = !was_in_motion;
                delta_time = std::chrono::duration<float>::zero();
                return;
            }

            camera->set_matrix(
                camera->get_matrix() *
                (glm::translate(
                    wasd.get_direction() * delta_time.count() * speed)
                )
            );

            if (camera->was_moved()){
                scene->request_redraw();
            }
        }else{
            if (was_in_motion){
                was_immobile = true;
                was_in_motion = !was_immobile;
                delta_time = std::chrono::duration<float>::zero();
                return;
            }
        }
    });

    // axis X, Y and Z
    auto axisXYZ = gfx::vertex_array::create(
        "axisXYZ", // name
        vector<glm::vec3>{ // vertices
            glm::vec3(0.0f, 0.0f, 0.0f),   // zero coordinate
            glm::vec3(100.0f, 0.0f, 0.0f), // X-axis
            glm::vec3(0.0f, 100.0f, 0.0f), // Y-axis
            glm::vec3(0.0f, 0.0f, 100.0f)  // Z-axis
        },
        vector<unsigned int>{0, 1, 0, 2, 0, 3}, // indices
        gfx::primitive_type::line // how to draw
    );

    auto axis_node = std::shared_ptr<gfx::node>(
        new gfx::node(nullptr, "axis_node")
    );

    scene->add(axis_node, axisXYZ);
    scene->add(wasd_movement);

    window->add(scene);

    window->set_key_reaction(
        [&wasd, &wasd_movement](
            gfx::window &wnd,
            gfx::key::code key,
            gfx::key::state state
        ){
            if (key == gfx::key::code::escape &&
                state == gfx::key::state::release
            ){
                wnd.close();
            }

            bool wasd_keys = false;

            if (key == gfx::key::code::charkey_w){
                wasd_keys = true;
                if (state == gfx::key::state::press){
                    wasd.enable_forward();
                }else if (state == gfx::key::state::release){
                    wasd.disable_forward();
                }
            }else if (key == gfx::key::code::charkey_a){
                wasd_keys = true;
                if (state == gfx::key::state::press){
                    wasd.enable_left();
                }else if (state == gfx::key::state::release){
                    wasd.disable_left();
                }
            }else if (key == gfx::key::code::charkey_s){
                wasd_keys = true;
                if (state == gfx::key::state::press){
                    wasd.enable_back();
                }else if (state == gfx::key::state::release){
                    wasd.disable_back();
                }
            }else if (key == gfx::key::code::charkey_d){
                wasd_keys = true;
                if (state == gfx::key::state::press){
                    wasd.enable_right();
                }else if (state == gfx::key::state::release){
                    wasd.disable_right();
                }
            }

            if (wasd_keys){
                if (wasd.is_in_motion()){
                    wnd.hide_mouse_cursor();
                    wasd_movement->start();
                }else{
                    wnd.show_mouse_cursor();
                    wasd_movement->stop();
                }
            }
        }
    );

    window->set_resize_reaction(
        [](gfx::window&, int, int){}
    );

    return gfx::run(*window);
}
