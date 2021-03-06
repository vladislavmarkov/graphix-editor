#include <chrono>
#include <gfx/movement.hpp>
#include <gfx/scene.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/transform.hpp>
#include <stdexcept>
#include <tuple>

#include "wasd.hpp"

using namespace std::chrono;
using std::invalid_argument;
using std::runtime_error;
using std::shared_ptr;
using std::string;
using std::tie;

namespace gfxe{

float wasd::mouse_coords::stopwatch::delta(){
    if (prev_ == high_resolution_clock::time_point(
            high_resolution_clock::duration::zero()
        )
    ){
        prev_ = high_resolution_clock::now();
        return high_resolution_clock::duration::zero().count();
    }

    high_resolution_clock::time_point current = high_resolution_clock::now();
    duration<float> result = duration_cast<duration<float>>(current - prev_);
    prev_ = current;
    return result.count();
}

void wasd::mouse_coords::stopwatch::reset(){
    prev_ = high_resolution_clock::time_point(
        high_resolution_clock::duration::zero()
    );
}

void wasd::recalc_direction() const{
    if (!modified_) return;

    direction_ = glm::vec3(0.0f, 0.0f, 0.0f);

    if (fwd_){ direction_ += cam_->get_forward(); }
    if (back_){ direction_ += cam_->get_backward(); }
    if (left_){ direction_ += cam_->get_left(); }
    if (right_){ direction_ += cam_->get_right(); }

    direction_ = glm::normalize(direction_);
}

shared_ptr<gfx::camera> wasd::init_camera() const{
    glm::vec3
        cam_pos = glm::vec3(3.0f, 4.0f, 5.0f),
        cam_dir = glm::normalize(-glm::vec3(3.0f, 4.0f, 5.0f)),
        cam_right = glm::normalize(
            glm::cross(cam_dir, glm::vec3(0.0f, 0.0f, 1.0f))
        ),
        cam_up = glm::normalize(glm::cross(cam_right, cam_dir));

    return gfx::camera::create(glm::lookAt(cam_pos, cam_dir, cam_up));
}

wasd::wasd(
    shared_ptr<gfx::window> wnd,
    shared_ptr<gfx::scene> scn
):
    wnd_(wnd),
    cam_(init_camera()),
    scn_(scn),
    mv_(gfx::movement::create()),
    mouse_coords_(wnd)
{
    scn->add(mv_);
    scn->set_camera(cam_);
    key_func_ = [this](gfx::window&, gfx::key::code key, gfx::key::state state){
        this->on_key(key, state);
    };

    wnd->set_key_reaction(key_func_);
    mv_->set_move_callback([this](){ on_move(); });
}

bool wasd::is_in_motion() const{
    return fwd_|| back_ || left_ || right_ || rotation_;
}

glm::vec3 wasd::get_direction() const{
    recalc_direction();
    modified_ = false;
    return direction_;
}

void wasd::enable_forward(){
    if (!fwd_){ fwd_ = true; modified_ = true; }
    disable_back();
}

void wasd::disable_forward(){
    if (fwd_){ fwd_ = false; modified_ = true; }
}

void wasd::enable_back(){
    if (!back_){ back_ = true; modified_ = true; }
    disable_forward();
}

void wasd::disable_back(){
    if (back_){ back_ = false; modified_ = true; }
}

void wasd::enable_left(){
    if (!left_){ left_ = true; modified_ = true; }
    disable_right();
}

void wasd::disable_left(){
    if (left_){ left_ = false; modified_ = true; }
}

void wasd::enable_right(){
    if (!right_){ right_ = true; modified_ = true; }
    disable_left();
}

void wasd::disable_right(){
    if (right_){ right_ = false; modified_ = true; }
}

void wasd::toggle_look(){
    rotation_ = !rotation_; modified_ = true;
}

void wasd::on_move(){
    auto wnd = wnd_.lock(); if (!wnd){
        throw runtime_error("window is expired");
    }

    float angle_x, angle_y, delta_t;
    tie(angle_x, angle_y, delta_t) = mouse_coords_.delta_angles();

    glm::mat4 tmp =
        cam_->get_matrix() *
        glm::inverse(glm::translate(cam_->get_position()));

    glm::quat qrh(
        glm::angleAxis(
            glm::degrees(-angle_x),
            glm::vec3(glm::normalize(
                tmp * glm::vec4(glm::vec3(0.0f, 0.0f, 1.0f), 0.0f)
            ))
        )
    );

    glm::normalize(qrh);

    tmp = glm::mat4_cast(qrh) * tmp;

    glm::quat qrv(
        glm::angleAxis(glm::degrees(-angle_y), glm::vec3(1.0f, 0.0f, 0.0f))
    );

    glm::normalize(qrv);

    tmp = glm::mat4_cast(qrv) * tmp;
    tmp = tmp * glm::translate(cam_->get_position());

    cam_->set_matrix(tmp);

    if (left_ || right_ || fwd_ || back_){
        modified_ = true;
        cam_->set_matrix(
            cam_->get_matrix() * (
                glm::translate(get_direction() * delta_t * motion_speed_)
            )
        );
    }

    auto scn = scn_.lock(); if (!scn){
        throw runtime_error("scene is null");
    }

    scn->request_redraw();
}

void wasd::on_key(gfx::key::code key, gfx::key::state state){
    auto wnd = wnd_.lock(); if (!wnd){
        throw runtime_error("window is expired");
    }

    if (state == gfx::key::state::repeat) return;

    bool motion = false;
    switch (key){
    case gfx::key::code::escape:
        if (state == gfx::key::state::release){ wnd->close(); } return;
    case gfx::key::code::charkey_w: motion = true;
        if (state == gfx::key::state::press) enable_forward(); else
        if (state == gfx::key::state::release) disable_forward(); break;
    case gfx::key::code::charkey_a: motion = true;
        if (state == gfx::key::state::press) enable_left(); else
        if (state == gfx::key::state::release) disable_left(); break;
    case gfx::key::code::charkey_s: motion = true;
        if (state == gfx::key::state::press) enable_back(); else
        if (state == gfx::key::state::release) disable_back(); break;
    case gfx::key::code::charkey_d: motion = true;
        if (state == gfx::key::state::press) enable_right(); else
        if (state == gfx::key::state::release) disable_right(); break;
    case gfx::key::code::charkey_x: motion = true;
        if (state == gfx::key::state::release) toggle_look(); break;
    default: break;
    }

    if (is_in_motion()){
        wnd->disable_mouse_cursor();
        if (motion) mv_->start();
        mouse_coords_.reset_watch();
    }else{
        wnd->enable_mouse_cursor(); if (motion) mv_->stop();
    }
}

}
