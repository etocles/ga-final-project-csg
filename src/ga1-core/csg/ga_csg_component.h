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

/// <summary>
/// A component that attaches to an entity in the viper engine.
/// Stores multiple references to csg objects which can be modified individually
/// As well as combined with Add, Subtract, and Intersect to create new ones.
/// </summary>
class ga_csg_component :public ga_component
{
public:
	/// <summary>
	/// Initializes a component with a primitive shape
	/// </summary>
	/// <param name="ent"> A pointer to the entity to which this component gets added to </param>
	/// <param name="which_shape"> An enum value dictating which primitive to spawn with the creation of this component</param>
	/// <param name="translation"> An optional parameter, specifying where to position the spawned CSG in 3D space</param>
	/// <param name="color"> An optional parameter, specifying which color to give the spawned primitive</param>
	ga_csg_component(ga_entity* ent, ga_csg::Shape which_shape, ga_vec3f translation = { 0.0f,0.0f,0.0f }, ga_vec3f color = { 1.0f,1.0f,1.0f });
	/// <summary>
	/// Initializes a component with the resultant of performing csg1 op csg2
	/// </summary>
	/// <param name="ent"> A pointer to the entity to which this component gets added to </param>
	/// <param name="csg1"> A reference to the csg object which is performing the operation </param>
	/// <param name="csg2"> A reference to the csg object which is the second argument in the operation </param>
	/// <param name="op"> An enum specifying which operation to be performed </param>
	ga_csg_component(ga_entity* ent, ga_csg& csg1, ga_csg& csg2, ga_csg::OP op = ga_csg::OP::ADD);
	virtual ~ga_csg_component();
	
	/// <summary>
	/// Assembles drawcalls for all the csgs in its possession, pushing them onto the draw stack
	/// </summary>
	/// <param name="params"></param>
	virtual void update(struct ga_frame_params* params) override;
	/// <summary>
	/// Removes any csgs queued for removal
	/// </summary>
	/// <param name="params"></param>
	virtual void late_update(struct ga_frame_params* params) override;

	/// <summary>
	/// Accessor which retrieves a csg component's child csg at the specified index
	/// </summary>
	/// <param name="i"> Index of the child csg to return </param>
	/// <returns> A pointer to the csg object located at that index. </returns>
	ga_csg* get_csg(int i = 0) { return _csgs[i]; };
	/// <summary>
	/// Adds a csg to the list of owned csg children
	/// </summary>
	/// <param name="csg"> Pointer to csg object to be added </param>
	void add(ga_csg* csg) { _csgs.push_back(csg); }
	/// <summary>
	/// Removes a csg from the list of owned csgs
	/// </summary>
	/// <param name="i"> Index of the csg to be removed </param>
	void remove(int i);
	/// <summary>
	/// Returns the amount of children csgs contained
	/// </summary>
	/// <returns> the amount of children csgs </returns>
	int size() { return _csgs.size(); }
	/// <summary>
	/// A nicety to easily assign a unique ID to each attached CSG
	/// </summary>
	/// <returns> the id to assign to the newly added CSG </returns>
	int get_id() { return nonce++; }

private:
	std::vector<ga_csg*> _csgs;
	int index_to_remove;
	int nonce = 0;
};
