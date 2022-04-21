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
	ga_csg_component(ga_entity* ent, ga_csg& csg1, ga_csg& csg2, ga_csg::OP op = ga_csg::OP::ADD);
	virtual ~ga_csg_component();
	virtual void update(struct ga_frame_params* params) override;
	virtual void late_update(struct ga_frame_params* params) override;

	ga_csg* get_csg(int i = 0) { return _csgs[i]; };
	void add(ga_csg* csg) { _csgs.push_back(csg); }
	void remove(int i);
	int size() { return _csgs.size(); }
	int get_id() { return nonce++; }

private:
	std::vector<ga_csg*> _csgs;
	int index_to_remove;
	int nonce = 0;
};
