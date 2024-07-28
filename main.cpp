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
#include <GL/glx.h>  // Header file fot the glx libraries.
#include <unistd.h>  // needed to sleep

#define WIN32_LEAN_AND_MEAN
// #include <ddraw.h>
#include <math.h>
// #include <mem.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
// #include <windows.h>
// #include <windowsx.h>

#include "Makrons.h"
#include "Treestruct.h"

/* ASCII code for the escape key. */
#define ESCAPE 27

/* The number of our GLUT window */
int window;

// A number of "always useful" constants:
const double pi = 3.141592654f; // Precission enough?

// These 'constants' are initiated in the function "doInit":
double rad; // A radian.
double pii; // 2 x pi.
double phi; // The golden ratio.

// A number of "always useful" variables:
bool tflag;     // A temporary flag.
int tmp;        // A temporary integer number.
double temp;    // A temporary floating point number.
double angle;   // Used to store an angle, (radians or degrees)
double cosx;    // Cos of an angle.
double siny;    // Sin of an angle.
double amp;     // The amplitude of ...
double length;  // The length of ...
double xlen;    // Length of x.
double ylen;    // Length of y.
double zlen;    // Length of z.
double llength; // If two lengths of ... is needed.

// Temporary n-counters:
int n;
int p;
int r;

//**** Four dimensions: ****

// Variable + temp dito:
double x, tmpx;
double y, tmpy;
double z, tmpz;
double t, tmpt; // Also used for 'temp'.

// Temp storage if a square is used more than once:
double xx;
double yy;
double zz;
double tt;

// Constant:
double a;
double b;
double c;
double d;

/* --------------------------------------------------------------------- *
                Fractal renderer variablel declarations:

    Notes:

        Below a mess!, (well it's getting better), but this section
      will be cleaner & get better comments in the near future.
      To download the latest version of this source-code, go to:

        "http://members.chello.se/solgrop/3dtree.htm"

                Some of this stuff is not used here, this code is
                originaly my 3D reversed Julia set creatorthat was
      based upon my 3D linear IFS renderer.
        (I'm back where I started =)

 * --------------------------------------------------------------------- */
// Structures for trees:
#define NUMTREES 32
DTBRA branches[NUMTREES][8];
DTIFS trees[NUMTREES];

// Main task flags:
bool runflag = false;
bool renderactive = false;
int programMode = 1;

// Image buffers & stuff:
long lk;                    // Size of screen line, (y coordinate incrementor).
long lpCols[PALSIZE];       // Buffer for colour palette.
long pict[BHEIGHT][BWIDTH]; // Buffer for picture.
int bpict[BHEIGHT][BWIDTH]; // Z-buffer for picture, max value = ZDEPTH.
int light[LHEIGHT][LWIDTH]; // Z-buffer for shadows, max value = ZDEPTH.

// Used for writing lines:
int lixs, liys, lixe, liye;
long lcol;
double lxs, lys, lxe, lye;
double RATIO, RERAT;

// Variable to store the background colour -
//(initiate to default):
long bgcolor = 0x00103050;

// Flag if a new set is to be rendered:
bool newset = false;

// Flag if shadowmap was locked:
bool lockshadow = false;

// Variables to store the current background modes:
int showbackground;
int groundsize;

// Number of branches texts:
const char *textbrmess[8] = {
    "Random branches", "Two branches", "Three branches", "Four branches",
    "Five branches",   "Six branches", "Seven branches", "Eight branches"};

// Background mode, text, colours and text colours:
const char *textbgmess[5] = {"On skyblue", "On blue", "On black", "Off black",
                             "Off white"};
// note: RGB
long bgcol[5] = {0x006080C0, 0x00103050, 0x00000000, 0x00000000, 0x00FFFFFF};
// note: BGR
long txcol[5] = {0x0040FFFF, 0x0080FFFF, 0x00FFFFFF, 0x0000FFFF, 0x00800000};

// Ground size texts:
const char *textgrmess[3] = {
    "Big ground",
    "Medium ground",
    "Small ground",
};

// Sizes for ground square:
double grounds[3] = {
    0.67f,
    0.45f,
    0.3f,
};

// Light model texts:
const char *textlight[2] = {"Dark", "Light"};

const char *textpales[3] = {"Normal (sunspot)", "Flourescent (moonspot)",
                            "Filament (noonspot)"};

const char *textfunky[2] = {"FUNKYCOLOURS off", "FUNKYCOLOURS on"};
// Temporary string buffer:
char stringbuf[256];

/* ---------------------------------------------------------------------
                VARIABLES FOR THE ITERATION-LOOP:
   --------------------------------------------------------------------- */
// Image scale ratios, (pic & shadows):
double ims = 2500, lims = 5000, size;

// Zoomfactor and zoom in/out factors:
double imszoom = 1.0f;

// Camera translation:
double CPOSX = 0.0f;
double CPOSY = 0.353f;
double CPOSZ = 0.0f;

