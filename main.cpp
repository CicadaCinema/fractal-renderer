//
// This code was created by Jeff Molofee '99 (ported to Linux/GLUT by Richard
// Campbell '99)
//
// If you've found this code useful, please let me know.
//
// Visit me at www.demonews.com/hosted/nehe
// (email Richard Campbell at ulmont@bellsouth.net)
//
// clang-format off
#include <GL/glew.h>
#include <GL/freeglut_std.h>
//#include <GL/gl.h>   // Header File For The OpenGL32 Library
//#include <GL/glu.h>  // Header File For The GLu32 Library
#include <GL/glut.h> // Header File For The GLUT Library
#include <GL/glx.h>  // Header file fot the glx libraries.
#include <glm/ext/matrix_clip_space.hpp>
#include <unistd.h>  // needed to sleep
// clang-format on

#include <ft2build.h>
#include FT_FREETYPE_H

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

#include "shader.h"

/* ASCII code for the escape key. */
#define ESCAPE 27

/* The number of our GLUT window */
int window;

// We will always render to this framebuffer.
unsigned int framebuffer;

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
Shader *basicShader;
glm::mat4 projectionMatrix;

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

float startTime;

// This function is called whenever `itersdone` is incremented.
// We use it for tracking our performance.
void benchmark() {
  if (itersdone == 0) {
    startTime = (float)clock() / CLOCKS_PER_SEC;
  }

  if (itersdone == 30000) {
    float timeElapsed = (float)clock() / CLOCKS_PER_SEC - startTime;
    printf("it took %f to do 30000 iterations\n", timeElapsed * 100);
  }
}

void rotatelight(void) {
  t = lryx * zt - lryy * xt;
  xt = lryx * xt + lryy * zt;
  zt = t;

  t = lrxx * yt - lrxy * zt;
  zt = lrxx * zt + lrxy * yt;
  yt = t;
}
void rotateview(void) {
  t = ryx * zt - ryy * xt;
  xt = ryx * xt + ryy * zt;
  zt = t;

  t = rxx * yt - rxy * zt;
  zt = rxx * zt + rxy * yt;
  yt = t;
}

void IFSlight(void) {
  // Rotate to light position:
  rotatelight();

  // Clip z:
  if ((zt > -1.0f) && (zt < 1.0f)) {

    // Do normal?:
    if (donormal) {
      nxt = dntx + CPOSX;
      nyt = dnty + CPOSY;
      nzt = dntz + CPOSZ;

      // Rotate normal z to light position!
      t = lryx * nzt - lryy * nxt;
      nxt = lryx * nxt + lryy * nzt;
      nzt = t;

      t = lrxx * nyt - lrxy * nzt;
      nzt = lrxx * nzt + lrxy * nyt;
      nyt = t;

      // Get light normal!
      x = dtx - ntx;
      y = dty - nty;
      z = dtz - ntz;

      t = sqrtl(x * x + y * y + z * z);
      nzt -= (z / t);
      nzt = (0.5f + nzt) / 2.0f;
    }
    // Do normal.

    // Get distance to light:
    size = (3.0f + zt) / 2.0f;
    t = (2.0f - size);

    // Calculate & calibrate luminousity:
    if (donormal)
      t = t * t * nzt;

    // Calculate & calibrate luminousity:
    if (donormal) {
      if ((logfoliage != USELOGS) && (lightness == 0)) {
        t = ((2.0f * t) + powl(nzt, 3.0f)) / 3.0f;
      } else {
        t = t * t * nzt;
      }
    }

    t = (1.0f + t) / 2.0f;

    if (t < minbright)
      minbright = t;
    t = t - minbright;
    if (t > maxbright)
      maxbright = t;
    t = t / maxbright;

    t = t * 2.0f;
    luma = t - 1.0f;
    if (luma < 0.0f)
      luma = 0.0f;
    luma = 1.0f + powl(luma, 8.0f);
    if (t > 1.0f)
      t = 1.0f;
    overexpose = int(255.0 * luma) - 0xFF;
    if (overexpose < 0)
      overexpose = 0;

    if (useglow)
      bright = int(48 * glow + 208 * t) & 0xFF;
    else
      bright = int(255 * t) & 0xFF;

    blight = bright >> 1;

    nZ = int((2.0f - size) * (ZDEPTH >> 1)) & 0x7FFF;
    zt = (lims * imszoom) / size;

    nY = LMIDY + int(yt * zt);
    nX = LMIDX + int(xt * zt);
    // Clip y:
    if ((nY >= 0) && (nY < LHEIGHT)) {
      // Clip x:
      if ((nX >= 0) && (nX < LWIDTH)) {
        if (light[nY][nX] > (nZ + 2)) {
          // Create shadow.
          bright = blight;
          overexpose = 0;
          luma = 1.0f;
        } else if (doshadow) {
          // Update counter for pixels written to shadow map:
          if ((light[nY][nX] + 2) < nZ)
            shadowswritten++;
          light[nY][nX] = nZ;
        } // Shadow or not.
      } // clip - X.
    } // clip - Y.
  } // clip - Z.

  return;
} // IFSlight.

