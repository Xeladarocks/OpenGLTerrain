#pragma once
#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>

#include <perlin/perlin.hpp>

#include "Shader.h"
#include "Camera.h"
#include <vector>
#include <time.h>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window, int key, int scancode, int action, int mods);
void initGL();

void push_vec3(std::vector<float> &array, glm::vec3 item) {
    array.push_back(item.x); array.push_back(item.y); array.push_back(item.z);
}
void push_ivec3(std::vector<unsigned int>& array, glm::ivec3 item) {
    array.push_back(item.x); array.push_back(item.y); array.push_back(item.z);
}

const int WIDTH = 800;
const int HEIGHT = 600;

// camera
Camera camera(glm::vec3(0.0f, 20.0f, 0.0f));

float lastX = WIDTH / 2.0f;
float lastY = HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

// perlin
siv::PerlinNoise perlin(time(0));
const int frequency = 32;
const int octaves = 8;

// map generation
const float max_y = 5.0f;
const float peak_scalar = 2.0f; // may go beyond max_y

float getNoise(const int &x, const int &z) {
    return std::pow(perlin.accumulatedOctaveNoise2D_0_1(x / (float)frequency, z / (float)frequency, octaves) * max_y, peak_scalar);
}
void generateWorld(std::vector<float> &vertices, std::vector<unsigned int> &indices, std::vector<float> &colors) {
    const int grid_size = 200;
    const float tile_size = 2;

    for (float x = -grid_size; x < grid_size; x+=tile_size) {
        for (float z = -grid_size; z < grid_size; z+=tile_size) {
            /** Vertices **/
            // 0
            push_vec3(vertices, glm::vec3(
                x, 
                getNoise(x + (float)grid_size, z + (float)grid_size),
                z));

            // 1
            push_vec3(vertices, glm::vec3(
                x + tile_size, 
                getNoise(x + tile_size + (float)grid_size, z + (float)grid_size),
                z));

            // 2
            push_vec3(vertices, glm::vec3(
                x + tile_size, 
                getNoise(x + tile_size + (float)grid_size, z + tile_size + (float)grid_size),
                z + tile_size));

            // 0
            push_vec3(vertices, glm::vec3(
                x,
                getNoise(x + (float)grid_size, z + (float)grid_size),
                z));

            // 2
            push_vec3(vertices, glm::vec3(
                x + tile_size,
                getNoise(x + tile_size + (float)grid_size, z + tile_size + (float)grid_size),
                z + tile_size));
            
            // 3
            push_vec3(vertices, glm::vec3(
                x,
                getNoise(x + (float)grid_size, z + tile_size + (float)grid_size),
                z + tile_size));
            /** End vertices **/

            /** Indices **/
            if (indices.size() != 0) {
                unsigned int idx = indices[indices.size() - 1];
                push_ivec3(indices, glm::vec3(idx + 1, idx + 2, idx + 3));
                push_ivec3(indices, glm::vec3(idx + 4, idx + 5, idx + 6));
            }
            else indices = { 0, 1, 2,   3, 4, 5 };
            /** End indices **/


            /** Coloring **/
            float height_color = std::pow(getNoise(x + (float)grid_size, z + (float)grid_size), 1.0f / peak_scalar)/max_y;
            for(int f = 0; f < 3; f++)
                push_vec3(colors, glm::vec3(0, height_color, 0.5-height_color));

            float height_color2 = std::pow(getNoise(x + (tile_size/2.0f) + (float)grid_size, z + (tile_size/2.0f) + (float)grid_size), 1.0f/peak_scalar)/max_y;
            for (int f = 0; f < 3; f++)
                push_vec3(colors, glm::vec3(0, height_color2, 0.5-height_color2));
            /** End Coloring **/
        }
    }

    printf("Vertices: %i\nIndices: %i\n", vertices.size(), indices.size());
}

int main() {

    /** SET-UP **/
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, (int)4.5);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, (int)4.5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment for Apple OS X support

    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "OpenGL", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    initGL();
    glViewport(0, 0, WIDTH, HEIGHT);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window, processInput);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    glfwSwapInterval(0);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    /** ------ **/

    Shader basicShader("Basic.vs.glsl", "Basic.fs.glsl");

    std::vector<float> vertices;
    std::vector<unsigned int> indices;
    std::vector<float> colors;

    generateWorld(vertices, indices, colors);

    unsigned int VBO, EBO, CBO, VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    /* ----------------------------------------------- */

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);
    // position attribute
    glVertexAttribPointer(
        0,          // index
        3,          // size
        GL_FLOAT,   // type
        GL_FALSE,   // normalized
        0,          // stride
        (void*) 0   // offset
    );
    glEnableVertexAttribArray(0);

    /* ----------------------------------------------- */

    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    /* ----------------------------------------------- */

    glGenBuffers(1, &CBO);
    glBindBuffer(GL_ARRAY_BUFFER, CBO);
    glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(float), &colors[0], GL_STATIC_DRAW);
    // color attribute
    glVertexAttribPointer(
        1, 
        3, 
        GL_FLOAT, 
        GL_FALSE, 
        0, 
        (void*) 0
    );
    glEnableVertexAttribArray(1);

    /* ----------------------------------------------- */


    glm::mat4 view(1.0f);
    glm::mat4 model(1.0f);
    glm::mat4 projection(1.0f);

    while (!glfwWindowShouldClose(window)) {

        camera.updateKeyboard(deltaTime);

        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        //Render
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // render the triangle
        basicShader.use();

        // camera/view transformation
        projection = glm::perspective(glm::radians(camera.Zoom), (float)WIDTH / (float)HEIGHT, 0.1f, 1000.0f);
        view = camera.GetViewMatrix();


        /* Uniforms */
        basicShader.setMat4("view", view);
        basicShader.setMat4("model", model);
        basicShader.setMat4("projection", projection);
        /* -------- */

        glBindVertexArray(VAO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO); // indices

        // Drawing types: GL_TRIANGLES = normal fill, GL_LINE_STRIP = wireframe
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &CBO);

    glfwTerminate();
	return 0;
}

void initGL() {
    glEnable(GL_DEPTH_TEST);   // Enable depth testing for z-culling
}

void processInput(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    bool toggle = false;
    if (action == GLFW_PRESS) toggle = true;
    else if (action == GLFW_RELEASE) toggle = false;
    else return;

    switch (key) {
        case GLFW_KEY_W: camera.toggles.forward = toggle; break;
        case GLFW_KEY_S: camera.toggles.backward = toggle; break;
        case GLFW_KEY_A: camera.toggles.left = toggle; break;
        case GLFW_KEY_D: camera.toggles.right = toggle; break;
        case GLFW_KEY_SPACE: camera.toggles.up = toggle; break;
        case GLFW_KEY_LEFT_SHIFT: camera.toggles.down = toggle; break;
        case GLFW_KEY_LEFT_CONTROL: camera.toggles.sprint = toggle; break;
    };
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) { // resize callback
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    camera.ProcessMouseScroll(yoffset);
}