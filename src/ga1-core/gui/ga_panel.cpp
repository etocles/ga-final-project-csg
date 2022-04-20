/*
** RPI Game Architecture Engine
**
** Portions adapted from:
** Viper Engine - Copyright (C) 2016 Velan Studios - All Rights Reserved
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include "ga_panel.h"
#include "ga_font.h"

#include "framework/ga_frame_params.h"

ga_panel::ga_panel(struct ga_frame_params* params, ga_vec2f min, ga_vec2f max)
{
	draw_outline(params, min, max, k_button_color, k_button_offset);
}

ga_panel::~ga_panel()
{
}
