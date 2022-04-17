

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

	// TODO: Propogate these important assemblies into each constructor
	ga_csg();
	ga_csg(Shape shp);
	ga_csg(ga_csg& other);;
	ga_csg(std::vector<ga_polygon>& polys);;
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

	ga_csg static Cube(ga_vec3f radius = { 1,1,1 },
				ga_vec3f center = { 0,0,0 });
	//ga_csg Sphere();
	//ga_csg Pyramid();

	void set_color(ga_vec3f& col) { _color = col; _material->set_color(col); };
	void translate(ga_vec3f& t); // TODO: Needed? YES NEEDED
	void extrude(ga_vec3f& t); // TODO: Implement


private:
	uint32_t make_vao(GLsizei& index_count);
	void default_values();
	class ga_material* _material;
	uint32_t _vao;
	GLsizei _index_count;
	uint32_t _vbos[3];
	ga_vec3f _color;
	std::vector<ga_polygon> _polygons;

	friend class ga_csg_component;
};

#endif