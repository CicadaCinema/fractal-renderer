//
// This code was created by Jeff Molofee '99 (ported to Linux/GLUT by Richard
// Campbell '99)
//
// If you've found this code useful, please let me know.
//
// Visit me at www.demonews.com/hosted/nehe
// (email Richard Campbell at ulmont@bellsouth.net)
//
#include <GL/gl.h>   // Header File For The OpenGL32 Library
#include <GL/glu.h>  // Header File For The GLu32 Library
#include <GL/glut.h> // Header File For The GLUT Library
#include <unistd.h>  // needed to sleep

/* ASCII code for the escape key. */
#define ESCAPE 27

/* The number of our GLUT window */
int window;

/* A general OpenGL initialization function.  Sets all of the initial
 * parameters. */
void InitGL(
    int Width,
    int Height) // We call this right after our OpenGL window is created.
{
  glClearColor(0.0f, 0.0f, 0.0f,
               0.0f);      // This Will Clear The Background Color To Black
  glClearDepth(1.0);       // Enables Clearing Of The Depth Buffer
  glDepthFunc(GL_LESS);    // The Type Of Depth Test To Do
  glEnable(GL_DEPTH_TEST); // Enables Depth Testing
  glShadeModel(GL_SMOOTH); // Enables Smooth Color Shading

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity(); // Reset The Projection Matrix

  gluPerspective(45.0f, (GLfloat)Width / (GLfloat)Height, 0.1f,
                 100.0f); // Calculate The Aspect Ratio Of The Window

  glMatrixMode(GL_MODELVIEW);
}

/* The function called when our window is resized (which shouldn't happen,
 * because we're fullscreen) */
void ReSizeGLScene(int Width, int Height) {
  if (Height == 0) // Prevent A Divide By Zero If The Window Is Too Small
    Height = 1;

  glViewport(0, 0, Width, Height);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  gluOrtho2D(0, Width, 0, Height);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}

/* The main drawing function. */
void DrawGLScene() {
  glClear(GL_DEPTH_BUFFER_BIT |
          GL_COLOR_BUFFER_BIT); // Clear The Screen And The Depth Buffer
  glLoadIdentity();             // Reset The View

  // draw a 100x100 green square
  glBegin(GL_POINTS);
  glColor3f(0.0f, 1.0f, 0.0f);
  for (int x = 0; x < 100; x++) {
    for (int y = 0; y < 100; y++) {
      glVertex2i(x, y);
    }
  }
  glEnd();

  // swap the buffers to display, since double buffering is used.
  glutSwapBuffers();
}

/* The function called whenever a key is pressed. */
void keyPressed(unsigned char key, int x, int y) {
  /* avoid thrashing this call */
  usleep(100);

  /* If escape is pressed, kill everything. */
  if (key == ESCAPE) {
    /* shut down our window */
    glutDestroyWindow(window);

    /* exit the program...normal termination. */
    exit(0);
  }
}

int main(int argc, char **argv) {
  /* Initialize GLUT state - glut will take any command line arguments that
     pertain to it or X Windows - look at its documentation at
     http://reality.sgi.com/mjk/spec3/spec3.html */
  glutInit(&argc, argv);

  /* Select type of Display mode:
     Double buffer
     RGBA color
     Alpha components supported
     Depth buffered for automatic clipping */
  glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_ALPHA | GLUT_DEPTH);

  /* get a 640 x 480 window */
  glutInitWindowSize(640, 480);

  /* the window starts at the upper left corner of the screen */
  glutInitWindowPosition(0, 0);

  /* Open a window */
  window = glutCreateWindow("my window");

  /* Register the function to do all our OpenGL drawing. */
  glutDisplayFunc(&DrawGLScene);

  /* Even if there are no events, redraw our gl scene. */
  glutIdleFunc(&DrawGLScene);

  /* Register the function called when our window is resized. */
  glutReshapeFunc(&ReSizeGLScene);

  /* Register the function called when the keyboard is pressed. */
  glutKeyboardFunc(&keyPressed);

  /* Initialize our window. */
  InitGL(640, 480);

  /* Start Event Processing Engine */
  glutMainLoop();

  return 1;
}