void DoMyStuff(void) {
  // FIXME: is this not handled already by the callback?
  /*
  // If someone's tap'n the [Alt] key:
  if (GetAsyncKeyState(VK_MENU)) {
    renderactive = false;
    programMode = 1;
    paintOnNextFrame = true;
    return;
  }
   */

  // Don't count pixels written for bottom plane:
  // Comment: The counter is updated inside the plot function.
  //          This is why I save the value, (to restore later).
  //          Ok! I could use a flag instaed, but did not =)
  spixelswritten = pixelswritten;
  sshadowswritten = shadowswritten;

  // Show background?
  if (showbackground < 3) {
    // ************************* //
    // * Bottom ******** IFS ! * //
    // ************************* //

    // Turn glow on:
    useglow = true;

    // Don't write to shadow map:
    doshadow = false;

    // Don't use normal, (maybe I will fix this later):
    donormal = false;

    // Iteration loop:
    for (pti = 6; pti >= 0; pti--) {
      bi = int(RND * 4);

      btx = (btx - tx[bi]) * sc[bi];
      bty = (bty - ty[bi]) * sc[bi];
      btz = (btz - tz[bi]) * sc[bi];

      // Attractor-glow:
      if (useglow) {
        t = sqrtl(btx * btx + bty * bty + btz * btz);
        if (t > blargel) {
          blargel = t;
        } // if blargel.
        t = pow((1.0f - t / blargel), 16.0f);
        bglow = (bglow + t) / 2.0f;
      } // Attractor-glow.

      btx += tx[bi];
      bty += ty[bi];
      btz += tz[bi];

      // Color:
      switch (colourmode) {

        // Disabled:
        //	  		case FUNKYCOLOURS:
        //					if ( bi & 0x1 )
        //						pali += ( ( PALSIZE -
        // pali ) >> 1 ); 					else
        // pali >>= 1; 					tcolor = lpCols [ pali
        // ]; 					tRed = 0xFF ^ ( ( tcolor >> 17 )
        //& 0x7F ); 					tGreen = 0xFF ^ ( (
        // tcolor >> 9 ) & 0x7F ); 					tBlue =
        // 0xFF ^ ( ( tcolor >> 1 ) & 0x7F );
        // bcr = ( ( bcr + tRed ) >> 1 ) & 0xFF;
        // bcg = ( ( bcg + tGreen ) >> 1 ) & 0xFF;
        // bcb = ( ( bcb + tBlue ) >> 1 ) & 0xFF; 	    	break; //
        // FUNKYCOLOURS.

      case NORMALCOLOURS:
      default:
        bcr = ((bcr + tcr[bi]) >> 1) & 0xFF;
        bcg = ((bcg + tcg[bi]) >> 1) & 0xFF;
        bcb = ((bcb + tcb[bi]) >> 1) & 0xFF;
        break; // NORMALCOLOURS.
      } // Switch colourmode.

      // Scale & translate to scene:
      xt = btx + CPOSX;
      yt = bty + CPOSY;
      zt = btz + CPOSZ;

      // it's light:
      glow = bglow;
      IFSlight();

      // Scale & translate to scene:
      xt = btx + CPOSX;
      yt = bty + CPOSY;
      zt = btz + CPOSZ;

      // Select colour for pixel:
      crt = bcr;
      cgt = bcg;
      cbt = bcb;

      // Plot pixel to scene:
      IFSplot();

    } // End of the iteration loop (the bottom-plane).
  } // if showbackground.

  // ************************//
  // * Fractal ******* IFS ! //
  // ************************//

  // FIRST DO IFS FOR FOLIAGE:

  int i;

  if (logfoliage != 1) {
    // Coordinate:
    dtx = xbuf[ui];
    dty = ybuf[ui];
    dtz = zbuf[ui];

    // Use shadow map?:
    if (lockshadow)
      doshadow = false;
    else
      doshadow = true;

    // Do not use normal for foliage:
    donormal = false;

    // Restore iteration counter:
    pixelswritten = spixelswritten;
    shadowswritten = sshadowswritten;

    // IFS-snurran!
    for (pti = 8; pti >= 0; pti--) {
      benchmark();
      itersdone++;
      di = 4 + int(RND * trees[treeinuse].branches);

      // Translate to scene:
      xt = dtx + CPOSX;
      yt = dty + CPOSY;
      zt = dtz + CPOSZ;

      // Save position:
      tmpx = xt;
      tmpy = yt;
      tmpz = zt;

      // Get luminousity & plot in the light's Z-table:
      IFSlight();

      // Translate to scene:
      xt = dtx + CPOSX;
      yt = dty + CPOSY;
      zt = dtz + CPOSZ;

      // Select colour for pixel:
      crt = dcr;
      cgt = dcg;
      cbt = dcb;

      // Plot pixel to scene:
      IFSplot();

      // **** TRANSFORMATIONS **** //
      // ** Pixel ** //
      // Rotations:

      i = di - 4;
      // Do twist?:
      if (trees[treeinuse].usetwst) {
        t = branches[treeinuse][i].twistc * dtz -
            branches[treeinuse][i].twists * dtx;
        dtx = branches[treeinuse][i].twistc * dtx +
              branches[treeinuse][i].twists * dtz;
        dtz = t;
      } // Do twist?.
      // Lean:
      t = branches[treeinuse][i].leanc * dtx -
          branches[treeinuse][i].leans * dty;
      dty = branches[treeinuse][i].leanc * dty +
            branches[treeinuse][i].leans * dtx;
      dtx = t;
      // Rotate:
      t = branches[treeinuse][i].rotatec * dtz -
          branches[treeinuse][i].rotates * dtx;
      dtx = branches[treeinuse][i].rotatec * dtx +
            branches[treeinuse][i].rotates * dtz;
      dtz = t;

      // Scale!
      // Global scale?
      if (trees[treeinuse].glblscl) {
        dtx *= branches[treeinuse][0].scale;
        dty *= branches[treeinuse][0].scale;
        dtz *= branches[treeinuse][0].scale;
      }
      // No!, all diffrent:
      else {
        dtx *= branches[treeinuse][i].scale;
        dty *= branches[treeinuse][i].scale;
        dtz *= branches[treeinuse][i].scale;
      }

      // Scale by heigth of branch?
      if (trees[treeinuse].sctrnsl) {
        dtx *= branches[treeinuse][i].height;
        dty *= branches[treeinuse][i].height;
        dtz *= branches[treeinuse][i].height;
      }

      // Translate!
      // Use heights?
      if (trees[treeinuse].usehig)
        dty += trees[treeinuse].height * branches[treeinuse][i].height;
      // No!, all at top of stem:
      else
        dty += trees[treeinuse].height;

      // Color!
      dcr = ((dcr + tcr[di]) >> 1) & 0xFF;
      dcg = ((dcg + tcg[di]) >> 1) & 0xFF;
      dcb = ((dcb + tcb[di]) >> 1) & 0xFF;

    } // End of the iteration loop (the IFS random walk).
  } // If logfoliage != 1.

  // Save current coordinate:
  xbuf[ui] = dtx;
  ybuf[ui] = dty;
  zbuf[ui] = dtz;

  // THEN DO L-IFSYS FOR STUB AND STEM:

  if (logfoliage != 2) {
    switch (useLoCoS) {
    case USECUBES:
      t = trees[treeinuse].radius;
      t += t * float(1 + RNDBOOL) * 0.01f;
      // Any point on a square:
      dtx = t;
      dty = -t + RND * t * 2.0f;
      dtz = -t + RND * t * 2.0f;
      // Create surface normal:
      dntx = dtx - 1.0f;
      dnty = dty;
      dntz = dtz;
      // Square to any side of a cube:
      if (0 != (tmp = 1 + int(RND * 6)))
        for (i = 0; i < tmp; i++) {
          t = -dty;
          dty = dtz;
          dtz = dtx;
          dtx = t;
          t = -dnty;
          dnty = dntz;
          dntz = dntx;
          dntx = t;
        }
      // Translate to start position:
      dty = dty + trees[treeinuse].height;
      dnty = dnty + trees[treeinuse].height;
      break; // Cubes.

    case USESPHEARS:
      t = trees[treeinuse].radius;
      t += t * float(1 + RNDBOOL) * 0.01f;
      // Any point in a cube:
      dtx = (-1.0f) + RND * 2.0f;
      dty = (-1.0f) + RND * 2.0f;
      dtz = (-1.0f) + RND * 2.0f;
      if (0 < (length = sqrtl(dtx * dtx + dty * dty + dtz * dtz))) {
        // Normalise to length = 1.0:
        dtx /= length;
        dty /= length;
        dtz /= length;
        // Create surface normal:
        dntx = -dtx;
        dnty = -dty;
        dntz = -dtz;
        // Set radius:
        dtx *= t;
        dty *= t;
        dtz *= t;
      }
      // Translate to start position:
      dty = dty + trees[treeinuse].height;
      dnty = dnty + trees[treeinuse].height;
      break; // Sphears.

    case USELOGS:
    default:
      // Create log!
      angle = 360.0f * rad * RND;
      t = RND;
      x = trees[treeinuse].radius * branches[treeinuse][0].scale;
      x = trees[treeinuse].radius - x;
      x = trees[treeinuse].radius - t * x;
      dtx = sinl(angle) * x;
      dty = t * trees[treeinuse].height;
      dtz = cosl(angle) * x;
      dntx = -sinl(angle);
      dnty = t * trees[treeinuse].height;
      dntz = -cosl(angle);
      break; // Logs.
    } // Switch useLoCoS.

    // Color:
    LoCoSPali = (PALSIZE >> 1);
    switch (colourmode) {
    case FUNKYCOLOURS:
      tcolor = lpCols[LoCoSPali];
      dcr = (tcolor >> 16) & 0xFF;
      dcg = (tcolor >> 8) & 0xFF;
      dcb = tcolor & 0xFF;
      break; // FUNKYCOLOURS.
    case NORMALCOLOURS:
    default:
      dcr = lcr[ilevels];
      dcg = lcg[ilevels];
      dcb = lcb[ilevels];
      break; // NORMALCOLOURS.
    } // Switch colourmode.

    // Use shadow map?:
    if (lockshadow)
      doshadow = false;
    else
      doshadow = true;

    // Use normal for stem and branches:
    donormal = true;

    // Restore iteration counter:
    pixelswritten = spixelswritten;
    shadowswritten = sshadowswritten;

    // IFS-snurran!
    for (pti = (ilevels - 1); pti >= 0; pti--) {
      benchmark();
      itersdone++;
      di = 4 + int(RND * trees[treeinuse].branches);

      // Translate to scene:
      xt = dtx + CPOSX;
      yt = dty + CPOSY;
      zt = dtz + CPOSZ;

      // Save position:
      tmpx = xt;
      tmpy = yt;
      tmpz = zt;

      // Get luminousity & plot in the light's Z-table:
      IFSlight();

      // Translate to scene:
      xt = dtx + CPOSX;
      yt = dty + CPOSY;
      zt = dtz + CPOSZ;

      // Select colour for pixel:
      crt = dcr;
      cgt = dcg;
      cbt = dcb;

      // Plot pixel to scene:
      IFSplot();

      // **** TRANSFORMATIONS **** //
      // ** Pixel ** //
      // Rotations:

      i = di - 4;
      // Do twist?:
      if (trees[treeinuse].usetwst) {
        t = branches[treeinuse][i].twistc * dtz -
            branches[treeinuse][i].twists * dtx;
        dtx = branches[treeinuse][i].twistc * dtx +
              branches[treeinuse][i].twists * dtz;
        dtz = t;
      } // Do twist?.
      // Lean:
      t = branches[treeinuse][i].leanc * dtx -
          branches[treeinuse][i].leans * dty;
      dty = branches[treeinuse][i].leanc * dty +
            branches[treeinuse][i].leans * dtx;
      dtx = t;
      // Rotate:
      t = branches[treeinuse][i].rotatec * dtz -
          branches[treeinuse][i].rotates * dtx;
      dtx = branches[treeinuse][i].rotatec * dtx +
            branches[treeinuse][i].rotates * dtz;
      dtz = t;

      // Scale!
      // Global scale?
      if (trees[treeinuse].glblscl) {
        dtx *= branches[treeinuse][0].scale;
        dty *= branches[treeinuse][0].scale;
        dtz *= branches[treeinuse][0].scale;
      }
      // No!, all diffrent:
      else {
        dtx *= branches[treeinuse][i].scale;
        dty *= branches[treeinuse][i].scale;
        dtz *= branches[treeinuse][i].scale;
      }

      // Scale by heigth of branch?
      if (trees[treeinuse].sctrnsl) {
        dtx *= branches[treeinuse][i].height;
        dty *= branches[treeinuse][i].height;
        dtz *= branches[treeinuse][i].height;
      }

      // Translate!
      // Use heights?
      if (trees[treeinuse].usehig)
        dty += trees[treeinuse].height * branches[treeinuse][i].height;
      // No!, all at top of stem:
      else
        dty += trees[treeinuse].height;

      // ** Pixel normal ** //
      // Rotations:

      // Do twist?:
      if (trees[treeinuse].usetwst) {
        t = branches[treeinuse][i].twistc * dntz -
            branches[treeinuse][i].twists * dntx;
        dntx = branches[treeinuse][i].twistc * dntx +
               branches[treeinuse][i].twists * dntz;
        dntz = t;
      } // Do twist?.
      // Lean:
      t = branches[treeinuse][i].leanc * dntx -
          branches[treeinuse][i].leans * dnty;
      dnty = branches[treeinuse][i].leanc * dnty +
             branches[treeinuse][i].leans * dntx;
      dntx = t;
      // Rotate:
      t = branches[treeinuse][i].rotatec * dntz -
          branches[treeinuse][i].rotates * dntx;
      dntx = branches[treeinuse][i].rotatec * dntx +
             branches[treeinuse][i].rotates * dntz;
      dntz = t;

      // Scale!
      // Global scale?
      if (trees[treeinuse].glblscl) {
        dntx *= branches[treeinuse][0].scale;
        dnty *= branches[treeinuse][0].scale;
        dntz *= branches[treeinuse][0].scale;
      }
      // No!, all diffrent:
      else {
        dntx *= branches[treeinuse][i].scale;
        dnty *= branches[treeinuse][i].scale;
        dntz *= branches[treeinuse][i].scale;
      }

      // Scale by heigth of branch?
      if (trees[treeinuse].sctrnsl) {
        dntx *= branches[treeinuse][i].height;
        dnty *= branches[treeinuse][i].height;
        dntz *= branches[treeinuse][i].height;
      }

      // Translate!
      // Use heights?
      if (trees[treeinuse].usehig)
        dnty += trees[treeinuse].height * branches[treeinuse][i].height;
      // No!, all at top of stem:
      else
        dnty += trees[treeinuse].height;

      // Re-normalize!
      dntx -= dtx;
      dnty -= dty;
      dntz -= dtz;
      t = sqrtl(dntx * dntx + dnty * dnty + dntz * dntz);
      dntx /= t;
      dnty /= t;
      dntz /= t;

      // Color:
      switch (colourmode) {
      case FUNKYCOLOURS:

        // Diffrent modes for diffrent
        // number of branches:
        switch (trees[treeinuse].branches - 1) {
        case 1:
          if (i & 0x1)
            LoCoSPali += ((PALSIZE - LoCoSPali) >> 1);
          else
            LoCoSPali >>= 1;
          break;

        case 2:
        case 3:
          if (i & 0x1)
            LoCoSPali += ((PALSIZE - LoCoSPali) >> 1);
          else
            LoCoSPali >>= 1;
          tmp = (PALSIZE >> 1) * ((i >> 1) & 0x01);
          LoCoSPali = tmp + (LoCoSPali >> 1);
          break;

        case 7:
        default:
          if (i & 0x1)
            LoCoSPali += ((PALSIZE - LoCoSPali) >> 1);
          else
            LoCoSPali >>= 1;

          tmp = (PALSIZE >> 2) * ((i >> 1) & 0x03);
          LoCoSPali = tmp + (LoCoSPali >> 2);
          break;
        } // switch numbranch.

        // Get selected colour from palette:
        tcolor = lpCols[LoCoSPali];
        tRed = (tcolor >> 16) & 0xFF;
        tGreen = (tcolor >> 8) & 0xFF;
        tBlue = tcolor & 0xFF;
        dcr = ((dcr + (tRed * 3)) >> 2) & 0xFF;
        dcg = ((dcg + (tGreen * 3)) >> 2) & 0xFF;
        dcb = ((dcb + (tBlue * 3)) >> 2) & 0xFF;

        break; // FUNKYCOLOURS.

      case NORMALCOLOURS:
      default:
        dcr = ((dcr + (lcr[pti] * 3)) >> 2) & 0xFF;
        dcg = ((dcg + (lcg[pti] * 3)) >> 2) & 0xFF;
        dcb = ((dcb + (lcb[pti] * 3)) >> 2) & 0xFF;
        break; // NORMALCOLOURS.
      } // Switch colourmode.

    } // End of the iteration loop (the tree L-IFS).
  } // If logfoliage != 2.
}

