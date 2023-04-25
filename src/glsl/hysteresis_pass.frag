#version 330 core

uniform sampler2D image;
uniform sampler2D noise;
uniform float rand_off;
in vec2 tex_coords;

out vec4 frag_col;

const float offset_x = 1.0 / 1200.0;
const float offset_y = 1.0 / 800.0;

void main() {
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

    vec4 current = texture(image, tex_coords);
    float noise = texture(noise, tex_coords * 0.5 + rand_off).r;
    //frag_col = vec4(noise);
    //return;
    frag_col = vec4(current.b);
    if(current.r != 1.0) {
        frag_col = vec4(current.b);
        if(frag_col.r < noise) 
            frag_col = vec4(0.0);
        else
            frag_col = vec4(1.0);
        return;
    }
    if(current.g == 1.0) {
        frag_col = vec4(0.0);
        if(frag_col.r < noise) 
            frag_col = vec4(0.0);
        else
            frag_col = vec4(1.0);
        return;
    }
    bool flag = false;
    for(int i = 0; i < 8; ++i) {
        if(texture(image, tex_coords).g == 1.0) {
            flag = true;
            break;
        }
    }
    if(flag) frag_col = vec4(0.0);
    else frag_col = vec4(current.b);
    if(frag_col.r < noise) 
        frag_col = vec4(0.0);
    else
        frag_col = vec4(1.0);
}
