#pragma once
#ifdef _MSC_VER                         // Check if MS Visual C compiler
#  pragma comment(lib, "opengl32.lib")  // Compiler-specific directive to avoid manually configuration
#  pragma comment(lib, "glu32.lib")     // Link libraries
#endif
// Ustalanie trybu tekstowego:
#ifdef _MSC_VER        // Check if MS Visual C compiler
#   ifndef _MBCS
#      define _MBCS    // Uses Multi-byte character set
#   endif
#   ifdef _UNICODE     // Not using Unicode character set
#      undef _UNICODE 
#   endif
#   ifdef UNICODE
#      undef UNICODE 
#   endif
#endif

#include <Windows.h>            // Window defines
#include <Windowsx.h>
#include <Commctrl.h>
#include <tchar.h>  
#include <gl\gl.h>              // OpenGL
#include <gl\glu.h>             // GLU library
#define _USE_MATH_DEFINES
#include <cmath>
#include <iostream>
#include <cstdio>
#include <ctime>
#include <string>
#include <vector>
#include <list>
#include "TwInterface.h"
#include "resource.h"           // About box resource identifiers.

GLfloat rand(GLint beg, GLint end);
unsigned char *LoadBitmapFile(char *filename, BITMAPINFOHEADER *bitmapInfoHeader);