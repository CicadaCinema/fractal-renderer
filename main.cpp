//
// This code was created by Jeff Molofee '99 (ported to Linux/GLUT by Richard
// Campbell '99)
//
// If you've found this code useful, please let me know.
//
// Visit me at www.demonews.com/hosted/nehe
// (email Richard Campbell at ulmont@bellsouth.net)
//
#include <GL/freeglut_std.h>
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

bool paintOnNextFrame;

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

void printsceneinfo(void) {
  TEXTBOX(700, 64, 784, 114, 0x00A0A0A0, 0x00FFFFFF);

  tmp = 8;
  textline(701, 65 + 0 * tmp, (char *)textbrmess[selnumbranch], SMALLFONT,
           0x00000080);
  textline(701, 65 + 1 * tmp, (char *)textbgmess[showbackground], SMALLFONT,
           0x00000080);
  textline(701, 65 + 2 * tmp, (char *)textgrmess[groundsize], SMALLFONT,
           0x00000080);
  textline(701, 65 + 3 * tmp, (char *)textlight[lightness], SMALLFONT,
           0x00000080);
  textline(701, 65 + 4 * tmp, (char *)textpales[whitershade], SMALLFONT,
           0x00000080);
  textline(701, 65 + 5 * tmp, (char *)textfunky[colourmode], SMALLFONT,
           0x00000080);
}

void printtreeinfo(void) {
  TEXTBOX(670, 10, 784, 60, 0x00A0A0A0, 0x00FFFFFF);
  lcol = 0x00000080;
  tmp = 12;
  sprintf(stringbuf, "Now growing tree #%02i.", ((treeinuse + 1) & 0x1F));
  textline(672, 10 + tmp * 0, stringbuf, MEDIUMFONT, lcol);
  sprintf(stringbuf, "Name:%s.", trees[treeinuse].name);
  textline(672, 10 + tmp * 1, stringbuf, MEDIUMFONT, lcol);
  sprintf(stringbuf, "%s.", textbrmess[trees[treeinuse].branches - 1]);
  textline(672, 10 + tmp * 2, stringbuf, MEDIUMFONT, lcol);
  sprintf(stringbuf, "Uh=%i Gs=%i Sbh=%i Ut=%i", trees[treeinuse].usehig,
          trees[treeinuse].glblscl, trees[treeinuse].sctrnsl,
          trees[treeinuse].usetwst);
  textline(672, 10 + tmp * 3, stringbuf, MEDIUMFONT, lcol);
}

