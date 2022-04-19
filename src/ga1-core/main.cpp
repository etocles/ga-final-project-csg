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

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#define STB_TRUETYPE_IMPLEMENTATION
#include <stb_truetype.h>

#if defined(GA_MINGW)
#include <unistd.h>
#endif

ga_font* g_font = nullptr;
static void set_root_path(const char* exepath);
static void gui_test(ga_frame_params* params, ga_entity* ent, ga_csg_component& ref);
ga_vec3f current_scale = { 1.0f,1.0f,1.0f };
ga_csg_component* temp;

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
	ga_csg_component csg_cube2(&csg_test_2, ga_csg::Shape::CUBE, {1.0f,1.5f,0.0f}, { 1.0f,0.0f,0.0f });
	sim->add_entity(&csg_test_2);

	// Create a CSG entity 3
	ga_entity csg_test_3;
	ga_csg_component csg_cube3(&csg_test_3, ga_csg::Shape::CUBE, { 0.5f,1.0f,1.0f }, { 0.0f,1.0f,0.0f });
	sim->add_entity(&csg_test_3);

	// Create a combination of the two
	ga_entity csg_test_COMBO;
	ga_csg_component csg_COMBINE(&csg_test_COMBO, *csg_cube1.get_csg(), *csg_cube2.get_csg());
	sim->add_entity(&csg_test_COMBO);

	ga_entity csg_test_COMBO2;
	ga_csg_component csg_COMBINE2(&csg_test_COMBO2, *csg_COMBINE.get_csg(), *csg_cube3.get_csg());
	csg_COMBINE2.set_pos({ 6.0f,1.0f,0.0f });
	csg_COMBINE.set_pos({ -1.0f,1.0f,0.0f });
	csg_cube1.set_pos({ -4.5f,1.0f,0.0f });
	csg_cube2.set_pos({ -3.5f,2.5f,0.0f });
	csg_cube3.set_pos({ 3.0f,2.0f,1.0f });
	sim->add_entity(&csg_test_COMBO2);


	ga_entity my_csg;
	ga_csg_component csg1(&my_csg, ga_csg::Shape::CUBE);
	csg1.set_pos({ 0.0f,0.0f,4.0f });
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
		gui_test(&params, &my_csg, csg_cube1);

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

static void gui_test(ga_frame_params* params, ga_entity* ent, ga_csg_component& ref)
{
	ga_label("CSG Demo", 10, 20, params);


	if (ga_button("Extrude Cube1", 20.0f, 100.0f, params).get_clicked(params))
	{
		current_scale += {0.1f, 0.0f, 0.0f};
		ref.set_scale(current_scale);
	}
	if (ga_button("Add a cube", 20.0f, 150.0f, params).get_clicked(params))
	{
		temp = new ga_csg_component(ent, ga_csg::Shape::CUBE, { 2.0f,0.0f,4.0f });
	}
	//if (ga_button("Show Model 2", 20.0f, 300.0f, params).get_clicked(params))
	//{
	//	//cube.set_active(false);
	//	//model.set_active(false);
	//	//model2.set_active(true);
	//}
}
