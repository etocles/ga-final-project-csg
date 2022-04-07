#pragma once

/*
** RPI Game Architecture Engine
**
** Portions adapted from:
** Viper Engine - Copyright (C) 2016 Velan Studios - All Rights Reserved
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include "math/ga_vec3f.h"

#include <string>
#include <vector>

/*
** A test component that generates an empty draw call.
*/
class ga_polygon
{
public:
	ga_polygon(class ga_entity* ent, const char* name);
	~ga_polygon();

	std::vector<ga_vec3f> _vertices;
	std::vector<ga_vec3f> _shared;
};