void manual(void) {
  // CLS:
  clearscreen(0x00FFF0E0);

  // Head texts:
  printsceneinfo();
  printtreeinfo();
  lcol = 0x00000080;
  textline(10, 10,
           (char *)"SunCode's 3D Dragon Tree IFS, the Demo, (first version "
                   "eight edition of May 2005)",
           BIGFONT, lcol);

  // Keys:
  TEXTBOX(30, 38, 556, 258, 0x00A0A0A0, 0x00D8F8E8);
  lcol = 0x00006000;
  tmp = 48;
  int yi = 56;
  int xi = 15;
  textline(tmp - 16, 38, (char *)"Keys to use in this demo:", BIGFONT, lcol);
  textline(
      tmp, yi + xi * 0,
      (char *)"[Esc] = Exit program! | [Alt] = exit from render | [SPACE] = "
              "Toggle this screen and render mode screen.",
      MEDIUMFONT, lcol);
  textline(
      tmp, yi + xi * 1,
      (char *)"[R] = Randomize tree. | [A] = Randomize angle of view | [Arrow "
              "keys] = Rotate the scene.",
      MEDIUMFONT, lcol);
  textline(tmp, yi + xi * 2,
           (char *)"[N] = Select number of branches, (random, 2, 3, 4, 5, 6, 7 "
                   "or 8) | "
                   "[1] - [8] = Randomize single branch.",
           MEDIUMFONT, lcol);
  textline(tmp, yi + xi * 3,
           (char *)"[C] = Clear buffers, (useful after, for example a palette "
                   "selection to clear out old pixels)",
           MEDIUMFONT, lcol);
  textline(
      tmp, yi + xi * 4,
      (char *)"[V] = Clear view, (useful after a selection, also removes my "
              "SunCode 'tag' from the screen =)",
      MEDIUMFONT, lcol);
  textline(
      tmp, yi + xi * 5,
      (char
           *)"[X] = Bigger tree. | [S] = Smaller tree. | [D] = Thicker tree. | "
             "[F] = Thinner tree.",
      MEDIUMFONT, lcol);
  textline(tmp, yi + xi * 6,
           (char *)"[Page Down] / [Page Up] = Zoom in / out. | [Home] = Reset "
                   "zoom and "
                   "angle of view.",
           MEDIUMFONT, lcol);
  textline(
      tmp, yi + xi * 7,
      (char *)"[I] = View information. | [T] = Test render, a white sheet, are "
              "pixels still written?, this will show that.",
      MEDIUMFONT, lcol);
  textline(
      tmp, yi + xi * 8,
      (char
           *)"[B] = Background, (on skyblue, on blue, on black, off black, off "
             "white). | [G] = Size of ground, (B, M, S).",
      MEDIUMFONT, lcol);
  textline(
      tmp, yi + xi * 9,
      (char *)"[P] = Randomize colours for foliage. | [E] = Extra foliage off, "
              "on, solo. | [U] = Use logs, cubes or sphears.",
      MEDIUMFONT, lcol);
  textline(
      tmp, yi + xi * 10,
      (char *)"[W] = Turn the whiter shade of pale to normal, flourescent or "
              "filament. | [L] = Toggle lights = light or dark",
      MEDIUMFONT, lcol);
  textline(tmp, yi + xi * 11, (char *)"[O] = Turn FUNKYCOLOURS on/off.",
           MEDIUMFONT, lcol);
  textline(
      tmp, yi + xi * 12,
      (char *)"[M] = More trees. | [F1] - [F4] = Toggle flags: 'Use heights', "
              "'Global scale', 'Scale by height' and 'Use twist'.",
      MEDIUMFONT, lcol);

  // Colours:
  viewcols();

  // Notes:
  TEXTBOX(322, 270, 784, 568, 0x00A0A0A0, 0x00D8E8F8);
  tmp = 340;
  lcol = 0x00600000;
  textline(tmp - 16, 270, (char *)"Notes:", BIGFONT, lcol);

  textline(
      tmp, 290,
      (char *)"Most trees looks awful but some don't and those are the ones we "
              "are looking for ;)",
      MEDIUMFONT, lcol);

  textline(tmp, 310,
           (char *)"A good idea is to render the image once to fill out the "
                   "shadow-map "
                   "and then press [C] to clear",
           MEDIUMFONT, lcol);
  textline(
      tmp, 322,
      (char *)"out the pixel and z-buffers and [V] to clear the view. And then "
              "render the image once again",
      MEDIUMFONT, lcol);
  textline(
      tmp, 334,
      (char *)"using the previously compleated shadow-map. This will make the "
              "shadows better.",
      MEDIUMFONT, lcol);

  textline(
      tmp, 354,
      (char *)"To save the current image you can press [Print Screen] and then "
              "[Esc] (to exit), and then the",
      MEDIUMFONT, lcol);
  textline(tmp, 366,
           (char *)"image is, (hopefully) on your clipboard, ready to get "
                   "pasted into "
                   "any graphics tool.",
           MEDIUMFONT, lcol);

  textline(
      tmp, 386,
      (char *)"The colours are choosen by random. Press [O] and you will enter "
              "'FUNKYCOLOURS' mode. If",
      MEDIUMFONT, lcol);
  textline(tmp, 398,
           (char *)"you do not like the current set of colours, then simply "
                   "press [P] "
                   "until random finds a set you like.",
           MEDIUMFONT, lcol);

  textline(
      tmp, 550,
      (char *)"This program is part of the public domain, (PD), distribute and "
              "make copys freely.",
      MEDIUMFONT, lcol);

  spacemess();
}

void spacemess(void) {
  if (itersdone == 0) {
    sprintf(
        stringbuf,
        "Press space to start render! %li iterations done, %li image pixels, "
        "%li shadow pixels.",
        itersdone, pixelswritten, shadowswritten);
    textline(64, 574, stringbuf, BIGFONT, lcol);
  } else {
    sprintf(stringbuf,
            "Press space to continue render! %li iterations done, %li image "
            "pixels, %li shadow pixels.",
            itersdone, pixelswritten, shadowswritten);
    textline(64, 574, stringbuf, BIGFONT, lcol);
  }
}

void viewcols(void) {
  TEXTBOX(30, 270, 312, 568, 0x00A0A0A0, bgcol[showbackground]);
  lcol = txcol[showbackground];
  textline(32, 270, (char *)"Foliage coloursation:", BIGFONT, lcol);
  textline(132, 556, (char *)"Press [P] to randomize", SMALLFONT, lcol);
  writecols();
}

void writecols(void) {
  tmp = 32;
  for (int i = 0; i < trees[treeinuse].branches; i++) {
    sprintf(stringbuf, "Branch # %i;", i + 1);
    textline(48, 290 + i * tmp, stringbuf, BIGFONT, txcol[showbackground]);

    tcolor = ((tcr[4 + i] << 16) + (tcg[4 + i] << 8) + tcb[4 + i]) & 0x00FFFFFF;
    FILLBOX(160, 290 + i * tmp, 306, 318 + i * tmp, tcolor);
  }
}

void clearscreen(long RGBdata) {
  glBegin(GL_POINTS);
  setColour(RGBdata);
  for (int x = 0; x < WIDTH; x++) {
    for (int y = 0; y < HEIGHT; y++) {
      glVertex2i(x, y);
    }
  }
  glEnd();
}

