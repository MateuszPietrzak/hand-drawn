//
// Created by mateuszp on 11/26/22.
//

#ifndef OPENGL_PROJECT_MESH_H
#define OPENGL_PROJECT_MESH_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include "Shader.h"

struct Vertex {
    glm::vec3 pos;
    glm::vec3 normal;
    glm::vec4 color;
};

class Mesh {
public:
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    Mesh(std::vector<Vertex> _vertices, std::vector<unsigned int> _indices);
    void Draw(Shader *shader);
private:
    unsigned int VAO, VBO, EBO;

    void setupMesh();
};


#endif //OPENGL_PROJECT_MESH_H
