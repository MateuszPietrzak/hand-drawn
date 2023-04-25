#version 330 core

uniform sampler2D image;
in vec2 tex_coords;

out vec4 frag_col;

const float offset_x = 1.0 / 1200.0;
const float offset_y = 1.0 / 800.0;

void main() {
    //frag_col = texture(image, tex_coords);
    //return;

    vec2 offsets[9] = vec2[](
            vec2(-offset_x,  offset_y), // top-left
            vec2( 0.0f,    offset_y), // top-center
            vec2( offset_x,  offset_y), // top-right
            vec2(-offset_x,  0.0f),   // center-left
            vec2( 0.0f,    0.0f),   // center-center
            vec2( offset_x,  0.0f),   // center-right
            vec2(-offset_x, -offset_y), // bottom-left
            vec2( 0.0f,   -offset_y), // bottom-center
            vec2( offset_x, -offset_y)  // bottom-right    
            );

    float kernel1[9] = float[](
            -1, 0, 1,
            -2, 0, 2,
            -1, 0, 1
            );

    float kernel2[9] = float[](
            -1, -2, -1,
            0, 0, 0,
            1, 2, 1
            );

    vec3 sampleTex[9];
    for(int i = 0; i < 9; i++) {
        sampleTex[i] = vec3(texture(image, tex_coords.st + offsets[i]));
    }

    float col1 = 0.0;
    for(int i = 0; i < 9; i++)
        col1 += sampleTex[i].r * kernel1[i];


    float col2 = 0.0;
    for(int i = 0; i < 9; i++)
        col2 += sampleTex[i].r * kernel2[i];

    float res = pow(col1*col1 + col2*col2, 0.5);
    frag_col = vec4(res, 0.0, texture(image, tex_coords).b, 0.0);
}