void showpic(void) {
  int x, y, xx, yy;

  glBegin(GL_POINTS);
  for (y = 0; y < HEIGHT; y++) {
    yy = y << 1;
    for (x = 0; x < WIDTH; x++) {
      xx = x << 1;

      tcolor = pict[yy][xx];
      tRed = (tcolor >> 16) & 0xFF;
      tGreen = (tcolor >> 8) & 0xFF;
      tBlue = tcolor & 0xFF;

      tcolor = pict[yy + 1][xx];
      tRed += (tcolor >> 16) & 0xFF;
      tGreen += (tcolor >> 8) & 0xFF;
      tBlue += tcolor & 0xFF;

      tcolor = pict[yy + 1][xx + 1];
      tRed += (tcolor >> 16) & 0xFF;
      tGreen += (tcolor >> 8) & 0xFF;
      tBlue += tcolor & 0xFF;

      tcolor = pict[yy][xx + 1];
      tRed += (tcolor >> 16) & 0xFF;
      tGreen += (tcolor >> 8) & 0xFF;
      tBlue += tcolor & 0xFF;

      tRed = (tRed >> 2) & 0xFF;
      tGreen = (tGreen >> 2) & 0xFF;
      tBlue = (tBlue >> 2) & 0xFF;

      tcolor = ((tRed << 16) + (tGreen << 8) + tBlue) & 0x00FFFFFF;
      setColour(tcolor);
      glVertex2i(x, y);
    }
  }
  glEnd();
}

void CreatePalette(void) {
  bool invert, vertin, lightobject, heatvawe, sinvawe, bakwrds;
  int fade, i;
  long tc;
  float fdout, fdin, fdout2, fdin2, fdouts, fdins, ufade0, ufade1, ufade2,
      ufade3, ampl;
  float rf, gf, bf, freq, rl, gl, bl;

  freq = 1.0f + RND * RND * RND * 256.0f;
  rf = freq * (1.0f + RND * pi);
  gf = freq * (1.0f + RND * pi);
  bf = freq * (1.0f + RND * pi);

  // Randomize palette-modes:
  invert = (RND > 0.5f);
  lightobject = (RND > 0.75f);
  vertin = (RND > 0.75f);
  heatvawe = (RND > 0.95f);
  sinvawe = (RND > 0.75f);
  bakwrds = (RND > 0.5f);

  fade = int(RND * 2);
  for (i = 0; i != PALSIZE; i++) {
    fdout = float(PALSIZE - i) / PALSIZE;
    if (bakwrds)
      fdout = float(i) / PALSIZE;

    fdout2 = fdout * fdout;
    fdouts = sqrt(fdout);

    fdin = 1.0f - fdout;
    fdin2 = 1.0f - fdouts;
    fdins = 1.0f - fdout2;

    ufade0 = fdout;
    ufade1 = fdins;
    ufade2 = fdout2;
    //    ufade3 = fdouts;

    //		if ( vertin )
    //     	ufade2 = 1.0f - ufade2;

    freq = rf * ufade0 * sqrt(ufade0);
    rl = (1.0f + cos(freq)) * 0.5f;
    freq = gf * ufade0 * sqrt(ufade0);
    gl = (1.0f + cos(freq)) * 0.5f;
    freq = bf * ufade0 * sqrt(ufade0);
    bl = (1.0f + cos(freq)) * 0.5f;

    if (lightobject) {
      if (vertin) {
        length = sqrt(rl * rl + gl * gl + bl * bl) * 2.0f;
        rl = ((1.0f + rl) / length) * fdin;
        gl = ((1.0f + gl) / length) * fdin;
        bl = ((1.0f + bl) / length) * fdin;
      } else {
        rl = gl = bl = fdin2;
      }
    }

    if (heatvawe) {
      freq = fdin2 * pii;
      rl = (1.0f - ((1.0f + sin(freq + rad * 240.0f)) * 0.5f * fdout)) * fdins;
      gl = (1.0f - ((1.0f + sin(freq + rad * 120.0f)) * 0.5f * fdout)) * fdins;
      bl = (1.0f - ((1.0f + sin(freq)) * 0.5f * fdout)) * fdins;
    } // heatvawe.

    if (sinvawe) {
      rl *= (1.0f + sin(fdout * pii * 4.1f)) / 2.0f;
      gl *= (1.0f + sin(fdout * pii * 4.2f)) / 2.0f;
      bl *= (1.0f + sin(fdout * pii * 4.3f)) / 2.0f;
    } // sinvawe.

    if (invert && vertin) {
      rl = (2.0f - rl) / 2.0f;
      gl = (2.0f - gl) / 2.0f;
      bl = (2.0f - bl) / 2.0f;
    } else if (invert) {
      rl = 1.0f - rl;
      gl = 1.0f - gl;
      bl = 1.0f - bl;
    } // Inverts.

    // Calibrate luminousity:
    amp = sqrtl(rl * rl + gl * gl + bl * bl);
    if (amp > 0.0f) {
      rl /= amp;
      gl /= amp;
      bl /= amp;
    } else {
      rl = 0.5f;
      gl = 0.5f;
      bl = 0.5f;
    }
    // Calibrate luminousity.

    tc = int(rl * float(0x01000000)) & 0x00ff0000;
    tc += int(gl * float(0x00010000)) & 0x0000ff00;
    tc += int(bl * float(0x00000100)) & 0x000000ff;
    lpCols[i] = tc;
  }
}

