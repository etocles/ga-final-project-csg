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



/// <summary>
/// An abstract representation of CSG, complete with the Union, Subtract, and Intersect operations
/// As well as modifications in space, such as translation, scale, and extrude.
/// Uses VAOs as well as shaders to draw to the screen.
/// Has name and color attributes as commodities. 
/// </summary>
class ga_csg
{
public:
	static enum class Shape { CUBE, SPHERE, PYRAMID };
	static enum class OP { ADD, SUB, INTERSECT};

	/// <summary>
	/// Creates an instance of the ga_csg class, colored white, resembling the provided shape enum
	/// Sets name to the name of the primitive
	/// </summary>
	/// <param name="shp"> The primitive shape to be created </param>
	ga_csg(Shape shp);

	/// <summary>
	/// Creates an instance of the ga_csg class with the same properties as another (color, polygons)
	/// Sets name to the same name of the csg being copied
	/// </summary>
	/// <param name="other"> The ga_csg instance to be duplicated </param>
	ga_csg(ga_csg& other);

	/// <summary>
	/// Creates an instance of the ga_csg class, colored white, from the specified polygons
	/// Sets name to "Poly"
	/// </summary>
	/// <param name="polys"> A vector of polygons which create a mesh </param>
	ga_csg(std::vector<ga_polygon*>& polys);
	~ga_csg() {
		glDeleteVertexArrays(1, (GLuint*)&_vao);
		glDeleteBuffers(3, _vbos);
	};

	
	/// <summary>
	/// Retrieve the CSG's polygons as they appear in unit-space
	/// </summary>
	/// <returns> Vector of polygons of the CSG centered at the origin, without transformations or scaling applied </returns>
	std::vector<ga_polygon*> get_polygons_raw() { 
		std::vector<ga_polygon*> temp;
		for (ga_polygon p : _polygons) temp.push_back(new ga_polygon(p));
		return temp;
	};

	/// <summary>
	/// Retrieve a certain CSG object's polygons with transformations
	/// VERY computationally and memory intensive operation.
	/// </summary>
	/// <returns> Vector of polygons of the CSG with transformations and scalings applied </returns>
	std::vector<ga_polygon*> get_polygons() {
		std::vector<ga_polygon*> res;
		for (int i = 0; i < _polygons.size(); i++) {
			std::vector<ga_csg_vertex> temp_verts;
			for (int j = 0; j < _polygons[i]._vertices.size(); j++) {
				ga_vec3f old_pos = _polygons[i]._vertices[j]._pos;
				ga_vec4f temp = { old_pos.x, old_pos.y, old_pos.z, 1.0f }; // might need to be 0.0
				temp = _transform.transform(temp);
				ga_vec3f new_pos = { temp.x, temp.y, temp.z };
				temp_verts.push_back(ga_csg_vertex(new_pos, _polygons[i]._vertices[j]._normal));
			}
			res.push_back(new ga_polygon(temp_verts));
		}
		return res;
	}

	/// <summary>
	/// Performs the Add operation on two CSG objects also represented 
	/// in the the form this + other.
	/// </summary>
	/// <param name="other"> The other csg to perform union with </param>
	/// <returns> A new csg which has polygons of both csgs </returns>
	ga_csg add(ga_csg& other);
	/// <summary>
	/// Performs the Subtract operation on two CSG objects also represented 
	/// in the the form this - other.
	/// </summary>
	/// <param name="other"> The other csg to perform union with </param>
	/// <returns> A new csg which has polygons of both csgs </returns>
	ga_csg subtract(ga_csg& other);
	/// <summary>
	/// Performs the Intersect operation on two CSG objects also represented 
	///	in the the form this XOR other.
	/// </summary>
	/// <param name="other"> The other csg to perform union with </param>
	/// <returns> A new csg which has polygons of both csgs </returns>
	ga_csg intersect(ga_csg& other);

	/// <summary>
	/// Creates a primitive unit length cube centered at the origin
	/// </summary>
	/// <returns> A csg instance that of a cube centered at origin </returns>
	ga_csg static Cube();
	/// <summary>
	/// Creates a primitive unit length sphere centered at the origin
	/// </summary>
	/// <returns> A csg instance that of a sphere centered at origin </returns>
	ga_csg static Sphere();
	/// <summary>
	/// Creates a primitive unit length pyramid centered at the origin
	/// </summary>
	/// <returns> A csg instance that of a pyramid centered at origin </returns>
	ga_csg static Pyramid();
	/// <summary>
	/// Sets a new color for the csg
	/// </summary>
	/// <param name="col"> The new color to change to, following the format {r,g,b} </param>
	void set_color(ga_vec3f col) { _color = col; _material->set_color(col); };
	/// <summary>
	/// Sets a new position for the csg
	/// </summary>
	/// <param name="col"> A position in 3D space, following the format {x,y,z} </param>
	void set_pos(ga_vec3f t);
	/// <summary>
	/// Sets a new scale for the csg
	/// </summary>
	/// <param name="col"> The new scale of the object, following the format {x,y,z} </param>
	void set_scale(ga_vec3f t);
	/// <summary>
	/// Extrudes an object in the specified direction (dir) an amount (amt)
	/// </summary>
	/// <param name="dir">
	/// The direction, or, "face" to extrude.
	/// </param>
	/// <param name="amt"> 
	/// A value greater than 0, which resembles how much to extrude by
	/// 0 < amt < 1 will extrude the object inwards by amt*100 percent
	/// 1 < amt will extrude the object's face outwards by (1-amt)*100 percent
	/// </param>
	/// <remarks>
	/// For example, calling extrude with the parameters (0,1,0) and (1.2) will make the
	/// top of the object extend, leaving it unchanged in 3D space.
	/// Calling extrude with the parameters (0,1,0) and (0.8) will instead make the top of the object 
	/// extrude inwards.
	/// </remarks>
	void extrude(ga_vec3f dir, float amt);
	/// <summary>
	/// Obtain the transform matrix this object uses to appear in 3D space
	/// </summary>
	/// <returns> A 4D matrix of floats representing translation and scale </returns>
	ga_mat4f get_transform() { return _transform; };
	/// <summary>
	/// Obtain the material of the object for modification
	/// </summary>
	/// <returns> A pointer to the material attached to this csg </returns>
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