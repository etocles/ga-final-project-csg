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

