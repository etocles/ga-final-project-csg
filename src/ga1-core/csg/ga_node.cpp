/*
** RPI Game Architecture Engine
**
** Portions adapted from:
** Viper Engine - Copyright (C) 2016 Velan Studios - All Rights Reserved
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include "ga_node.h"

ga_node::ga_node(ga_node& other)
{
	_plane = new ga_plane(*(other._plane));
	_front = new ga_node(*(other._front));
	_back = new ga_node(*(other._back));
	for (int i = 0; i < other._polygons.size(); i++) {
		_polygons.push_back(ga_polygon(other._polygons[i]));
	}
}

ga_node::ga_node(std::vector<ga_polygon>& polys)
{
	_plane = nullptr;
	_front = nullptr;
	_back = nullptr;
	if (polys.size() > 0) build(polys);
}

void ga_node::invert()
{
	// flip all polygons
	for (int i = 0; i < _polygons.size(); i++) {
		_polygons[i].flip();
	}
	// also flip plane
	_plane->flip();
	// recursively invert
	if (_front) _front->invert();
	if (_back) _back->invert();
	// swap 
	ga_node* temp = _front;
	_front = _back;
	_back = temp;
}

ga_node ga_node::inverted()
{
	return ga_node();
}

void ga_node::clip_polygons(std::vector<ga_polygon>& polys)
{
}

void ga_node::clip_to(ga_node& bsp)
{
}

std::vector<ga_polygon> ga_node::all_polygons()
{
	return std::vector<ga_polygon>();
}
