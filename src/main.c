#include <stdio.h>
#include <stdlib.h>

#define GLFW_INCLUDE_NONE
#include <glad/glad.h>
#include <GLFW/glfw3.h>

/*  Vertex & Fragment Shaders (white square)  */
static const char *vertex_src =
"#version 330 core\n"
"layout(location = 0) in vec2 aPos;\n"
"void main() {\n"
"    gl_Position = vec4(aPos.xy, 0.0, 1.0);\n"
"}\n";

static const char *fragment_src =
"#version 330 core\n"
"out vec4 FragColor;\n"
"void main() {\n"
"    FragColor = vec4(1.0, 1.0, 1.0, 1.0);\n"
"}\n";

/*  Shader helpers  */
static GLuint compile_shader(GLenum type, const char *src) {
    GLuint s = glCreateShader(type);
    glShaderSource(s, 1, &src, NULL);
    glCompileShader(s);
    GLint ok = 0;
    glGetShaderiv(s, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        char log[512];
        glGetShaderInfoLog(s, sizeof(log), NULL, log);
        fprintf(stderr, "Shader compile error: %s\n", log);
        glDeleteShader(s);
        return 0;
    }
    return s;
}

static GLuint link_program(GLuint v, GLuint f) {
    GLuint p = glCreateProgram();
    glAttachShader(p, v);
    glAttachShader(p, f);
    glLinkProgram(p);
    GLint ok = 0;
    glGetProgramiv(p, GL_LINK_STATUS, &ok);
    if (!ok) {
        char log[512];
        glGetProgramInfoLog(p, sizeof(log), NULL, log);
        fprintf(stderr, "Program link error: %s\n", log);
        glDeleteProgram(p);
        return 0;
    }
    return p;
}

/*  main  */
int main(void) {
    if (!glfwInit()) {
        fprintf(stderr, "GLFW init failed\n");
        return EXIT_FAILURE;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(640, 480, "Small White Square + Input Logger", NULL, NULL);
    if (!window) {
        fprintf(stderr, "GLFW window failed\n");
        glfwTerminate();
        return EXIT_FAILURE;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        fprintf(stderr, "Failed to load GLAD\n");
        glfwDestroyWindow(window);
        glfwTerminate();
        return EXIT_FAILURE;
    }

    /*  Small Square Geometry  */
    float half = 0.05f;  //
    float vertices[] = {
        -half, -half,
         half, -half,
         half,  half,
        -half,  half
    };
    unsigned int indices[] = { 0, 1, 2, 2, 3, 0 };

    GLuint VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    GLuint vsh = compile_shader(GL_VERTEX_SHADER, vertex_src);
    GLuint fsh = compile_shader(GL_FRAGMENT_SHADER, fragment_src);
    GLuint program = link_program(vsh, fsh);
    glDeleteShader(vsh);
    glDeleteShader(fsh);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    printf("Small white square running. Press arrow keys or ESC — check console.\n");

    /*  Main loop  */
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        int up    = glfwGetKey(window, GLFW_KEY_UP)    == GLFW_PRESS;
        int down  = glfwGetKey(window, GLFW_KEY_DOWN)  == GLFW_PRESS;
        int left  = glfwGetKey(window, GLFW_KEY_LEFT)  == GLFW_PRESS;
        int right = glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS;
        int quit  = glfwGetKey(window, GLFW_KEY_ESCAPE)== GLFW_PRESS;

        printf("Up:%d Down:%d Left:%d Right:%d Quit:%d\r", up, down, left, right, quit);
        fflush(stdout);

        if (quit) glfwSetWindowShouldClose(window, GLFW_TRUE);

        glClear(GL_COLOR_BUFFER_BIT);
        glUseProgram(program);
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
    }

    printf("\nExiting...\n");

    glDeleteProgram(program);
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    glfwDestroyWindow(window);
    glfwTerminate();
    return EXIT_SUCCESS;
}
