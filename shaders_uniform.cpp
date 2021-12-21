// #include <glad/glad.h>
// #include <GLFW/glfw3.h>

#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include <stdio.h>

#include <OpenGL/gl3.h> //(My code compiles without this line)
#define GLFW_INCLUDE_GLCOREARB
#include "GLFW/glfw3.h"

// #include <opencv2/opencv.hpp>
#include <iostream>
#include <cmath>
#include <vector>
#include <string>

// Use AronCLibNew instead
#include "AronLib.h"
#include "AronCLibNew.h"

using namespace std;

/*
  Ref: https://learnopengl.com/Getting-started/Shaders
  Data: 18-08-2020
  KEY: opengl shader, vertex shader fragment shader, shader tutorial, shader uniform
  Compile: opengl_compile.sh

  UPDATE: Sat 18 Dec 23:33:42 2021 
  1. Use raw string in shader string
  2. Add translation matrix to translate the curve
 */

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

std::string fragStr = R"(
#version 330 core
    out vec4 FragColor;
    uniform vec4 ourColor;
    void main()
    {
       FragColor = ourColor;
    }
)";

std::string vertexShaderStr = R"(
#version 330 core
    layout (location = 0) in vec3 aPos;
    void main()
    {
    mat4 translate1;
    translate1 = mat4(1.0, 0.0, 0.0,  0.0,
					  0.0, 1.0, 0.0,  0.0,
					  0.0, 0.0, 1.0,  0.0,  
					  -0.5, -0.5, 0.0, 1.0);
       gl_Position = translate1 * vec4(aPos, 1.0);
    }
)";

const char *vertexShaderSource = vertexShaderStr.c_str();

const char *fragmentShaderSource = fragStr.c_str(); 

void fun(float a, int sz, float arr[300]){
  vector<float> vx;
  vector<float> vy;
  float del = (float)1/sz;
  for(int i = 0; i < sz; i++){
    float val = del*i;
    vx.push_back(val);
    vy.push_back(a*val*val);
    arr[i] = a*val*val;
  }

  /**
   *  0 1 2 3 4 5
   *  -3 -2 -1 0 1 2
   */
  vector<float> repv = repeatVec(100, (float)0.0);
  int k = 0;
  for(int i = 0; i < 100; i++){
    arr[k] = vx[i];
    arr[k+1] = vy[i];
    arr[k+2] = repv[i];
    k = k + 3;
  }
}
/**
 *   1/2
 *   1 x 1/2,     2 x 1/2
 *    (1/2)(1/2)    1 x 1
 *  
 *
 *  
 *   |               |             |
 *   offset+0     offset+1     offset+2
 */ 
const int xyzVertexes = 3*3;
void fillArray(float arr[xyzVertexes]){
  int nVert = (int)(xyzVertexes/3); 
  float delta = (float)1/nVert;
  for(int i = 0; i < nVert; i++){
    float val = delta*(i + 1);
    int offset = 3*i ;
    arr[offset+0] = val;
    arr[offset+1] = val*val;
    arr[offset+2] = 0.0f;
  }
}

void drawTriangle(unsigned int VBO2, unsigned int VAO2){
    // BEG_Draw_triangle
    float triVert[] = {  //  R     G     B
         0.5f, -0.5f, 0.0f,   1.0f, 0.0f, 0.0f, 
        -0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f, 
         0.0f,  0.5f, 0.0f,   0.0f, 0.0f, 1.0f 
    };

    // unsigned int VBO2, VAO2;
    //            |      +-> vertex array object
    //            +-> vertex buffer object
    glGenVertexArrays(1, &VAO2);
    //                |
    //                + -> the number of vertex array object names to generate
    //
    glGenBuffers(1, &VBO2);

    glBindVertexArray(VAO2);

    glBindBuffer(GL_ARRAY_BUFFER, VBO2);
    glBufferData(GL_ARRAY_BUFFER, sizeof(triVert), triVert, GL_STATIC_DRAW);

    //                       + -> specify number of components per generic vertex attribute
    //                       ↓ 
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), (void*)0);
    //                                                 ↑ 
    //                                                 + next vertex shift 6 floats
    //
    glEnableVertexAttribArray(0);

    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), (void*)(3*sizeof(float)));
    //                                                 ↑
    //                                                 + -> next color shift 6 floats
    //
    glEnableVertexAttribArray(1);
}



