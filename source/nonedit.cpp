#include <iostream>
#include <cmath>
#include <fstream>
#include <vector>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "main.h"

using namespace std;

/* Initialise glfw window, I/O callbacks and the renderer to use */
/* Nothing to Edit here */
GLFWwindow *initGLFW(int width, int height)
{
    GLFWwindow *window; // window desciptor/handle

    glfwSetErrorCallback(error_callback);
    if (!glfwInit())
    {
        // exit(EXIT_FAILURE);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(width, height, "Sample OpenGL 3.3 Application", NULL, NULL);

    if (!window)
    {
        glfwTerminate();
        // exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(window);
    // Initialize GLEW, Needed in Core profile
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (err != GLEW_OK)
    {
        cout << "Error: Failed to initialise GLEW : " << glewGetErrorString(err) << endl;
        exit(1);
    }
    glfwSwapInterval(1);

    /* --- register callbacks with GLFW --- */

    /* Register function to handle window resizes */
    /* With Retina display on Mac OS X GLFW's FramebufferSize
       is different from WindowSize */
    glfwSetFramebufferSizeCallback(window, reshapeWindow);
    glfwSetWindowSizeCallback(window, reshapeWindow);

    /* Register function to handle window close */
    glfwSetWindowCloseCallback(window, quit);

    /* Register function to handle keyboard input */
    glfwSetKeyCallback(window, keyboard);      // general keyboard input
    glfwSetCharCallback(window, keyboardChar); // simpler specific character handling

    /* Register function to handle mouse click */
    glfwSetMouseButtonCallback(window, mouseButton); // mouse button clicks
    glfwSetScrollCallback(window, scroll_callback);

    return window;
}

/* Function to load Shaders - Use it as it is */
GLuint LoadShaders(const char *vertex_file_path, const char *fragment_file_path)
{

    // Create the shaders
    GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
    GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

    // Read the Vertex Shader code from the file
    std::string VertexShaderCode;
    std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);
    if (VertexShaderStream.is_open())
    {
        std::string Line = "";
        while (getline(VertexShaderStream, Line))
            VertexShaderCode += "\n" + Line;
        VertexShaderStream.close();
    }

    // Read the Fragment Shader code from the file
    std::string FragmentShaderCode;
    std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
    if (FragmentShaderStream.is_open())
    {
        std::string Line = "";
        while (getline(FragmentShaderStream, Line))
            FragmentShaderCode += "\n" + Line;
        FragmentShaderStream.close();
    }

    GLint Result = GL_FALSE;
    int InfoLogLength;

    // Compile Vertex Shader
    printf("Compiling shader : %s\n", vertex_file_path);
    char const *VertexSourcePointer = VertexShaderCode.c_str();
    glShaderSource(VertexShaderID, 1, &VertexSourcePointer, NULL);
    glCompileShader(VertexShaderID);

    // Check Vertex Shader
    glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    std::vector<char> VertexShaderErrorMessage(InfoLogLength);
    glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
    fprintf(stdout, "%s\n", &VertexShaderErrorMessage[0]);

    // Compile Fragment Shader
    printf("Compiling shader : %s\n", fragment_file_path);
    char const *FragmentSourcePointer = FragmentShaderCode.c_str();
    glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer, NULL);
    glCompileShader(FragmentShaderID);

    // Check Fragment Shader
    glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    std::vector<char> FragmentShaderErrorMessage(InfoLogLength);
    glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
    fprintf(stdout, "%s\n", &FragmentShaderErrorMessage[0]);

    // Link the program
    fprintf(stdout, "Linking program\n");
    GLuint ProgramID = glCreateProgram();
    glAttachShader(ProgramID, VertexShaderID);
    glAttachShader(ProgramID, FragmentShaderID);
    glLinkProgram(ProgramID);

    // Check the program
    glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
    glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    std::vector<char> ProgramErrorMessage(max(InfoLogLength, int(1)));
    glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
    fprintf(stdout, "%s\n", &ProgramErrorMessage[0]);

    glDeleteShader(VertexShaderID);
    glDeleteShader(FragmentShaderID);

    return ProgramID;
}

