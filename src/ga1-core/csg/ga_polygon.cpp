/*
** RPI Game Architecture Engine
**
** Portions adapted from:
** Viper Engine - Copyright (C) 2016 Velan Studios - All Rights Reserved
** Evan Wallace - CSG.js - https://github.com/evanw/csg.js
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include "ga_polygon.h"
#include <cassert>

ga_polygon::ga_polygon()
{
}
ga_polygon::ga_polygon(std::vector<ga_vertex>& verts)
{
	_vertices = verts;
}
ga_polygon::~ga_polygon()
{
}

void ga_polygon::get_vbo_info(std::vector<ga_vec3f>& verts,
								std::vector<ga_vec3f>& normals,
								std::vector<int>& indices,
								std::vector<ga_vec3f>& colors,
								const ga_vec3f col)
{
	int start_index = indices.size();
	if (isTri()) {
		for (int i = 0; i < 3; i++) {
			colors.push_back(col);
			verts.push_back(_vertices[i]._pos);
			normals.push_back(_vertices[i]._normal);
			indices.push_back(start_index+i); // TODO: Potentially wrong order
		}
	}
	// split into two tris, add them
	else if (isQuad()) {
		int arr[] = { 0,1,2,0,2,3 };
		for (int i : arr) {
			colors.push_back(col);
			verts.push_back(_vertices[i]._pos);
			normals.push_back(_vertices[i]._normal);
			indices.push_back(start_index + i); // TODO: Potentially wrong order
		}
	}
	// if n-gon, break program // TODO: Take out, extremely volatile.
	else {
		assert(false);
	}
}

ga_polygon::ga_polygon(std::vector<ga_vertex>& verts, std::vector<ga_vec3f>& shared)
{
	_vertices = verts;
	_shared = shared;
	_plane = ga_plane(verts[0]._pos, verts[0]._pos, verts[0]._pos);
}

ga_polygon::ga_polygon(ga_polygon& other)
{
	_vertices = other._vertices;
	_shared = other._shared;
	_plane = other._plane;
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


void split_polygon(ga_plane& plane,
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
				float t = (plane._w - plane._normal.dot(vi._pos)) / plane._normal.dot(vj._pos - vi._pos);
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
