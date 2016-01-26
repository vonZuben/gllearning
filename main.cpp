#define PI 3.14159

#define GLEW_STATIC
#include <GL/glew.h>

#include <GLFW/glfw3.h>
#include <iostream>

#include <math.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
//#include <glm/gtc/matrix_transform.hpp>
//#include <glm/gtc/type_ptr.hpp>

int shouldDraw;

int w, h;

static void wMove(GLFWwindow* window, int x, int y){
    shouldDraw = 0;
}

void setView(){
    glm::mat4 view;
    view = glm::perspective((float)PI/2.0f, (float)w/h, 0.01f, 100.0f);
    glm::vec3 pos = glm::vec3(0.0, 0.0, 1.0);
    view *= glm::lookAt(pos, pos + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, 1.0, 0.0));

    glUniformMatrix4fv(6, 1, GL_FALSE, &view[0][0]);
}

static void wResize(GLFWwindow* window, int width, int height){
    glViewport(0,0,width,height);
    w = width;
    h= height;
    setView();
}

void setRot(float xangle, float yangle, float zangle){
    float zrMat[] = {
        (float)cos(zangle), (float)-sin(zangle), 0, 0,
        (float)sin(zangle), (float)cos(zangle), 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
    };

    glUniformMatrix4fv(3, 1, GL_FALSE, zrMat);

    float yrMat[] = {
        (float)cos(yangle), 0, (float)-sin(yangle), 0,
        0, 1, 0, 0,
        (float)sin(yangle), 0, (float)cos(yangle), 0,
        0, 0, 0, 1
    };

    glUniformMatrix4fv(4, 1, GL_FALSE, yrMat);

    float xrMat[] = {
        1, 0, 0, 0,
        0, (float)cos(xangle), (float)-sin(xangle), 0,
        0, (float)sin(xangle), (float)cos(xangle), 0,
        0, 0, 0, 1
    };

    glUniformMatrix4fv(5, 1, GL_FALSE, xrMat);
}

void mulFLoatArray(unsigned int aSize, float* array, float factor){
    for (unsigned int i = 0; i < aSize; i++)
        array[i]*=factor;
}


int main(int argc, char** argv){

    shouldDraw = 1;

    glfwInit();

    GLFWwindow* mainWindw = glfwCreateWindow(1000, 800, "Opengl", NULL, NULL);
    w = 1000;
    h = 800;
    //GLFWwindow* mainWindw = glfwCreateWindow(1920, 1080, "Opengl", glfwGetPrimaryMonitor(), NULL);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    glfwMakeContextCurrent(mainWindw);

    glewInit();

    const char* vSrc[] = {
        "#version 440 \n \
            layout(location=2)in vec3 position; \
            layout(location=1)uniform float ma; \
            layout(location=3)uniform mat4 zrot; \
            layout(location=4)uniform mat4 yrot; \
            layout(location=5)uniform mat4 xrot; \
            layout(location=6)uniform mat4 view; \
            out vec4 pcolor; \
            void main() \
            { \
                gl_Position = view * zrot * yrot * xrot * vec4(position, 1.0); \
                pcolor = vec4(1 - pow(ma-1, 2), 1 - pow(ma-3, 2), 1 - pow(ma-5, 2), 1.0); \
            } \
        "
    };

                //color = vec4(1.0, 0.0 , 0.0, 1.0);
    const char* fSrc[] = {
        "#version 440 \n \
            out vec4 color; \
            in vec4 pcolor; \
            void main() \
            { \
                color = pcolor; \
            } \
        "
    };

    GLuint vShader = glCreateShader(GL_VERTEX_SHADER);
    GLuint fShader = glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(vShader, 1, vSrc, NULL);
    glShaderSource(fShader, 1, fSrc, NULL);

    glCompileShader(vShader);
    int rv;
    glGetShaderiv(fShader, GL_COMPILE_STATUS, &rv);
    char log[4096];
    glGetShaderInfoLog(fShader, 4096, NULL, log);
    std::cout << log << '\n';

    glCompileShader(fShader);
    glGetShaderiv(fShader, GL_COMPILE_STATUS, &rv);
    glGetShaderInfoLog(fShader, 4096, NULL, log);
    std::cout << log << '\n';

    GLuint sprgm = glCreateProgram();
    glAttachShader(sprgm, vShader);
    glAttachShader(sprgm, fShader);

    glBindFragDataLocation(sprgm, 0, "color");

    glLinkProgram(sprgm);
    glGetProgramiv(sprgm, GL_LINK_STATUS, &rv);
    glGetProgramInfoLog(sprgm, 4096, NULL, log);
    std::cout << log << '\n';

    glUseProgram(sprgm);

    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    float verts[] = {
        0.5, 0.5, 0.0,
        0.5, -0.5, 0.0,
        -0.5, -0.5, 0.0,
        -0.5, 0.5, 0.0,

        0.5, 0.5, 1.0,
        0.5, -0.5, 1.0,
        -0.5, -0.5, 1.0,
        -0.5, 0.5, 1.0,
    };

    mulFLoatArray(sizeof(verts) / sizeof(float), verts, 0.5);

    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);

    //GLint sPos = glGetAttribLocation(sprgm, "position");
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(2);

    unsigned int elems[] = {
        0, 1, 1, 2, 2, 3, 3, 0,
        4, 5, 5, 6, 6, 7, 7, 4,
        0, 4, 1, 5, 2, 6, 3, 7
    };

    GLuint ebo;
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elems), elems, GL_STATIC_DRAW);

    glfwSetWindowSizeCallback(mainWindw, wResize);
    glfwSetWindowPosCallback(mainWindw, wMove);

    setView();

    int fps;
    float lt = glfwGetTime();

    float ma = 0;
    while(!glfwWindowShouldClose(mainWindw)){

        float ct = glfwGetTime();
        if (ct - lt > 1.0){
            std::cout << "\033[2J";
            std::cout << "\033[1;1H";
            std::cout << fps << '\n';
            fps = 0;
            lt = ct;
            shouldDraw = 1;
        }

        if (shouldDraw){

            fps++;
            ma += 0.005;
            if (ma > 6.00f)
                ma = 0.0f;
            glUniform1f(1, ma);
            setRot(PI/8 * ma, PI/0.3 * ma, PI/1.3 * ma);

            glClearColor(0.0, 0.0, 0.0, 0.0);

            glUseProgram(sprgm);
            glClear(GL_COLOR_BUFFER_BIT);

           // glEnable(GL_POINT_SMOOTH);
           // glPointSize(7.0f);
           // glDrawArrays(GL_POINTS, 0, 8);
            glDrawElements(GL_LINES, sizeof(elems) / sizeof(int), GL_UNSIGNED_INT, 0);

            glfwSwapBuffers(mainWindw);
        }
        glfwPollEvents();
    }


    glDeleteProgram(sprgm);

    glDeleteShader(vShader);
    glDeleteShader(fShader);

    glDeleteVertexArrays(1, &vao);

    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);

    glfwTerminate();
    return 0;
}