/* Generate VAO, VBOs and return VAO handle */
struct VAO *create3DObject(GLenum primitive_mode, int numVertices, int numTriangles, const GLfloat *vertex_buffer_data, unsigned int *index_buffer_data, GLenum fill_mode)
{
    struct VAO *vao = new struct VAO;
    vao->PrimitiveMode = GL_TRIANGLES;
    vao->NumVertices = numVertices;
    vao->NumTriangles = numTriangles;
    vao->FillMode = fill_mode;

    // Create Vertex Array Object
    // Should be done after CreateWindow and before any other GL calls
    glGenVertexArrays(1, &(vao->VertexArrayID)); // VAO
    glBindVertexArray(vao->VertexArrayID);

    glGenBuffers(1, &(vao->VertexBuffer)); // VBO - vertices
    glGenBuffers(1, &(vao->IndexBuffer));  // VBO - indices

    glBindBuffer(GL_ARRAY_BUFFER, vao->VertexBuffer);                                                     // Bind the VBO vertices
    glBufferData(GL_ARRAY_BUFFER, 6 * numVertices * sizeof(GLfloat), vertex_buffer_data, GL_STATIC_DRAW); // Copy the vertices into VBO

    glVertexAttribPointer(
        0,                   // attribute 0. Vertices
        3,                   // size (x,y,z)
        GL_FLOAT,            // type
        GL_FALSE,            // normalized?
        6 * sizeof(GLfloat), // stride
        (void *)0            // array buffer offset
    );
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(
        1,                            // attribute 1. Color
        3,                            // size (r,g,b)
        GL_FLOAT,                     // type
        GL_FALSE,                     // normalized?
        6 * sizeof(GLfloat),          // stride
        (void *)(3 * sizeof(GLfloat)) // array buffer offset
    );
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vao->IndexBuffer);                                                           // Bind the VBO indices
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 3 * numTriangles * sizeof(unsigned int), index_buffer_data, GL_STATIC_DRAW); // Copy the indices
    return vao;
}

struct VAO *create3DObject(GLenum primitive_mode, int numVertices, const GLfloat *vertex_buffer_data, GLenum fill_mode)
{
    cout << "hello\n";
    struct VAO *vao = new struct VAO;
    vao->PrimitiveMode = GL_TRIANGLES;
    vao->NumVertices = numVertices;
    vao->NumTriangles = 0;
    vao->FillMode = fill_mode;

    // Create Vertex Array Object
    // Should be done after CreateWindow and before any other GL calls
    glGenVertexArrays(1, &(vao->VertexArrayID)); // VAO
    glBindVertexArray(vao->VertexArrayID);

    glGenBuffers(1, &(vao->VertexBuffer)); // VBO - vertices

    glBindBuffer(GL_ARRAY_BUFFER, vao->VertexBuffer);                                                     // Bind the VBO vertices
    glBufferData(GL_ARRAY_BUFFER, 6 * numVertices * sizeof(GLfloat), vertex_buffer_data, GL_STATIC_DRAW); // Copy the vertices into VBO

    glVertexAttribPointer(
        0,                   // attribute 0. Vertices
        3,                   // size (x,y,z)
        GL_FLOAT,            // type
        GL_FALSE,            // normalized?
        6 * sizeof(GLfloat), // stride
        (void *)0            // array buffer offset
    );
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(
        1,                            // attribute 1. Color
        3,                            // size (r,g,b)
        GL_FLOAT,                     // type
        GL_FALSE,                     // normalized?
        6 * sizeof(GLfloat),          // stride
        (void *)(3 * sizeof(GLfloat)) // array buffer offset
    );
    glEnableVertexAttribArray(1);

    return vao;
}

/* Render the VBOs handled by VAO */
void draw3DObject(struct VAO *vao)
{
    glBindVertexArray(vao->VertexArrayID);
    if (vao->NumTriangles)
        glDrawElements(vao->PrimitiveMode, vao->NumTriangles * 3, GL_UNSIGNED_INT, nullptr); // Starting from vertex 0; 3 vertices total -> 1 triangle
    else
        glDrawArrays(vao->PrimitiveMode, 0, vao->NumVertices);
}
