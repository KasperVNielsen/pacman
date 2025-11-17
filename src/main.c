
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define GLFW_INCLUDE_NONE
#include <glad/glad.h>
#include <GLFW/glfw3.h>

/*  Vertex & Fragment Shaders  */
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
/* Wall struct & helpers */
typedef struct { float x, y; float halfW, halfH; } Rect;


static int rects_overlap(float ax, float ay, float aHalfX, float aHalfY,
float bx, float by, float bHalfX, float bHalfY) {

float aVisHalfX = aHalfX * 0.5f;
float aVisHalfY = aHalfY * 0.5f;
float bVisHalfX = bHalfX * 0.5f;
float bVisHalfY = bHalfY * 0.5f;


return (fabsf(ax - bx) < (aVisHalfX + bVisHalfX)) && (fabsf(ay - by) < (aVisHalfY + bVisHalfY));
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

    /*  Unit square geometry (centered at origin, size 1)  */
    float half_unit = 1.0f;
    float vertices[] = {
        -0.5f, -0.5f,
         0.5f, -0.5f,
         0.5f,  0.5f,
        -0.5f,  0.5f
    };
    unsigned int indices[] = {0,1,2, 2,3,0};

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

    /*  Compile shaders  */
    GLuint vsh = compile_shader(GL_VERTEX_SHADER, vertex_src);
    GLuint fsh = compile_shader(GL_FRAGMENT_SHADER, fragment_src);
    if (!vsh || !fsh) { return EXIT_FAILURE; }
    GLuint program = link_program(vsh, fsh);
    glDeleteShader(vsh);
    glDeleteShader(fsh);
    if (!program) return EXIT_FAILURE;

    GLint loc_uOffset = glGetUniformLocation(program, "uOffset");
    GLint loc_uScale  = glGetUniformLocation(program, "uScale");
    GLint loc_uColor  = glGetUniformLocation(program, "uColor");

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    /* coordinate space is NDC (-1..1). */
Rect walls[] = {
    /* outer frame */
    { -0.95f,  0.0f, 0.05f, 0.95f },  // left outer
    {  0.95f,  0.0f, 0.05f, 0.95f },  // right outer
    {  0.00f,  0.95f, 0.90f, 0.05f },  // top outer
    {  0.00f, -0.95f, 0.90f, 0.05f },  // bottom outer

    /* top corridor bars */
    { -0.50f,  0.72f, 0.30f, 0.04f },  // left top horizontal
    {  0.50f,  0.72f, 0.30f, 0.04f },  // right top horizontal

    /* bottom corridor bars */
    { -0.50f, -0.72f, 0.30f, 0.04f },  // left bottom horizontal
    {  0.50f, -0.72f, 0.30f, 0.04f },  // right bottom horizontal

    /* vertical quarter dividers (create the long corridors) */
    { -0.65f,  0.20f, 0.04f, 0.40f },  // left upper vertical
    { -0.65f, -0.20f, 0.04f, 0.40f },  // left lower vertical
    {  0.65f,  0.20f, 0.04f, 0.40f },  // right upper vertical
    {  0.65f, -0.20f, 0.04f, 0.40f },  // right lower vertical

    /* center chamber */
    {  0.00f,  0.00f, 0.16f, 0.12f },  // central house 

    /* small separators near center to form choke points */
    { -0.28f,  0.12f, 0.10f, 0.03f },
    { -0.28f, -0.12f, 0.10f, 0.03f },
    {  0.28f,  0.12f, 0.10f, 0.03f },
    {  0.28f, -0.12f, 0.10f, 0.03f },

    /* corner chambers  */
    { -0.80f,  0.80f, 0.12f, 0.12f },
    {  0.80f,  0.80f, 0.12f, 0.12f },
    { -0.80f, -0.80f, 0.12f, 0.12f },
    {  0.80f, -0.80f, 0.12f, 0.12f }
};
const int wall_count = (int)(sizeof(walls)/sizeof(walls[0]));

typedef struct { float x, y, r; } Pellet;

Pellet pellets[] = {
    { -0.7f,  0.7f, 0.02f },
    { -0.5f,  0.7f, 0.02f },
    { -0.3f,  0.7f, 0.02f },
    // add more
};
const int pellet_count = sizeof(pellets) / sizeof(pellets[0]);

    /*  player square  */
    float half = 0.05f;  /* half-size of player square (side = 0.1) */
    float posX = 0.3f, posY = 0.3f; /* start bottom-left area */
    const float speed = 1.2f; /* NDC units per second */

    double lastTime = glfwGetTime();
    printf("Square + walls running. Arrow keys move it; ESC quits.\n");

    while (!glfwWindowShouldClose(window)) {
        double now = glfwGetTime();
        float dt = (float)(now - lastTime);
        lastTime = now;

        glfwPollEvents();

        int up    = glfwGetKey(window, GLFW_KEY_UP)    == GLFW_PRESS;
        int down  = glfwGetKey(window, GLFW_KEY_DOWN)  == GLFW_PRESS;
        int leftk = glfwGetKey(window, GLFW_KEY_LEFT)  == GLFW_PRESS;
        int rightk= glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS;
        int quit  = glfwGetKey(window, GLFW_KEY_ESCAPE)== GLFW_PRESS;

        printf("Up:%d Down:%d Left:%d Right:%d Quit:%d\r", up, down, leftk, rightk, quit);
        fflush(stdout);

        if (quit) { glfwSetWindowShouldClose(window, GLFW_TRUE); break; }

        /* compute desired movement (NDC) */
        float dx = 0.0f, dy = 0.0f;
        if (leftk)  dx -= speed * dt;
        if (rightk) dx += speed * dt;
        if (up)     dy += speed * dt;
        if (down)   dy -= speed * dt;

        /* attempt X movement, check collision with walls */
        float newX = posX + dx;
        int collideX = 0;
        for (int i = 0; i < wall_count; ++i) {
            if (rects_overlap(newX, posY, half, half, walls[i].x, walls[i].y, walls[i].halfW, walls[i].halfH)) {
                collideX = 1; break;
            }
        }
        if (!collideX) posX = newX;

        /* attempt Y movement, check collision */
        float newY = posY + dy;
        int collideY = 0;
        for (int i = 0; i < wall_count; ++i) {
            if (rects_overlap(posX, newY, half, half, walls[i].x, walls[i].y, walls[i].halfW, walls[i].halfH)) {
                collideY = 1; break;
            }
        }
        if (!collideY) posY = newY;

        /* clamp to NDC so player stays in -1..1 */
        float limit = 1.0f - half;
        if (posX > limit) posX = limit;
        if (posX < -limit) posX = -limit;
        if (posY > limit) posY = limit;
        if (posY < -limit) posY = -limit;

        /* render */
        int fbw, fbh;
        glfwGetFramebufferSize(window, &fbw, &fbh);
        glViewport(0, 0, fbw, fbh);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(program);

        /* draw walls (blue) */
        for (int i = 0; i < wall_count; ++i) {
            glUniform2f(loc_uOffset, walls[i].x, walls[i].y);
            glUniform2f(loc_uScale, walls[i].halfW * 1.0f, walls[i].halfH * 1.0f); // scale is size
            glUniform3f(loc_uColor, 0.0f, 0.0f, 1.0f);
            glBindVertexArray(VAO);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        }
        /* draw pellets (yellow) */
        for (int i = 0; i < pellet_count; ++i) {
            glUniform2f(loc_uOffset, pellets[i].x, pellets[i].y);
            glUniform2f(loc_uScale, pellets[i].r, pellets[i].r);
            glUniform3f(loc_uColor, 1.0f, 1.0f, 0.0f); // yellow
            glBindVertexArray(VAO);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        }

        /* draw player (white) */
        glUniform2f(loc_uOffset, posX, posY);
        glUniform2f(loc_uScale, half * 1.0f, half * 1.0f);
        glUniform3f(loc_uColor, 1.0f, 1.0f, 0.0f);
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
