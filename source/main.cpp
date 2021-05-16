#include "main.h"
#include "timer.h"
#include "ball.h"

using namespace std;

GLMatrices Matrices;
GLuint     programID;
GLFWwindow *window;

/**************************
* Customizable functions *
**************************/

Ball ball1;


int indexSolid;
int cameraRotate = 0;
float screen_zoom = 1, screen_center_x = 0, screen_center_y = 0;
float camera_rotation_angle = 0;

Timer t60(1.0 / 60);

glm::vec3 eye (0, 0, 10);
glm::vec3 target;
glm::vec3 up (0, 1, 0);
glm::vec3 front(0, 0, -1);
glm::vec3 worldUp (0, 1, 0);
glm::vec3 Right = glm::normalize(glm::cross(front, worldUp));
glm::vec3 axis;

/* Render the scene with openGL */
/* Edit this function according to your assignment */
void draw() {
    // clear the color and depth in the frame buffer
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // use the loaded shader program
    // Don't change unless you know what you are doing
    glUseProgram (programID);

    // // Eye - Location of camera. Don't change unless you are sure!!
    // glm::vec3 eye ( 5*cos(camera_rotation_angle*M_PI/180.0f), 0, 5*sin(camera_rotation_angle*M_PI/180.0f) );
    // // Target - Where is the camera looking at.  Don't change unless you are sure!!
    // glm::vec3 target (0, 0, 0);
    // // Up - Up vector defines tilt of camera.  Don't change unless you are sure!!
    // glm::vec3 up (0, 1, 0);

    // Compute Camera matrix (view)
    Matrices.view = glm::lookAt( eye, eye + front, up ); // Rotating Camera for 3D
    // Don't change unless you are sure!!
    // Matrices.view = glm::lookAt(glm::vec3(0, 0, 3), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0)); // Fixed camera for 2D (ortho) in XY plane

    // Compute ViewProject matrix as view/camera might not be changed for this frame (basic scenario)
    // Don't change unless you are sure!!
    glm::mat4 VP = Matrices.projection * Matrices.view;

    // Send our transformation to the currently bound shader, in the "MVP" uniform
    // For each model you render, since the MVP will be different (at least the M part)
    // Don't change unless you are sure!!
    glm::mat4 MVP;  // MVP = Projection * View * Model

    // Scene render
    ball1.draw(VP);
}

void updateCamera(){
    front = glm::normalize(ball1.position - eye);
    Right = glm::normalize(glm::cross(front, worldUp));
    up = glm::normalize(glm::cross(Right, front));
}

void updateAxis(){
    updateCamera();
    axis = up;
}

void cameraObjectRotation(){
    if(cameraRotate){
        glm::mat4 rotateMatrix = glm::rotate(glm::radians(1.0f), axis);
        glm::mat4 t1 = glm::translate(-ball1.position);
        glm::mat4 t2 = glm::translate(ball1.position);
        eye = glm::vec3(t2 * rotateMatrix * t1 * glm::vec4(eye, 1));
        target = glm::vec3(ball1.position);
        updateCamera();
    }
}

void tick_input(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS){
        glfwSetWindowShouldClose(window, true);
    }

    // Camera Movement
    // const float cameraSpeed = 0.05f; 
    // if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS){
    //     eye.y -= cameraSpeed;
    //     target.y -= cameraSpeed;
    // }
    // if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS){
    //     eye.y += cameraSpeed;
    //     target.y += cameraSpeed;
    // }
    // if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS){
    //     eye.x -= cameraSpeed;
    //     target.x -= cameraSpeed;
    // }
    // if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS){
    //     eye.x += cameraSpeed;
    //     target.x += cameraSpeed;
    // }
    // if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS){
    //     eye.z += cameraSpeed;
    //     target.z += cameraSpeed;
    // }
    // if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS){
    //     eye.z -= cameraSpeed;
    //     target.z -= cameraSpeed;
    // }

    const float cameraSpeed = 0.05f; 
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS){
        eye -= up * cameraSpeed;
        // target -= up * cameraSpeed;
        if (cameraRotate)
            updateAxis();
    }
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS){
        eye += up * cameraSpeed;
        // target += up * cameraSpeed;
        if (cameraRotate)
            updateAxis();
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS){
        eye -= Right * cameraSpeed;
        // target -= Right * cameraSpeed;
        if (cameraRotate)
            updateAxis();
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS){
        eye += Right * cameraSpeed;
        // target += Right * cameraSpeed;
        if (cameraRotate)
            updateAxis();
    }
    if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS){
        eye += front * cameraSpeed;
        // target += front * cameraSpeed;
        if (cameraRotate)
            updateAxis();
    }
    if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS){
        eye -= front * cameraSpeed;
        // target -= front * cameraSpeed;
        if (cameraRotate)
            updateAxis();
    }

    // Ball Movement
    const float ballSpeed = 0.05f; 
    if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS){
        ball1.position.y += ballSpeed;
        if (cameraRotate)
            updateAxis();
    }
    if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS){
        ball1.position.y -= ballSpeed;
        if (cameraRotate)
            updateAxis();
    }
    if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS){
        ball1.position.x -= ballSpeed;
        if (cameraRotate)
            updateAxis();
    }
    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS){
        ball1.position.x += ballSpeed;
        if (cameraRotate)
            updateAxis();
    }
    if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS){
        ball1.position.z -= ballSpeed;
        if (cameraRotate)
            updateAxis();
    }
    if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS){
        ball1.position.z += ballSpeed;
        if (cameraRotate)
            updateAxis();
    }

    // Teleportation
    if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS){
        eye = glm::vec3(0, 0, 10);
        // target = glm::vec3(ball1.position);
        updateCamera();
    }

    if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS){
        eye = glm::vec3(5, 0, 5);
        // target = glm::vec3(ball1.position);
        updateCamera();
    }

    if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS){
        eye = glm::vec3(10, 0, 0);
        // target = glm::vec3(ball1.position);
        updateCamera();
    }

    // Rotation of Camera
    // if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS){
    //     if (!cameraRotate){
    //         cameraRotate = 1;
    //         updateAxis();
    //     }
    //     if(cameraRotate){
    //         // glm::vec3 axis = glm::normalize(glm::cross(ball1.position - eye, Right));
    //         glm::mat4 rotateMatrix = glm::rotate(glm::radians(1.0f), axis);
    //         glm::mat4 t1 = glm::translate(-ball1.position);
    //         glm::mat4 t2 = glm::translate(ball1.position);
    //         eye = glm::vec3(t2 * rotateMatrix * t1 * glm::vec4(eye, 1));
    //         target = glm::vec3(ball1.position);
    //         updateCamera();
    //     }
    // }

    // if (glfwGetKey(window, GLFW_KEY_T) == GLFW_RELEASE){
    //     cameraRotate = !cameraRotate;
    // }

    // Rotation of Ball
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS){
        ball1.isRotate = 1;
    }

    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_RELEASE){
        ball1.isRotate = 0;
    }
}

