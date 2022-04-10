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

ga_polygon::ga_polygon()
{
}
ga_polygon::~ga_polygon()
{
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

