#version 460

in vec2 UV;

uniform sampler2D center_heatmap;
uniform sampler2D repel_heatmap;

out vec4 out_color;

void main() {
    //vec3 color = texture2D(center_heatmap, UV).xyz;
    vec3 color = vec3(0);
    out_color = vec4(color, 1);
}