void cameraSpin(){
    cameraRotate = !cameraRotate;
    if(cameraRotate){
        updateAxis();
    }
}

void tick_elements() {
    if (ball1.isRotate)
        ball1.tick();
    if (cameraRotate){
        cameraObjectRotation();
    }
    camera_rotation_angle += 1;
}

/* Initialize the OpenGL rendering properties */
/* Add all the models to be created here */
void initGL(GLFWwindow *window, int width, int height, int indexSolid) {
    /* Objects should be created before any other gl function and shaders */
    // Create the models

    ball1       = Ball(0, 0, 0, indexSolid);

    // Create and compile our GLSL program from the shaders
    programID = LoadShaders("../source/shaders/shader.vert", "../source/shaders/shader.frag");
    // Get a handle for our "MVP" uniform
    Matrices.MatrixID = glGetUniformLocation(programID, "MVP");


    reshapeWindow (window, width, height);

    // Background color of the scene
    glClearColor (COLOR_BACKGROUND.r / 256.0, COLOR_BACKGROUND.g / 256.0, COLOR_BACKGROUND.b / 256.0, 0.0f); // R, G, B, A
    glClearDepth (1.0f);

    glEnable (GL_DEPTH_TEST);
    glDepthFunc (GL_LEQUAL);

    cout << "VENDOR: " << glGetString(GL_VENDOR) << endl;
    cout << "RENDERER: " << glGetString(GL_RENDERER) << endl;
    cout << "VERSION: " << glGetString(GL_VERSION) << endl;
    cout << "GLSL: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << endl;
}


int main(int argc, char **argv) {
    srand(time(0));
    int width  = 600;
    int height = 600;

    cout << "Options:\n" << "1. Undecagonal Pyramid\n" << "2. Hexagonal Dipyramid\n" << "3. Elongated Square Dipyramid\n" << "4. Dodecahedron (FUN)\n" << "-------------------\n\n";

    do{
        cout << "Enter the Solid No from (1, 2, 3, 4): ";
        cin >> indexSolid;
    } while(indexSolid != 1 && indexSolid != 2 && indexSolid != 3 && indexSolid != 4);
    cout << "You chose solid number " << indexSolid << "\n";

    window = initGLFW(width, height);

    initGL (window, width, height, indexSolid);

    /* Draw in loop */
    while (!glfwWindowShouldClose(window)) {
        // Process timers

        if (t60.processTick()) {
            // 60 fps
            // OpenGL Draw commands
            draw();
            // Swap Frame Buffer in double buffering
            glfwSwapBuffers(window);

            tick_elements();
            tick_input(window);
        }

        // Poll for Keyboard and mouse events
        glfwPollEvents();
    }

    quit(window);
}

bool detect_collision(bounding_box_t a, bounding_box_t b) {
    return (abs(a.x - b.x) * 2 < (a.width + b.width)) &&
           (abs(a.y - b.y) * 2 < (a.height + b.height));
}

void reset_screen() {
    float top    = screen_center_y + 4 / screen_zoom;
    float bottom = screen_center_y - 4 / screen_zoom;
    float left   = screen_center_x - 4 / screen_zoom;
    float right  = screen_center_x + 4 / screen_zoom;
    // Matrices.projection = glm::ortho(left, right, bottom, top, 0.1f, 500.0f);
    Matrices.projection = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 500.0f);
}
