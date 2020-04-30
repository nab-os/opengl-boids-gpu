#version 460

in vec2 UV;

//layout(rgba32f, binding = 0) uniform image2D bird_movements;
uniform sampler2D bird_movements;

uniform ivec2 grid_size;

out vec4 out_color;

void main() {
	//vec4 bird_infos = imageLoad(bird_movements, ivec2(UV * grid_size));
	vec4 bird_infos = texture2D(bird_movements, UV);
    vec3 color = vec3(0);
    if (bird_infos.z > 0) {
        color = vec3(UV, 1);
        //color = vec3(bird_infos.xy, 0.5);
    } else {
        discard;
    }
    out_color = vec4(color, 1);
}
