#pragma once

#ifndef GA_PLANE_H
#define GA_PLANE_H

/*
** RPI Game Architecture Engine
**
** Portions adapted from:
** Viper Engine - Copyright (C) 2016 Velan Studios - All Rights Reserved
** Evan Wallace - CSG.js - https://github.com/evanw/csg.js
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include "math/ga_vec3f.h"

/*
** A plane data structure for CSG
*/
class ga_plane
{
public:
	ga_plane();
	ga_plane(ga_vec3f& a, ga_vec3f& b, ga_vec3f& c);
	ga_plane(const ga_plane& other);
	~ga_plane();

	void flip();
	ga_plane flipped();

	const float EPSILON = .00001f;

	ga_plane operator=(const ga_plane& other);

	ga_vec3f _normal;
	float _w;
};

#endif