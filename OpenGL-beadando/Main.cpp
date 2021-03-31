/**
    @author Balazs Orehovszki
*/

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <shader_s.h>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

void rotatePolygon(float rotation);
void scalePolygon(float scale);
void movePolygon(int direction);

// Settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// Default display mode
int displayMode = GL_FILL;

// Define the vertices
float vertices[] = {
     0.0f,   0.9f,  0.0f,
     0.55f,  0.2f,  0.0f,
     0.4f,  -0.5f,  0.0f,
    -0.4f,  -0.5f,  0.0f, 
    -0.55f,  0.2f,  0.0f
};

// Define the triangles
unsigned int indices[] = {
    0, 3, 4,    // first triangle
    0, 3, 2,    // second triangle
    0, 1, 2     // third triangle
};

unsigned int shaderID;
glm::mat4 transform = glm::mat4(1.0f); // Identity matrix

int main()
{
    // GLFW: initialize and configure
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // GLFW window creation
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Orehovszki Balazs | THORXS", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    glfwSetKeyCallback(window, key_callback);

    // GLAD: load all OpenGL function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // Build and compile the shader programs
    Shader shader("vertex.glsl", "fragment.glsl");
    shaderID = shader.ID;

    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    // Bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*) 0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);

    // Set the default display mode
    glPolygonMode(GL_FRONT_AND_BACK, displayMode);

    // Render loop
    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT);

        shader.use();

        // Get matrix uniform location and set matrix
        unsigned int transformLoc = glGetUniformLocation(shaderID, "transform");
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));

        glBindVertexArray(VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized

        // Render the triangles
        glDrawElements(GL_TRIANGLES, sizeof(indices), GL_UNSIGNED_INT, 0);

        // GLFW: swap buffers and poll IO events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // De-allocate all resources
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    // GLFW: terminate, clearing all previously allocated GLFW resources.
    glfwTerminate();
    return 0;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    // TODO: Implement flags to make transformations go smoother.

    // V - Change the display mode
    if (key == GLFW_KEY_V && action == GLFW_PRESS)
    {
        switch (displayMode) {
        case GL_LINE:
            displayMode = GL_POINT;
            break;
        case GL_FILL:
            displayMode = GL_LINE;
            break;
        case GL_POINT:
            displayMode = GL_FILL;
            break;
        default:
            std::cout << "Invalid display mode!" << std::endl;
            break;
        }

        glPolygonMode(GL_FRONT_AND_BACK, displayMode);
    }

    // Q - Rotate left
    if (key == GLFW_KEY_Q)
    {
        rotatePolygon(1.0f);
    }
    // E - Rotate right
    if (key == GLFW_KEY_E)
    {
        rotatePolygon(-1.0f);
    }

    // R - Scale Up
    if (key == GLFW_KEY_R && action == GLFW_PRESS)
    {
        scalePolygon(1.1f);
    }
    // F - Scale down
    if (key == GLFW_KEY_F && action == GLFW_PRESS)
    {
        scalePolygon(0.9f);
    }

    // W - Move forward
    if (key == GLFW_KEY_W)
    {
        movePolygon(1);
    }
    // A - Move left
    if (key == GLFW_KEY_A)
    {
        movePolygon(2);
    }
    // S - Move backward
    if (key == GLFW_KEY_S)
    {
        movePolygon(3);
    }
    // D - Move right
    if (key == GLFW_KEY_D)
    {
        movePolygon(4);
    }
    // TODO: Support to move across

    if (key == GLFW_KEY_C)
    {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f); // Change background color
        glClear(GL_COLOR_BUFFER_BIT);
    }
}

// Rotate the polygon in a specific direction
void rotatePolygon(float rotation)
{
    transform = glm::rotate(transform, glm::radians(1.0f), glm::vec3(0.0f, 0.0f, rotation));
}

// Scale the polygon by a given amount
void scalePolygon(float scaleAmount)
{
    transform = glm::scale(transform, glm::vec3(scaleAmount, scaleAmount, scaleAmount));
}

void movePolygon(int direction)
{
    glm::vec3 vector;

    // Forward
    if (direction == 1)
    {
        vector = glm::vec3(0.0f, 0.01f, 0.0f);
    }
    // Left
    if (direction == 2)
    {
        vector = glm::vec3(-0.01f, 0.0f, 0.0f);
    }
    // Down
    if (direction == 3)
    {
        vector = glm::vec3(0.0f, -0.01f, 0.0f);
    }
    // Backward
    if (direction == 4)
    {
        vector = glm::vec3(0.01f, 0.0f, 0.0f);
    }
    
    transform = glm::translate(transform, vector);
}

// GLFW: whenever the window size changed (by OS or user resize) this callback function executes
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions
    glViewport(0, 0, width, height);
}