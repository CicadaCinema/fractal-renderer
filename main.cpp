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

  clearscreen(0x00E0BBBB);

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

  // draw a 100x100 red square, on top of the green one
  glBegin(GL_POINTS);
  glColor3f(1.0f, 0.0f, 0.0f);
  for (int x = 50; x < 150; x++) {
    for (int y = 50; y < 150; y++) {
      glVertex2i(x, y);
    }
  }
  glEnd();

  TEXTBOX(350, 200, 400, 300, 0x00FFFFFF, 0x000000FF);

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
