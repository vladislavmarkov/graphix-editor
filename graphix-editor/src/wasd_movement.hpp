#pragma once
#ifndef __GRAPHIX_WASD_MOVEMENT_H__
#define __GRAPHIX_WASD_MOVEMENT_H__

#include <gfx/camera.hpp>
#include <glm/glm.hpp>
#include <memory>

namespace gfxe{

class wasd_movement{
    std::weak_ptr<gfx::camera> camera_;

    // cache
    mutable glm::vec3 direction_;

    bool fwd_{false};
    bool back_{false};
    bool left_{false};
    bool right_{false};
    mutable bool modified_{true};

    void recalc_direction() const{
        if (!modified_){ return; }

        direction_ = glm::vec3(0.0f, 0.0f, 0.0f);
        auto shared_camera = camera_.lock();
        if (!shared_camera){ return; }

        if (fwd_){ direction_ += shared_camera->get_forward(); }
        if (back_){ direction_ += shared_camera->get_backward(); }
        if (left_){ direction_ += shared_camera->get_left(); }
        if (right_){ direction_ += shared_camera->get_right(); }

        direction_ = glm::normalize(direction_);
    }

public:
    wasd_movement(std::shared_ptr<gfx::camera> camera): camera_(camera){}

    bool is_in_motion() const{ return fwd_|| back_ || left_ || right_; }

    void set_modified(){ modified_ = true; }

    glm::vec3 get_direction() const{
        recalc_direction();
        modified_ = false;
        return direction_;
    }

    void enable_forward(){
        if (!fwd_){ fwd_ = true; modified_ = true; }
        disable_back();
    }

    void disable_forward(){
        if (fwd_){ fwd_ = false; modified_ = true; }
    }

    void enable_back(){
        if (!back_){ back_ = true; modified_ = true; }
        disable_forward();
    }

    void disable_back(){
        if (back_){ back_ = false; modified_ = true; }
    }

    void enable_left(){
        if (!left_){ left_ = true; modified_ = true; }
        disable_right();
    }

    void disable_left(){
        if (left_){ left_ = false; modified_ = true; }
    }

    void enable_right(){
        if (!right_){ right_ = true; modified_ = true; }
        disable_left();
    }

    void disable_right(){
        if (right_){ right_ = false; modified_ = true; }
    }
};

}

#endif // __GRAPHIX_WASD_MOVEMENT_H__
