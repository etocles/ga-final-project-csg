

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
	ga_csg()
	{
		_polygons = Cube()._polygons;
		_color = { 1.0f,1.0f,1.0f };
		_material = new ga_csg_material();
		_material->init();
		_material->set_color(_color);
		_vao = make_vao(_index_count);
	}
	ga_csg(ga_csg& other) { _polygons = other._polygons; };
	ga_csg(std::vector<ga_polygon>& polys) { _polygons = polys; };
	~ga_csg() {
		glDeleteVertexArrays(1, (GLuint*)&_vao);
		glDeleteBuffers(3, _vbos);
	};

	/* Drawing the CSG to the screen */
	void assemble_drawcall(ga_static_drawcall& draw);

	/* Retrieve a certain CSG objects polygons */
	std::vector<ga_polygon> to_polygons() { return _polygons; };

	ga_csg add(ga_csg& other);
	//ga_csg subtract(ga_csg& other);
	//ga_csg intersect(ga_csg& other);

	ga_csg Cube(ga_vec3f radius = { 1,1,1 },
				ga_vec3f center = { 0,0,0 });
	//ga_csg Sphere();
	//ga_csg Pyramid();

	void set_color(ga_vec3f& col) { _color = col; };


private:
	uint32_t make_vao(GLsizei& index_count);
	GLsizei get_index_count();
	class ga_material* _material;
	uint32_t _vao;
	GLsizei _index_count;
	uint32_t _vbos[4];
	ga_vec3f _color;
	std::vector<ga_polygon> _polygons;

	friend class ga_csg_component;
};

#endif