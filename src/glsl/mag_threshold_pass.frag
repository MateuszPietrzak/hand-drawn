#version 330 core

uniform sampler2D image;
in vec2 tex_coords;

out vec4 frag_col;

const float offset_x = 1.0 / 1200.0;
const float offset_y = 1.0 / 800.0;

void main() {
    vec2 offsets[8] = vec2[](
            vec2(-offset_x,  offset_y), // top-left
            vec2( 0.0f,    offset_y), // top-center
            vec2( offset_x,  offset_y), // top-right
            vec2(-offset_x,  0.0f),   // center-left
            vec2( offset_x,  0.0f),   // center-right
            vec2(-offset_x, -offset_y), // bottom-left
            vec2( 0.0f,   -offset_y), // bottom-center
            vec2( offset_x, -offset_y)  // bottom-right    
            );
    float mx = 0.0;
    for(int i = 0; i < 8; i++) {
        mx = max(mx, texture(image, tex_coords + offsets[i]).r);
    }
    vec4 cur = texture(image, tex_coords);
    if(cur.r >= mx)
        frag_col = vec4(cur.r, 0.0, cur.b, 0.0);
    else
        frag_col = vec4(0.0, 0.0, cur.b, 0.0);
    return;
}
