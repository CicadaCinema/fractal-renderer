// clang-format off
#include <GL/glew.h>
#include <GL/freeglut.h>
// clang-format on

#include <filesystem>
#include <glm/glm.hpp>
#include <glm/gtc/random.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <chrono>
#include <cstddef>
#include <sstream>
#include <vector>

struct Vertex {
  glm::vec2 pos;
  glm::vec3 color;
};
std::vector<Vertex> verts;
GLuint vbo = 0;
GLuint dlist = 0;

void addSomePoints() {
  for (size_t i = 0; i < 1000; i++) {
    Vertex vert;
    vert.pos =
        glm::linearRand(glm::vec2(100.0f, 100.0f), glm::vec2(500.0f, 500.0f));
    vert.color = glm::linearRand(glm::vec3(0.00f, 0.0f, 0.0f),
                                 glm::vec3(1.0f, 1.0f, 1.0f));
    verts.push_back(vert);
  }
}

void init() {
  // init geometry
  addSomePoints();

  // create VBO
  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * verts.size(), verts.data(),
               GL_STATIC_DRAW);
}

void keyboard(unsigned char key, int x, int y) {}

void display() {
  // timekeeping
  static std::chrono::steady_clock::time_point prv =
      std::chrono::steady_clock::now();
  std::chrono::steady_clock::time_point cur = std::chrono::steady_clock::now();
  const float dt = std::chrono::duration<float>(cur - prv).count();
  prv = cur;

  glClearColor(0, 0, 0, 1);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // Define projection matrix.
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(0, 600, 600, 0);

  // Define model matrix.
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  // send a bit more data to vbo
  addSomePoints();
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * verts.size(), verts.data(),
               GL_STATIC_DRAW);

  // render
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_COLOR_ARRAY);
  glVertexPointer(2, GL_FLOAT, sizeof(Vertex), (void *)offsetof(Vertex, pos));
  glColorPointer(3, GL_FLOAT, sizeof(Vertex), (void *)offsetof(Vertex, color));
  glDrawArrays(GL_POINTS, 0, verts.size());
  glDisableClientState(GL_VERTEX_ARRAY);
  glDisableClientState(GL_COLOR_ARRAY);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  // info/frame time output
  std::stringstream msg;
  msg << "Using vertex buffer object";
  msg << std::endl;
  msg << "Frame time: " << (dt * 1000.0f) << " ms";
  msg << std::endl;
  msg << "Number of points: " << verts.size();
  glColor3ub(255, 255, 0);
  glWindowPos2i(10, 45);
  glutBitmapString(GLUT_BITMAP_9_BY_15,
                   (unsigned const char *)(msg.str().c_str()));

  glutSwapBuffers();
}

int main(int argc, char **argv) {
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
  glutInitWindowSize(600, 600);
  glutCreateWindow("my window");
  glewInit();
  init();
  glutDisplayFunc(display);
  glutKeyboardFunc(keyboard);
  glutIdleFunc(display);
  glutMainLoop();
  return 0;
}
