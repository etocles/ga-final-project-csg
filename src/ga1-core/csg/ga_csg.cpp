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

ga_csg::~ga_csg() {
    glDeleteVertexArrays(1, (GLuint*)&_vao);
    glDeleteBuffers(4, _vbos);
}
ga_csg::ga_csg(ga_csg& other) {
	_polygons = other._polygons;
}
ga_csg::ga_csg(std::vector<ga_polygon>& polys) {
	_polygons = polys;
}

std::vector<ga_polygon> ga_csg::to_polygons() {
	return _polygons;
}

void ga_csg::assemble_drawcall(ga_static_drawcall& draw) {
    draw._vao = make_vao();
    draw._index_count = get_index_count();
}


ga_csg Cube(ga_vec3f& radius) {
    // TODO: Complete implementation
    std::vector<ga_polygon> polys;
    ga_vec3f center = ga_vec3f::zero_vector();
    //int arr[][][] = { {{0, 4, 6, 2},{-1, 0, 0}},
    //                {{1, 3, 7, 5},{+1, 0, 0}},
    //                {{0, 1, 5, 4},{0, -1, 0}},
    //                {{2, 6, 7, 3},{0, +1, 0}},
    //                {{0, 2, 3, 1},{0, 0, -1}},
    //                {{4, 5, 7, 6},{0, 0, +1}} };

    return ga_csg(polys);
    /*
    map(function(info) {
        return new CSG.Polygon(info[0].map(function(i) {
            var pos = new CSG.Vector(
                c.x + r[0] * (2 * !!(i & 1) - 1),
                c.y + r[1] * (2 * !!(i & 2) - 1),
                c.z + r[2] * (2 * !!(i & 4) - 1)
            );
            return new CSG.Vertex(pos, new CSG.Vector(info[1]));
        }));
    };
    */
}