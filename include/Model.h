//
// Created by mateuszp on 11/26/22.
//

#ifndef OPENGL_PROJECT_MODEL_H
#define OPENGL_PROJECT_MODEL_H

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Shader.h"
#include "Mesh.h"

class Model {
public:
    Model(const char* path);
    void Draw(Shader *shader);
private:
    std::vector<Mesh> meshes;
    std::string dir;
    void loadModel(std::string path);
    void processNode(aiNode *node, const aiScene *scene);
    Mesh processMesh(aiMesh *mesh, const aiScene *scene);
};


#endif //OPENGL_PROJECT_MODEL_H
