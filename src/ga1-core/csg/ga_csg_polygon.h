
#ifndef GA_POLYGON_H
#define GA_POLYGON_H

/*
** RPI Game Architecture Engine
**
** Portions adapted from:
** Viper Engine - Copyright (C) 2016 Velan Studios - All Rights Reserved
** Evan Wallace - CSG.js - https://github.com/evanw/csg.js
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include "math/ga_vec3f.h"
#include "ga_plane.h"
#include "ga_csg_vertex.h"
#include <GL/glew.h>

#include <string>
#include <vector>

/*
** A polygon data structure for CSG
*/
class ga_polygon
{
public:
	ga_polygon();
	ga_polygon(std::vector<ga_csg_vertex>& verts);
	ga_polygon(std::vector<ga_csg_vertex>& verts, std::vector<ga_vec3f>& shared);
	ga_polygon(const ga_polygon& other);
	void flip();
	ga_polygon flipped();
	~ga_polygon();

	void get_vbo_info(std::vector<ga_vec3f>& verts,
					  std::vector<ga_vec3f>& normals,
					  std::vector<GLushort>& indices,
					  std::vector<ga_vec3f>& colors,
					  const ga_vec3f col);

	bool isTri() { return _vertices.size() == 3; };
	bool isQuad() { return _vertices.size() == 4; };

	std::vector<ga_csg_vertex> _vertices;
	std::vector<ga_vec3f> _shared; //TODO: What structure is this a std::vector of...
	ga_csg_plane _plane;
};

void split_polygon(ga_csg_plane& plane,
					ga_polygon& polygon,
					std::vector<ga_polygon>& coplanar_front,
					std::vector<ga_polygon>& coplanar_back,
					std::vector<ga_polygon>& front,
					std::vector<ga_polygon>& back);

#endif