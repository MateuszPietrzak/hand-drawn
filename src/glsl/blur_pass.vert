#version 330 core

layout (location = 0) in vec2 in_pos;

out vec2 tex_coords;

void main() {
    gl_Position = vec4(in_pos, 0.0, 1.0);
    tex_coords = (in_pos + 1.0)/2.0;
}
