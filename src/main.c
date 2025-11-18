// src/main.c
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define GLFW_INCLUDE_NONE
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "game.h"
#include "input.h"

/* Vertex & Fragment Shaders (same as before) */
static const char *vertex_src =
"#version 330 core\n"
"layout(location = 0) in vec2 aPos;\n"
"uniform vec2 uOffset;\n"
"uniform vec2 uScale;\n"
"void main() {\n"
"    vec2 p = aPos * uScale + uOffset;\n"
"    gl_Position = vec4(p, 0.0, 1.0);\n"
"}\n";

static const char *fragment_src =
"#version 330 core\n"
"uniform vec3 uColor;\n"
"out vec4 FragColor;\n"
"void main() {\n"
"    FragColor = vec4(uColor, 1.0);\n"
"}\n";

/* Shader helpers */
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

int main(void)
{
    if (!glfwInit()) {
        fprintf(stderr, "GLFW init failed\n");
        return EXIT_FAILURE;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(1280, 720, "pman", NULL, NULL);
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

    /* Unit quad geometry (centered at origin, coordinates in [-0.5, +0.5]) */
    float vertices[] = {
        -0.5f, -0.5f,
         0.5f, -0.5f,
         0.5f,  0.5f,
        -0.5f,  0.5f
    };
    unsigned int indices[] = {0,1,2, 2,3,0};

    GLuint VAO = 0, VBO = 0, EBO = 0;
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

    /* Compile & link shaders */
    GLuint vsh = compile_shader(GL_VERTEX_SHADER, vertex_src);
    GLuint fsh = compile_shader(GL_FRAGMENT_SHADER, fragment_src);
    if (!vsh || !fsh) {
        fprintf(stderr, "shader compile failed\n");
        return EXIT_FAILURE;
    }
    GLuint program = link_program(vsh, fsh);
    glDeleteShader(vsh);
    glDeleteShader(fsh);
    if (!program) {
        fprintf(stderr, "program link failed\n");
        return EXIT_FAILURE;
    }

    /* Get uniform locations */
    GLint loc_uOffset = glGetUniformLocation(program, "uOffset");
    GLint loc_uScale  = glGetUniformLocation(program, "uScale");
    GLint loc_uColor  = glGetUniformLocation(program, "uColor");

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    /* Initialize game (passes program & VAO so game_render can use them) */
    Game game;
    if (!game_init(&game, program, VAO)) {
        fprintf(stderr, "game init failed\n");
        // cleanup
        glDeleteProgram(program);
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
        glfwDestroyWindow(window);
        glfwTerminate();
        return EXIT_FAILURE;
    }

    InputState inp = {0};
    double lastTime = glfwGetTime();

    while (!glfwWindowShouldClose(window)) {
        double now = glfwGetTime();
        float dt = (float)(now - lastTime);
        lastTime = now;

        glfwPollEvents();
        input_poll(window, &inp);
        if (inp.quit) { glfwSetWindowShouldClose(window, GLFW_TRUE); break; }

        game_update(&game, dt, inp.up, inp.down, inp.left, inp.right);

        int fbw, fbh;
        glfwGetFramebufferSize(window, &fbw, &fbh);
        glViewport(0, 0, fbw, fbh);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(program);
        game_render(&game, loc_uOffset, loc_uScale, loc_uColor);

        glfwSwapBuffers(window);
    }

    game_shutdown(&game);

    /* cleanup GL objects */
    glDeleteProgram(program);
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