void ShowPalette(int mode) {
  int pr;
  int pp;
  int pz;

  // Draw border:
  FILLBOX(WIDTH - 306, 294, WIDTH - 46, 554, 0x00A0A0A0);

  tmp = WIDTH - 304;

  glBegin(GL_POINTS);
  switch (mode) {
  case SERP:
    for (pr = (256 - 1); pr >= 0; pr--) {
      for (pp = 0; pp < 256; pp++) {
        setColour(
            lpCols[(int(pp * (PALSIZE / 256)) | int(pr * (PALSIZE / 256)))]);
        glVertex2i(tmp + pp, HEIGHT - 304 + pr);
      }
    }
    break;

  case ABSZ:
    for (pr = (256 - 1); pr >= 0; pr--) {
      for (pp = 0; pp < 256; pp++) {
        pz = 1 + int((sqrt((pr - 128) * (pr - 128) + (pp - 128) * (pp - 128)) /
                      725) *
                     PALSIZE);
        pz = sqrt(pz) * sqrt(PALSIZE);
        setColour(lpCols[PALSIZE - (pz & (PALSIZE - 1))]);
        glVertex2i(tmp + pp, HEIGHT - 304 + pr);
      }
    }
    break;

  case HOTB:
  default:
    break;
  }
  glEnd();
}

void drawLine(void) {
  long double llen, ldx, ldy;
  long int lpx, lpy, lnum;

  // clip to ( -4 / 3 ) < x < ( 4 / 3 ) / -1 < y < 1
  if (fabsl(lxs) > RATIO) {
    temp = RATIO / fabsl(lxs);
    lxs = (fabsl(lxs) * temp) * SGN(lxs);
    lys = (fabsl(lys) * temp) * SGN(lys);
  }
  if (fabsl(lxe) > RATIO) {
    temp = RATIO / fabsl(lxe);
    lxe = (fabsl(lxe) * temp) * SGN(lxe);
    lye = (fabsl(lye) * temp) * SGN(lye);
  }
  if (fabsl(lys) > 1.0f) {
    temp = 1.0f / fabsl(lys);
    lxs = (fabsl(lxs) * temp) * SGN(lxs);
    lys = (fabsl(lys) * temp) * SGN(lys);
  }
  if (fabsl(lye) > 1.0f) {
    temp = 1.0f / fabsl(lye);
    lxe = (fabsl(lxe) * temp) * SGN(lxe);
    lye = (fabsl(lye) * temp) * SGN(lye);
  }
  // clip ends.

  ldx = lxe - lxs;
  ldy = lye - lys;
  llen = sqrtl(ldx * ldx + ldy * ldy);
  ldx = ldx / llen;
  ldy = -ldy / llen;
  lxs = lxs * MIDX;
  lys = -lys * MIDY;
  lnum = int(llen * MIDY) + 1;

  glBegin(GL_POINTS);
  setColour(lcol);
  do {
    lpx = int(MIDX + lxs);
    lpy = int(MIDY + lys);
    glVertex2i(lpx, lpy);
    lxs += ldx;
    lys += ldy;
  } while (--lnum);
  glEnd();
}

void drawMulticolLine(void) {
  long double llen, ldx, ldy;
  long int lpx, lpy, lnum;

  // clip to ( -4 / 3 ) < x < ( 4 / 3 ) / -1 < y < 1
  if (fabsl(lxs) > RATIO) {
    temp = RATIO / fabsl(lxs);
    lxs = (fabsl(lxs) * temp) * SGN(lxs);
    lys = (fabsl(lys) * temp) * SGN(lys);
  }
  if (fabsl(lxe) > RATIO) {
    temp = RATIO / fabsl(lxe);
    lxe = (fabsl(lxe) * temp) * SGN(lxe);
    lye = (fabsl(lye) * temp) * SGN(lye);
  }
  if (fabsl(lys) > 1.0f) {
    temp = 1.0f / fabsl(lys);
    lxs = (fabsl(lxs) * temp) * SGN(lxs);
    lys = (fabsl(lys) * temp) * SGN(lys);
  }
  if (fabsl(lye) > 1.0f) {
    temp = 1.0f / fabsl(lye);
    lxe = (fabsl(lxe) * temp) * SGN(lxe);
    lye = (fabsl(lye) * temp) * SGN(lye);
  }
  // clip ends.

  ldx = lxe - lxs;
  ldy = lye - lys;
  llen = sqrtl(ldx * ldx + ldy * ldy);
  ldx = ldx / llen;
  ldy = -ldy / llen;
  lxs = lxs * MIDY;
  lys = -lys * MIDY;
  lnum = int(llen * MIDY) + 1;

  glBegin(GL_POINTS);
  do {
    lpx = int(MIDX + lxs);
    lpy = int(MIDY + lys);
    setColour(lpCols[lnum & (PALSIZE - 1)]);
    glVertex2i(lpx, lpy);
    lxs += ldx;
    lys += ldy;
  } while (--lnum);
  glEnd();
}

