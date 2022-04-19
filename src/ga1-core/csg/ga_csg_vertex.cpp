/*
** RPI Game Architecture Engine
**
** Portions adapted from:
** Viper Engine - Copyright (C) 2016 Velan Studios - All Rights Reserved
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include "ga_csg_vertex.h"

ga_csg_vertex::ga_csg_vertex()
{
	_pos = ga_vec3f::zero_vector();
	_normal = ga_vec3f::zero_vector();
}

ga_csg_vertex::ga_csg_vertex(ga_vec3f& p, ga_vec3f& n)
{
	_pos = p;
	_normal = n;
}

ga_csg_vertex::ga_csg_vertex(const ga_csg_vertex& other)
{
	_pos = other._pos;
	_normal = other._normal;
}

void ga_csg_vertex::flip()
{
	_normal = -_normal;
}

ga_csg_vertex ga_csg_vertex::flipped()
{
	ga_csg_vertex temp = ga_csg_vertex(*this);
	temp.flip();
	return temp;
}

ga_csg_vertex ga_csg_vertex::interpolate(ga_csg_vertex& b, float t)
{
	ga_csg_vertex temp = ga_csg_vertex();
	temp._pos = ga_vec3f_lerp(_pos, b._pos, t);
	temp._normal = ga_vec3f_lerp(_normal, b._normal, t);
	return temp;
}

ga_csg_vertex ga_csg_vertex::transform(ga_mat4f& _mat)
{
	ga_vec3f old_pos = _pos;
	ga_vec4f temp = { old_pos.x, old_pos.y, old_pos.z, 1.0f }; // might need to be 0.0
	temp = _mat.transform(temp);
	ga_vec3f new_pos = { temp.x, temp.y, temp.z };
	return ga_csg_vertex(new_pos, _normal);
}
