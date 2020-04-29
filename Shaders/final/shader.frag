#version 460

in vec2 UV;

layout(rgba32f, binding = 0) uniform image2D bird_positions;
uniform sampler2D bird_final;

out vec4 out_color;

void main() {
	vec3 color = texture2D(bird_final, UV).xyz;
    out_color = vec4(color, 1);
}