void drawBoxi(void) {
  int boxw, boxh;

  // clip:
  if (lixs < 0)
    lixs = 0;
  if (lixs >= WIDTH)
    lixs = WIDTH - 1;

  if (lixe < 0)
    lixe = 0;
  if (lixe >= WIDTH)
    lixe = WIDTH - 1;

  if (liys < 0)
    liys = 0;
  if (liys >= HEIGHT)
    liys = HEIGHT - 1;

  if (liye < 0)
    liye = 0;
  if (liye >= HEIGHT)
    liye = HEIGHT - 1;
  // clip ends.

  // Swap coords?
  if (lixs > lixe) {
    tmp = lixs;
    lixs = lixe;
    lixe = tmp;
  }
  if (liys > liye) {
    tmp = liys;
    liys = liye;
    liye = tmp;
  }

  // Calculate with & height of box:
  boxw = lixe - lixs;
  boxh = liye - liys;

  glBegin(GL_POINTS);
  setColour(lcol);
  do {
    for (int index = 0; index < boxw; index++)
      glVertex2i(index + lixs, liys);
    liys++;
  } while (--boxh > 0);
  glEnd();
}

void newrender(void) {
  // Reset parameters:
  btx = tx[0];
  bty = ty[0];
  btz = tz[0];

  bcr = tcr[0];
  bcg = tcg[0];
  bcb = tcb[0];

  dtx = 1.0f;
  dty = 1.0f;
  dtz = 1.0f;

  dcr = tcr[4];
  dcg = tcg[4];
  dcb = tcb[4];

  // Palette index:
  pali = 0;

  // Min & max brightness:
  switch (lightness) {
  case 1:
    minbright = (1.0f / 3.0f);
    maxbright = (1.0f / 2.0f);
    break;
  default:
    minbright = 1.0f;
    maxbright = 0.0001f;
    break;
  } // color selector.

  ui = 0;
  xbuf[ui] = 0.0f;
  ybuf[ui] = 0.0f;
  zbuf[ui] = 0.0f;
}

void SunCode(void) {
  TEXTBOX(10, 10, 67, 26, 0x00A0A0A0, 0x00FFFFFF);
  textline(10, 10, (char *)"SunCode", BIGFONT, 0x00400000);
}

void clearscreenbufs(long RGBdata) {
  // Clear iteration and pixel counter:
  itersdone = 0;
  pixelswritten = 0;

  for (p = 0; p < BHEIGHT; p++) {
    for (r = 0; r < BWIDTH; r++) {
      // pixels-buffer:
      pict[p][r] = RGBdata;
      // View Z-buffer:
      bpict[p][r] = 0;
    }
  }
}

void clearallbufs(long RGBdata) {
  // Clear pixel counter for shadow map:
  shadowswritten = 0;

  if (lockshadow)
    trees[treeinuse].radius /= sqrtl(twup);

  lockshadow = false;

  // Light-source Z-buffer:
  for (p = 0; p < LHEIGHT; p++)
    for (r = 0; r < LWIDTH; r++)
      light[p][r] = 0;

  // Image pixel & z-buffer:
  clearscreenbufs(RGBdata);
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
  glDepthFunc(GL_ALWAYS);  // The Type Of Depth Test To Do
  glEnable(GL_DEPTH_TEST); // Enables Depth Testing
  glShadeModel(GL_SMOOTH); // Enables Smooth Color Shading

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity(); // Reset The Projection Matrix

  gluOrtho2D(0, Width, Height, 0);

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

  gluOrtho2D(0, Width, Height, 0);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}

/* The main drawing function. */
void DrawGLScene() {
  glClear(GL_DEPTH_BUFFER_BIT |
          GL_COLOR_BUFFER_BIT); // Clear The Screen And The Depth Buffer
  glLoadIdentity();             // Reset The View

  if (paintOnNextFrame) {
    switch (programMode) {
    case 0:
      // Copy & ant-anilize from pixel-buffer to screen:
      showpic();
      // Initiate iteration parameters:
      newrender();
      renderactive = true;
      // Clear screen if set was changed?:
      if (newset) {
        clearallbufs(bgcol[showbackground]);
        newset = false;
        // Then - agin! =)
        showpic();
      }
      // "tag" screen =)
      SunCode();
      break;
    case 1:
      // View info screen:
      manual();
      break;
    }
    glutSwapBuffers();
    paintOnNextFrame = false;
  }
}

