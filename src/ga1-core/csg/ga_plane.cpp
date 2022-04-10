/*
** RPI Game Architecture Engine
**
** Portions adapted from:
** Viper Engine - Copyright (C) 2016 Velan Studios - All Rights Reserved
** Evan Wallace - CSG.js - https://github.com/evanw/csg.js
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include "ga_plane.h"

ga_plane::ga_plane()
{
    _normal = ga_vec3f::zero_vector();
    _w = 0.0f;
}
ga_plane::~ga_plane()
{
}

ga_plane::ga_plane(ga_vec3f& a, ga_vec3f& b, ga_vec3f& c) {
	_normal = ga_vec3f_cross((b - a), (c - a)).normal();
}

ga_plane::ga_plane(ga_plane& other)
{
	_normal = other._normal;
	_w = other._w;
}

void ga_plane::flip()
{
	_normal = -_normal;
	_w = -_w;
}

ga_plane ga_plane::flipped()
{
	ga_plane temp(*this);
    temp.flip();
	return temp;
}

void ga_plane::splitPolygon(ga_polygon& polygon, 
						std::vector<ga_polygon>& coplanar_front, 
						std::vector<ga_polygon>& coplanar_back, 
						std::vector<ga_polygon>& front, 
						std::vector<ga_polygon>& back)
{
	const int COPLANAR = 0;
	const int FRONT = 1;
	const int BACK = 2;
	const int SPANNING = 3;

    // Classify each point as well as the entire polygon into one of the above
   // four classes.
    int polygonType = 0;
    std::vector<int> types;
    for (int i = 0; i < polygon._vertices.size(); i++) {
        int t = _normal.dot(polygon._vertices[i]._pos) - _w;
        int type = (t < -EPSILON) ? BACK : (t > EPSILON) ? FRONT : COPLANAR;
        polygonType |= type;
        types.push_back(type);
    }

    // Put the polygon in the correct list, splitting it when necessary.
    switch (polygonType) {
        case COPLANAR:
            (_normal.dot(polygon._plane._normal) > 0 ? coplanar_front : coplanar_back).push_back(polygon);
            break;
        case FRONT:
            front.push_back(polygon);
            break;
        case BACK:
            back.push_back(polygon);
            break;
        case SPANNING:
            std::vector<ga_vertex> f;
            std::vector<ga_vertex> b;
            for (int i = 0; i < polygon._vertices.size(); i++) {
                int j = (i + 1) % polygon._vertices.size();
                int ti = types[i];
                int tj = types[j];
                ga_vertex vi = polygon._vertices[i];
                ga_vertex vj = polygon._vertices[j];
                if (ti != BACK) f.push_back(vi);
                if (ti != FRONT) b.push_back(ti != BACK ? ga_vertex(vi) : vi);
                if ((ti | tj) == SPANNING) {
                    float t = (_w - _normal.dot(vi._pos)) / _normal.dot(vj._pos - vi._pos);
                    ga_vertex v = vi.interpolate(vj, t);
                    f.push_back(v);
                    b.push_back(ga_vertex(v));
                }
            }
            if (f.size() >= 3) front.push_back(ga_polygon(f, polygon._shared));
            if (b.size() >= 3) back.push_back(ga_polygon(b, polygon._shared));
            break;
    }
}

ga_plane ga_plane::operator=(ga_plane& other)
{
    return ga_plane(other);
}
