#pragma once
#ifndef __GRAPHIX_WASD_MOVEMENT_H__
#define __GRAPHIX_WASD_MOVEMENT_H__

#include <chrono>
#include <gfx/window.hpp>
#include <glm/glm.hpp>
#include <memory>

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

    class stopwatch{
        std::chrono::high_resolution_clock::time_point prev_{
            std::chrono::high_resolution_clock::time_point(
                std::chrono::high_resolution_clock::duration::zero()
            )
        };

    public:
        float delta();
        void reset();
    } wasd_sw_;

    const float motion_speed_{1.0f};
    const float rotation_speed_{0.005f};

    // cache
    mutable glm::vec3 direction_;

    bool fwd_{false};
    bool back_{false};
    bool left_{false};
    bool right_{false};
    bool rotation_{false};
    mutable bool modified_{true};

    void rotate_camera();
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
