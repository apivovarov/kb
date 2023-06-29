#include <memory>
#include <fmt/format.h>

#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

//#include "linmath.h"

//#include <stdlib.h>
//#include <stdio.h>

static void error_callback(int error, const char* description) {
  fmt::println(stderr, "Error: {}", description);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

static void compile_shaders(const GLuint& program, const std::string& vertex_shader_text, const std::string& fragment_shader_text) {
  GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
  const char* const vertex_shader_text_arr[1] = {vertex_shader_text.c_str()};
  glShaderSource(vertex_shader, 1, vertex_shader_text_arr, NULL);
  glCompileShader(vertex_shader);

  GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
  const char* const fragment_shader_text_arr[1] = {fragment_shader_text.c_str()};
  glShaderSource(fragment_shader, 1, fragment_shader_text_arr, NULL);
  glCompileShader(fragment_shader);

  glAttachShader(program, vertex_shader);
  glAttachShader(program, fragment_shader);
  glLinkProgram(program);
}

static const struct {
    float x, y;
    float r, g, b;
} vertices[3] =
{
    { -0.6f, -0.4f, 1.f, 0.f, 0.f },
    {  0.6f, -0.4f, 0.f, 1.f, 0.f },
    {   0.f,  0.6f, 0.f, 0.f, 1.f }
};

static const char* vertex_shader_text =
"#version 110\n"
"attribute vec3 vCol;\n"
"attribute vec2 vPos;\n"
"varying vec3 color;\n"
"void main()\n"
"{\n"
"    gl_Position = vec4(vPos, 0.0, 1.0);\n"
"    color = vCol;\n"
"}\n";

static const char* fragment_shader_text =
"#version 110\n"
"varying vec3 color;\n"
"void main()\n"
"{\n"
"    gl_FragColor = vec4(color, 1.0);\n"
"}\n";

int main(int, char **) {
  glfwSetErrorCallback(error_callback);

  if (!glfwInit())
      exit(EXIT_FAILURE);

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

  //window = glfwCreateWindow(640, 480, "Simple example", NULL, NULL);

  std::shared_ptr<GLFWwindow> window(
    glfwCreateWindow(640, 480, "Simple example", NULL, NULL),
    // Deleter
    [](GLFWwindow* w) {
      glfwDestroyWindow(w);
      glfwTerminate();
      fmt::println("glfw terminated");
    }
  );

  if (!window) {
      glfwTerminate();
      exit(EXIT_FAILURE);
  }

  glfwSetKeyCallback(window.get(), key_callback);

  glfwMakeContextCurrent(window.get());
  //glfwGetProcAddress();
  gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
  glfwSwapInterval(1);

  // NOTE: OpenGL error checks have been omitted for brevity

  GLuint vertex_buffer;
  glGenBuffers(1, &vertex_buffer);
  glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  GLuint prog = glCreateProgram();
  compile_shaders(prog, vertex_shader_text, fragment_shader_text);

  // GLint mvp_location = glGetUniformLocation(prog, "MVP");
  GLint vpos_location = glGetAttribLocation(prog, "vPos");
  GLint vcol_location = glGetAttribLocation(prog, "vCol");

  glEnableVertexAttribArray(vpos_location);
  glVertexAttribPointer(vpos_location, 2, GL_FLOAT, GL_FALSE, sizeof(vertices[0]), (void*) 0);
  glEnableVertexAttribArray(vcol_location);
  glVertexAttribPointer(vcol_location, 3, GL_FLOAT, GL_FALSE, sizeof(vertices[0]), (void*) (sizeof(float) * 2));

  while (!glfwWindowShouldClose(window.get())) {
    float ratio;
    int width, height;
    glfwGetFramebufferSize(window.get(), &width, &height);
    ratio = width / (float) height;

    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT);

   glUseProgram(prog);

   glDrawArrays(GL_TRIANGLES, 0, 3);

    glfwSwapBuffers(window.get());
    glfwPollEvents();
  }

  return 0;
}
