#pragma once

/*
** RPI Game Architecture Engine
**
** Portions adapted from:
** Viper Engine - Copyright (C) 2016 Velan Studios - All Rights Reserved
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <string>
#include <iostream>

/*
** A test component that generates an empty draw call.
*/
class ga_csg
{
public:
	ga_csg(class ga_entity* ent, const char* name);
	~ga_csg();

private:
	std::string _name;
};