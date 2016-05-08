#pragma once
#ifndef __GRAPHIX_WASD_MOVEMENT_H__
#define __GRAPHIX_WASD_MOVEMENT_H__

#include <chrono>
#include <gfx/window.hpp>
#include <glm/glm.hpp>
#include <memory>
#include <stdexcept>
#include <tuple>

namespace gfx{

class camera;
class movement;
class scene;

}

namespace gfxe{

class wasd{
    std::weak_ptr<gfx::window> wnd_;
    std::shared_ptr<gfx::camera> cam_;
    std::weak_ptr<gfx::scene> scn_;
    std::shared_ptr<gfx::movement> mv_;
    std::function<void(gfx::window&, gfx::key::code, gfx::key::state)>
        key_func_;

    const float motion_speed_{1.0f};

    class mouse_coords{
        const float rotation_speed_{0.005f};

        double prev_x_{0}, prev_y_{0};
        std::weak_ptr<gfx::window> wnd_;

        class stopwatch{
            std::chrono::high_resolution_clock::time_point prev_{
                std::chrono::high_resolution_clock::time_point(
                    std::chrono::high_resolution_clock::duration::zero()
                )
            };

        public:
            float delta();
            void reset();
        } sw_;

    public:
        mouse_coords(std::shared_ptr<gfx::window> wnd): wnd_(wnd){}

        std::tuple<float, float> delta_coords(){
            auto wnd = wnd_.lock(); if (!wnd){
                throw std::runtime_error("window is expired");
            }

            double x, y; wnd->get_cursor_pos(x, y);
            float
                delta_x = static_cast<float>(prev_x_ - x),
                delta_y = static_cast<float>(prev_y_ - y);

            prev_x_ = x; prev_y_ = y;
            return std::make_tuple(delta_x, delta_y);
        }

        std::tuple<float, float, float> delta_angles(){
            float delta_x, delta_y, delta_t = sw_.delta();
            std::tie(delta_x, delta_y) = delta_coords();
            float delta_distance = delta_t * rotation_speed_;
            return std::make_tuple(
                delta_distance * delta_x,
                delta_distance * delta_y,
                delta_t
            );
        }

        void reset_watch(){ sw_.reset(); }
    } mouse_coords_;

    // cache
    mutable glm::vec3 direction_;

    bool
        fwd_{false}, back_{false}, left_{false}, right_{false},
        rotation_{false};

    mutable bool modified_{true};

    void recalc_direction() const;
    std::shared_ptr<gfx::camera> init_camera() const;

public:
    wasd(std::shared_ptr<gfx::window> wnd, std::shared_ptr<gfx::scene> scn);

    inline bool is_in_motion() const;
    inline glm::vec3 get_direction() const;
    inline void enable_forward();
    inline void disable_forward();
    inline void enable_back();
    inline void disable_back();
    inline void enable_left();
    inline void disable_left();
    inline void enable_right();
    inline void disable_right();
    inline void enable_rotation();
    inline void disable_rotation();
    inline void toggle_look();

    void on_move();
    void on_key(gfx::key::code, gfx::key::state);
};

}

#endif // __GRAPHIX_WASD_MOVEMENT_H__
