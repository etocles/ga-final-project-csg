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
    a.clip_to(b);
    b.clip_to(a);
    b.invert();
    b.clip_to(a);
    b.invert();
    a.build(b.all_polygons());
    return ga_csg(a.all_polygons());
}

uint32_t ga_csg::make_vao(GLsizei& index_count)
{
    std::vector<ga_vec3f> verts;
    std::vector<ga_vec3f> normals;
    std::vector<ga_vec3f> color;
    std::vector<int> indices;
    for (int i = 0; i < _polygons.size(); i++) {
        _polygons[i].get_vbo_info(verts, normals, indices, color, _color);
    }

    // TODO: Make sure shader aligns with this
    glBindBuffer(GL_ARRAY_BUFFER, _vbos[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, _vbos[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(normals), normals.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);

    //glBindBuffer(GL_ARRAY_BUFFER, _vbos[2]);
    //glBufferData(GL_ARRAY_BUFFER, sizeof(color), color.data(), GL_STATIC_DRAW);
    //glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
    //glEnableVertexAttribArray(2);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _vbos[3]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices.data(), GL_STATIC_DRAW);

    glBindVertexArray(0);

    index_count = indices.size();
    return _vao;
}

GLsizei ga_csg::get_index_count()
{
    return GLsizei();
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
ga_csg ga_csg::Cube(ga_vec3f radius, ga_vec3f center) {
    center = ga_vec3f::zero_vector();
    // what the csg will be made with
    std::vector<ga_polygon> polys;
    // hard coded data for a cube
    std::vector<ConstructInfo> temp = 
                    {{{0, 4, 6, 2},{-1, 0, 0}},
                    {{1, 3, 7, 5},{+1, 0, 0}},
                    {{0, 1, 5, 4},{0, -1, 0}},
                    {{2, 6, 7, 3},{0, +1, 0}},
                    {{0, 2, 3, 1},{0, 0, -1}},
                    {{4, 5, 7, 6},{0, 0, +1}}};
    for (int i = 0; i < temp.size(); i++) {
        std::vector<ga_csg_vertex> vs;
        for (int j = 0; j < 4; j++) {
            ga_vec3f pos;
            pos.x = center.x + radius.axes[0] * (2 * !!((int)temp[i].ind.x & 1) - 1);
            pos.y = center.y + radius.axes[1] * (2 * !!((int)temp[i].ind.y & 2) - 1);
            pos.z = center.z + radius.axes[2] * (2 * !!((int)temp[i].ind.z & 4) - 1);
            vs.push_back(ga_csg_vertex(pos, temp[i].norm));
        }
        polys.push_back(ga_polygon(vs));
    }

    return ga_csg(polys);
}