void drawAll() {
  // printf("drawing\n");

  // Read from my framebuffer, and write to the default framebuffer.
  glBindFramebuffer(GL_READ_FRAMEBUFFER, framebuffer);
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

  // Copy the colour data.
  glBlitFramebuffer(0, 0, WIDTH, HEIGHT, 0, 0, WIDTH, HEIGHT,
                    GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT |
                        GL_STENCIL_BUFFER_BIT,
                    GL_NEAREST);

  // Show what's on the default framebuffer.
  glutSwapBuffers();

  // Now bind my framebuffer again so that subsequent operations are drawn
  // there.
  glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
}

void initiateIFS(void) {
  // create constants
  phi = (1.0f + sqrtl(5.0f)) / 2.0f;
  pii = 2.0f * pi;
  rad = pii / 360.0f;
  RATIO = (double)WIDTH / (double)HEIGHT;
  RERAT = (double)HEIGHT / (double)WIDTH;

  // Load preset trees:
  loadtrees();

  // Create a fractal set:
  newsetup();

  // Also create a colour-palette:
  CreatePalette();

  // Background is visible:
  showbackground = 0;

  // Clear buffers:
  clearallbufs(bgcol[showbackground]);
}

int opensource(const char *fname) {
  if (NULL == (infile = fopen(fname, "r")))
    return (WASERROR);

  if (0 == (filesize = fread(tree, sizeof(char), sizeof(tree[0]) * 31, infile)))
    return (WASERROR);

  return (WASNOERROR);
}

