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



void ga_csg::default_values()
{
    _color = { 1.0f,1.0f,1.0f };
    _material = new ga_csg_material();
    _material->init();
    _material->set_color(_color);
    _vao = make_vao();
    _transform.make_identity();
}

#pragma region CONSTRUCTORS
ga_csg::ga_csg(ga_csg::Shape shp) {
    switch (shp) {
        case Shape::CUBE:
            _polygons = Cube()._polygons;
            name = "Cube";
            break;
        case Shape::SPHERE:
            _polygons = Sphere()._polygons;
            name = "Sphere";
            break;
        case Shape::PYRAMID:
            _polygons = Pyramid()._polygons;
            name = "Pyramid";
            break;
    }
    default_values();
    _vao = make_vao();
}

ga_csg::ga_csg(ga_csg& other) {
    _polygons = other._polygons;
    default_values();
    _color = other._color;
    _material->set_color(_color);
    _vao = make_vao();
    name = other.name;
}

ga_csg::ga_csg(std::vector<ga_polygon>& polys) {
    _polygons = polys;
    default_values();
    _vao = make_vao();
    name = "Poly";
}

#pragma endregion

#pragma region OPERATIONS

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
    std::vector<ga_polygon> own_adjusted_polys = get_polygons();
    std::vector<ga_polygon> other_adjusted_polys = other.get_polygons();
    ga_node a = ga_node(own_adjusted_polys);
    ga_node b = ga_node(other_adjusted_polys);
    a.clip_to(ga_node(own_adjusted_polys));
    b.clip_to(ga_node(other_adjusted_polys));
    //b.invert();
    //b.clip_to(ga_node(other_adjusted_polys));
    //b.invert();
    a.build(b.all_polygons());
    ga_csg temp =  ga_csg(a.all_polygons());
    temp.set_color(ga_vec3f_lerp(_color, other._color, 0.5));
    //temp._polygons = std::vector<ga_polygon>(temp._polygons.begin(), temp._polygons.begin() + temp._polygons.size()*0.2);
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
    std::vector<ga_polygon> own_adjusted_polys = get_polygons();
    std::vector<ga_polygon> other_adjusted_polys = other.get_polygons();
    ga_node a = ga_node(own_adjusted_polys);
    ga_node b = ga_node(other_adjusted_polys);
    a.clip_to(ga_node(other_adjusted_polys));
    b.clip_to(ga_node(own_adjusted_polys));
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

    std::vector<ga_polygon> own_adjusted_polys = get_polygons();
    std::vector<ga_polygon> other_adjusted_polys = other.get_polygons();
    ga_node a = ga_node(own_adjusted_polys);
    ga_node b = ga_node(other_adjusted_polys);
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

#pragma endregion 

#pragma region DRAWING TO SCREEN

