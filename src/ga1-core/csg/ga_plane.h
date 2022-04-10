#pragma once

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
#include "ga_polygon.h"

/*
** A plane data structure for CSG
*/
class ga_plane
{
public:
	ga_plane();
	ga_plane(ga_vec3f& a, ga_vec3f& b, ga_vec3f& c);
	ga_plane(ga_plane& other);
	~ga_plane();

	void flip();
	ga_plane flipped();
	void splitPolygon(ga_polygon& polygon,
					std::vector<ga_polygon> &coplanar_front, 
					std::vector<ga_polygon> &coplanar_back, 
					std::vector<ga_polygon> &front, 
					std::vector<ga_polygon> &back);

	const float EPSILON = .00001f;

	ga_plane operator=(ga_plane& other);

private:
	ga_vec3f _normal;
	float _w;
};