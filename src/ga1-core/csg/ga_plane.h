

#ifndef GA_CSG_PLANE_H
#define GA_CSG_PLANE_H

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
class ga_csg_plane
{
public:
	ga_csg_plane();
	ga_csg_plane(ga_vec3f& a, ga_vec3f& b, ga_vec3f& c);
	ga_csg_plane(const ga_csg_plane& other);
	~ga_csg_plane();

	void flip();
	ga_csg_plane flipped();

	const float EPSILON = .00001f;

	ga_csg_plane operator=(const ga_csg_plane& other);

	ga_vec3f _normal;
	float _w;
};

#endif