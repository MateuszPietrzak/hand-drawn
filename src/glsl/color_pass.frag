#version 330 core


in vec3 normal;
in vec4 color;
in vec3 frag_pos;
in vec3 eye_pos;

out vec4 frag_col;

float near = 0.1; 
float far  = 100.0; 

void main() {
    float ambient_strength = 0.2;
    vec3 ambient = vec3(ambient_strength);

    vec3 norm = normalize(normal);
    vec3 light_dir = normalize(vec3(0.3, 1.0, -1.0));
    float diff = max(dot(norm, light_dir), 0.0);
    vec3 diffuse = vec3(diff);

    float specular_strength = 0.5;
    vec3 view_dir = normalize(eye_pos - frag_pos);
    vec3 reflect_dir = reflect(-light_dir, norm);
    float spec = pow(max(dot(view_dir, reflect_dir), 0.0), 32);
    vec3 specular = vec3(spec * specular_strength);

    vec3 result = ambient + diffuse + specular;
    float luminance = dot(result.xyz,vec3(0.2126, 0.7152, 0.0722));
    frag_col = vec4(vec3(luminance), 1.0);
}
