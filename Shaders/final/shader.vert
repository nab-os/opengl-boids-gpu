#version 460

in vec2 in_Vertex;

out vec2 UV;

void main() {
    gl_Position = vec4(in_Vertex * 2 - 1, 0, 1);

    UV = in_Vertex;
}

