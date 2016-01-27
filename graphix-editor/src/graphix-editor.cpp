#include <chrono>
#include <gfx/graphix.hpp>
#include <gfx/matrix-helpers.hpp>
#include <gfx/node.hpp>
#include <gfx/scene.hpp>
#include <gfx/vertex-array.hpp>
#include <gfx/window.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/dual_quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/transform.hpp>
#include <iostream>
#include <memory>
#include <tuple>
#include <vector>

#include "wasd.hpp"

using std::make_tuple;
using std::shared_ptr;
using std::string;
using std::tie;
using std::tuple;
using std::vector;

shared_ptr<gfx::scene> init_scene(
    const gfx::window &wnd,
    const std::string &source_filename
){
    return gfx::scene::load(
        90.0f, wnd.get_width(), wnd.get_height(),
        0.1f, 100.0f,
        shared_ptr<gfx::camera>(),
        glm::vec4(0.0f, 0.0f, 0.0f, 1.0f),
        source_filename
    );
}

tuple<shared_ptr<gfx::drawable>, shared_ptr<gfx::node>> init_xyz(){
    auto xyz = gfx::vertex_array::create(
        "xyz", // name
        vector<glm::vec3>{ // vertices
            glm::vec3(0.0f, 0.0f, 0.0f),   // zero coordinate
            glm::vec3(100.0f, 0.0f, 0.0f), // X-axis
            glm::vec3(0.0f, 100.0f, 0.0f), // Y-axis
            glm::vec3(0.0f, 0.0f, 100.0f)  // Z-axis
        },
        vector<unsigned int>{0, 1, 0, 2, 0, 3}, // indices
        gfx::primitive_type::line // how to draw
    );

    auto xyz_node = std::shared_ptr<gfx::node>(
        new gfx::node(nullptr, "xyz_node")
    );

    return make_tuple(xyz, xyz_node);
}

int main(int argc, char **argv){
    if (argc < 2){
        throw std::invalid_argument("scene file expected");
    }

    auto wnd = gfx::window::create("Graphix Editor");
    auto scn = init_scene(*wnd, argv[1]);
    gfxe::wasd wasd(wnd, scn);
    shared_ptr<gfx::drawable> xyz;
    shared_ptr<gfx::node> xyz_node;
    tie(xyz, xyz_node) = init_xyz();
    scn->add(xyz_node, xyz);
    wnd->add(scn);

    return gfx::run(*wnd);
}
