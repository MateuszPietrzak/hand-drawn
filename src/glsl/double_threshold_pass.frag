#version 330 core

uniform sampler2D image;
in vec2 tex_coords;

out vec4 frag_col;

void main() {
    float low_threshold = 0.4;
    float high_threshold = 0.8;

    vec4 cur = texture(image, tex_coords);
    float flag = 0.0;

    if(cur.r >= low_threshold)
        flag = 0.5;
    if(cur.r >= high_threshold)
        flag = 1.0;

    frag_col = vec4(cur.r, flag, cur.b, 1.0);
}