void keyDownCallbackSpecial(int key, int x, int y) {
  /* avoid thrashing this call */
  usleep(100);

  // Do keys, (two modes)
  switch (programMode) {
  case 0:
    // Render screen:
    switch (key) {
      // FIXME: do we even need this? we already have such functionality
      /*
  case VK_ESCAPE:
    renderactive = false;
    PostMessage(hWnd, WM_CLOSE, 0, 0);
    break; // Escape.
       */

    case GLUT_KEY_PAGE_UP:
      imszoom *= twup;
      break; // page up.

    case GLUT_KEY_PAGE_DOWN:
      imszoom *= twdwn;
      break; // page down.

    case GLUT_KEY_HOME:
      ryv = 0.0f * rad;
      rxv = 0.0f * rad;
      CamAng();
      imszoom = 1.0f;
      newrender();
      clearallbufs(bgcol[showbackground]);
      clearscreen(bgcol[showbackground]);
      SunCode();
      break; // page down.

    case GLUT_KEY_UP:
      rxv += 0.01f;
      if (int(rxv) > 180)
        rxv = 180.0f;
      CamAng();
      break; // Up key.

    case GLUT_KEY_RIGHT:
      ryv += 0.01f;
      if (int(ryv) > 180)
        ryv = 180.0f;
      CamAng();
      break; // Right key.

    case GLUT_KEY_DOWN:
      rxv -= 0.01f;
      if (int(rxv) < -180)
        rxv = -180.0f;
      CamAng();
      break; // Down key.

    case GLUT_KEY_LEFT:
      ryv -= 0.01f;
      if (int(ryv) < -180)
        ryv = -180.0f;
      CamAng();
      break; // Left key.

    case GLUT_KEY_F1:
      trees[treeinuse].usehig = (trees[treeinuse].usehig + 1) & 0x01;
      newrender();
      clearallbufs(bgcol[showbackground]);
      clearscreen(bgcol[showbackground]);
      printtreeinfo();
      SunCode();
      break; // F1.

    case GLUT_KEY_F2:
      trees[treeinuse].glblscl = (trees[treeinuse].glblscl + 1) & 0x01;
      newrender();
      clearallbufs(bgcol[showbackground]);
      clearscreen(bgcol[showbackground]);
      printtreeinfo();
      SunCode();
      break; // F2.

    case GLUT_KEY_F3:
      trees[treeinuse].sctrnsl = (trees[treeinuse].sctrnsl + 1) & 0x01;
      newrender();
      clearallbufs(bgcol[showbackground]);
      clearscreen(bgcol[showbackground]);
      printtreeinfo();
      SunCode();
      break; // F3.

    case GLUT_KEY_F4:
      trees[treeinuse].usetwst = (trees[treeinuse].usetwst + 1) & 0x01;
      newrender();
      clearallbufs(bgcol[showbackground]);
      clearscreen(bgcol[showbackground]);
      printtreeinfo();
      SunCode();
      break; // F4.

    } // wparam (render screen).
    break;

    // Intro screen:
  case 1:
    switch (key) {
      // FIXME: do we even need this? we already have such functionality
      /*
  case VK_ESCAPE:
    PostMessage(hWnd, WM_CLOSE, 0, 0);
    break; // Escape.
       */

    case GLUT_KEY_F1:
      trees[treeinuse].usehig = (trees[treeinuse].usehig + 1) & 0x01;
      newrender();
      clearscreenbufs(bgcol[showbackground]);
      printtreeinfo();
      break; // F1.

    case GLUT_KEY_F2:
      trees[treeinuse].glblscl = (trees[treeinuse].glblscl + 1) & 0x01;
      newrender();
      clearscreenbufs(bgcol[showbackground]);
      printtreeinfo();
      break; // F2.

    case GLUT_KEY_F3:
      trees[treeinuse].sctrnsl = (trees[treeinuse].sctrnsl + 1) & 0x01;
      newrender();
      clearscreenbufs(bgcol[showbackground]);
      printtreeinfo();
      break; // F3.

    case GLUT_KEY_F4:
      trees[treeinuse].usetwst = (trees[treeinuse].usetwst + 1) & 0x01;
      newrender();
      clearscreenbufs(bgcol[showbackground]);
      printtreeinfo();
      break; // F4.

    } // wparam (info screen).

    break;
  }
}

void keyUpCallbackSpecial(int key, int x, int y) {
  /* avoid thrashing this call */
  usleep(100);

  // Functions only active if render screen:
  if (renderactive) {
    switch (key) {
    case GLUT_KEY_PAGE_UP:
    case GLUT_KEY_PAGE_DOWN:
      clearallbufs(bgcol[showbackground]);
      clearscreen(bgcol[showbackground]);
      SunCode();
      break; // Clear all keys.
    case GLUT_KEY_LEFT:
    case GLUT_KEY_RIGHT:
    case GLUT_KEY_DOWN:
    case GLUT_KEY_UP:
      clearscreenbufs(bgcol[showbackground]);
      clearscreen(bgcol[showbackground]);
      SunCode();
      break; // Clear screen keys.
    }
  }
}

