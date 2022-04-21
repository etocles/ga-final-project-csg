#version 400

uniform vec3 u_color;
uniform bool u_highlighted;
uniform bool u_selected;
uniform bool u_is_secondary;

const vec3 light_dir = normalize(vec3(1,-.1,-.5));

const vec3 ambient = vec3(0.05, 0.05, 0.05);
const vec3 diffuse = vec3(.5,.5,.5);
const vec3 highlight = vec3(1.5,1.5,1.5);
const vec3 primary_selection = vec3(1.0,1.0,1.9);
const vec3 secondary_selection = vec3(1.9,1.0,1.0);

in vec3 o_normal;

void main(void)
{
	
//	if (u_lighting_enabled)
//	{
	vec3 diffuse_color = diffuse * max(dot(-light_dir, o_normal),0);
	vec3 lit_color = ambient + diffuse_color;
	if (u_highlighted) lit_color *= highlight;
	if (u_selected)	   lit_color *= (u_is_secondary) ? secondary_selection : primary_selection;
	gl_FragColor = vec4(lit_color * u_color,1.0);
//	}
}