uint32_t ga_csg::make_vao()
{
    std::vector<ga_vec3f> verts;
    std::vector<ga_vec3f> normals;
    std::vector<GLushort> indices;
    for (int i = 0; i < _polygons.size(); i++) {
        _polygons[i].get_vbo_info(_transform, verts, normals, indices);
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

#pragma endregion

#pragma region SHAPES
// Creates a unit cube, centered at the origin.
ga_csg ga_csg::Cube() {
    // what the csg will be made with
    std::vector<ga_polygon> polys;
    std::vector<ga_vec3f> vertices = {
        // Front
        {-0.5, -0.5,  0.5},
        { 0.5, -0.5,  0.5},
        { 0.5,  0.5,  0.5},
        {-0.5,  0.5,  0.5},
        // Top
        {-0.5,  0.5,  0.5},
        { 0.5,  0.5,  0.5},
        { 0.5,  0.5, -0.5},
        {-0.5,  0.5, -0.5},
        // Back
        { 0.5, -0.5, -0.5},
        {-0.5, -0.5, -0.5},
        {-0.5,  0.5, -0.5},
        { 0.5,  0.5, -0.5},
         // Bottom
        { -0.5, -0.5, -0.5},
        {  0.5, -0.5, -0.5},
        {  0.5, -0.5,  0.5},
        { -0.5, -0.5,  0.5},
         // Left
        { -0.5, -0.5, -0.5},
        { -0.5, -0.5,  0.5},
        { -0.5,  0.5,  0.5},
        { -0.5,  0.5, -0.5},
         // Right
        {  0.5, -0.5,  0.5},
        {  0.5, -0.5, -0.5},
        {  0.5,  0.5, -0.5},
        {  0.5,  0.5,  0.5},
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
            vs.push_back(ga_csg_vertex(vertices[i+j], norms[i/4]));
        }
        polys.push_back(ga_polygon(vs));
    }

    ga_csg temp = ga_csg(polys);
    return temp;
}
// Creates a unit pyramid, centered at the origin.
ga_csg ga_csg::Pyramid() {
    // what the csg will be made with
    std::vector<ga_polygon> polys;
    std::vector<std::vector<ga_vec3f>> vertgroups = {
        // Bottom
        std::vector<ga_vec3f>({
            { -0.5, -0.5, -0.5},
            {  0.5, -0.5, -0.5},
            {  0.5, -0.5,  0.5},
            { -0.5, -0.5,  0.5}
        }),
        // Front
        std::vector<ga_vec3f>({
            { 0.0, 0.5, 0.0},
            {  0.5, -0.5,  0.5},
            { -0.5, -0.5,  0.5}
        }),
        // Back
        std::vector<ga_vec3f>({
            { 0.0, 0.5, 0.0},
            { -0.5, -0.5, -0.5},
            {  0.5, -0.5, -0.5}
        }),
        // Left
        std::vector<ga_vec3f>({
            { 0.0, 0.5, 0.0},
            { -0.5, -0.5,  0.5},
            { -0.5, -0.5, -0.5}
        }),
        // Right
        std::vector<ga_vec3f>({
            { 0.0, 0.5, 0.0},
            {  0.5, -0.5,  0.5},
            {  0.5, -0.5, -0.5}
        }),
    };

    std::vector<ga_vec3f> norms = {
        {0, -1, 0}, // Bottom
        {0, 0.89442, +0.4472}, // Front  
        {0, 0.89442, -0.4472}, // Back        
        {-0.4472, 0.89442, 0}, // Left     
        {+0.4472, 0.89442, 0} // Right    
    };

    for (int i = 0; i < vertgroups.size(); i++) {
        std::vector<ga_csg_vertex> vs;
        for (int j = 0; j < vertgroups[i].size(); j++) {
            vs.push_back(ga_csg_vertex(vertgroups[i][j], norms[i]));
        }
        polys.push_back(ga_polygon(vs));
    }

    ga_csg temp = ga_csg(polys);
    return temp;
}
// Creates a unit sphere, centered at the origin.
// TODO: Implement, rn still makes a cube
ga_csg ga_csg::Sphere() {
    // what the csg will be made with
    std::vector<ga_polygon> polys;
    std::vector<ga_csg_vertex> verts;
    int slices = 16;
    int stacks = 8;

    auto vertex = [&](float theta, float phi) {
        theta *= GA_PI * 2;
        phi *= GA_PI;
        ga_vec3f dir = {
           cos(theta) * sin(phi),
           cos(phi),
           sin(theta) * sin(phi)
        };
        verts.push_back(ga_csg_vertex(dir, dir));
    };

    for (int i = 0; i < slices; i++) {
        for (int j = 0; j < stacks; j++) {
            verts.clear();
            vertex(i / slices, j / stacks);
            if (j > 0) vertex((i + 1) / slices, j / stacks);
            if (j < stacks - 1) vertex((i + 1) / slices, (j + 1) / stacks);
            vertex(i / slices, (j + 1) / stacks);
            polys.push_back(ga_polygon(verts));
        }
    }
  

    ga_csg temp = ga_csg(polys);
    return temp;
}
#pragma endregion

void ga_csg::set_pos(ga_vec3f t)
{
    _transform.set_translation(t);
}
void ga_csg::set_scale (ga_vec3f t)
{
    _transform.data[0][0] = t.axes[0];
    _transform.data[1][1] = t.axes[1];
    _transform.data[2][2] = t.axes[2];
}
void ga_csg::extrude(ga_vec3f dir, float amt) {
    ga_vec3f s = { 1.0f,1.0f,1.0f };    // keep all the other dimensions intact
    s += dir.scale_result(amt - 1);
    ga_vec3f currentLengthInDimension = { abs(dir.x) > 0 ? _transform.data[0][0] : 0.0,
                                          abs(dir.y) > 0 ? _transform.data[1][1] : 0.0,
                                          abs(dir.z) > 0 ? _transform.data[2][2] : 0.0 };
    _transform.data[0][0] *= s.axes[0]; // do this for ALL axes
    _transform.data[1][1] *= s.axes[1]; // do this for ALL axes
    _transform.data[2][2] *= s.axes[2]; // do this for ALL axes
    // Move object by a factor of q(adding to previous pos) 
    // To maintain a similar position to where it was prior to scaling
    ga_vec3f q = currentLengthInDimension.scale_result((0.5) * (amt - 1.0));
    _transform.set_translation(_transform.get_translation() + q);
}