/* The function called whenever a key is pressed down. */
void keyDownCallback(unsigned char key, int x, int y) {
  /* avoid thrashing this call */
  usleep(100);

  // Do keys, (two modes)
  switch (programMode) {
  case 0:
    // Render screen:
    switch (key) {
    case ESCAPE:
      /* If escape is pressed, kill everything. */
      /* shut down our window */
      glutDestroyWindow(window);

      /* exit the program...normal termination. */
      exit(0);
      break;
    case ' ':
      renderactive = false;
      programMode = 1;
      paintOnNextFrame = true;
      break;
      // FIXME: implement these keys exactly!
      /*
  case '1':
  case '2':
  case '3':
  case '4':
  case '5':
  case '6':
  case '7':
  case '8':
    randombranch(wParam - 49);
    clearallbufs(bgcol[showbackground]);
    clearscreen(bgcol[showbackground]);
    printtreeinfo();
    writecols();
    clearViewmess();
    SunCode();
    break; // 1 - 8.
       */

    case 'A':
      ryv = -180.0f * rad + RND * 360.0f * rad;
      ryx = cosl(ryv);
      ryy = sinl(ryv);
      rxv = -10.0f * rad + RND * 100.0f * rad;
      rxx = cosl(rxv);
      rxy = sinl(rxv);
      newrender();
      clearscreenbufs(bgcol[showbackground]);
      clearscreen(bgcol[showbackground]);
      SunCode();
      break; // A.

    case 'B':
      showbackground++;
      if (showbackground > 4)
        showbackground = 0;
      newrender();
      clearscreenbufs(bgcol[showbackground]);
      clearscreen(bgcol[showbackground]);
      SunCode();
      writecols();
      clearViewmess();
      break; // B.

    case 'C':
      clearscreenbufs(bgcol[showbackground]);
      trees[treeinuse].radius *= sqrtl(twup);
      break; // C.

    case 'D':
      trees[treeinuse].radius *= twup;
      newrender();
      break; // D.

    case 'E':
      if (++logfoliage >= 3)
        logfoliage = 0;
      newrender();
      clearallbufs(bgcol[showbackground]);
      clearscreen(bgcol[showbackground]);
      SunCode();
      printsceneinfo();
      if (logfoliage != 1)
        writecols();
      break; // E.

    case 'F':
      trees[treeinuse].radius *= twdwn;
      newrender();
      break; // F.

    case 'G':
      groundsize++;
      if (groundsize > 2)
        groundsize = 0;
      // Re initiate ground-IFS
      createbackground();
      newrender();
      clearscreenbufs(bgcol[showbackground]);
      clearscreen(bgcol[showbackground]);
      SunCode();
      printsceneinfo();
      break; // G.

    case 'I':
      printsceneinfo();
      lcol = txcol[showbackground];
      pixelsmess();
      printtreeinfo();
      writecols();
      if (colourmode)
        ShowPalette(ABSZ);
      SunCode();
      break; // I.

      // Don't push this key!!!
      // (The function does not work good yet.)
    case 'K':
      if (!lockshadow)
        lockshadow = true;
      trees[treeinuse].radius *= sqrtl(twup);
      clearscreenbufs(bgcol[showbackground]);
      break; // K.

    case 'L':
      lightness++;
      if (lightness > 1)
        lightness = 0;
      newrender();
      clearscreenbufs(bgcol[showbackground]);
      clearscreen(bgcol[showbackground]);
      SunCode();
      printsceneinfo();
      break; // L.

    case 'M':
      if (++treeinuse >= NUMTREES)
        treeinuse = 0;
      newsetup();
      clearallbufs(bgcol[showbackground]);
      clearscreen(bgcol[showbackground]);
      SunCode();
      printtreeinfo();
      printsceneinfo();
      writecols();
      break; // M.

    case 'N':
      if (++selnumbranch > 7)
        selnumbranch = 0;
      newsetup();
      clearallbufs(bgcol[showbackground]);
      clearscreen(bgcol[showbackground]);
      SunCode();
      printtreeinfo();
      printsceneinfo();
      writecols();
      break; // N.

    case 'O':
      clearscreen(bgcol[showbackground]);
      if (++colourmode > 1)
        colourmode = 0;
      if (colourmode) {
        CreatePalette();
        ShowPalette(ABSZ);
      }
      stemcols();
      leafcols();
      printsceneinfo();
      newrender();
      writecols();
      clearViewmess();
      clearscreenbufs(bgcol[showbackground]);
      SunCode();
      break; // N.

    case 'P':
      clearscreen(bgcol[showbackground]);
      if (colourmode) {
        CreatePalette();
        ShowPalette(ABSZ);
      }
      stemcols();
      leafcols();
      printsceneinfo();
      newrender();
      writecols();
      clearViewmess();
      clearscreenbufs(bgcol[showbackground]);
      SunCode();
      break; // P.

    case 'R':
      treeinuse = 31;
      newsetup();
      clearallbufs(bgcol[showbackground]);
      clearscreen(bgcol[showbackground]);
      printtreeinfo();
      writecols();
      clearViewmess();
      SunCode();
      break; // R.

    case 'S':
      trees[treeinuse].height *= twdwn;
      newrender();
      break; // S.

    case 'T':
      FILLBOX(0, 0, WIDTH, HEIGHT, 0x00FFFFFF);
      SunCode();
      lcol = 0x00000080;
      pixelsmess();
      break; // T.

    case 'U':
      if (++useLoCoS >= 3)
        useLoCoS = 0;
      else
        trees[treeinuse].radius = fabsl(trees[treeinuse].height / 2.0f);
      newrender();
      clearallbufs(bgcol[showbackground]);
      clearscreen(bgcol[showbackground]);
      SunCode();
      break; // U.

    case 'V':
      showpic();
      break; // V.

    case 'W':
      whitershade++;
      if (whitershade > 2)
        whitershade = 0;
      newrender();
      clearscreenbufs(bgcol[showbackground]);
      clearscreen(bgcol[showbackground]);
      SunCode();
      printsceneinfo();
      break; // W.

    case 'X':
      trees[treeinuse].height *= twup;
      newrender();
      break; // X.

    } // wparam (render screen).
    break;

    // Intro screen:
  case 1:
    switch (key) {
    case ESCAPE:
      /* If escape is pressed, kill everything. */
      /* shut down our window */
      glutDestroyWindow(window);

      /* exit the program...normal termination. */
      exit(0);
      break;
    case ' ':
      programMode = 0;
      paintOnNextFrame = true;
      break; // Space.

    case 'B':
      showbackground++;
      if (showbackground > 4)
        showbackground = 0;
      newrender();
      newset = true;
      printsceneinfo();
      viewcols();
      break; // B.

    case 'E':
      if (++logfoliage >= 3)
        logfoliage = 0;
      newrender();
      clearallbufs(bgcol[showbackground]);
      break; // E.

    case 'I':
      printsceneinfo();
      printtreeinfo();
      break; // I.

    case 'L':
      lightness++;
      if (lightness > 1)
        lightness = 0;
      newrender();
      newset = true;
      printsceneinfo();
      break; // L.

    case 'M':
      if (++treeinuse >= NUMTREES)
        treeinuse = 0;
      newsetup();
      clearallbufs(bgcol[showbackground]);
      viewcols();
      printtreeinfo();
      break; // M.

    case 'N':
      if (++selnumbranch > 7)
        selnumbranch = 0;
      newsetup();
      clearallbufs(bgcol[showbackground]);
      viewcols();
      printsceneinfo();
      printtreeinfo();
      break; // N.

    case 'O':
      if (++colourmode > 1)
        colourmode = 0;
      if (colourmode)
        CreatePalette();
      stemcols();
      leafcols();
      newrender();
      viewcols();
      printsceneinfo();
      clearscreenbufs(bgcol[showbackground]);
      break; // N.

    case 'P':
      if (colourmode)
        CreatePalette();
      stemcols();
      leafcols();
      newrender();
      viewcols();
      clearscreenbufs(bgcol[showbackground]);
      break; // P.

    case 'R':
      treeinuse = 31;
      newsetup();
      newset = true;
      printtreeinfo();
      viewcols();
      break; // R.

    case 'U':
      if (++useLoCoS >= 3)
        useLoCoS = 0;
      else
        trees[treeinuse].radius = (fabsl(trees[treeinuse].height) / 3.0f);
      newrender();
      break; // U.

    case 'W':
      whitershade++;
      if (whitershade > 2)
        whitershade = 0;
      newrender();
      newset = true;
      printsceneinfo();
      break; // W.

    } // wparam (info screen).

    break;
  }
}

/* The function called whenever a key is released. */
void keyUpCallback(unsigned char key, int x, int y) {
  /* avoid thrashing this call */
  usleep(100);

  // Functions only active if render screen:
  if (renderactive) {
    switch (key) {
    case 'D':
    case 'F':
    case 'S':
    case 'X':
      clearallbufs(bgcol[showbackground]);
      clearscreen(bgcol[showbackground]);
      SunCode();
      break; // Clear all keys.
    }
  }
}

int main(int argc, char **argv) {
  // my initial state
  paintOnNextFrame = true;

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

  /* Register the functions called when the keyboard is pressed. */
  glutKeyboardUpFunc(&keyUpCallback);
  glutKeyboardFunc(&keyDownCallback);
  glutSpecialUpFunc(&keyUpCallbackSpecial);
  glutSpecialFunc(&keyDownCallbackSpecial);

  /* Initialize our window. */
  InitGL(WIDTH, HEIGHT);

  /* Start Event Processing Engine */
  glutMainLoop();

  return 1;
}