// Physical screen coordinates, (x, y & z-buffer) + temp dito:
int nX, nXt;
int nY, nYt;
int nZ, nZt;

//// Light! ////

// Flag if surface normal is in use:
bool donormal;

// Flag if pixel is written to the shadows map:
bool doshadow;

// Light models:
int whitershade = 0;
int lightness = 1;

// Various temp variables used to calculate lights:
int ncols, bright, blight, tbright, overexpose, toverexpose;
double minbright, maxbright, luma, tluma;

// Light rotation angles & cos + sin for these:
double lrxx, lrxy, lrxv;
double lryx, lryy, lryv;

// Attractor-glow!
bool useglow;
double glow = 1.0f, largel = 0.0001f;
double bglow = 1.0f, blargel = 0.0001f;
double dglow = 1.0f, dlargel = 0.0001f;

// Palette index:
int pali = 0;
// "Second root" palette index:
int pali2 = 0;

// Temp storage for colour, (used for "second root").
int tcolr, tcolg, tcolb;

// Rotator, (cos, sin, angle):
double rxx, rxy, rxv;
double ryx, ryy, ryv;

//// IFS! IFS!! IFS!!! ////
long itersdone = 0;      // Number of iterations done so far.
long pixelswritten = 0;  // Number of pixels written to the image Z-buffer.
long spixelswritten;     // Storage for number of pixels written.
long shadowswritten = 0; // Number of pixels written to the shadow map Z-buffer.
long sshadowswritten;    // Storage for number of shadows written.
int pti;                 // Index counter for IFS loop.
double btx, bty, btz;    // Bottom plane 3D point.
double dtx, dty, dtz;    // 3D point - the fractal.
double ltx, lty, ltz;    // 3D point Light position.
double xt, yt, zt;       // Pixel position in scene.
double ntx, nty, ntz;    // Normal of pixel (if needed).
double dntx, dnty, dntz; // Normal of pixel (if needed)!
double nxt, nyt, nzt;    // Pixel normal position in scene.

// ISF random index, ("background" and "dragon"):
int bi, di;

// Translators for IFS:
//( + 4 is the background 4 point space fill square)
double tx[ANTAL + 4];
double ty[ANTAL + 4];
double tz[ANTAL + 4];

// Height of stem where the branch resides:
//( + 4 is the background 4 point space fill square)
double brheight[ANTAL + 4];

// Storage and temp for scale factors for IFS:
double sc[ANTAL + 4];

// Rotators for branches, (cos, sin):
// (not used anymore, it's in a struct nowdays)
//  double	trxx [ ANTAL + 4 ], trxy [ ANTAL + 4 ];
//  double	tryx [ ANTAL + 4 ], tryy [ ANTAL + 4 ];

// Rotators for IFS, (cos, sin, angle):
double drxx, drxy, drxv;
double dryx, dryy, dryv;
double drzx, drzy, drzv;

// Colours for IFS:
unsigned char tcr[ANTAL + 4];
unsigned char tcg[ANTAL + 4];
unsigned char tcb[ANTAL + 4];

// Various variables used to store R, G, B values:
int bcr, bcg, bcb, dcr, dcg, dcb, crt, cgt, cbt;
int tRed, tGreen, tBlue;

// Various variables used to store xRGB values:
long int color, tcolor, bcolor, dcolor;

// Number of colours used:
int nCols;

// Parameter positions saved here:
double xbuf[10];
double ybuf[10];
double zbuf[10];
// Index for buffer:
int ui = 9;

// Number of levels used
short int ilevels = 18;

// Number of branches selected:
int numbranch;
int selnumbranch;

// Index for presets buffers:
int treeinuse = 0;

// RGB values for maximum 32 levels:
unsigned char lcr[32];
unsigned char lcg[32];
unsigned char lcb[32];

// Increaser/decreaser for tree sizes:
double twup, twdwn;

// Preset scale ratios:
double scales[8];

// Index for log + foliage, log and foliage modes:
int logfoliage = 0;

// Index for use log or cube or sphear:
int useLoCoS = 0;

// Coloursation-mode:
int colourmode = 0;
int LoCoSPali = 0;

// **************
// **** FILE ****
// **************
// Buffer for source file:
char filebuf[32768];
// Size of input file:
unsigned int filesize;
// File handle:
FILE *infile;

// Buffer for trees:
struct ATREE tree[NUMTREES];

/* --------------------------------------------------------------------- *
                        DECLARE FUNCTIONS
 * --------------------------------------------------------------------- */

void CamAng(void);
void clearallbufs(long RGBdata);
void clearscreen(long RGBdata);
void clearscreenbufs(long RGBdata);
void clearViewmess(void);
void createbackground(void);
void CreatePalette(void);
void DoMyStuff(void);
void drawBox(void);
void drawBoxi(void);
void drawLine(void);
void drawMulticolLine(void);
static void finiObjects(void);
double getlevel(double xmin, double xmax, double ystart, double yend, double x,
                double Q);
