


#ifndef GA_NODE_H
#define GA_NODE_H
/*
** RPI Game Architecture Engine
**
** Portions adapted from:
** Viper Engine - Copyright (C) 2016 Velan Studios - All Rights Reserved
** Evan Wallace - CSG.js - https://github.com/evanw/csg.js
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include "ga_csg_polygon.h"

/*
Holds a node in a BSP tree. A BSP tree is built from a collection of polygons
by picking a polygon to split along. That polygon (and all other coplanar
polygons) are added directly to that node and the other polygons are added to
the front and/or back subtrees. This is not a leafy BSP tree since there is
no distinction between internal and leaf nodes.
*/
class ga_node
{
public:
	ga_node() {
		_plane = nullptr;
		_front = nullptr;
		_back = nullptr;
	}
	ga_node(ga_node& other);
	ga_node(std::vector<ga_polygon*>& polys);
	~ga_node() { }

	void invert();
	ga_node inverted();

	void clip_to(ga_node& bsp);
	std::vector<ga_polygon*> clip_polygons(std::vector<ga_polygon*>& polys);
	std::vector<ga_polygon*> all_polygons();

	void build(std::vector<ga_polygon*>& polys);

	ga_csg_plane* _plane;
	ga_node* _front;
	ga_node* _back;
	std::vector<ga_polygon*> _polygons;
};

#endif