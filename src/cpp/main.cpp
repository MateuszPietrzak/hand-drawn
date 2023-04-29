#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION

#include <iostream>
#include "../../include/Shader.h"
#include "../../include/Camera.h"
#include "../../include/Model.h"
#define STB_IMAGE_IMPLEMENTATION
#include "../../include/Texture.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <random>

int WIDTH = 1200, HEIGHT = 800;

float delta_time = 0.0f;
float last_frame = 0.0f;
bool reroll_dither = false;
float time_since_reroll;

Camera* cam;
GLFWwindow* window;

void init();
void framebufferSizeCallback(GLFWwindow* window, int width, int height);
void mouseCallback(GLFWwindow* window, double x_pos, double y_pos);
void processInput(GLFWwindow* window);

Model* scene;

int main() {
    init();

    std::mt19937 rng(1);
    std::uniform_real_distribution<float> dist(1, 30);

    auto color_pass = new Shader("src/glsl/color_pass.vert", "src/glsl/color_pass.frag");
    auto sobel_pass = new Shader("src/glsl/sobel_pass.vert", "src/glsl/sobel_pass.frag");
    auto mag_threshold_pass = new Shader("src/glsl/mag_threshold_pass.vert", "src/glsl/mag_threshold_pass.frag");
    auto double_threshold_pass = new Shader("src/glsl/double_threshold_pass.vert", "src/glsl/double_threshold_pass.frag");
    auto hysteresis_pass = new Shader("src/glsl/hysteresis_pass.vert", "src/glsl/hysteresis_pass.frag");
    auto blur_pass = new Shader("src/glsl/blur_pass.vert", "src/glsl/blur_pass.frag");

    cam = new Camera(WIDTH, HEIGHT);

    scene = new Model("assets/scene.obj");

    glm::mat4 projection = glm::perspective((float)(M_PI / 4), (float)(WIDTH) / (float)(HEIGHT), 0.1f, 300.0f);

    unsigned int FBO;
    glGenFramebuffers(1, &FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);

    unsigned int framebuffer_color_texture;
    glGenTextures(1, &framebuffer_color_texture);
    glBindTexture(GL_TEXTURE_2D, framebuffer_color_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, WIDTH, HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, framebuffer_color_texture, 0);

    unsigned int RBO;
    glGenRenderbuffers(1, &RBO);
    glBindRenderbuffer(GL_RENDERBUFFER, RBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, WIDTH, HEIGHT);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, RBO);

    unsigned int FBO2;
    glGenFramebuffers(1, &FBO2);
    glBindFramebuffer(GL_FRAMEBUFFER, FBO2);

    unsigned int framebuffer_color_texture2;
    glGenTextures(1, &framebuffer_color_texture2);
    glBindTexture(GL_TEXTURE_2D, framebuffer_color_texture2);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, WIDTH, HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, framebuffer_color_texture2, 0);

    unsigned int noise_texture = Texture::createTexture("assets/noise.png");

    float vertices[] = {
        -1.0f, -1.0f,
        -1.0f, 1.0f,
        1.0f, -1.0f,
        1.0f, 1.0f,
        -1.0f, 1.0f,
        1.0f, -1.0f
    };
    unsigned int quadVAO;
    unsigned int quadVBO;

    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);

    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    while(!glfwWindowShouldClose(window)) {
        processInput(window);

        //RENDER SCENE
        glBindFramebuffer(GL_FRAMEBUFFER, FBO);
        glEnable(GL_DEPTH_TEST);
        glClearColor(0.01f, 0.01f, 0.01f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        color_pass->use();


        color_pass->setMat4("projection", projection);
        color_pass->setMat4("view", cam->getMatrix());
        color_pass->setVec3("in_eye_pos", cam->getPos());

        glm::mat4  model;
        model = glm::mat4(1.0f);
        color_pass->setMat4("model", model);

        scene->Draw(color_pass);

        //SOBEL PASS
        glBindFramebuffer(GL_FRAMEBUFFER, FBO2);
        glDisable(GL_DEPTH_TEST);
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, framebuffer_color_texture);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, noise_texture);
        glActiveTexture(GL_TEXTURE0);
        sobel_pass->use();
        sobel_pass->setInt("image", 0);


        glBindVertexArray(quadVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        //MAGNITUDE THERSHOLD PASS
        glBindFramebuffer(GL_FRAMEBUFFER, FBO);
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glBindTexture(GL_TEXTURE_2D, framebuffer_color_texture2);
        mag_threshold_pass->use();
        mag_threshold_pass->setInt("image", 0);

        glDrawArrays(GL_TRIANGLES, 0, 6);

        //DOUBLE_THERSHOLD_PASS
        glBindFramebuffer(GL_FRAMEBUFFER, FBO2);
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glBindTexture(GL_TEXTURE_2D, framebuffer_color_texture);
        double_threshold_pass->use();
        double_threshold_pass->setInt("image", 0);

        glDrawArrays(GL_TRIANGLES, 0, 6);

        //HYSTERESIS PASS
        glBindFramebuffer(GL_FRAMEBUFFER, FBO);
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glBindTexture(GL_TEXTURE_2D, framebuffer_color_texture2);
        hysteresis_pass->use();
        hysteresis_pass->setInt("image", 0);
        hysteresis_pass->setInt("noise", 1);

        if(reroll_dither && time_since_reroll >= 0.05) {
            hysteresis_pass->setFloat("rand_off", dist(rng));
            reroll_dither = false;
            time_since_reroll -= 0.05;
        }
        if(time_since_reroll >= 0.5) {
            hysteresis_pass->setFloat("rand_off", dist(rng));
            time_since_reroll -= 0.5;
        }

        glDrawArrays(GL_TRIANGLES, 0, 6);

        //BLUR PASS

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glBindTexture(GL_TEXTURE_2D, framebuffer_color_texture);
        blur_pass->use();
        blur_pass->setInt("image", 0);

        glDrawArrays(GL_TRIANGLES, 0, 6);

        glfwSwapBuffers(window);
        glfwPollEvents();

        delta_time = glfwGetTime() - last_frame;
        time_since_reroll += delta_time;
        last_frame = glfwGetTime();
    }
    glDeleteFramebuffers(1, &FBO);

    glfwTerminate();

    return 0;
}


void init() {

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(WIDTH, HEIGHT, "OpenGL", NULL, NULL);
    if(window == NULL) {
        std::cout << "Failed to create a window" << std::endl;
        glfwTerminate();
        return;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return;
    }

    glEnable(GL_DEPTH_TEST);

    //glEnable(GL_CULL_FACE);
    //glCullFace(GL_BACK);

    glViewport(0, 0, WIDTH, HEIGHT);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouseCallback);
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
    WIDTH = width;
    HEIGHT = height;
}

void mouseCallback(GLFWwindow* window, double x_pos, double y_pos) {
    if(cam->processMouse(x_pos, y_pos, delta_time)) {
        reroll_dither = true;
    }
}

void processInput(GLFWwindow* window) {
    if(cam->processKeyboard(window, delta_time)) {
        reroll_dither = true;
    }
}
