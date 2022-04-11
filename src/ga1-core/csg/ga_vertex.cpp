/*
** RPI Game Architecture Engine
**
** Portions adapted from:
** Viper Engine - Copyright (C) 2016 Velan Studios - All Rights Reserved
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include "ga_vertex.h"

ga_vertex::ga_vertex()
{
	_pos = ga_vec3f::zero_vector();
	_normal = ga_vec3f::zero_vector();
}

ga_vertex::ga_vertex(ga_vec3f& p, ga_vec3f& n)
{
	_pos = p;
	_normal = n;
}

ga_vertex::ga_vertex(const ga_vertex& other)
{
	_pos = other._pos;
	_normal = other._normal;
}

void ga_vertex::flip()
{
	_normal = -_normal;
}

ga_vertex ga_vertex::flipped()
{
	ga_vertex temp = ga_vertex(*this);
	temp.flip();
	return temp;
}

ga_vertex ga_vertex::interpolate(ga_vertex& b, float t)
{
	ga_vertex temp = ga_vertex();
	temp._pos = ga_vec3f_lerp(_pos, b._pos, t);
	temp._normal = ga_vec3f_lerp(_normal, b._normal, t);
	return temp;
}