void IFSlight(void);
void IFSplot(void);
void initiateIFS(void);
void initiatetext(void);
void leafcols(void);
void LitAng(void);
void loadtrees(void);
void loadtree(void);
void manual(void);
void newrender(void);
void newsetup(void);
int opensource(const char *fname);
void pixelsmess(void);
void printsceneinfo(void);
void printtreeinfo(void);
void randombranch(int indx);
void rotatelight(void);
void rotateview(void);
int SGN(double x);
void ShowPalette(int mode);
void showpic(void);
void spacemess(void);
void stemcols(void);
void SunCode(void);
void textline(int curposx, int curposy, char *stringdata, int fontindex,
              long textcolor);
void unrotatelight(void);
void unrotateview(void);
void viewcols(void);
void writecols(void);

GLuint base;  // base display list for the font set.
GLfloat cnt1; // 1st counter used to move text & for coloring.
GLfloat cnt2; // 2nd counter used to move text & for coloring.

GLvoid BuildFont(GLvoid) {
  Display *dpy;
  XFontStruct *fontInfo; // storage for our font.

  base = glGenLists(96); // storage for 96 characters.

  // load the font.  what fonts any of you have is going
  // to be system dependent, but on my system they are
  // in /usr/X11R6/lib/X11/fonts/*, with fonts.alias and
  // fonts.dir explaining what fonts the .pcf.gz files
  // are.  in any case, one of these 2 fonts should be
  // on your system...or you won't see any text.

  // get the current display.  This opens a second
  // connection to the display in the DISPLAY environment
  // value, and will be around only long enough to load
  // the font.
  dpy = XOpenDisplay(NULL); // default to DISPLAY env.

  fontInfo = XLoadQueryFont(
      dpy, "-adobe-helvetica-medium-r-normal--18-*-*-*-p-*-iso8859-1");
  if (fontInfo == NULL) {
    fontInfo = XLoadQueryFont(dpy, "fixed");
    if (fontInfo == NULL) {
      printf("no X font available?\n");
    }
  }

  // after loading this font info, this would probably be the time
  // to rotate, scale, or otherwise twink your fonts.

  // start at character 32 (space), get 96 characters (a few characters past z),
  // and store them starting at base.
  glXUseXFont(fontInfo->fid, 32, 96, base);

  // free that font's info now that we've got the
  // display lists.
  XFreeFont(dpy, fontInfo);

  // close down the 2nd display connection.
  XCloseDisplay(dpy);
}

GLvoid KillFont(GLvoid) // delete the font.
{
  glDeleteLists(base, 96); // delete all 96 characters.
}

GLvoid glPrint(char *text) // custom gl print routine.
{
  if (text == NULL) { // if there's no text, do nothing.
    return;
  }

  glPushAttrib(GL_LIST_BIT); // alert that we're about to offset the display
                             // lists with glListBase
  glListBase(base - 32);     // sets the base character to 32.

  glCallLists(strlen(text), GL_UNSIGNED_BYTE,
              text); // draws the display list text.
  glPopAttrib();     // undoes the glPushAttrib(GL_LIST_BIT);
}

/* --------------------------------------------------------------------- *
                        Return sign of x:
 * --------------------------------------------------------------------- */
int SGN(double x) {
  if (x < 0.0f)
    return (-1);
  else
    return (1);
} // SGN.

void setColour(long c) {
  unsigned char r = (c & 0x00FF0000) >> 16;
  unsigned char g = (c & 0x0000FF00) >> 8;
  unsigned char b = c & 0x000000FF;

  glColor3ub(r, g, b);
}

void textline(int curposx, int curposy, char *stringdata, int fontindex,
              long textcolor) {
  // FIXME: support setting font size
  /*
  // Select font:
  switch (fontindex) {
  case 0:
    SelectObject(hdc, smallfont);
    break;
  case 1:
    SelectObject(hdc, mediumfont);
    break;
  case 2:
    SelectObject(hdc, bigfont);
    break;
  } // Select font.
  */

  setColour(textcolor);
  glRasterPos2i(curposx, curposy);
  glPrint(stringdata);
}

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

  gluOrtho2D(0, Width, 0, Height);

  glMatrixMode(GL_MODELVIEW);

  BuildFont();
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

  glColor3f(1.0f, 0.0f, 0.0f);
  glRasterPos2i(50, 50);
  glPrint((char *)"The quick brown fox jumps over the lazy dog.");
  textline(20, 60, (char *)"The cow jumped over the moon.", 0, 0x00FF00FF);

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

  /* get a window */
  glutInitWindowSize(WIDTH, HEIGHT);

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
  InitGL(WIDTH, HEIGHT);

  /* Start Event Processing Engine */
  glutMainLoop();

  return 1;
}
