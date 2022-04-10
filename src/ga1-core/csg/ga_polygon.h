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
#include "ga_plane.h"
#include "ga_vertex.h"

#include <string>
#include <vector>

/*
** A polygon data structure for CSG
*/
class ga_polygon
{
public:
	ga_polygon();
	ga_polygon(std::vector<ga_vertex>& verts);
	ga_polygon(std::vector<ga_vertex>& verts, std::vector<ga_vec3f>& shared);
	ga_polygon(ga_polygon& other);
	void flip();
	ga_polygon flipped();
	~ga_polygon();

	bool isTri() { return _vertices.size() == 3; };

	std::vector<ga_vertex> _vertices;
	std::vector<ga_vec3f> _shared; //TODO: What structure is this a std::vector of...
	ga_plane _plane;
};