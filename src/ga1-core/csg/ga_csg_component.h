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

#include "entity/ga_component.h"
#include "entity/ga_entity.h"
#include "ga_csg.h"

#include <cstdint>

/*
** Renderable basic textured cubed.
*/
class ga_csg_component :public ga_component
{
public:
	ga_csg_component(class ga_entity* ent);
	virtual ~ga_csg_component();

	virtual void update(struct ga_frame_params* params) override;

private:
	ga_csg* _csg;
};
