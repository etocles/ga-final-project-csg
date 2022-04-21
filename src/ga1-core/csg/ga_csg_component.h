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
#include <string>

/*
** A component that attaches to the viper engine in order to provide
** CSG functionality
*/
class ga_csg_component :public ga_component
{
public:
	ga_csg_component(ga_entity* ent, ga_csg::Shape which_shape, ga_vec3f translation = { 0.0f,0.0f,0.0f }, ga_vec3f color = { 1.0f,1.0f,1.0f });
	ga_csg_component(ga_entity* ent, ga_csg& csg1, ga_csg& csg2);
	virtual ~ga_csg_component();

	virtual void update(struct ga_frame_params* params) override;


	void set_pos(ga_vec3f t) { _csg->translate(t); }
	void set_scale(ga_vec3f t) { _csg->scale(t); }
	void set_color (ga_vec3f c) { _csg->set_color(c); }
	void do_extrude(ga_vec3f t, float amt) { _csg->extrude(t,amt); }
	ga_csg* get_csg() { return _csg; };

	std::string name;
private:
	ga_csg* _csg;
};
