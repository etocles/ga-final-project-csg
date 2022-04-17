/*
** RPI Game Architecture Engine
**
** Portions adapted from:
** Viper Engine - Copyright (C) 2016 Velan Studios - All Rights Reserved
** Evan Wallace - CSG.js - https://github.com/evanw/csg.js
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include "ga_csg_polygon.h"
#include <cassert>
#include <iostream>
#include <algorithm>

ga_polygon::ga_polygon()
{
}
ga_polygon::ga_polygon(std::vector<ga_csg_vertex>& verts)
{
	_vertices = verts;
	//_plane = ga_csg_plane(verts[0]._pos, verts[1]._pos, verts[2]._pos);
	// no clue why = doesn't work, so we have to do this instead.
	_plane._normal = ga_csg_plane(verts[0]._pos, verts[1]._pos, verts[2]._pos)._normal;
	_plane._w = ga_csg_plane(verts[0]._pos, verts[1]._pos, verts[2]._pos)._w;
}
ga_polygon::ga_polygon(std::vector<ga_csg_vertex>& verts, std::vector<ga_vec3f>& shared)
{
	_vertices = verts;
	_shared = shared;
	// no clue why = doesn't work, so we have to do this instead.
	_plane._normal = ga_csg_plane(verts[0]._pos, verts[1]._pos, verts[2]._pos)._normal;
	_plane._w = ga_csg_plane(verts[0]._pos, verts[1]._pos, verts[2]._pos)._w;
}

ga_polygon::ga_polygon(const ga_polygon& other)
{
	_vertices = other._vertices;
	_shared = other._shared;
	// no clue why = doesn't work, so we have to do this instead.
	_plane._normal = other._plane._normal;
	_plane._w = other._plane._w;
}
ga_polygon::~ga_polygon()
{
}

void ga_polygon::get_vbo_info(std::vector<ga_vec3f>& verts,
								std::vector<ga_vec3f>& normals,
								std::vector<GLushort>& indices,
								std::vector<ga_vec3f>& colors,
								const ga_vec3f col)
{
	int start_index = indices.size() == 0 
						? 0 
						: *std::max_element(indices.begin(), indices.end())+1;
	if (isTri()) {
		for (int i = 0; i < 3; i++) {
			colors.push_back(col);
			verts.push_back(_vertices[i]._pos);
			normals.push_back(_vertices[i]._normal);
			indices.push_back(start_index + i);
		}
	}
	// split into two tris, add them
	else if (isQuad()) {
		int arr[] = { 0,1,2,2,3,0 };
		for (int i = 0; i < 4; i++) {
			verts.push_back(_vertices[i]._pos);
			normals.push_back(_vertices[i]._normal);
			colors.push_back(col);
		}
		for (int i = 0; i < 6; i ++) {
			indices.push_back(start_index + arr[i]);
		}
	}
	// if n-gon, break program // TODO: Take out, extremely volatile.
	else {
		assert(false);
	}
}


void ga_polygon::flip()
{
	for (int i = _vertices.size() - 1; i >= 0; i--) {
		_vertices[i].flip();
	}
	_plane.flip();
}

ga_polygon ga_polygon::flipped()
{
	ga_polygon temp = ga_polygon(*this);
	temp.flip();
	return temp;
}


void split_polygon(ga_csg_plane& plane,
					ga_polygon& polygon,
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
		int t = plane._normal.dot(polygon._vertices[i]._pos) - plane._w;
		int type = (t < -plane.EPSILON) ? BACK : (t > plane.EPSILON) ? FRONT : COPLANAR;
		polygonType |= type;
		types.push_back(type);
	}

	// Put the polygon in the correct list, splitting it when necessary.
	switch (polygonType) {
	case COPLANAR:
		(plane._normal.dot(polygon._plane._normal) > 0 ? coplanar_front : coplanar_back).push_back(polygon);
		break;
	case FRONT:
		front.push_back(polygon);
		break;
	case BACK:
		back.push_back(polygon);
		break;
	case SPANNING:
		std::vector<ga_csg_vertex> f;
		std::vector<ga_csg_vertex> b;
		for (int i = 0; i < polygon._vertices.size(); i++) {
			int j = (i + 1) % polygon._vertices.size();
			int ti = types[i];
			int tj = types[j];
			ga_csg_vertex vi = polygon._vertices[i];
			ga_csg_vertex vj = polygon._vertices[j];
			if (ti != BACK) f.push_back(vi);
			if (ti != FRONT) b.push_back(ti != BACK ? ga_csg_vertex(vi) : vi);
			if ((ti | tj) == SPANNING) {
				float t = (plane._w - plane._normal.dot(vi._pos)) / plane._normal.dot(vj._pos - vi._pos);
				ga_csg_vertex v = vi.interpolate(vj, t);
				f.push_back(v);
				b.push_back(ga_csg_vertex(v));
			}
		}
		if (f.size() >= 3) front.push_back(ga_polygon(f, polygon._shared));
		if (b.size() >= 3) back.push_back(ga_polygon(b, polygon._shared));
		break;
	}
}
