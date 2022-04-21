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
    _csgs.push_back(new ga_csg(which_shape));
    _csgs[0]->set_pos(translation);
    _csgs[0]->set_color(color);
}

ga_csg_component::ga_csg_component(class ga_entity* ent, ga_csg& csg1, ga_csg& csg2) : ga_component(ent) {
    _csgs.push_back(new ga_csg(csg1.add(csg2)));
}

ga_csg_component::~ga_csg_component() {
    for (int i = 0; i < _csgs.size(); i++) delete _csgs[i];
}

void ga_csg_component::update(ga_frame_params* params) {
    float dt = std::chrono::duration_cast<std::chrono::duration<float>>(params->_delta_time).count();
    
    std::vector<ga_static_drawcall> draws;
    for (int i = 0; i < _csgs.size(); i++) {
        ga_static_drawcall draw;
        draw._name = _csgs[i]->name;
        draw._transform = get_entity()->get_transform();
        draw._draw_mode = GL_TRIANGLES;
        //_csg->assemble_drawcall(draw);    
        draw._vao = _csgs[i]->_vao;
        draw._index_count = _csgs[i]->_index_count;
        _csgs[i]->_material->set_transform(_csgs[i]->_transform);
        draw._material = _csgs[i]->_material;
        draws.push_back(draw);
    }    
    while (params->_static_drawcall_lock.test_and_set(std::memory_order_acquire)) {}
    for (ga_static_drawcall d : draws) params->_static_drawcalls.push_back(d);
    params->_static_drawcall_lock.clear(std::memory_order_release);
}

void ga_csg_component::late_update(ga_frame_params* params)
{
    if (index_to_remove < 0) return;
    _csgs.erase(_csgs.begin() + index_to_remove);
    // Commented because the fiber workers do not have access to gl* Functions and this breaks the program
    //delete _csgs[index]; 
    index_to_remove = -1;
}

void ga_csg_component::remove(int i)
{
    index_to_remove = i;
}
