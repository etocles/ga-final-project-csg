/*
** RPI Game Architecture Engine
**
** Portions adapted from:
** Viper Engine - Copyright (C) 2016 Velan Studios - All Rights Reserved
** Evan Wallace - CSG.js - https://github.com/evanw/csg.js
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include "ga_csg_component.h"

ga_csg_component::ga_csg_component(class ga_entity* ent, ga_csg::Shape which_shape, ga_vec3f translation, ga_vec3f color) : ga_component(ent) {
    _csg = new ga_csg(which_shape);
    _csg->translate(translation);
    _csg->set_color(color);
    name = (which_shape == ga_csg::Shape::CUBE) ? "Cube" : "Pyramid";
}

ga_csg_component::ga_csg_component(class ga_entity* ent, ga_csg& csg1, ga_csg& csg2) : ga_component(ent) {
    _csg = new ga_csg(csg1.add(csg2));
}

ga_csg_component::~ga_csg_component() {
    delete _csg;
}

void ga_csg_component::update(ga_frame_params* params) {
    float dt = std::chrono::duration_cast<std::chrono::duration<float>>(params->_delta_time).count();

    ga_static_drawcall draw;
    draw._name = "ga_csg_component";
    draw._transform = get_entity()->get_transform();
    draw._draw_mode = GL_TRIANGLES;
    //_csg->assemble_drawcall(draw);    
    draw._vao = _csg->_vao;
    draw._index_count = _csg->_index_count;
    _csg->_material->set_transform(_csg->_transform);
    draw._material = _csg->_material;

    while (params->_static_drawcall_lock.test_and_set(std::memory_order_acquire)) {}
    params->_static_drawcalls.push_back(draw);
    params->_static_drawcall_lock.clear(std::memory_order_release);
}
