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

ga_font* g_font = nullptr;
static void set_root_path(const char* exepath);
static void gui_test(ga_frame_params* params, ga_entity* ent);
std::vector<ga_csg_component*> csg_objs;
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
	csg_COMBINE2.set_pos({ 6.0f,1.0f,0.0f });
	csg_COMBINE.set_pos({ -1.0f,1.0f,0.0f });
	csg_cube1.set_pos({ -4.0f,1.0f,0.0f });
	csg_cube2.set_pos({ -3.5f,1.5f,0.0f });
	csg_cube3.set_pos({ 3.0f,1.25f,0.5f });
	sim->add_entity(&csg_test_COMBO2);


	ga_entity my_csg;
	ga_csg_component csg1(&my_csg, ga_csg::Shape::CUBE);
	csg1.set_pos({ 0.0f,0.0f,4.0f });
	csg_objs.push_back(&csg1);
	sim->add_entity(&my_csg);

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
		gui_test(&params, &my_csg);

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

static void gui_test(ga_frame_params* params, ga_entity* ent)
{

	// Variable initializations 
	ga_csg_component* selected = (selected_index >= 0) ? csg_objs[selected_index] : NULL;
	ga_csg_component* selected2 = (selected_index_2 >= 0) ? csg_objs[selected_index_2] : NULL;
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
	for (int i = 0; i < csg_objs.size(); i++) {
		ga_button temp = ga_button(std::to_string(i).c_str(), 20.0f + i *25.0f, 80.0f, params);

		// if hovered, highlight and inform user
		if (temp.get_hover(params))
		{
			hovered = csg_objs[i]->name;
			// highlight the actual selected object via shader
			csg_objs[i]->get_csg()->get_material()->set_highlight(true);
		}
		else {
			// unhighlight self if not being hovered
			csg_objs[i]->get_csg()->get_material()->set_highlight(false);
		}

		// if clicked, check what kind of click
		if (temp.get_clicked(params))
		{
			// if right click
			if (params->_mouse_click_mask > 2) {
				selected_index_2 = i;
				selected2 = csg_objs[i];
				std::cout << "FIr!!ed " << i << " selected: " << selected_index  << " selected2: " << selected_index_2<< std::endl;
			}
			// if left mouse click
			else {
				selected_index = i;
				selected = csg_objs[i];
				std::cout<<"FIred " << i << " selected: " << selected_index << " selected2: " << selected_index_2 << std::endl;
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
		csg_objs.push_back(new ga_csg_component(ent, ga_csg::Shape::CUBE, { xpos,ypos,zpos }));
	}
	if (add_pyramid_button.get_clicked(params))
	{
		float xpos = fmodf(rand(), 5.0f);
		float ypos = fmodf(rand(), 5.0f);
		float zpos = fmodf(rand(), 5.0f);
		csg_objs.push_back(new ga_csg_component(ent, ga_csg::Shape::PYRAMID, { xpos,ypos,zpos }));
	}

	// if Primary Object is selected
	if (selected_index >= 0) {
		ga_label(("Object #1 Selected: " + selected->name + std::to_string(selected_index)).c_str(), 10, 120.0f, params);
		selected->get_csg()->get_material()->set_highlight(true);


		ga_label scale_label = ga_label(("Scale " + selected->name).c_str(), 20.0f, 150.0f, params);
		ga_label extrd_label = ga_label(("Extrude " + selected->name).c_str(), 20.0f, 200.0f, params);
		ga_label trans_label = ga_label(("Translate " + selected->name).c_str(), 20.0f, 250.0f, params);
		ga_panel inspector = ga_panel(params, { 15,130 }, { 200, 280 });


		// Scale Functions
		ga_vec3f current;
		for (int i = 0; i < mods.size(); i++) {
			if (ga_button(mods[i], 20.0f + i*30.0f, 170.0f, params).get_pressed(params))
			{
				current = selected->get_csg()->get_transform().get_scale();
				current += mod_dirs[i].scale_result(amt);
				selected->set_scale(current);
			}
		}
		// Extrude Functions
		for (int i = 0; i < mods.size(); i++) {
			if (ga_button(mods[i], 20.0f + i*30.0f, 220.0f, params).get_pressed(params))
			{
				selected->do_extrude(mod_dirs[i], 1+amt);
			}
		}
		// Move Functions
		for (int i = 0; i < mods.size(); i++) {
			if (ga_button(mods[i], 20.0f + i*30.0f, 270.0f, params).get_pressed(params))
			{
				current = selected->get_csg()->get_transform().get_translation();
				current += mod_dirs[i].scale_result(amt);
				selected->set_pos(current);
			}
		}
	}
	// if Secondary Object is selected
	if (selected_index_2 >= 0 && selected_index_2 != selected_index) {
		ga_label(("Object #2 Selected: " + selected2->name + std::to_string(selected_index_2)).c_str(), 10, 300, params);
		selected2->get_csg()->get_material()->set_highlight(true);


		ga_label scale_label = ga_label(("Scale " + selected->name).c_str(), 20.0f, 330.0f, params);
		ga_label extrd_label = ga_label(("Extrude " + selected->name).c_str(), 20.0f, 380.0f, params);
		ga_label trans_label = ga_label(("Translate " + selected->name).c_str(), 20.0f, 430.0f, params);
		ga_panel inspector = ga_panel(params, { 15,310 }, { 200, 460 });


		// Scale Functions
		ga_vec3f current;
		for (int i = 0; i < mods.size(); i++) {
			if (ga_button(mods[i], 20.0f + i * 30.0f, 350.0f, params).get_pressed(params))
			{
				current = selected2->get_csg()->get_transform().get_scale();
				current += mod_dirs[i].scale_result(amt);
				selected2->set_scale(current);
			}
		}
		// Extrude Functions
		for (int i = 0; i < mods.size(); i++) {
			if (ga_button(mods[i], 20.0f + i * 30.0f, 400.0f, params).get_pressed(params))
			{
				selected2->do_extrude(mod_dirs[i], 1+amt);
			}
		}
		// Move Functions
		for (int i = 0; i < mods.size(); i++) {
			if (ga_button(mods[i], 20.0f + i * 30.0f, 450.0f, params).get_pressed(params))
			{
				current = selected2->get_csg()->get_transform().get_translation();
				current += mod_dirs[i].scale_result(amt);
				selected2->set_pos(current);
			}
		}
	}

	// If there is a selection on both fronts, allow union, sub, and intersect operations
	if (!(selected_index >= 0 && selected_index_2 >= 0 && selected_index_2 != selected_index)) return;

	// TODO: Implement Add, Subtract, and Intersect options here
	if (ga_button("Union Operation!", 20.0f, 600.0f, params).get_clicked(params)) {
		std::cout << "Got Here" << std::endl;
	}
}
