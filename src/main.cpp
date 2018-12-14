#include <GLFW/glfw3.h>
#include <OpenGL/gl3.h>

#include <iostream>
#include <cmath>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader_s.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);
void break_lines(float A[3], float B[3], float C[3], std::vector<float> &vertices, int &size, int depth);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// wireframe settings
bool showWireframe = false;

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

    // glfw window creation
    // --------------------
    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // build and compile shader program
    // --------------------------------
    Shader ourShader("shaders/3.3.shader.vs", "shaders/3.3.shader.fs");

    // set up mainVertices
    // --------------
    std::vector<float> mainVertices;
    int verticesSize;
    float A[3] = { 0.0f,  0.5f, 0.0f };
    float B[3] = { 0.5f, -0.5f, 0.0f };
    float C[3] = { -0.5f, -0.5f, 0.0f };
    break_lines(A, B, C, mainVertices, verticesSize, 2);
    std::cout << "vertices size is " << mainVertices.size() << std::endl;

    //     // positions        // color
    //     0.0f,  0.5f, 0.0f,  1.0f, 0.0f, 0.0f, // top
    //     0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f, // bottom right
    //     -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom left
    // };
    float texCoords[] = {
        1.0f, 0.0f, // lower-right corner
        0.0f, 0.0f, // lower-left corner
        0.5f, 1.0f  // top-center corner
    };

    // texture 1
    // ---------
    unsigned int texture1;
    glGenTextures(1, &texture1);
    glBindTexture(GL_TEXTURE_2D, texture1); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load and generate the texture
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true); // because openGL regards 0.0 for y as the bottom whereas images at the top
    unsigned char *data = stbi_load("textures/container.jpg", &width, &height, &nrChannels, 0);
    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        std::cerr << "Failed to load texture 1!" << std::endl;
    }
    // texture 2
    // ---------
    unsigned int texture2;
    glGenTextures(1, &texture2);
    glBindTexture(GL_TEXTURE_2D, texture2); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    data = stbi_load("textures/shrek.jpg", &width, &height, &nrChannels, 0);
    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        std::cerr << "Failed to load texture 2!" << std::endl;
    }
    stbi_image_free(data);

    // Set up Vertex Buffer Object and Vertex Array Object
    // ---------------------------------------------------
    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    // 1) bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);
    // from now on every call to GL_ARRAY_BUFFER affects currently bound buffer
    // 2) copy mainVertices array into a buffer
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, mainVertices.size() * sizeof(float), &mainVertices[0], GL_STATIC_DRAW);
    // 3) set position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // unbind buffer
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    // remember: do NOT unbind the EBO while a VAO is active as the bound element buffer object IS stored in the VAO; keep the EBO bound.
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // set up uniform shader variable
        // float timeValue = glfwGetTime();
        // float greenValue = (sin(timeValue) / 2.0f) + 0.5f;
        // int vertexColorLocation = glGetUniformLocation(shaderProgram, "ourColor");
        // Draw triangle
        ourShader.use();

        glBindVertexArray(VAO);
        glEnable(GL_PROGRAM_POINT_SIZE);
        glDrawArrays(GL_POINTS, 0, 100);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    else if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        showWireframe ? glPolygonMode(GL_FRONT_AND_BACK, GL_LINE) : glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        showWireframe = !showWireframe;
    }
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// A, B and C are coordinates of the triangle vertices
void break_lines(float A[3], float B[3], float C[3], std::vector<float> &vertices, int &size, int depth) {
    if(depth > 0) {
        float D[3] = { (A[0]+B[0])/2, (A[1]+B[1])/2, 0.0f }; // AB
        float E[3] = { (A[0]+C[0])/2, (A[1]+C[1])/2, 0.0f }; // AC
        float F[3] = { (B[0]+C[0])/2, (B[1]+C[1])/2, 0.0f }; // BC

        // so new triangles are DFB, DEF, ECF, AED
        float newVertices[] = {
            D[0], D[1], D[2], 1.0f, 0.0f, 0.0f, // top
            F[0], F[1], F[2], 1.0f, 0.0f, 0.0f, // bottom right
            B[0], B[1], B[2], 1.0f, 0.0f, 0.0f, // bottom left

            D[0], D[1], D[2], 1.0f, 0.0f, 0.0f, // top left
            E[0], E[1], E[2], 1.0f, 0.0f, 0.0f, // top right
            F[0], F[1], F[2], 1.0f, 0.0f, 0.0f, // bottom

            E[0], E[1], E[2], 1.0f, 0.0f, 0.0f, // top
            C[0], C[1], C[2], 1.0f, 0.0f, 0.0f, // bottom right
            F[0], F[1], F[2], 1.0f, 0.0f, 0.0f, // bottom left

            A[0], A[1], A[2], 1.0f, 0.0f, 0.0f, // top
            E[0], E[1], E[2], 1.0f, 0.0f, 0.0f, // bottom right
            D[0], D[1], D[2], 1.0f, 0.0f, 0.0f, // bottom left
        };

        size += 72;
        vertices.insert(vertices.end(), &newVertices[0], &newVertices[size]);
        std::cout << "HI" << std::endl;
        break_lines(A, D, E, vertices, size, --depth);
    }

}