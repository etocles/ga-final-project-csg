/*
** RPI Game Architecture Engine
**
** Portions adapted from:
** Viper Engine - Copyright (C) 2016 Velan Studios - All Rights Reserved
** Evan Wallace - CSG.js - https://github.com/evanw/csg.js
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

void ga_node::clip_to(ga_node& bsp)
{
	_polygons = bsp.clip_polygons(_polygons);
	if (_front) _front->clip_to(bsp);
	if (_back) _back->clip_to(bsp);
}

std::vector<ga_polygon> ga_node::clip_polygons(std::vector<ga_polygon>& polys)
{
	if (!_plane) return std::vector<ga_polygon>(_polygons);
	std::vector<ga_polygon> front;
	std::vector<ga_polygon> back;
	for (int i = 0; i < _polygons.size(); i++) {
		split_polygon(*_plane, _polygons[i], front, back, front, back);
	}
	if (_front) front = _front->clip_polygons(front);
	if (_back) back = _back->clip_polygons(back);
	else back.clear();
	// front.concat(back)
	front.insert(front.end(), back.begin(), back.end());
	return front;
}

std::vector<ga_polygon> ga_node::all_polygons()
{
	std::vector<ga_polygon> polygons = _polygons;
	if (_front) {
		polygons.insert(polygons.end(),
			_front->all_polygons().begin(), 
			_front->all_polygons().end());
	}
	if (_back) {
		polygons.insert(polygons.end(),
			_back->all_polygons().begin(),
			_back->all_polygons().end());
	}
	return polygons;
}

void ga_node::build(std::vector<ga_polygon>& polys)
{
	if (polys.size() == 0) return;
	if (!_plane) _plane = new ga_plane(polys[0]._plane);
	std::vector<ga_polygon> front;
	std::vector<ga_polygon> back;
	for (int i = 0; i < _polygons.size(); i++) {
		split_polygon(*_plane, polys[i], _polygons, _polygons, front, back);
	}
	if (front.size() != 0) {
		if (!_front) _front = new ga_node();
		_front->build(front);
	}
	if (back.size() != 0) {
		if (!_back) _back = new ga_node();
		_back->build(back);
	}
}
