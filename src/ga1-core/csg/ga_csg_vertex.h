

#ifndef GA_CSG_VERTEX_H
#define GA_CSG_VERTEX_H

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

#include <string>
#include <vector>

/*
** A vertex data structure for CSG
*/
class ga_csg_vertex
{
public:
	ga_csg_vertex();
	ga_csg_vertex(ga_vec3f& p, ga_vec3f& n);
	ga_csg_vertex(const ga_csg_vertex& other);
	~ga_csg_vertex() {

	}

	void flip();
	ga_csg_vertex flipped();
	ga_csg_vertex interpolate(ga_csg_vertex& b, float t);

	ga_vec3f _pos;
	ga_vec3f _normal;
};

#endif