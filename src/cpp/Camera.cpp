//
// Created by mateuszp on 11/26/22.
//

#include "../../include/Camera.h"

Camera::Camera(int width, int height) {
    yaw = 90.0f, pitch = -10.0f;
    last_x = (float)width / 2.0f;
    last_y = (float)height / 2.0f;
    cam_pos = glm::vec3(0.0f, 3.0f, -3.0f);
    cam_front = glm::vec3(0.0f, 0.0f, -1.0f);
    cam_up = glm::vec3(0.0f, 1.0f, 0.0f);
    direction = glm::vec3(0.0f);
}

bool Camera::processMouse(double x_pos, double y_pos, float delta_time) {
    bool res = (x_pos != 0 || y_pos != 0);
    float sensitivity = delta_time * 10.0f;
    float x_offset = x_pos - last_x;
    float y_offset = last_y - y_pos;

    yaw += x_offset * sensitivity;
    pitch += y_offset * sensitivity;

    if(pitch < -89.0f) pitch = -89.0f;
    if(pitch > 89.9f) pitch = 89.0f;

    last_x = x_pos;
    last_y = y_pos;
    return res;
}

bool Camera::processKeyboard(GLFWwindow* window, float delta_time) {
    bool res = false;
    const float cam_speed = 5.0f * delta_time;
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
        res = true;
    }
    if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        cam_pos += glm::normalize(glm::vec3(cam_front.x, 0.0f, cam_front.z)) * cam_speed;
        res = true;
    }
    if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        cam_pos -= glm::normalize(glm::vec3(cam_front.x, 0.0f, cam_front.z)) * cam_speed;
        res = true;
    }
    if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        cam_pos -= glm::normalize(glm::cross(cam_front, cam_up)) * cam_speed;
        res = true;
    }
    if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        cam_pos += glm::normalize(glm::cross(cam_front, cam_up)) * cam_speed;
        res = true;
    }
    if(glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
        cam_pos.y -= cam_speed;
        res = true;
    }
    if(glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        cam_pos.y += cam_speed;
        res = true;
    }

    return res;
}

glm::mat4 &Camera::getMatrix() {
    view = glm::mat4(1.0f);
    direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction.y = sin(glm::radians(pitch));
    direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cam_front = glm::normalize(direction);
    view = glm::lookAt(cam_pos, cam_pos + cam_front, cam_up);
    return view;
}