void loadtrees(void) {

#define USESETUPS
#if defined USESETUPS

  numbranch = 2;
  int i;
  int j;
  t = sqrtl(1.0f / 2.0f);
  for (j = 0; j < NUMTREES; j++) {
    // Preset default branches for one tree:
    angle = 45.0f * rad;
    for (i = 0; i < 8; i++) {
      branches[j][i].height = 0.5f;
      branches[j][i].scale = t;
      branches[j][i].leanc = cosl(angle);
      branches[j][i].leans = sinl(angle);
      branches[j][i].rotatec = cosl(angle * 3.0f + angle * i * 3.0f);
      branches[j][i].rotates = sinl(angle * 3.0f + angle * i * 3.0f);
      branches[j][i].twistc = cosl(angle);
      branches[j][i].twists = sinl(angle);
      angle = -angle;
    }
    // Preset default trees:
    strcpy(trees[j].name, "Default tree");
    trees[j].branches = numbranch;
    trees[j].usehig = false;
    trees[j].glblscl = false;
    trees[j].sctrnsl = false;
    trees[j].usetwst = false;
    trees[j].radius = 0.011f;
    trees[j].height = -0.235f;
    trees[j].stem = branches[i][0];
    trees[j].branch_1 = branches[j][1];
    trees[j].branch_2 = branches[j][2];
    trees[j].branch_3 = branches[j][3];
    trees[j].branch_4 = branches[j][4];
    trees[j].branch_5 = branches[j][5];
    trees[j].branch_6 = branches[j][6];
    trees[j].branch_7 = branches[j][7];
  }

  numbranch = 3;
  i = 0;
  strcpy(trees[i].name, "Sierpinski Tree");
  trees[i].branches = numbranch;
  trees[i].usehig = false;
  trees[i].glblscl = true;
  trees[i].sctrnsl = false;
  trees[i].usetwst = false;
  trees[i].radius = 0.005f;
  trees[i].height = -0.35f;
  trees[i].stem = branches[0][0];
  trees[i].branch_1 = branches[i][1];
  trees[i].branch_2 = branches[i][2];
  trees[i].branch_3 = branches[i][3];
  trees[i].branch_4 = branches[i][4];
  trees[i].branch_5 = branches[i][5];
  trees[i].branch_6 = branches[i][6];
  trees[i].branch_7 = branches[i][7];

  t = (1.0f / 2.0f);
  branches[i][0].height = 1.0f;
  branches[i][0].scale = t;
  angle = 0.0f * rad;
  branches[i][0].leanc = cosl(angle);
  branches[i][0].leans = sinl(angle);
  branches[i][0].rotatec = cosl(angle);
  branches[i][0].rotates = sinl(angle);
  angle = 90.0f * rad;
  branches[i][0].twistc = cosl(angle);
  branches[i][0].twists = sinl(angle);

  branches[i][1].height = 0.5f;
  angle = 120.0f * rad;
  branches[i][1].leanc = cosl(angle);
  branches[i][1].leans = sinl(angle);
  angle = 0.0f * rad;
  branches[i][1].rotatec = cosl(angle);
  branches[i][1].rotates = sinl(angle);

  branches[i][2].height = 0.5f;
  angle = 120.0f * rad;
  branches[i][2].leanc = cosl(angle);
  branches[i][2].leans = sinl(angle);
  angle = 180.0f * rad;
  branches[i][2].rotatec = cosl(angle);
  branches[i][2].rotates = sinl(angle);

  numbranch = 4;
  i = 1;
  strcpy(trees[i].name, "Sierps tetra");
  trees[i].branches = numbranch;
  trees[i].usehig = false;
  trees[i].glblscl = true;
  trees[i].sctrnsl = false;
  trees[i].usetwst = false;
  trees[i].radius = 0.005f;
  trees[i].height = -0.35f;
  trees[i].stem = branches[0][0];
  trees[i].branch_1 = branches[i][1];
  trees[i].branch_2 = branches[i][2];
  trees[i].branch_3 = branches[i][3];
  trees[i].branch_4 = branches[i][4];
  trees[i].branch_5 = branches[i][5];
  trees[i].branch_6 = branches[i][6];
  trees[i].branch_7 = branches[i][7];

  t = (1.0f / 2.0f);
  branches[i][0].height = 1.0f;
  branches[i][0].scale = t;
  angle = 0.0f * rad;
  branches[i][0].leanc = cosl(angle);
  branches[i][0].leans = sinl(angle);
  branches[i][0].rotatec = cosl(angle);
  branches[i][0].rotates = sinl(angle);
  angle = 90.0f * rad;
  branches[i][0].twistc = cosl(angle);
  branches[i][0].twists = sinl(angle);

  branches[i][1].height = 0.5f;
  angle = 110.0f * rad;
  branches[i][1].leanc = cosl(angle);
  branches[i][1].leans = sinl(angle);
  angle = 60.0f * rad;
  branches[i][1].rotatec = cosl(angle);
  branches[i][1].rotates = sinl(angle);

  branches[i][2].height = 0.5f;
  angle = 110.0f * rad;
  branches[i][2].leanc = cosl(angle);
  branches[i][2].leans = sinl(angle);
  angle = 180.0f * rad;
  branches[i][2].rotatec = cosl(angle);
  branches[i][2].rotates = sinl(angle);

  branches[i][3].height = 0.5f;
  angle = 110.0f * rad;
  branches[i][3].leanc = cosl(angle);
  branches[i][3].leans = sinl(angle);
  angle = 300.0f * rad;
  branches[i][3].rotatec = cosl(angle);
  branches[i][3].rotates = sinl(angle);

  numbranch = 2;
  i = 2;
  strcpy(trees[i].name, "Lévy curve Tree");
  trees[i].branches = numbranch;
  trees[i].usehig = false;
  trees[i].glblscl = true;
  trees[i].sctrnsl = false;
  trees[i].usetwst = false;
  trees[i].radius = 0.015f;
  trees[i].height = -0.23f;
  trees[i].stem = branches[1][0];
  trees[i].branch_1 = branches[i][1];
  trees[i].branch_2 = branches[i][2];
  trees[i].branch_3 = branches[i][3];
  trees[i].branch_4 = branches[i][4];
  trees[i].branch_5 = branches[i][5];
  trees[i].branch_6 = branches[i][6];
  trees[i].branch_7 = branches[i][7];

  t = sqrtl(1.0f / 2.0f);
  branches[i][0].height = 1.0f;
  branches[i][0].scale = t;
  angle = 45.0f * rad;
  branches[i][0].leanc = cosl(angle);
  branches[i][0].leans = sinl(angle);
  angle = 0.0f * rad;
  branches[i][0].rotatec = cosl(angle);
  branches[i][0].rotates = sinl(angle);

  angle = 45.0f * rad;
  branches[i][1].leanc = cosl(angle);
  branches[i][1].leans = sinl(angle);
  angle = 180.0f * rad;
  branches[i][1].rotatec = cosl(angle);
  branches[i][1].rotates = sinl(angle);

  numbranch = 2;
  i = 3;
  strcpy(trees[i].name, "Y-Tree!");
  trees[i].branches = numbranch;
  trees[i].usehig = false;
  trees[i].glblscl = true;
  trees[i].sctrnsl = false;
  trees[i].usetwst = true;
  trees[i].radius = 0.013f;
  trees[i].height = -0.25f;
  trees[i].stem = branches[2][0];
  trees[i].branch_1 = branches[i][1];
  trees[i].branch_2 = branches[i][2];
  trees[i].branch_3 = branches[i][3];
  trees[i].branch_4 = branches[i][4];
  trees[i].branch_5 = branches[i][5];
  trees[i].branch_6 = branches[i][6];
  trees[i].branch_7 = branches[i][7];

  t = sqrtl(1.0f / 2.0f);
  branches[i][0].height = 1.0f;
  branches[i][0].scale = t;
  angle = 45.0f * rad;
  branches[i][0].leanc = cosl(angle);
  branches[i][0].leans = sinl(angle);
  angle = 0.0f * rad;
  branches[i][0].rotatec = cosl(angle);
  branches[i][0].rotates = sinl(angle);
  angle = 90.0f * rad;
  branches[i][0].twistc = cosl(angle);
  branches[i][0].twists = sinl(angle);

  angle = -45.0f * rad;
  branches[i][1].leanc = cosl(angle);
  branches[i][1].leans = sinl(angle);
  angle = 0.0f * rad;
  branches[i][1].rotatec = cosl(angle);
  branches[i][1].rotates = sinl(angle);
  angle = -90.0f * rad;
  branches[i][1].twistc = cosl(angle);
  branches[i][1].twists = sinl(angle);

  numbranch = 4;
  i = 4;
  strcpy(trees[i].name, "Paratrooper");
  trees[i].branches = numbranch;
  trees[i].usehig = false;
  trees[i].glblscl = true;
  trees[i].sctrnsl = false;
  trees[i].usetwst = false;
  trees[i].radius = 0.010f;
  trees[i].height = -0.375f;
  trees[i].stem = branches[2][0];
  trees[i].branch_1 = branches[i][1];
  trees[i].branch_2 = branches[i][2];
  trees[i].branch_3 = branches[i][3];
  trees[i].branch_4 = branches[i][4];
  trees[i].branch_5 = branches[i][5];
  trees[i].branch_6 = branches[i][6];
  trees[i].branch_7 = branches[i][7];

  t = (1.0f / 2.0f);
  branches[i][0].height = 1.0f;
  branches[i][0].scale = t;
  angle = 45.0f * rad;
  branches[i][0].leanc = cosl(angle);
  branches[i][0].leans = sinl(angle);
  angle = 0.0f * rad;
  branches[i][0].rotatec = cosl(angle);
  branches[i][0].rotates = sinl(angle);

  angle = 45.0f * rad;
  branches[i][1].leanc = cosl(angle);
  branches[i][1].leans = sinl(angle);
  angle = 90.0f * rad;
  branches[i][1].rotatec = cosl(angle);
  branches[i][1].rotates = sinl(angle);

  angle = 45.0f * rad;
  branches[i][2].leanc = cosl(angle);
  branches[i][2].leans = sinl(angle);
  angle = 180.0f * rad;
  branches[i][2].rotatec = cosl(angle);
  branches[i][2].rotates = sinl(angle);

  angle = 45.0f * rad;
  branches[i][3].leanc = cosl(angle);
  branches[i][3].leans = sinl(angle);
  angle = 270.0f * rad;
  branches[i][3].rotatec = cosl(angle);
  branches[i][3].rotates = sinl(angle);

  numbranch = 5;
  i = 5;
  strcpy(trees[i].name, "Twister spruce");
  trees[i].branches = numbranch;
  trees[i].usehig = false;
  trees[i].glblscl = false;
  trees[i].sctrnsl = false;
  trees[i].usetwst = true;
  trees[i].radius = 0.005f;
  trees[i].height = -0.26f;
  trees[i].stem = branches[2][0];
  trees[i].branch_1 = branches[i][1];
  trees[i].branch_2 = branches[i][2];
  trees[i].branch_3 = branches[i][3];
  trees[i].branch_4 = branches[i][4];
  trees[i].branch_5 = branches[i][5];
  trees[i].branch_6 = branches[i][6];
  trees[i].branch_7 = branches[i][7];

  branches[i][0].height = 1.0f;
  branches[i][0].scale = (phi - 1.0f);
  angle = 0.0f * rad;
  branches[i][0].leanc = cosl(angle);
  branches[i][0].leans = sinl(angle);
  angle = 0.0f * rad;
  branches[i][0].rotatec = cosl(angle);
  branches[i][0].rotates = sinl(angle);
  angle = 22.5f * rad;
  branches[i][0].twistc = cosl(angle);
  branches[i][0].twists = sinl(angle);

  t = (2.0f - phi);
  branches[i][1].height = 0.5f;
  branches[i][1].scale = t;
  angle = 90.0f * rad;
  branches[i][1].leanc = cosl(angle);
  branches[i][1].leans = sinl(angle);
  angle = 0.0f * rad;
  branches[i][1].rotatec = cosl(angle);
  branches[i][1].rotates = sinl(angle);
  angle = 0.0f * rad;
  branches[i][1].twistc = cosl(angle);
  branches[i][1].twists = sinl(angle);

  branches[i][2].height = 0.5f;
  branches[i][2].scale = t;
  angle = 90.0f * rad;
  branches[i][2].leanc = cosl(angle);
  branches[i][2].leans = sinl(angle);
  angle = 90.0f * rad;
  branches[i][2].rotatec = cosl(angle);
  branches[i][2].rotates = sinl(angle);
  angle = 0.0f * rad;
  branches[i][2].twistc = cosl(angle);
  branches[i][2].twists = sinl(angle);

  branches[i][3].height = 0.5f;
  branches[i][3].scale = t;
  angle = 90.0f * rad;
  branches[i][3].leanc = cosl(angle);
  branches[i][3].leans = sinl(angle);
  angle = 180.0f * rad;
  branches[i][3].rotatec = cosl(angle);
  branches[i][3].rotates = sinl(angle);
  angle = 0.0f * rad;
  branches[i][3].twistc = cosl(angle);
  branches[i][3].twists = sinl(angle);

  branches[i][4].height = 0.5f;
  branches[i][4].scale = t;
  angle = 90.0f * rad;
  branches[i][4].leanc = cosl(angle);
  branches[i][4].leans = sinl(angle);
  angle = 270.0f * rad;
  branches[i][4].rotatec = cosl(angle);
  branches[i][4].rotates = sinl(angle);
  angle = 0.0f * rad;
  branches[i][4].twistc = cosl(angle);
  branches[i][4].twists = sinl(angle);

#endif // USESETUPS

  if (WASERROR == opensource("Trees.IFS")) {
    printf("could not load trees\n");
    exit(1);
  }

  // FIXME: start loading trees from file
  // Load the preset trees and
  // branches from file-buffer:
  // loadtree();
}

