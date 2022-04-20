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
ga_vec3f current_scale = { 1.0f,1.0f,1.0f };
std::vector<ga_csg_component*> csg_objs;
int selected_index = -1;

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
	ga_label("CSG Demo", 10, 20, params);

	// initializations
	ga_csg_component* selected = (selected_index >= 0) ? csg_objs[selected_index] : NULL;
	std::string hovered = "NONE";

	// SHOW ALL OF THE OBJECTS
	for (int i = 0; i < csg_objs.size(); i++) {
		ga_button temp = ga_button(std::to_string(i).c_str(), 20.0f + i *25.0f, 80.0f, params);
		if (temp.get_hover(params))
		{
			hovered = "#" + std::to_string(i);
		}
		if (temp.get_clicked(params))
		{
			selected_index = i;
			selected = csg_objs[i];
		}
	}
	
	// DISPLAY CSG OBJECT HEADER 
	if (hovered != "NONE") ga_label(("CSG Objs (" + hovered + ")").c_str(), 10, 50, params);
	else ga_label("CSG Objs", 10, 50, params);



	// ALWAYS MAINTAIN FUNCTIONALITY TO ADD MORE OBJECTS
	if (ga_button("Add a cube", 20.0f, 350.0f, params).get_clicked(params))
	{
		float xpos = fmodf(rand(), 5.0f);
		float ypos = fmodf(rand(), 5.0f);
		float zpos = fmodf(rand(), 5.0f);
		csg_objs.push_back(new ga_csg_component(ent, ga_csg::Shape::CUBE, { xpos,ypos,zpos }));
	}if (ga_button("Add a pyramid", 20.0f, 400.0f, params).get_clicked(params))
	{
		float xpos = fmodf(rand(), 5.0f);
		float ypos = fmodf(rand(), 5.0f);
		float zpos = fmodf(rand(), 5.0f);
		csg_objs.push_back(new ga_csg_component(ent, ga_csg::Shape::PYRAMID, { xpos,ypos,zpos }));
	}

	if (selected_index < 0) return;
	ga_label(("Object Selected: " + selected->name).c_str(), 10, 120, params);

	if (ga_button(("Scale " + selected->name).c_str(), 20.0f, 150.0f, params).get_clicked(params))
	{
		current_scale += {0.1f, 0.0f, 0.0f};
		selected->set_scale(current_scale);
	}
	if (ga_button(("Extrude " + selected->name).c_str(), 20.0f, 200.0f, params).get_clicked(params))
	{
		/*current_scale += {0.1f, 0.0f, 0.0f};
		ref.set_scale(current_scale);*/

		selected->do_extrude({ 1.0f,0.0f,0.0f }, 3.0f);
	/*	ga_vec3f cur_pos = ref.get_csg()->_transform.get_translation();
		ga_vec3f cur_scale = { ref.get_csg()->_transform.data[0][0],
							   ref.get_csg()->_transform.data[1][1],
							   ref.get_csg()->_transform.data[2][2] };
		std::cout << "Current Pos: "
			<< "\n\tx : " << cur_pos.x
			<< "\n\ty : " << cur_pos.y
			<< "\n\tz : " << cur_pos.z << std::endl
			<< "Current Scale: "
			<< "\n\tx : " << cur_scale.x
			<< "\n\ty : " << cur_scale.y
			<< "\n\tz : " << cur_scale.z << std::endl << std::endl;*/
 			
	}
	
}