int main(){
    // glfw: initialize and configure
    // ------------------------------
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    /*
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    */

    // build and compile our shader program
    // ------------------------------------
    // vertex shader
    int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    // check for shader compile errors
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    // fragment shader
    int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    // check for shader compile errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    // link shaders
    int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader); // vertexShader - vertex shader code
    glAttachShader(shaderProgram, fragmentShader); // fragmentShader - fragment shader code
    glLinkProgram(shaderProgram);                 // link shader
    // check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // BEG_Draw_triangle
    float triVert[] = {  //  R     G     B
         0.5f, -0.5f, 0.0f,   1.0f, 0.0f, 0.0f, 
        -0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f, 
         0.0f,  0.5f, 0.0f,   0.0f, 0.0f, 1.0f 
    };

    unsigned int VBO2, VAO2;
    //            |      +-> vertex array object
    //            +-> vertex buffer object
    glGenVertexArrays(1, &VAO2);
    //                |
    //                + -> the number of vertex array object names to generate
    //
    glGenBuffers(1, &VBO2);

    glBindVertexArray(VAO2);

    glBindBuffer(GL_ARRAY_BUFFER, VBO2);
    glBufferData(GL_ARRAY_BUFFER, sizeof(triVert), triVert, GL_STATIC_DRAW);

    //                       + -> specify number of components per generic vertex attribute
    //                       ↓ 
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), (void*)0);
    //                                                 ↑ 
    //                                                 + next vertex shift 6 floats
    //
    glEnableVertexAttribArray(0);

    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), (void*)(3*sizeof(float)));
    //                                                 ↑
    //                                                 + -> next color shift 6 floats
    //
    glEnableVertexAttribArray(1);
    // END_Draw_triangle
    

    // BEG_Draw_triangle
    float triVert3[] = {  //  R     G     B
         0.9f,  0.9f, 0.0f,   1.0f, 0.0f, 0.0f, 
         0.6f,  0.8f, 0.0f,   0.0f, 1.0f, 0.0f, 
         0.9f,  0.2f, 0.0f,   0.0f, 0.0f, 1.0f 
    };

    unsigned int VBO3, VAO3;
    //            |      +-> vertex array object
    //            +-> vertex buffer object
    glGenVertexArrays(1, &VAO3);
    //                |
    //                + -> the number of vertex array object names to generate
    //
    glGenBuffers(1, &VBO3);

    glBindVertexArray(VAO3);

    glBindBuffer(GL_ARRAY_BUFFER, VBO3);
    glBufferData(GL_ARRAY_BUFFER, sizeof(triVert3), triVert3, GL_STATIC_DRAW);

    //                       + -> specify number of components per generic vertex attribute
    //                       ↓ 
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), (void*)0);
    //                                                 ↑ 
    //                                                 + next vertex shift 6 floats
    //
    glEnableVertexAttribArray(0);

    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), (void*)(3*sizeof(float)));
    //                                                 ↑
    //                                                 + -> next color shift 6 floats
    //
    glEnableVertexAttribArray(1);
    
    
    // render loop
    // -----------
    while (!glfwWindowShouldClose(window)) {
        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // be sure to activate the shader before any calls to glUniform
        glUseProgram(shaderProgram);

        // BEG_triangle_1
        int vertexColorLocation = glGetUniformLocation(shaderProgram, "ourColor");
        glUniform4f(vertexColorLocation, 0.0f, 1.0f, 0.0f, 1.0f);

        glBindVertexArray(VAO2);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 3);
        // END_triangle_1
        //

        // BEG_triangle_3
        int vertexColorLocation3 = glGetUniformLocation(shaderProgram, "ourColor");
        glUniform4f(vertexColorLocation3, 1.0f, 0.0f, 0.0f, 1.0f);

        glBindVertexArray(VAO3);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 3);
        // END_triangle_3

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    
    // glDeleteVertexArrays(0, &VAO);
    // glDeleteBuffers(0, &VBO);
    
    glDeleteVertexArrays(1, &VAO2);
    glDeleteBuffers(1, &VBO2);
    glDeleteProgram(shaderProgram);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}


