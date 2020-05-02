#version 460

in vec2 UV;

out vec4 out_color;

void main() {
    out_color = vec4(UV, 1, 1);
}
