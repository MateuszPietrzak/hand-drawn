//
// Created by mateuszp on 11/26/22.
//

#ifndef OPENGL_PROJECT_CAMERA_H
#define OPENGL_PROJECT_CAMERA_H

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Camera {
public:
    Camera(int width, int height);
    ~Camera();

    bool processMouse(double x_pos, double y_pos, float delta_time);
    bool processKeyboard(GLFWwindow* window, float delta_time);
    glm::mat4& getMatrix();
    glm::vec3& getPos(){return cam_pos;}
private:
    float yaw, pitch;
    float last_x, last_y;
    glm::vec3 cam_pos;
    glm::vec3 cam_front;
    glm::vec3 cam_up;
    glm::vec3 direction;
    glm::mat4 view;
};


#endif //OPENGL_PROJECT_CAMERA_H
