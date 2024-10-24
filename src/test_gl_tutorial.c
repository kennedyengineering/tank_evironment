// Learning from https://learnopengl.com/Getting-started/
// Hello Window X
// Hello Triangle 

/* NOTES */
/*
OpenGL uses normalized device coordinates (NDC) (-1 to 1), ignore coordinates outside of that range
It is a 3D graphics library, so all coordinates are in 3D.
NDC coordinates are transformed to screen-space coordinates via glViewport.

*/

#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdbool.h>

// Adjust viewport when user resizes window
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
} 

// Process user input
void processInput (GLFWwindow *window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)   // else is GLFW_RELEASE
        glfwSetWindowShouldClose(window, true);
}

// Draw a triangle
void renderTriangle() {

    // Define vertex data
    float vertices[] = {
        -0.5f, -0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
        0.0f,  0.5f, 0.0f
    };

    // Create vertex buffer object
    unsigned int VBO;
    glGenBuffers(1, &VBO);

    // Bind buffer object to the GL_ARRAY_BUFFER target
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    // Copy vertex data to buffer memory
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);      // GL_STATIC_DRAW specifies how we want the graphics card to manage to given data.
                                                                                    // Is static because position does not change, if changes frequently use GL_DYNAMIC_DRAW

    // Define vertex shader in GLSL
    const char *vertexShaderSource = "#version 450 core\n"      // begin with declaration of version and if using core functionality
        "layout (location = 0) in vec3 aPos;\n"                 // declare input vertex attributes, create a vec3 (3 floats, vec1 to vec4 are valid) named aPos
        "void main()\n"
        "{\n"
        "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n" // set the output of the vertex shader, whatever gl_Position is set to is the output of the vertex shader (is vec4)
        "}\0";

    // Create vertex shader
    unsigned int vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);    // GL_VERTEX_SHADER is type we want to create

    // Attach shader source code to the shader object
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);

    // Compile shader
    glCompileShader(vertexShader);

    // Check for compilation success
    int  success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        printf("ERROR, SHADER VERTEX COMPILATION FAILED:\n %s", infoLog);
    }

    // Define fragment shader in GLSL
    const char *fragmentShaderSource = "#version 450 core\n"
        "out vec4 FragColor;\n"                                 // only requires one output variable (vec4) that defines the final color output
        "void main()\n"
        "{\n"
        "   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"        
        "}\0";

    // Create, attach source, and compile shader
    unsigned int fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    // Create a shader program (link shader outputs to other shader inputs to create a pipeline)
    unsigned int shaderProgram;
    shaderProgram = glCreateProgram();

    // Attach shaders
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);

    // Link shaders together
    glLinkProgram(shaderProgram);

    // Check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if(!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        printf("ERROR, SHADER PROGRAM LINKING FAILED:\n %s", infoLog);
    }

    // Activate program
    glUseProgram(shaderProgram);

    // Delete shader objects after linking to program object
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

}



int main()
{
    // Initialize GLFW
    glfwInit();

    // Configure GLFW 
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);  // I Have OpenGL version 4.5 installed
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create GLFW window object
    GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        printf("Failed to create GLFW window");
        glfwTerminate();
        return -1;
    }

    // Make the context of out window the main context on the current thread
    glfwMakeContextCurrent(window);

    // Tell OpenGL the size of the rendering window
    glViewport(0, 0, 800, 600); // (lower left corner coordinate x, y, window width, height in pixels)
                                // 2D coordinates are mapped from (-1, 1) to (0, 800) and (0, 600)
    
    // Attach resize callback to GLFW
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // Define render loop
    while(!glfwWindowShouldClose(window))   // check if GLFW is instructed to close
    {
        // process user input
        processInput(window);

        // render
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // check and call events and swap buffers
        glfwSwapBuffers(window);            // show output to screen
        glfwPollEvents();                   // check for keyboard events, update window state, call callback functions
    }

    // Delete GLFW resources
    glfwTerminate();

    return 0;
}