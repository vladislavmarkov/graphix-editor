#include <chrono>
#include <gfx/graphix.h>
#include <gfx/matrix-helpers.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/dual_quaternion.hpp>
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
        0.1f, 100.0f, camera, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f),
        "/home/vlad/Downloads/audi_r8.blend"
    );

    gfxe::wasd_movement wasd(camera);
    auto wasd_movement = gfx::movement::create();
    wasd_movement->set_move_callback([&window, &wasd, &scene, &camera](){
        static const float speed = 1.0f;
        static const float mouse_speed = 0.005f;

        static auto prev = std::chrono::high_resolution_clock::now();

        static std::chrono::high_resolution_clock::time_point current; current =
            std::chrono::high_resolution_clock::now();

        static std::chrono::duration<float> delta_time; delta_time =
            std::chrono::duration_cast<std::chrono::duration<float>>(
                current - prev
            );

        prev = current;

        if (delta_time.count() > 0.05f){
            delta_time = std::chrono::duration<float>::zero();
        }

        static double prev_x = 0, prev_y = 0;
        static double x, y;
        static bool prev_init = [&window](double &_x_, double &_y_)->bool{
            window->get_cursor_pos(_x_, _y_);
            return true;
        }(prev_x, prev_y);
        (void)prev_init;

        window->get_cursor_pos(x, y);

        static float horizontal_angle = 0; horizontal_angle =
            mouse_speed * delta_time.count() *
            float(prev_x - x);

        static float vertical_angle = 0; vertical_angle =
            mouse_speed * delta_time.count() *
            float(prev_y - y);

        prev_x = x;
        prev_y = y;

        glm::mat4 tmp =
            camera->get_matrix() *
            glm::inverse(glm::translate(camera->get_position()));

        glm::quat qrh(
            glm::angleAxis(
                glm::degrees(-horizontal_angle),
                glm::vec3(glm::normalize(
                    tmp * glm::vec4(glm::vec3(0.0f, 0.0f, 1.0f), 0.0f)
                ))
            )
        );

        glm::normalize(qrh);

        tmp = glm::mat4_cast(qrh) * tmp;

        glm::quat qrv(
            glm::angleAxis(
                glm::degrees(-vertical_angle),
                glm::vec3(1.0f, 0.0f, 0.0f)
            )
        );

        glm::normalize(qrv);

        tmp = glm::mat4_cast(qrv) * tmp;
        tmp = tmp * glm::translate(camera->get_position());

        camera->set_matrix(tmp);

        wasd.set_modified();

        camera->set_matrix(
            camera->get_matrix() *
            (glm::translate(wasd.get_direction() * delta_time.count() * speed))
        );

        scene->request_redraw();
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

            if (key == gfx::key::code::charkey_x){
                if (state == gfx::key::state::press){
                    wnd.disable_mouse_cursor();
                }else if (state == gfx::key::state::release){
                    wnd.enable_mouse_cursor();
                }
            }

            if (wasd_keys){
                if (wasd.is_in_motion()){
                    wnd.disable_mouse_cursor();
                    wasd_movement->start();
                }else{
                    wnd.enable_mouse_cursor();
                    wasd_movement->stop();
                }
            }
        }
    );

    window->set_mouse_move_reaction(
        [&wasd, &scene](gfx::window &wnd, double x, double y){
            auto camera = scene->get_camera();
            if (!camera ||
                wasd.is_in_motion() ||
                gfx::key::state::press != wnd.get_key(gfx::key::code::charkey_x)
            ) return;

            static const float mouse_speed = 0.02f;

            static auto prev = std::chrono::high_resolution_clock::now();

            static std::chrono::high_resolution_clock::time_point current;
            current = std::chrono::high_resolution_clock::now();

            static std::chrono::duration<float> delta_time; delta_time =
                std::chrono::duration_cast<std::chrono::duration<float>>(
                    current - prev
                );

            prev = current;

            if (delta_time.count() > 0.05f){
                delta_time = std::chrono::duration<float>::zero();
            }

            static double prev_x = x, prev_y = y;

            static float horizontal_angle = 0; horizontal_angle =
                mouse_speed * delta_time.count() *
                float(prev_x - x);

            static float vertical_angle = 0; vertical_angle =
                mouse_speed * delta_time.count() *
                float(prev_y - y);

            prev_x = x;
            prev_y = y;

            glm::mat4 tmp =
                camera->get_matrix() *
                glm::inverse(glm::translate(camera->get_position()));

            glm::quat qrh(
                glm::angleAxis(
                    glm::degrees(-horizontal_angle),
                    glm::vec3(glm::normalize(
                        tmp * glm::vec4(glm::vec3(0.0f, 0.0f, 1.0f), 0.0f)
                    ))
                )
            );

            glm::normalize(qrh);

            tmp = glm::mat4_cast(qrh) * tmp;

            glm::quat qrv(
                glm::angleAxis(
                    glm::degrees(-vertical_angle),
                    glm::vec3(1.0f, 0.0f, 0.0f)
                )
            );

            glm::normalize(qrv);

            tmp = glm::mat4_cast(qrv) * tmp;
            tmp = tmp * glm::translate(camera->get_position());

            camera->set_matrix(tmp);

            wasd.set_modified();
            (void)wasd.get_direction();

            scene->request_redraw();
        }
    );

    return gfx::run(*window);
}
