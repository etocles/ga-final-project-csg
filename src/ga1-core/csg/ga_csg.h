#ifndef GA_CSG_H
#define GA_CSG_H

/*
** RPI Game Architecture Engine
**
** Portions adapted from:
** Viper Engine - Copyright (C) 2016 Velan Studios - All Rights Reserved
** Evan Wallace - CSG.js - https://github.com/evanw/csg.js
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/
//#include "entity/ga_component.h"
#include "ga_csg_polygon.h"
#include "framework/ga_frame_params.h"
#include "graphics/ga_material.h"

/*
** The main architecture for CSG.
** Also implements assemble_drawcall() which is used
** to draw things to the screen with OpenGL using VAOs and VBOs
*/
class ga_csg
{
public:
	static enum class Shape { CUBE, SPHERE, PYRAMID };
	static enum class OP { ADD, SUB, INTERSECT};

	ga_csg(Shape shp);
	ga_csg(ga_csg& other);
	ga_csg(std::vector<ga_polygon>& polys);
	~ga_csg() {
		glDeleteVertexArrays(1, (GLuint*)&_vao);
		glDeleteBuffers(3, _vbos);
	};

	/* Retrieve the CSG's polygons as they appear in unit-space */
	std::vector<ga_polygon> get_polygons_raw() { return _polygons; };

	/* Retrieve a certain CSG object's polygons with transformations
	*  !! VERY EXPENSIVE OPERATION !!
	*/
	std::vector<ga_polygon> get_polygons() {
		std::vector<ga_polygon> res;
		for (int i = 0; i < _polygons.size(); i++) {
			std::vector<ga_csg_vertex> temp_verts;
			for (int j = 0; j < _polygons[i]._vertices.size(); j++) {
				ga_vec3f old_pos = _polygons[i]._vertices[j]._pos;
				ga_vec4f temp = { old_pos.x, old_pos.y, old_pos.z, 1.0f }; // might need to be 0.0
				temp = _transform.transform(temp);
				ga_vec3f new_pos = { temp.x, temp.y, temp.z };
				temp_verts.push_back(ga_csg_vertex(new_pos, _polygons[i]._vertices[j]._normal));
			}
			res.push_back(ga_polygon(temp_verts));
		}
		return res;
	}

	ga_csg add(ga_csg& other);
	ga_csg subtract(ga_csg& other);
	ga_csg intersect(ga_csg& other);

	ga_csg static Cube();
	ga_csg static Sphere();
	ga_csg static Pyramid();

	void set_color(ga_vec3f col) { _color = col; _material->set_color(col); };
	void set_pos(ga_vec3f t);
	void set_scale(ga_vec3f t);
	void extrude(ga_vec3f dir, float amt);

	ga_mat4f get_transform() { return _transform; };
	ga_csg_material* get_material() { return _material; };

	std::string name;
	int id;
private:
	uint32_t make_vao();
	void default_values();
	class ga_csg_material* _material;
	uint32_t _vao;
	GLsizei _index_count;
	uint32_t _vbos[3];
	ga_vec3f _color;
	ga_mat4f _transform;
	std::vector<ga_polygon> _polygons;

	friend class ga_csg_component;
};

#endif