void loadtree(void) {
  for (int i = 0; i < 31; i++) {
    memcpy(trees[i].name, tree[i].name, 15);
    trees[i].name[15] = 0;
    trees[i].branches = tree[i].branches;
    trees[i].usehig = tree[i].usehig;
    trees[i].glblscl = tree[i].glblscl;
    trees[i].sctrnsl = tree[i].sctrnsl;
    trees[i].usetwst = tree[i].usetwst;
    trees[i].radius = tree[i].radius;
    trees[i].height = tree[i].height;
    branches[i][0].height = tree[i].height0;
    branches[i][0].scale = tree[i].scale0;
    branches[i][0].leanc = tree[i].leanc0;
    branches[i][0].leans = tree[i].leans0;
    branches[i][0].rotatec = tree[i].rotatec0;
    branches[i][0].rotates = tree[i].rotates0;
    branches[i][0].twistc = tree[i].twistc0;
    branches[i][0].twists = tree[i].twists0;

    branches[i][1].height = tree[i].height1;
    branches[i][1].scale = tree[i].scale1;
    branches[i][1].leanc = tree[i].leanc1;
    branches[i][1].leans = tree[i].leans1;
    branches[i][1].rotatec = tree[i].rotatec1;
    branches[i][1].rotates = tree[i].rotates1;
    branches[i][1].twistc = tree[i].twistc1;
    branches[i][1].twists = tree[i].twists1;

    branches[i][2].height = tree[i].height2;
    branches[i][2].scale = tree[i].scale2;
    branches[i][2].leanc = tree[i].leanc2;
    branches[i][2].leans = tree[i].leans2;
    branches[i][2].rotatec = tree[i].rotatec2;
    branches[i][2].rotates = tree[i].rotates2;
    branches[i][2].twistc = tree[i].twistc2;
    branches[i][2].twists = tree[i].twists2;

    branches[i][3].height = tree[i].height3;
    branches[i][3].scale = tree[i].scale3;
    branches[i][3].leanc = tree[i].leanc3;
    branches[i][3].leans = tree[i].leans3;
    branches[i][3].rotatec = tree[i].rotatec3;
    branches[i][3].rotates = tree[i].rotates3;
    branches[i][3].twistc = tree[i].twistc3;
    branches[i][3].twists = tree[i].twists3;

    branches[i][4].height = tree[i].height4;
    branches[i][4].scale = tree[i].scale4;
    branches[i][4].leanc = tree[i].leanc4;
    branches[i][4].leans = tree[i].leans4;
    branches[i][4].rotatec = tree[i].rotatec4;
    branches[i][4].rotates = tree[i].rotates4;
    branches[i][4].twistc = tree[i].twistc4;
    branches[i][4].twists = tree[i].twists4;

    branches[i][5].height = tree[i].height5;
    branches[i][5].scale = tree[i].scale5;
    branches[i][5].leanc = tree[i].leanc5;
    branches[i][5].leans = tree[i].leans5;
    branches[i][5].rotatec = tree[i].rotatec5;
    branches[i][5].rotates = tree[i].rotates5;
    branches[i][5].twistc = tree[i].twistc5;
    branches[i][5].twists = tree[i].twists5;

    branches[i][6].height = tree[i].height6;
    branches[i][6].scale = tree[i].scale6;
    branches[i][6].leanc = tree[i].leanc6;
    branches[i][6].leans = tree[i].leans6;
    branches[i][6].rotatec = tree[i].rotatec6;
    branches[i][6].rotates = tree[i].rotates6;
    branches[i][6].twistc = tree[i].twistc6;
    branches[i][6].twists = tree[i].twists6;

    branches[i][7].height = tree[i].height7;
    branches[i][7].scale = tree[i].scale7;
    branches[i][7].leanc = tree[i].leanc7;
    branches[i][7].leans = tree[i].leans7;
    branches[i][7].rotatec = tree[i].rotatec7;
    branches[i][7].rotates = tree[i].rotates7;
    branches[i][7].twistc = tree[i].twistc7;
    branches[i][7].twists = tree[i].twists7;
  } // forlooping
}

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

