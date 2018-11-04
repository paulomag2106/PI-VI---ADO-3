#include "text.h"

GLFWwindow *window;
int width, height, prevWidth, prevHeight;

int main() {
    srand((int)time(NULL));
    xOffset = yOffset = 0.f;
    zoom = WIDTH/10 + 50;
    v3 camerapos = {0,0,zoom};

    weightV2 *points = malloc(sizeof(weightV2));
    genWeightedBlueNoise(points, 30, 512, 20);
    free(points);

    width = WIDTH;
    height = HEIGHT;

    // Initialize GLFW
    if(!glfwInit()) {
        puts("Error loading GLFW.");
        return -1;
    }

    // Parametrize GLFW with Number of Samples(MultiSampling) and OpenGL Version
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Initialize window
    window = glfwCreateWindow(width, height, "PI VI - ADO 3", NULL, NULL);
    if(!window) {
        puts("Error opening GLFW window.");
        glfwTerminate();
        return -1;
    }

    // Set window as active, fix ratio and set Swap Interval(VSYNC)
    glfwMakeContextCurrent(window);
    glfwSetWindowAspectRatio(window, 16, 9);
    glfwSwapInterval(1);

    // Initialize GLEW
    if(glewInit() != GLEW_OK) {
        puts("Error loading GLEW.");
        glfwTerminate();
        return -1;
    }

    // Set glfwInputMode
    //glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

    // Set Clear color
    glClearColor(0.153f, 0.067f, 0.200f, 0.f);

    glDisable(GL_DITHER | GL_MULTISAMPLE);
    glDisable(GL_DEPTH_CLAMP);

    // Enable depth test
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    // Enable face culling
    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CCW);
    glCullFace(GL_BACK);

    // Enable blend test
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Create and compile our GLSL program from the shaders
    programID = loadShaders("vertex.shader", "fragment.shader");
    glUseProgram(programID);

    // Perspective Projection matrix
    float FOV = M_PI/4.f;
    float near = 0.5f;
    float far = 200.f;
    float ratio = (float)width/height;
    m4x4 Projection = perspective(FOV, near, far, ratio);

    // Push Projection Matrix to GLSL
    GLuint matrixID = glGetUniformLocation(programID, "Projection");
    glUniformMatrix4fv(matrixID, 1, GL_TRUE, (GLfloat *)&Projection);

    // View Matrix
    m4x4 View = loadIdm4x4();
    GLuint ViewID = glGetUniformLocation(programID, "View");

    // Set Keys and Mouse functions callback on GLFW
    glfwSetKeyCallback(window, (void *)key_callback);
    glfwSetCursorPosCallback(window, (void *)cursor_position_callback);
    glfwSetScrollCallback(window, (void *)scroll_callback);

    // Create Map
    object map = makeShapeObject(RECT, (v3){WIDTH/20, WIDTH/20, 0.f}, (v3){1.f,1.f,1.f}, NULL,
                                 GL_STATIC_DRAW, 0);

    //////////////////////////////////////////////////////////////////////////////
    //                                MAIN LOOP                                 //
    //////////////////////////////////////////////////////////////////////////////

    while(!glfwGetKey(window, GLFW_KEY_ESCAPE) &&
    !glfwWindowShouldClose(window)) {

        glfwGetWindowSize(window, &width, &height);
        glViewport(0,0, 2*width, 2*height);

        // Get deltaTime and FPS
        double currentTime = glfwGetTime();
        deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        // Zoom View Matrix
        camerapos.z = zoom;
        View = lookAt(camerapos,
                      (v3){camerapos.x,camerapos.y,0},(v3){0,1,0});
        // Update View Matrix
        glUniformMatrix4fv(ViewID, 1, GL_TRUE, (GLfloat *)&View);

        // Clear screen
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Draw Objects
        drawObject(&map);

        glfwSwapBuffers(window);
        glfwPollEvents();

    }

    glDeleteProgram(programID);

    // Free objects
    freeObject(&map);

    // Free textobjects

    // Close window
    glfwTerminate();

    return 0;
}