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

ga_plane::ga_plane(const ga_plane& other)
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


ga_plane ga_plane::operator=(const ga_plane& other)
{
    return ga_plane(other);
}