void createbackground(void) {
  // Size of ground square:
  x = grounds[groundsize];
  // Heihgt: (center as is =)
  t = 0.0f;

  // Translation coordinates:
  tx[0] = -x;
  ty[0] = t;
  tz[0] = x;

  tx[1] = x;
  ty[1] = t;
  tz[1] = x;

  tx[2] = x;
  ty[2] = t;
  tz[2] = -x;

  tx[3] = -x;
  ty[3] = t;
  tz[3] = -x;

  // Colours:
  tcr[2] = tcr[0] = 0xFF * RND;
  tcg[2] = tcg[0] = 0xFF * RND;
  tcb[2] = tcb[0] = 0xFF * RND;
  tcr[3] = tcr[1] = 0xFF;
  tcg[3] = tcg[1] = 0xFF;
  tcb[3] = tcb[1] = 0xFF;

  // Scale:
  for (int i = 0; i < 4; i++)
    sc[i] = (1.0f / 2.0f);
}

void randombranch(int indx) {
  if (indx == 0) {
    // Always set 'stem' to top of the stub:
    branches[31][indx].height = 1.0f;
    // Choose phase for slope angle:
    // Rather point up than sideways for stem-branch (RND^3):
    t = RND * RND * RND;
  } else {
    // Set height of stem where the brach resides:
    branches[31][indx].height = 1.0f - RND * RND * 0.9f;
    // Choose phase for slope angle:
    t = RND * RND;
  }

  // Set scale:
  branches[31][indx].scale = scales[int(RND * 7)];

  // Set random slope angle:
  angle = 180.0f * rad * t;
  branches[31][indx].leans = sinl(angle);
  branches[31][indx].leanc = cosl(angle);

  // Set random rotation angle:
  angle = 360.0f * rad * RND;
  branches[31][indx].rotates = cosl(angle);
  branches[31][indx].rotatec = sinl(angle);

  // Set random twist angle:
  angle = 360.0f * rad * RND;
  branches[31][indx].twistc = cosl(angle);
  branches[31][indx].twists = sinl(angle);
}

void leafcols(void) {
  switch (colourmode) {
  case FUNKYCOLOURS:
    for (int i = 4; i < (ANTAL + 4); i++) {
      tcr[i] = 0xFF * RND;
      tcg[i] = 0xFF * RND;
      tcb[i] = 0xFF * RND;
    }
    break; // FUNKYCOLOURS.
  case NORMALCOLOURS:
  default:
    for (int i = 4; i < (ANTAL + 4); i++) {
      tcr[i] = 0x60 + 160 * RND * RND;
      tcg[i] = 0x80 + 128 * RND;
      tcb[i] = 0x20 + 32 * RND;
    }
    break; // NORMALCOLOURS.
  } // Switch colourmode.
}

void stemcols(void) {
  for (int i = 0; i <= ilevels; i++) {
    lcr[i] = 0xFF;
    lcg[i] = 0xC0;
    lcb[i] = 0x80;
  }

  lcr[0] = 0x00;
  lcg[0] = 0x80;
  lcb[0] = 0x00;

  lcr[1] = 0x30;
  lcg[1] = 0x90;
  lcb[1] = 0x10;

  lcr[3] = 0x60;
  lcg[3] = 0xA0;
  lcb[3] = 0x30;

  lcr[4] = 0xC0;
  lcg[4] = 0xB0;
  lcb[4] = 0x50;

  lcr[ilevels] = 0x10 * 12;
  lcg[ilevels] = 0x0C * 12;
  lcb[ilevels] = 0x08 * 12;

  lcr[ilevels - 1] = 0x10 * 13;
  lcg[ilevels - 1] = 0x0C * 13;
  lcb[ilevels - 1] = 0x08 * 13;

  lcr[ilevels - 2] = 0x10 * 14;
  lcg[ilevels - 2] = 0x0C * 14;
  lcb[ilevels - 2] = 0x08 * 14;

  lcr[ilevels - 3] = 0x10 * 15;
  lcg[ilevels - 3] = 0x0C * 15;
  lcb[ilevels - 3] = 0x08 * 15;
}

