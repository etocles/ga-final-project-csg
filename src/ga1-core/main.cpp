/*
** RPI Game Architecture Engine
**
** Portions adapted from:
** Viper Engine - Copyright (C) 2016 Velan Studios - All Rights Reserved
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include "framework/ga_camera.h"
#include "framework/ga_compiler_defines.h"
#include "framework/ga_input.h"
#include "framework/ga_sim.h"
#include "framework/ga_output.h"
#include "jobs/ga_job.h"

#include "entity/ga_entity.h"
#include "entity/ga_lua_component.h"

#include "graphics/ga_cube_component.h"
#include "graphics/ga_program.h"

#include "csg/ga_csg_component.h"

#include "physics/ga_physics_component.h"
#include "physics/ga_shape.h"

#include "gui/ga_font.h"
#include "gui/ga_label.h"
#include "gui/ga_panel.h"
#include "gui/ga_button.h"
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#define STB_TRUETYPE_IMPLEMENTATION
#include <stb_truetype.h>

#if defined(GA_MINGW)
#include <unistd.h>
#endif

ga_font* g_font = nullptr; // general font (WHITE)
static void set_root_path(const char* exepath);
static void gui_test(ga_frame_params* params, ga_csg_component& ent);
int selected_index = -1;
int selected_index_2 = -1;

int main(int argc, const char** argv)
{
	set_root_path(argv[0]);

	ga_job::startup(0xffff, 256, 256);

	// Create objects for three phases of the frame: input, sim and output.
	ga_input* input = new ga_input();
	ga_sim* sim = new ga_sim();
	ga_output* output = new ga_output(input->get_window());

	// Create the default font:
	g_font = new ga_font("VeraMono.ttf", 16.0f, 512, 512);

	// Create camera.
	ga_camera* camera = new ga_camera({ 0.0f, 7.0f, 20.0f });
	ga_quatf rotation;
	rotation.make_axis_angle(ga_vec3f::y_vector(), ga_degrees_to_radians(180.0f));
	camera->rotate(rotation);
	rotation.make_axis_angle(ga_vec3f::x_vector(), ga_degrees_to_radians(15.0f));
	camera->rotate(rotation);

	// Create a CSG entity
	ga_entity csg_test_1;
	ga_csg_component csg_cube1(&csg_test_1, ga_csg::Shape::CUBE);
	sim->add_entity(&csg_test_1);

	// Create a CSG entity 2
	ga_entity csg_test_2;
	ga_csg_component csg_cube2(&csg_test_2, ga_csg::Shape::CUBE, {0.5f,0.5f,0.0f}, { 1.0f,0.0f,0.0f });
	sim->add_entity(&csg_test_2);

	// Create a CSG entity 3
	ga_entity csg_test_3;
	ga_csg_component csg_cube3(&csg_test_3, ga_csg::Shape::CUBE, { 0.25f,0.25f,0.5f }, { 0.0f,1.0f,0.0f });
	sim->add_entity(&csg_test_3);

	// Create a combination of the two
	ga_entity csg_test_COMBO;
	ga_csg_component csg_COMBINE(&csg_test_COMBO, *csg_cube1.get_csg(), *csg_cube2.get_csg());
	sim->add_entity(&csg_test_COMBO);

	ga_entity csg_test_COMBO2;
	ga_csg_component csg_COMBINE2(&csg_test_COMBO2, *csg_COMBINE.get_csg(), *csg_cube3.get_csg());
	csg_COMBINE2.get_csg()->set_pos({ 6.0f,1.0f,0.0f });
	csg_COMBINE.get_csg()->set_pos({ -1.0f,1.0f,0.0f });
	csg_cube1.get_csg()->set_pos({ -4.0f,1.0f,0.0f });
	csg_cube2.get_csg()->set_pos({ -3.5f,1.5f,0.0f });
	csg_cube3.get_csg()->set_pos({ 3.0f,1.25f,0.5f });
	sim->add_entity(&csg_test_COMBO2);


	ga_entity my_csg_entity;
	ga_csg_component my_csg(&my_csg_entity, ga_csg::Shape::CUBE);
	sim->add_entity(&my_csg_entity);
	my_csg.get_csg()->set_pos({ 0.0f,0.0f,4.0f });
	my_csg.get_csg()->name = "Cube";
	my_csg.get_csg()->id = my_csg.get_id();

	ga_entity floor;
	ga_plane floor_plane;
	ga_physics_component floor_collider(&floor, &floor_plane, 0.0f);
	floor_plane._point = { 0.0f, -1.0f, 0.0f };
	floor_plane._normal = { 0.0f, 1.0f, 0.0f };
	sim->add_entity(&floor);

	// Main loop:
	while (true)
	{
		// We pass frame state through the 3 phases using a params object.
		ga_frame_params params;

		// Gather user input and current time.
		if (!input->update(&params))
		{
			break;
		}

		// Update the camera.
		camera->update(&params);

		// Run gameplay.
		sim->update(&params);

		// Perform the late update.
		sim->late_update(&params);

		// Run gui test.
		gui_test(&params, my_csg);

		// Draw to screen.
		output->update(&params);
	}

	delete output;
	delete sim;
	delete input;
	delete camera;

	ga_job::shutdown();

	return 0;
}

char g_root_path[256];
static void set_root_path(const char* exepath)
{
#if defined(GA_MSVC)
	strcpy_s(g_root_path, sizeof(g_root_path), exepath);

	// Strip the executable file name off the end of the path:
	char* slash = strrchr(g_root_path, '\\');
	if (!slash)
	{
		slash = strrchr(g_root_path, '/');
	}
	if (slash)
	{
		slash[1] = '\0';
	}
#elif defined(GA_MINGW)
	char* cwd;
	char buf[PATH_MAX + 1];
	cwd = getcwd(buf, PATH_MAX + 1);
	strcpy_s(g_root_path, sizeof(g_root_path), cwd);

	g_root_path[strlen(cwd)] = '/';
	g_root_path[strlen(cwd) + 1] = '\0';
#endif
}

static void gui_test(ga_frame_params* params, ga_csg_component& comp)
{

	// Variable initializations 
	ga_csg* selected = (selected_index >= 0) ? comp.get_csg(selected_index) : NULL;
	ga_csg* selected2 = (selected_index_2 >= 0) ? comp.get_csg(selected_index_2) : NULL;
	std::string hovered = "NONE";
	std::vector<const char*> mods = { "+x","-x","+y","-y","+z","-z" };
	std::vector<ga_vec3f> mod_dirs = { {1.0f,0.0f,0.0f}, //"+x",
										{-1.0f,0.0f,0.0f}, //"-x",
										{0.0f,1.0f,0.0f}, //"+y",
										{0.0f,-1.0f,0.0f}, //"-y",
										{0.0f,0.0f,1.0f}, //"+z",
										{0.0f,0.0f,-1.0f} //"-z" 
									};
	const float amt = 0.1f;



	// GUI Component initializations
	ga_label title =  ga_label("CSG Demo", 10, 20, params);
	ga_button add_cube_button = ga_button("Add Cube", 20.0f, 700.0f, params);
	ga_button add_pyramid_button = ga_button("Add Pyramid", 100.0f, 700.0f, params);
	// SHOW ALL OF THE OBJECTS
	for (int i = 0; i < comp.size(); i++) {
		ga_csg* cur = comp.get_csg(i);
		ga_button temp = ga_button(std::to_string(cur->id).c_str(), 20.0f + i *25.0f, 80.0f, params);
		cur->get_material()->set_highlight(false);
		cur->get_material()->set_selected(false);
		// if hovered, highlight and inform user
		if (temp.get_hover(params))
		{
			hovered = comp.get_csg(i)->name;
			// highlight the actual selected object via shader
			cur->get_material()->set_highlight(true);
		}

		// if clicked, check what kind of click
		if (temp.get_clicked(params))
		{
			// if right click
			if (params->_mouse_click_mask > 2) {
				selected_index_2 = i;
				selected2 = comp.get_csg(selected_index_2);
				//std::cout << "FIr!!ed " << i << " selected: " << selected_index  << " selected2: " << selected_index_2<< std::endl;
			}
			// if left mouse click
			else {
				selected_index = i;
				selected = comp.get_csg(selected_index);
				//std::cout<<"FIred " << i << " selected: " << selected_index << " selected2: " << selected_index_2 << std::endl;
			}
		}
	}
	
	// DISPLAY CSG OBJECT HEADER 
	if (hovered != "NONE") ga_label(("CSG Objs (" + hovered + ")").c_str(), 10, 50, params);
	else ga_label("CSG Objs", 10, 50, params);


	// ALWAYS MAINTAIN FUNCTIONALITY TO ADD MORE OBJECTS
	if (add_cube_button.get_clicked(params))
	{
		float xpos = fmodf(rand(), 5.0f);
		float ypos = fmodf(rand(), 5.0f);
		float zpos = fmodf(rand(), 5.0f);
		ga_csg* temp = new ga_csg(ga_csg::Shape::CUBE);
		temp->set_pos({ xpos,ypos,zpos });
		temp->name = "Cube";
		temp->id = comp.get_id();
		comp.add(temp);
	}
	if (add_pyramid_button.get_clicked(params))
	{
		float xpos = fmodf(rand(), 5.0f);
		float ypos = fmodf(rand(), 5.0f);
		float zpos = fmodf(rand(), 5.0f);
		ga_csg* temp = new ga_csg(ga_csg::Shape::PYRAMID);
		temp->set_pos({ xpos,ypos,zpos });
		temp->name = "Pyramid";
		temp->id = comp.get_id();
		comp.add(temp);
	}

	// if Primary Object is selected
	if (selected_index >= 0) {
		g_font->print(params,
			("Object #1 Selected: " + selected->name + std::to_string(selected->id)).c_str(),
			10,
			120,
			{ 0.4,0.4,0.9 });
		selected->get_material()->set_selected(true);
		selected->get_material()->set_secondary(false);


		ga_label scale_label = ga_label("Scale", 20.0f, 150.0f, params);
		ga_label extrd_label = ga_label("Extrude", 20.0f, 200.0f, params);
		ga_label out_dir_label = ga_label("Out:", 20.0f, 220.0f, params);
		ga_label in_dir_label = ga_label("In: ", 20.0f, 240.0f, params);
		ga_label trans_label = ga_label("Translate", 20.0f, 260.0f, params);
		ga_panel inspector = ga_panel(params, { 15,135 }, { 250, 295 });

		// Remove the object from the vector
		if (ga_button("x", 230, 150, params, {0.8f, 0.2f, 0.2f}).get_clicked(params)) {
			comp.remove(selected_index);
			selected_index = -1;
			return;
		}


		// Scale Functions
		ga_vec3f current;
		for (int i = 0; i < mods.size(); i++) {
			if (ga_button(mods[i], 25.0f + i*30.0f, 170.0f, params).get_pressed(params))
			{
				current = selected->get_transform().get_scale();
				current += mod_dirs[i].scale_result(amt);
				selected->set_scale(current);
			}
		}
		// Extrude Functions (OUT)
		for (int i = 0; i < mods.size(); i++) {
			if (ga_button(mods[i], 60.0f + i*30.0f, 220.0f, params).get_pressed(params))
			{
				float temp = (mod_dirs[i].dot({ 1.0f,1.0f,1.0f }) > 0) ? 1 + amt : 1 - amt;
				selected->extrude(mod_dirs[i], temp);
			}
		}

		// Extrude Functions (IN)
		for (int i = 0; i < mods.size(); i++) {
			if (ga_button(mods[i], 60.0f + i * 30.0f, 240.0f, params).get_pressed(params))
			{
				float temp = (mod_dirs[i].dot({1.0f,1.0f,1.0f}) < 0) ? 1 + amt : 1 - amt;
				selected->extrude(mod_dirs[i], temp);
			}
		}
		// Move Functions
		for (int i = 0; i < mods.size(); i++) {
			if (ga_button(mods[i], 25.0f + i*30.0f, 280.0f, params).get_pressed(params))
			{
				current = selected->get_transform().get_translation();
				current += mod_dirs[i].scale_result(amt);
				selected->set_pos(current);
			}
		}
	}
	// if Secondary Object is selected
	if (selected_index_2 >= 0 && selected_index_2 != selected_index) {
		//ga_label(, 10, 320, params);
		g_font->print(params, 
			("Object #2 Selected: " + selected2->name + std::to_string(selected2->id)).c_str(),
			10, 
			320, 
			{ 0.9,0.4,0.4 });
		selected2->get_material()->set_selected(true);
		selected2->get_material()->set_secondary(true);

		ga_label scale_label = ga_label("Scale", 20.0f, 350.0f, params);
		ga_label extrd_label = ga_label("Extrude", 20.0f, 400.0f, params);
		ga_label out_dir_label = ga_label("Out:", 20.0f, 420.0f, params);
		ga_label in_dir_label = ga_label("In: ", 20.0f, 440.0f, params);
		ga_label trans_label = ga_label("Translate", 20.0f, 460.0f, params);
		ga_panel inspector = ga_panel(params, { 15,335 }, { 250, 495 });

		// Remove the object from the vector
		if (ga_button("x", 230, 350, params, { 0.8f, 0.2f, 0.2f }).get_clicked(params)) {
			comp.remove(selected_index_2);
			selected_index_2 = -1;
			return;
		}
		// Scale Functions
		ga_vec3f current;
		for (int i = 0; i < mods.size(); i++) {
			if (ga_button(mods[i], 25.0f + i * 30.0f, 370.0f, params).get_pressed(params))
			{
				current = selected2->get_transform().get_scale();
				current += mod_dirs[i].scale_result(amt);
				selected2->set_scale(current);
			}
		}

		//// Extrude Functions
		//for (int i = 0; i < mods.size(); i++) {
		//	if (ga_button(mods[i], 25.0f + i * 30.0f, 420.0f, params).get_pressed(params))
		//	{
		//		selected2->extrude(mod_dirs[i], 1+amt);
		//	}
		//}


		// Extrude Functions (OUT)
		for (int i = 0; i < mods.size(); i++) {
			if (ga_button(mods[i], 60.0f + i * 30.0f, 420.0f, params).get_pressed(params))
			{
				float temp = (mod_dirs[i].dot({ 1.0f,1.0f,1.0f }) > 0) ? 1 + amt : 1 - amt;
				selected2->extrude(mod_dirs[i], temp);
			}
		}

		// Extrude Functions (IN)
		for (int i = 0; i < mods.size(); i++) {
			if (ga_button(mods[i], 60.0f + i * 30.0f, 440.0f, params).get_pressed(params))
			{
				float temp = (mod_dirs[i].dot({ 1.0f,1.0f,1.0f }) < 0) ? 1 + amt : 1 - amt;
				selected2->extrude(mod_dirs[i], temp);
			}
		}

		// Move Functions
		for (int i = 0; i < mods.size(); i++) {
			if (ga_button(mods[i], 25.0f + i * 30.0f, 480.0f, params).get_pressed(params))
			{
				current = selected2->get_transform().get_translation();
				current += mod_dirs[i].scale_result(amt);
				selected2->set_pos(current);
			}
		}
	}

	// If there is a selection on both fronts, allow union, sub, and intersect operations
	if (!(selected_index >= 0 && selected_index_2 >= 0 && selected_index_2 != selected_index)) return;

	
	ga_label("Operations", 20, 580, params);
	ga_panel inspector = ga_panel(params, { 15,590 }, { 240, 620 });
	ga_button union_button = ga_button("Union", 20.0f, 610.0f, params);
	ga_button sub_button = ga_button("Subtract", 75.0f, 610.0f, params);
	ga_button intersect_button = ga_button("Intersect", 155.0f, 610.0f, params);
	if (union_button.get_clicked(params)) {
		ga_csg* temp = new ga_csg(selected->add(*selected2));
		std::string name1 = (selected->name[0] == '(') ? "(" + selected->name : "(" + selected->name + std::to_string(selected->id);
		std::string name2 = (selected2->name[0] == '(') ? selected2->name + ")" : selected2->name + std::to_string(selected2->id) + ")";
		temp->name = name1 + "+" + name2;
		temp->id = comp.get_id();
		comp.add(temp);
	}
	if (sub_button.get_clicked(params)) {
		ga_csg* temp = new ga_csg(selected->subtract(*selected2));
		std::string name1 = (selected->name[0] == '(') ? "(" + selected->name : "(" + selected->name + std::to_string(selected->id);
		std::string name2 = (selected2->name[0] == '(') ? selected2->name + ")" : selected2->name + std::to_string(selected2->id) + ")";
		temp->name = name1 + "-" + name2;
		temp->id = comp.get_id();
		comp.add(temp);
	}
	if (intersect_button.get_clicked(params)) {
		ga_csg* temp = new ga_csg(selected->subtract(*selected2));
		std::string name1 = (selected->name[0] == '(') ? "(" + selected->name : "(" + selected->name + std::to_string(selected->id);
		std::string name2 = (selected2->name[0] == '(') ? selected2->name + ")" : selected2->name + std::to_string(selected2->id) + ")"; 
		temp->name = name1 + "x" + name2;
		temp->id = comp.get_id();
		comp.add(temp);
	}
}
