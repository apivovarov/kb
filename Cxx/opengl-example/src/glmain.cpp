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

int main(int, char **) {
  //GLFWwindow* window;
  GLuint vertex_buffer, vertex_shader, fragment_shader, program;
  GLint mvp_location, vpos_location, vcol_location;

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
  gladLoadGL();
  glfwSwapInterval(1);

  while (!glfwWindowShouldClose(window.get())) {
    float ratio;
    int width, height;
    glfwGetFramebufferSize(window.get(), &width, &height);
    ratio = width / (float) height;
    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT);


    glfwSwapBuffers(window.get());
    glfwPollEvents();
  }

  return 0;
}
