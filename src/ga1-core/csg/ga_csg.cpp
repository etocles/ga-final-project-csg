/*
** RPI Game Architecture Engine
**
** Portions adapted from:
** Viper Engine - Copyright (C) 2016 Velan Studios - All Rights Reserved
** Evan Wallace - CSG.js - https://github.com/evanw/csg.js
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include "ga_csg.h"
#include "ga_node.h"
#include "math/ga_vec3f.h"
#include "math/ga_vec4f.h"
#include <vector>



// TODO: Propogate these important assemblies into each constructor
void ga_csg::default_values()
{
    _color = { 1.0f,1.0f,1.0f };
    _material = new ga_csg_material();
    _material->init();
    _material->set_color(_color);
    _current_center = { 0.0f,0.0f,0.0f };
    _vao = make_vao();
}


ga_csg::ga_csg(ga_csg::Shape shp) {
    switch (shp) {
        case Shape::CUBE:
            _polygons = Cube()._polygons;
            break;
        case Shape::SPHERE:
            //_polygons = Sphere()._polygons;
            break;
        case Shape::PYRAMID:
            //_polygons = Pyramid()._polygons;
            break;
    }
    default_values();
    _vao = make_vao();
}

ga_csg::ga_csg(ga_csg& other) {
    _polygons = other._polygons;
    _current_center = other._current_center;
    default_values();
    _color = other._color;
    _material->set_color(_color);
    _vao = make_vao();
}

ga_csg::ga_csg(std::vector<ga_polygon>& polys) {
    _polygons = polys;
    default_values();
    _vao = make_vao();
}

// Return a new CSG solid representing space in either this solid or in the
  // solid `csg`. Neither this solid nor the solid `csg` are modified.
  // 
  //     A.union(B)
  // 
  //     +-------+            +-------+
  //     |       |            |       |
  //     |   A   |            |       |
  //     |    +--+----+   =   |       +----+
  //     +----+--+    |       +----+       |
  //          |   B   |            |       |
  //          |       |            |       |
  //          +-------+            +-------+
  // 
ga_csg ga_csg::add(ga_csg& other)
{
    ga_node a = ga_node(_polygons);
    ga_node b = ga_node(other._polygons);
    a.clip_to(ga_node(other._polygons));
    b.clip_to(ga_node(_polygons));
    //b.invert();
    //b.clip_to(ga_node(other._polygons));
    //b.invert();
    a.build(b.all_polygons());
    ga_csg temp =  ga_csg(a.all_polygons());
    temp.set_color(ga_vec3f_lerp(_color, other._color, 0.5));
    temp.make_vao();
    return temp;
}

// Return a new CSG solid representing space in this solid but not in the
 // solid `csg`. Neither this solid nor the solid `csg` are modified.
 // 
 //     A.subtract(B)
 // 
 //     +-------+            +-------+
 //     |       |            |       |
 //     |   A   |            |       |
 //     |    +--+----+   =   |    +--+
 //     +----+--+    |       +----+
 //          |   B   |
 //          |       |
 //          +-------+
 // 
ga_csg ga_csg::subtract(ga_csg& other)
{
    // TODO: NEEDS DEBUGGING
    ga_node a = ga_node(_polygons);
    ga_node b = ga_node(other._polygons);
    a.clip_to(ga_node(other._polygons));
    b.clip_to(ga_node(_polygons));
    a.invert();
    a.clip_to(b);
    b.clip_to(a);
    b.invert();
    b.clip_to(a);
    b.invert();
    a.build(b.all_polygons());
    a.invert();
    ga_csg temp = ga_csg(a.all_polygons());
    temp.set_color(ga_vec3f_lerp(_color, other._color, 0.5));
    temp.make_vao();
    return temp;
}

// Return a new CSG solid representing space both this solid and in the
// solid `csg`. Neither this solid nor the solid `csg` are modified.
// 
//     A.intersect(B)
// 
//     +-------+
//     |       |
//     |   A   |
//     |    +--+----+   =   +--+
//     +----+--+    |       +--+
//          |   B   |
//          |       |
//          +-------+
// 
ga_csg ga_csg::intersect(ga_csg& other){
    // TODO: NEEDS DEBUGGING
    ga_node a = ga_node(_polygons);
    ga_node b = ga_node(other._polygons);
    a.invert();
    b.clip_to(a);
    b.invert();
    a.clip_to(b);
    b.clip_to(a);
    a.build(b.all_polygons());
    a.invert();
    ga_csg temp = ga_csg(a.all_polygons());
    temp.set_color(ga_vec3f_lerp(_color, other._color, 0.5));
    temp.make_vao();
    return temp;
}


uint32_t ga_csg::make_vao()
{
    std::vector<ga_vec3f> verts;
    std::vector<ga_vec3f> normals;
    std::vector<ga_vec3f> color;
    std::vector<GLushort> indices;
    for (int i = 0; i < _polygons.size(); i++) {
        _polygons[i].get_vbo_info(verts, normals, indices, color, _color);
    }

    glGenVertexArrays(1, &_vao);
    glBindVertexArray(_vao);
    glGenBuffers(3, _vbos);

    glBindBuffer(GL_ARRAY_BUFFER, _vbos[0]);
    glBufferData(GL_ARRAY_BUFFER, verts.size() * 3 * sizeof(float), verts.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, _vbos[1]);
    glBufferData(GL_ARRAY_BUFFER, normals.size() * 3 * sizeof(float), normals.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _vbos[2]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLshort)*indices.size(), &indices[0], GL_STATIC_DRAW);

    glBindVertexArray(0);

    _index_count = indices.size();
    return _vao;
}


void ga_csg::assemble_drawcall(ga_static_drawcall& draw) {
    draw._vao = _vao;
    draw._index_count = _index_count;
    draw._material = _material;
}

struct ConstructInfo {
public:
    ga_vec4f ind;
    ga_vec3f norm;
};
ga_csg ga_csg::Cube(ga_vec3f center) {
    // what the csg will be made with
    std::vector<ga_polygon> polys;

    //for (int i = 0; i < temp.size(); i++) {
    //    std::vector<ga_csg_vertex> vs;
    //    for (int j = 0; j < 4; j++) {
    //        ga_vec3f pos;
    //        pos.x = center.x + radius.axes[0] * (2 * !!((int)temp[i].ind.x & 1) - 1);
    //        pos.y = center.y + radius.axes[1] * (2 * !!((int)temp[i].ind.y & 2) - 1);
    //        pos.z = center.z + radius.axes[2] * (2 * !!((int)temp[i].ind.z & 4) - 1);
    //        vs.push_back(ga_csg_vertex(pos, temp[i].norm));
    //    }
    //    polys.push_back(ga_polygon(vs));
    //}

    std::vector<ga_vec3f> vertices = {
        // Front
        {-1.0, -1.0,  1.0},
        { 1.0, -1.0,  1.0},
        { 1.0,  1.0,  1.0},
        {-1.0,  1.0,  1.0},
        // Top
        {-1.0,  1.0,  1.0},
        { 1.0,  1.0,  1.0},
        { 1.0,  1.0, -1.0},
        {-1.0,  1.0, -1.0},
        // Back
        { 1.0, -1.0, -1.0},
        {-1.0, -1.0, -1.0},
        {-1.0,  1.0, -1.0},
        { 1.0,  1.0, -1.0},
         // Bottom
        { -1.0, -1.0, -1.0},
        {  1.0, -1.0, -1.0},
        {  1.0, -1.0,  1.0},
        { -1.0, -1.0,  1.0},
         // Left
        { -1.0, -1.0, -1.0},
        { -1.0, -1.0,  1.0},
        { -1.0,  1.0,  1.0},
        { -1.0,  1.0, -1.0},
         // Right
        {  1.0, -1.0,  1.0},
        {  1.0, -1.0, -1.0},
        {  1.0,  1.0, -1.0},
        {  1.0,  1.0,  1.0},
    };

    std::vector<ga_vec3f> norms = {
        {0, 0, +1}, // Front
        {0, -1, 0}, // Top
        {0, 0, -1}, // Back
        {0, -1, 0}, // Bottom
        {-1, 0, 0}, // Left
        {+1, 0, 0}  // Right
    };

    for (int i = 0; i < vertices.size(); i += 4) {
        std::vector<ga_csg_vertex> vs;
        for (int j = 0; j < 4; j++) {
            vs.push_back(ga_csg_vertex(center + vertices[i+j], norms[i/4]));
        }
        polys.push_back(ga_polygon(vs));
    }

    ga_csg temp = ga_csg(polys);
    temp._current_center = center;
    return temp;
}

void ga_csg::translate(ga_vec3f& t)
{
    for (int i = 0; i < _polygons.size(); i++) {
        for (int j = 0; j < _polygons[i]._vertices.size(); j++) {
            ga_vec3f old_pos = _polygons[i]._vertices[j]._pos;
            ga_vec3f new_pos = old_pos - _current_center + t;
            _polygons[i]._vertices[j]._pos = new_pos;
        }
    }
    _current_center = t;
    make_vao();
}
