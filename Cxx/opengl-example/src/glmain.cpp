#include <fmt/format.h>
#include <memory>

// glad/gl.h should be included before GLFW/glfw3.h
#include <glad/gl.h>
// GLFW_INCLUDE_NONE prevents the GLFW header from including the OpenGL header
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>


static const std::string vertex_shader_text {
#include "glmain.vert"
};

static const std::string fragment_shader_text {
#include "glmain.frag"
};

#include "linmath.h"

// #include <stdlib.h>
// #include <stdio.h>

static void error_callback(int error, const char *description) {
  fmt::println(stderr, "Error: {}", description);
}

static void key_callback(GLFWwindow *window, int key, int scancode, int action,
                         int mods) {
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    glfwSetWindowShouldClose(window, GLFW_TRUE);
}

static void compile_shaders(const GLuint &program,
                            const std::string &vertex_shader_text,
                            const std::string &fragment_shader_text) {
  GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
  const char *const vertex_shader_text_arr[1] = {vertex_shader_text.c_str()};
  glShaderSource(vertex_shader, 1, vertex_shader_text_arr, NULL);
  glCompileShader(vertex_shader);

  GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
  const char *const fragment_shader_text_arr[1] = {
      fragment_shader_text.c_str()};
  glShaderSource(fragment_shader, 1, fragment_shader_text_arr, NULL);
  glCompileShader(fragment_shader);

  glAttachShader(program, vertex_shader);
  glAttachShader(program, fragment_shader);
  glLinkProgram(program);
}

static const struct {
  float x, y;
  float r, g, b;
} vertices[3] = {{-0.6f, -0.4f, 1.f, 0.f, 0.f},
                 {0.6f, -0.4f, 0.f, 1.f, 0.f},
                 {0.f, 0.6f, 0.f, 0.f, 1.f}};

int main(int, char **) {
  glfwSetErrorCallback(error_callback);

  if (!glfwInit())
    exit(EXIT_FAILURE);

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

   std::shared_ptr<GLFWwindow> window(
      glfwCreateWindow(640, 480, "Simple example", NULL, NULL),
      // Deleter
      [](GLFWwindow *w) {
        glfwDestroyWindow(w);
        glfwTerminate();
        fmt::println("glfw terminated");
      });

  if (!window) {
    glfwTerminate();
    exit(EXIT_FAILURE);
  }

  glfwSetKeyCallback(window.get(), key_callback);

  glfwMakeContextCurrent(window.get());
  gladLoadGL((GLADloadfunc)glfwGetProcAddress);
  glfwSwapInterval(1);

  // NOTE: OpenGL error checks have been omitted for brevity

  GLuint vertex_buffer;
  glGenBuffers(1, &vertex_buffer);
  glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  GLuint prog = glCreateProgram();
  compile_shaders(prog, vertex_shader_text, fragment_shader_text);

  GLint mvp_location = glGetUniformLocation(prog, "MVP");
  GLint vpos_location = glGetAttribLocation(prog, "vPos");
  GLint vcol_location = glGetAttribLocation(prog, "vCol");

  glEnableVertexAttribArray(vpos_location);
  glVertexAttribPointer(vpos_location, 2, GL_FLOAT, GL_FALSE,
                        sizeof(vertices[0]), (void *)0);
  glEnableVertexAttribArray(vcol_location);
  glVertexAttribPointer(vcol_location, 3, GL_FLOAT, GL_FALSE,
                        sizeof(vertices[0]), (void *)(sizeof(float) * 2));

  while (!glfwWindowShouldClose(window.get())) {
    float ratio;
    int width, height;
    mat4x4 m, p, mvp;
    glfwGetFramebufferSize(window.get(), &width, &height);
    ratio = width / (float)height;

    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT);

    mat4x4_identity(m);
    mat4x4_rotate_X(m, m, (float) glfwGetTime() * 1.0);
    mat4x4_ortho(p, -ratio, ratio, -1.f, 1.f, 1.f, -1.f);
    mat4x4_mul(mvp, p, m);

    glUseProgram(prog);
    glUniformMatrix4fv(mvp_location, 1, GL_FALSE, (const GLfloat*) mvp);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    glfwSwapBuffers(window.get());
    glfwPollEvents();
  }

  return 0;
}