void LitAng(void) {
  lryx = cosl(lryv);
  lryy = sinl(lryv);
  lrxx = cosl(lrxv);
  lrxy = sinl(lrxv);
}

void CamAng(void) {
  ryx = cosl(ryv);
  ryy = sinl(ryv);
  rxx = cosl(rxv);
  rxy = sinl(rxv);
}

void clearViewmess(void) {
  textline(360, 2, (char *)"Press [V] to clear view.", MEDIUMFONT,
           txcol[showbackground]);
}

void pixelsmess(void) {
  sprintf(
      stringbuf,
      "%li iterations done, %li image pixels, %li shadow pixels.  (press [V] "
      "to clear view).",
      itersdone, pixelswritten, shadowswritten);
  textline(192, 2, stringbuf, MEDIUMFONT, lcol);
}

void newsetup(void) {
  // A rest from my 3D reversed Julia IFS demo:
  // (http://members.chello.se/solgrop/3djulia.htm)
  // Was used to select among presets. Also stored the.
  // coordinate for the diffrent presets, that part remains.
  ui = 0;

  // Preset a number of scale ratios:
  scales[0] = sqrtl(2.0f / 3.0f);
  scales[1] = sqrtl(1.0f / 2.0f);
  scales[2] = (2.0f / 3.0f);
  scales[3] = (phi - 1.0f);
  scales[4] = sqrtl(1.0f / 3.0f);
  scales[5] = (1.0f / 2.0f);
  scales[6] = (2.0f - phi);

  //////////////////////////////
  // *** FRACTAL SETUPS ! *** //

  //////////////////////////////

  // 2D Sierpinski squares as ground plate:
  createbackground();

  // Use 'half-note' steps for size modifier:
  t = (1.0f / 64.0f);
  twup = pow(2.0f, t);
  twdwn = pow(2.0f, -t);

  // ********************
  // RANDOMIZE PRESET #31
  // ********************
  //(And that's #00 in the demo, I'm using (n+1) AND 0x1F
  // for print, but it is not the first tree in the list =)

  // First name the tree:
  strcpy(trees[31].name, "Random Tree");

  // Set number of branches for the tree:
  // (First case is 'random branches',
  //  second uses the preset numbers).
  if (selnumbranch == 0)
    numbranch = 2 + RND * RND * 7;
  else
    numbranch = selnumbranch + 1;
  trees[31].branches = numbranch;

  // Randomize flags:
  trees[31].usehig = RNDBOOL;
  trees[31].glblscl = RNDBOOL;
  trees[31].sctrnsl = RNDBOOL;
  trees[31].usetwst = RNDBOOL;

  // Randomize radius and height for the root-stem (stub):
  t = (1.0 + RND);
  trees[31].radius = 0.002f + 0.04f * RND * RND;
  trees[31].height = -t / (4.0f + (12.0f * RND));

  // Randomize branches:
  // (Height, scale, slope and rotation).
  for (int i = 0; i < 8; i++)
    randombranch(i);

  // Create colours for the leafs:
  leafcols();

  // And the colours for the stem:
  stemcols();

  // End Fractal!

  /// ******* Light & Camera ******* ///

  // Light-angle:
  lryv = -26.0f * rad;
  lrxv = 45.0f * rad;
  LitAng();

  // Calculate position of light source!
  ntx = 0.0f;
  nty = 0.0f;
  ntz = 1.0f;

  // Rotate to position!
  t = lryx * yt - lryy * zt;
  zt = lryx * zt + lryy * yt;
  yt = t;

  t = lrxx * zt - lrxy * xt;
  xt = lrxx * xt + lrxy * zt;
  zt = t;

  // Camera-angle:
  ryv = 0.0f * rad;
  rxv = 0.0f * rad;
  CamAng();
  // Camera-angle.

  // IFS rotators!
  drzv = 0.0f * rad;
  drzx = cosl(drzv);
  drzy = sinl(drzv);
  dryv = 0.0f * rad;
  dryx = cosl(dryv);
  dryy = sinl(dryv);

  // Reset parameters!
  btx = tx[0];
  bty = ty[0];
  btz = tz[0];

  bcr = tcr[0];
  bcg = tcg[0];
  bcb = tcb[0];

  dtx = tx[4];
  dty = ty[4];
  dtz = tz[4];

  dcr = tcr[4];
  dcg = tcg[4];
  dcb = tcb[4];

  newrender();
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

void IFSplot(void) {
  // Rotate to angle of view:
  rotateview();

  // Clip z:
  glBegin(GL_POINTS);
  if ((zt > -1.0f) && (zt < 1.0f)) {
    size = (3.0f + zt) / 2.0f;
    nZ = int((2.0f - size) * (ZDEPTH >> 1)) & 0x7FFF;
    zt = (ims * imszoom) / size;

    nY = BMIDY + int(yt * zt);
    nX = BMIDX + int(xt * zt);
    // Clip y:
    if ((nY >= 0) && (nY < BHEIGHT)) {
      // Clip x:
      if ((nX >= 0) && (nX < BWIDTH)) {
        // Plot if Point closer to viewer than
        // the previous at the position:
        if (bpict[nY][nX] < nZ) {
          // Write new depth to z-buffer:
          bpict[nY][nX] = nZ;

          // Update pixel counter:
          pixelswritten++;

          // Brighter than average?
          if (overexpose) {
            crt = int((crt + overexpose) / luma);
            cgt = int((cgt + overexpose) / luma);
            cbt = int((cbt + overexpose) / luma);
          } // Overexpose.

          // Whiter shade of pale?:
          if (whitershade) {
            // Cold in varm:
            if (whitershade == 1) {
              crt = (((crt * 3) + bright) >> 2) & 0xFF;
              cgt = (((cgt << 1) + bright) / 3) & 0xFF;
              cbt = ((cbt + bright) >> 1) & 0xFF;
            }
            // Varm in cold:
            else {
              crt = ((crt + bright) >> 1) & 0xFF;
              cgt = (((cgt << 1) + bright) / 3) & 0xFF;
              cbt = (((cbt * 3) + bright) >> 2) & 0xFF;
            }
          } // Whiter shade of pale.

          crt = ((crt * bright) >> 8) & 0xFF;
          cgt = ((cgt * bright) >> 8) & 0xFF;
          cbt = ((cbt * bright) >> 8) & 0xFF;
          tcolor = ((crt << 16) + (cgt << 8) + cbt) & 0xFFFFFF;
          pict[nY][nX] = tcolor;

          // ******************************
          // Anti anlize from pixel-buffer:
          // ******************************
          // 2x2 grid:
          nY = nY & 0xFFFE;
          nX = nX & 0xFFFE;

          // Reset colours:
          ncols = 4;
          tRed = 0x00;
          tBlue = 0x00;
          tGreen = 0x00;

          // 2x2 pixels to 1 pixel:
          for (int yi = 0; yi < 2; yi++) {
            nYt = nY + yi;
            if ((nYt >= 0) && (nYt < BHEIGHT)) {
              for (int xi = 0; xi < 2; xi++) {
                nXt = nX + xi;
                if ((nXt >= 0) && (nXt < BWIDTH)) {
                  tcolor = pict[nYt][nXt];
                  tRed += (tcolor >> 16) & 0xFF;
                  tGreen += (tcolor >> 8) & 0xFF;
                  tBlue += tcolor & 0xFF;
                } // Clip x.
              } // for xi.
            } // Clip y.
          } // for yi.
          tRed = (tRed / ncols) & 0xFF;
          tGreen = (tGreen / ncols) & 0xFF;
          tBlue = (tBlue / ncols) & 0xFF;
          // End anti anilize.

          // Convert 8-bit red, green & blue to 32-bit xRGB:
          tcolor = ((tRed << 16) + (tGreen << 8) + tBlue) & 0x00FFFFFF;

          // ***********************
          // Write to screen buffer:
          // ***********************
          // 2x2 grid to 1x1 dito:
          nY = nY >> 1;
          nX = nX >> 1;
          setColour(tcolor);
          glVertex2i(nX, nY);
        } // Z-plot view.
      } // clip - X.
    } // clip - Y.
  } // clip - Z.
  glEnd();
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
  unsigned int r = (RGBdata & 0x00FF0000) >> 16;
  unsigned int g = (RGBdata & 0x0000FF00) >> 8;
  unsigned int b = RGBdata & 0x000000FF;
  glClearColor((float)r / 255.0f, (float)g / 255.0f, (float)b / 255.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);
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
  long tc;
  float fdout, fdin, fdout2, fdin2, fdouts, fdins, ufade0;
  float rl, gl, bl;

  float freq = 1.0f + RND * RND * RND * 256.0f;
  float rf = freq * (1.0f + RND * pi);
  float gf = freq * (1.0f + RND * pi);
  float bf = freq * (1.0f + RND * pi);

  // Randomize palette-modes:
  bool invert = (RND > 0.5f);
  bool lightobject = (RND > 0.75f);
  bool vertin = (RND > 0.75f);
  bool heatvawe = (RND > 0.95f);
  bool sinvawe = (RND > 0.75f);
  bool bakwrds = (RND > 0.5f);

  for (int i = 0; i != PALSIZE; i++) {
    fdout = float(PALSIZE - i) / PALSIZE;
    if (bakwrds)
      fdout = float(i) / PALSIZE;

    fdout2 = fdout * fdout;
    fdouts = sqrt(fdout);

    fdin = 1.0f - fdout;
    fdin2 = 1.0f - fdouts;
    fdins = 1.0f - fdout2;

    ufade0 = fdout;
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
  // Draw border:
  FILLBOX(WIDTH - 306, 294, WIDTH - 46, 554, 0x00A0A0A0);

  tmp = WIDTH - 304;

  glBegin(GL_POINTS);
  switch (mode) {
  case SERP:
    for (int pr = (256 - 1); pr >= 0; pr--) {
      for (int pp = 0; pp < 256; pp++) {
        setColour(
            lpCols[(int(pp * (PALSIZE / 256)) | int(pr * (PALSIZE / 256)))]);
        glVertex2i(tmp + pp, HEIGHT - 304 + pr);
      }
    }
    break;

  case ABSZ:
    int pz;
    for (int pr = (256 - 1); pr >= 0; pr--) {
      for (int pp = 0; pp < 256; pp++) {
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

void drawBoxi(void) {
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

  int vertices[] = {
      lixs, liys, 0, lixe, liys, 0, lixs, liye, 0,
      lixe, liys, 0, lixs, liye, 0, lixe, liye, 0,
  };
  int colour[] = {(int)lcol, (int)lcol, (int)lcol,
                  (int)lcol, (int)lcol, (int)lcol};

  // Generate a VAO and a VBO
  unsigned int VAO, VBO, VBOcolour;
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &VBOcolour);

  // Bind the VAO
  glBindVertexArray(VAO);

  // Copy vertex data into the buffer's memory
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  // Specify how the vertex data should be interpreted
  glVertexAttribPointer(0, 3, GL_INT, GL_FALSE, 3 * sizeof(int), (void *)0);
  glEnableVertexAttribArray(0);

  glBindBuffer(GL_ARRAY_BUFFER, VBOcolour);
  glBufferData(GL_ARRAY_BUFFER, sizeof(colour), colour, GL_STATIC_DRAW);

  glVertexAttribPointer(1, 3, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(int),
                        (void *)0);
  glEnableVertexAttribArray(1);

  // in render loop:
  basicShader->use();
  basicShader->setMat4("projection", projectionMatrix);

  // render the triangle
  glBindVertexArray(VAO);
  glDrawArrays(GL_TRIANGLES, 0, 6);

  // unbind everything
  glBindVertexArray(0);
  glUseProgram(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
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
  glLoadIdentity(); // Reset The View

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
    drawAll();
    paintOnNextFrame = false;
  }

  // if render screen is up then iterate:
  if ((programMode == 0) && renderactive) {
    DoMyStuff();
    drawAll();
  }
}

void keyDownCallbackSpecial(int key, int x, int y) {
  /* avoid thrashing this call */
  usleep(100);

  bool valid_keypress = true;

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
    default:
      valid_keypress = false;
      break;

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
    default:
      valid_keypress = false;
      break;

    } // wparam (info screen).

    break;
  default:
    valid_keypress = false;
    break;
  }

  if (valid_keypress) {
    drawAll();
  }
}

void keyUpCallbackSpecial(int key, int x, int y) {
  /* avoid thrashing this call */
  usleep(100);

  // Functions only active if render screen:
  if (renderactive) {
    bool valid_keypress = true;

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
    default:
      valid_keypress = false;
      break;
    }

    if (valid_keypress) {
      drawAll();
    }
  }
}

/* The function called whenever a key is pressed down. */
void keyDownCallback(unsigned char key, int x, int y) {
  /* avoid thrashing this call */
  usleep(100);

  bool valid_keypress = true;

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
    default:
      valid_keypress = false;
      break;

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
    default:
      valid_keypress = false;
      break;

    } // wparam (info screen).

    break;
  default:
    valid_keypress = false;
    break;
  }

  if (valid_keypress) {
    drawAll();
  }
}

