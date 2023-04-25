#version 330 core

layout (location = 0) in vec3 input_pos;
layout (location = 1) in vec3 input_normal;
layout (location = 2) in vec4 input_color;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform vec3 in_eye_pos;

out vec3 normal;
out vec4 color;
out vec3 frag_pos;
out vec3 eye_pos;

void main() {
    normal = mat3(transpose(inverse(model))) * input_normal;
    color = input_color;
    frag_pos = vec3(model * vec4(input_pos, 1.0));
    eye_pos = in_eye_pos;
    gl_Position = projection * view * model *  vec4(input_pos, 1.0);
}
