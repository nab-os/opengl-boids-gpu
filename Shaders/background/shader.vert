#version 460

in vec3 in_Vertex;

out vec2 UV;

void main() {
    gl_Position = vec4(in_Vertex, 1);

    UV = (in_Vertex.xy + 1) * 0.5;
}