/* The function called whenever a key is released. */
void keyUpCallback(unsigned char key, int x, int y) {
  /* avoid thrashing this call */
  usleep(100);

  // Functions only active if render screen:
  if (renderactive) {
    bool valid_keypress = true;

    switch (key) {
    case 'D':
    case 'F':
    case 'S':
    case 'X':
      clearallbufs(bgcol[showbackground]);
      clearscreen(bgcol[showbackground]);
      SunCode();
      break; // Clear all keys.
    default:
      valid_keypress = false;
      break;
    }

    if (valid_keypress) {
      drawAll();
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

  initiateIFS();

  /* Initialize our window. */
  InitGL(WIDTH, HEIGHT);

  glewInit();

  glGenFramebuffers(1, &framebuffer);
  glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
  unsigned int textureColorbuffer;
  glGenTextures(1, &textureColorbuffer);
  glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, WIDTH, HEIGHT, 0, GL_RGB,
               GL_UNSIGNED_BYTE, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glBindTexture(GL_TEXTURE_2D, 0);

  // attach it to currently bound framebuffer object
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                         textureColorbuffer, 0);

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
    printf("bad\n");
    exit(1);
  }

  Shader basicShaderProgram("basic.vs", "basic.fs");
  basicShader = &basicShaderProgram;
  projectionMatrix =
      glm::ortho(0.0f, (float)WIDTH, (float)HEIGHT, 0.0f, -1.0f, 1.0f);

  // FIXME: stop pretending that space is pressed on launch
  programMode = 0;
  paintOnNextFrame = true;

  /* Start Event Processing Engine */
  glutMainLoop();

  return 1;
}
