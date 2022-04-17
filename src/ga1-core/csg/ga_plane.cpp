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

ga_csg_plane::ga_csg_plane()
{
    _normal = ga_vec3f::zero_vector();
    _w = 0.0f;
}
ga_csg_plane::~ga_csg_plane()
{
}

ga_csg_plane::ga_csg_plane(ga_vec3f& a, ga_vec3f& b, ga_vec3f& c) {
	_normal = ga_vec3f_cross((b - a), (c - a)).normal();
	_w = _normal.dot(a);
}

ga_csg_plane::ga_csg_plane(const ga_csg_plane& other)
{
	_normal = other._normal;
	_w = other._w;
}

void ga_csg_plane::flip()
{
	_normal = -_normal;
	_w = -_w;
}

ga_csg_plane ga_csg_plane::flipped()
{
	ga_csg_plane temp(*this);
    temp.flip();
	return temp;
}


ga_csg_plane ga_csg_plane::operator=(const ga_csg_plane& other)
{
	ga_csg_plane temp;
	temp._normal = other._normal;
	temp._w = other._w;
	return temp;
}
