#define  _CRT_SECURE_NO_WARNINGS

#include "main.h"
#include "machine.h"
#include "Creator.h"

#define glRGB(x, y, z)	glColor3ub((GLubyte)x, (GLubyte)y, (GLubyte)z)
#define BITMAP_ID 0x4D42		// identyfikator formatu BMP
#define GL_PI 3.14159265358979
static HPALETTE hPalette = NULL;

static LPCTSTR lpszAppName = "Maszyna 1R2T";
static HINSTANCE hInstance;


static GLsizei lastHeight;
static GLsizei lastWidth;
// Opis tekstury

//********************************************************************************************************************************************OBIEKTY
static Machine machine;
static float CameraAngleV[4] = {0, 0, 0, 1};
static draw::Point3D CameraAngle(70, 0, -30);
static GLint view_mode = 2;
static MoveType mt = MV_SAFE;
static draw::Point3D movePoint;



#define _TIMER (USER_TIMER_MINIMUM)
#define _USE_TIMER

#define WND_X 1920
#define WND_Y 1080

void printHUD(HDC hDC);
//*************************

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
void SetDCPixelFormat(HDC hDC);


void ChangeSize(GLsizei w, GLsizei h)
{
	GLfloat nRange = 100.0f;
	GLfloat fAspect;
	if (h == 0)
		h = 1;

	lastWidth = w;
	lastHeight = h;

	fAspect = (GLfloat)w / (GLfloat)h;
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	if (w <= h) {
		glOrtho(-nRange, nRange, -nRange*h / w, nRange*h / w, -nRange * 5, nRange * 5);
	} else {
		glOrtho(-nRange*fAspect, nRange*fAspect, -nRange, nRange, -nRange * 1.5f, nRange * 9);	
	}
	// Establish perspective: 
	//gluPerspective(180.0f, fAspect, -nRange * 2, nRange * 2);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}
void SetupRC()
{
	glEnable(GL_DEPTH_TEST);
	glFrontFace(GL_CCW);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_LIGHTING);
	glEnable(GL_NORMALIZE);

	GLfloat ambientLight0[] = { 0.25f, 0.25f, 0.25f, 1.0f }; // { 0.3f, 0.3f, 0.3f, 1.0f };
	GLfloat diffuseLight0[] = { 0.03f, 0.03f, 0.03f, 1.0f };
	GLfloat specular0[] = { 0.01f, 0.01f, 0.01f, 1.0f }; // { 1.0f, 1.0f, 1.0f, 1.0f};

	glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight0);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight0);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specular0);
	//glLightf(GL_LIGHT0, GL_SPOT_EXPONENT, 64);
	glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, 180);

	glEnable(GL_LIGHT0);

	GLfloat ambientLight1[] = { 0.05f, 0.05f, 0.05f, 1.0f }; // { 0.3f, 0.3f, 0.3f, 1.0f };
	GLfloat diffuseLight1[] = { 0.4f, 0.4f, 0.4f, 1.0f };
	GLfloat specular1[] = { 0.8f, 0.8f, 0.8f, 1.0f }; // { 1.0f, 1.0f, 1.0f, 1.0f};
	
	glLightfv(GL_LIGHT1,GL_AMBIENT, ambientLight1);
	glLightfv(GL_LIGHT1,GL_DIFFUSE, diffuseLight1);
	glLightfv(GL_LIGHT1,GL_SPECULAR, specular1);
	//glLightf(GL_LIGHT1, GL_SPOT_EXPONENT, 10);
	glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, 90);

	glEnable(GL_LIGHT1); 

	GLfloat ambientLight2[] = { 0.05f, 0.05f, 0.05f, 1.0f }; // { 0.3f, 0.3f, 0.3f, 1.0f };
	GLfloat diffuseLight2[] = { 0.5f, 0.5f, 0.5f, 1.0f };
	GLfloat specular2[] = { 0.5f, 0.5f, 0.5f, 1.0f }; // { 1.0f, 1.0f, 1.0f, 1.0f};

	glLightfv(GL_LIGHT2, GL_AMBIENT, ambientLight1);
	glLightfv(GL_LIGHT2, GL_DIFFUSE, diffuseLight1);
	glLightfv(GL_LIGHT2, GL_SPECULAR, specular1);
	//glLightf(GL_LIGHT1, GL_SPOT_EXPONENT, 10);
	glLightf(GL_LIGHT2, GL_SPOT_CUTOFF, 180);

	glEnable(GL_LIGHT2);

	/* Enable color tracking */
	glEnable(GL_COLOR_MATERIAL);

	// Set Material properties to follow glColor values */
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT);
	glColorMaterial(GL_FRONT_AND_BACK, GL_DIFFUSE);
	glColorMaterial(GL_FRONT_AND_BACK, GL_SPECULAR);
	glColorMaterial(GL_FRONT_AND_BACK, GL_SHININESS);
	
	glClearColor(0, 0, 0, 1);
}
// LoadBitmapFile
// opis: ≥aduje mapÍ bitowπ z pliku i zwraca jej adres.
//       Wype≥nia strukturÍ nag≥Ûwka.
//	 Nie obs≥uguje map 8-bitowych.
unsigned char *LoadBitmapFile(char *filename, BITMAPINFOHEADER *bitmapInfoHeader)
{
	FILE *filePtr;								// wskaünik pozycji pliku
	BITMAPFILEHEADER	bitmapFileHeader;		// nag≥Ûwek pliku
	unsigned char		*bitmapImage;			// dane obrazu
	int					imageIdx = 0;			// licznik pikseli
	unsigned char		tempRGB;				// zmienna zamiany sk≥adowych

	// otwiera plik w trybie "read binary"
	filePtr = fopen(filename, "rb");
	if (filePtr == NULL)
		return NULL;
	// wczytuje nag≥Ûwek pliku
	fread(&bitmapFileHeader, sizeof(BITMAPFILEHEADER), 1, filePtr);
	// sprawdza, czy jest to plik formatu BMP
	if (bitmapFileHeader.bfType != BITMAP_ID) {
		fclose(filePtr);

		return NULL;
	}
	// wczytuje nag≥Ûwek obrazu
	fread(bitmapInfoHeader, sizeof(BITMAPINFOHEADER), 1, filePtr);
	// ustawia wskaünik pozycji pliku na poczπtku danych obrazu
	fseek(filePtr, bitmapFileHeader.bfOffBits, SEEK_SET);
	// przydziela pamiÍÊ buforowi obrazu
	bitmapImage = (unsigned char*)malloc(bitmapInfoHeader->biSizeImage);
	// sprawdza, czy uda≥o siÍ przydzieliÊ pamiÍÊ
	if (!bitmapImage) {
		free(bitmapImage);
		fclose(filePtr);

		return NULL;
	}
	// wczytuje dane obrazu
	fread(bitmapImage, 1, bitmapInfoHeader->biSizeImage, filePtr);
	// sprawdza, czy dane zosta≥y wczytane
	if (bitmapImage == NULL) {
		fclose(filePtr);

		return NULL;
	}
	// zamienia miejscami sk≥adowe R i B 
	for (imageIdx = 0; imageIdx < bitmapInfoHeader->biSizeImage; imageIdx += 3) {
		tempRGB = bitmapImage[imageIdx];
		bitmapImage[imageIdx] = bitmapImage[imageIdx + 2];
		bitmapImage[imageIdx + 2] = tempRGB;
	}
	// zamyka plik i zwraca wskaünik bufora zawierajπcego wczytany obraz
	fclose(filePtr);

	return bitmapImage;
}
// Called to draw scene
void RenderScene()
{
	//float normal[3];	// Storeage for calculated surface normal
	// Clear the window with current clearing color
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// Save the matrix state and do the rotations

	//********************************************************************************************************************************************DRAW
	if (view_mode == 1)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	if (view_mode == 2)
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	machine.draw(CameraAngle - draw::Point3D(CameraAngleV[0] * 360, CameraAngleV[1] * 360, CameraAngleV[2] * 360));
	machine.execOrders();
	TwDraw();

	glMatrixMode(GL_MODELVIEW);
	// Flush drawing commands
	glFlush();
}
// Select the pixel format for a given device context
void SetDCPixelFormat(HDC hDC)
{
	int nPixelFormat;

	static PIXELFORMATDESCRIPTOR pfd = {
		sizeof(PIXELFORMATDESCRIPTOR),			// Size of this structure
		1,                                      // Version of this structure    
		PFD_DRAW_TO_WINDOW |                    // Draw to Window (not to bitmap)
		PFD_SUPPORT_OPENGL |					// Support OpenGL calls in window
		PFD_DOUBLEBUFFER,                       // Double buffered
		PFD_TYPE_RGBA,                          // RGBA Color mode
		32,                                     // Want 24bit color 
		0,0,0,0,0,0,                            // Not used to select mode
		0,0,                                    // Not used to select mode
		0,0,0,0,0,                              // Not used to select mode
		32,                                     // Size of depth buffer
		0,                                      // Not used to select mode
		0,                                      // Not used to select mode
		PFD_MAIN_PLANE,                         // Draw in main plane
		0,                                      // Not used to select mode
		0,0,0 };                                // Not used to select mode

	nPixelFormat = ChoosePixelFormat(hDC, &pfd); // Choose a pixel format that best matches that described in pfd
	SetPixelFormat(hDC, nPixelFormat, &pfd); // Set the pixel format for the device context
}
// If necessary, creates a 3-3-2 palette for the device context listed.
HPALETTE GetOpenGLPalette(HDC hDC)
{
	HPALETTE hRetPal = NULL;	// Handle to palette to be created
	PIXELFORMATDESCRIPTOR pfd;	// Pixel Format Descriptor
	LOGPALETTE *pPal;			// Pointer to memory for logical palette
	int nPixelFormat;			// Pixel format index
	int nColors;				// Number of entries in palette
	int i;						// Counting variable
	BYTE RedRange, GreenRange, BlueRange; // Range for each color entry (7,7,and 3)
	nPixelFormat = GetPixelFormat(hDC); // Get the pixel format index and retrieve the pixel format description
	DescribePixelFormat(hDC, nPixelFormat, sizeof(PIXELFORMATDESCRIPTOR), &pfd);
	// Does this pixel format require a palette?  If not, do not create a
	// palette and just return NULL
	if (!(pfd.dwFlags & PFD_NEED_PALETTE))
		return NULL;
	// Number of entries in palette.  8 bits yeilds 256 entries
	nColors = 1 << pfd.cColorBits;
	// Allocate space for a logical palette structure plus all the palette entries
	pPal = (LOGPALETTE*)malloc(sizeof(LOGPALETTE) + nColors * sizeof(PALETTEENTRY));
	// Fill in palette header 
	pPal->palVersion = 0x300;		// Windows 3.0
	pPal->palNumEntries = nColors; // table size
    // Build mask of all 1's.  This creates a number represented by having
    // the low order x bits set, where x = pfd.cRedBits, pfd.cGreenBits, and
    // pfd.cBlueBits.  
	RedRange = (1 << pfd.cRedBits) - 1;
	GreenRange = (1 << pfd.cGreenBits) - 1;
	BlueRange = (1 << pfd.cBlueBits) - 1;
	// Loop through all the palette entries
	for (i = 0; i < nColors; i++) {
		// Fill in the 8-bit equivalents for each component
		pPal->palPalEntry[i].peRed = (i >> pfd.cRedShift) & RedRange;
		pPal->palPalEntry[i].peRed = (unsigned char)(
			(double)pPal->palPalEntry[i].peRed * 255.0 / RedRange);

		pPal->palPalEntry[i].peGreen = (i >> pfd.cGreenShift) & GreenRange;
		pPal->palPalEntry[i].peGreen = (unsigned char)(
			(double)pPal->palPalEntry[i].peGreen * 255.0 / GreenRange);

		pPal->palPalEntry[i].peBlue = (i >> pfd.cBlueShift) & BlueRange;
		pPal->palPalEntry[i].peBlue = (unsigned char)(
			(double)pPal->palPalEntry[i].peBlue * 255.0 / BlueRange);

		pPal->palPalEntry[i].peFlags = (unsigned char)NULL;
	}
	// Create the palette
	hRetPal = CreatePalette(pPal);
	// Go ahead and select and realize the palette for this device context
	SelectPalette(hDC, hRetPal, FALSE);
	RealizePalette(hDC);
	// Free the memory used for the logical palette structure
	free(pPal);
	// Return the handle to the new palette
	return hRetPal;
}
// Entry point of all Windows programs
int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	MSG         msg;            // Windows message structure
	WNDCLASS	wc;             // Windows class structure
	HWND        hWnd;           // Storeage for window handle

	hInstance = hInst;

	// Register Window style
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = (WNDPROC)WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = NULL;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	// No need for background brush for OpenGL window
	wc.hbrBackground = NULL;
	wc.lpszMenuName = NULL;
	wc.lpszClassName = lpszAppName;
	// Register the window class
	if (RegisterClass(&wc) == 0)
		return 1;
	// Create the main application window
	hWnd = CreateWindow(lpszAppName, lpszAppName, WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
		CW_DEFAULT, CW_DEFAULT, WND_X, WND_Y, NULL, NULL, hInstance, NULL);
	
	// If window was not created, quit
	if (hWnd == NULL)
		return FALSE;

	Creator::setHInstance(hInstance);
	Creator::setParentHWnd(hWnd);

	/* AntTweakBar */
	TwInit(TW_OPENGL, NULL);
	TwWindowSize(WND_X, WND_Y);
	MainTw();
	MoveStatsTw();
	MoveTw();
	MachineTw();
	// Display the window
	ShowWindow(hWnd, SW_SHOWMAXIMIZED);
	UpdateWindow(hWnd);

	/* Timer */
	UINT_PTR timer_id = SetTimer(hWnd, 1, _TIMER, NULL);
	
	// Process application messages until the application closes
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
		SendMessage(hWnd, WM_PAINT, 0, 0);
	}

	return msg.wParam;
}

// Window procedure, handles all messages for this program
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	static HGLRC hRC;               // Permenant Rendering context
	static HDC hDC;                 // Private GDI Device context

	if (TwEventWin(hWnd, message, wParam, lParam)) // send event message to AntTweakBar
		return 0;

	switch (message) {
		// Window creation, setup for OpenGL
	case WM_CREATE:
		// Store the device context
		hDC = GetDC(hWnd);
		// Select the pixel format
		SetDCPixelFormat(hDC);
		// Create palette if needed
		hPalette = GetOpenGLPalette(hDC);
		// Create the rendering context and make it current
		hRC = wglCreateContext(hDC);
		wglMakeCurrent(hDC, hRC);
		SetupRC();
		machine.loadTextures();
		break;
		// Window is being destroyed, cleanup
	case WM_DESTROY:
		TwTerminate();
		// Deselect the current rendering context and delete it
		wglMakeCurrent(hDC, NULL);
		wglDeleteContext(hRC);
		// Delete the palette if it was created
		if (hPalette != NULL)
			DeleteObject(hPalette);
		// Tell the application to terminate after the window
		// is gone.
		PostQuitMessage(0);
		break;
		// Window is resized.
	case WM_SIZE:
		// Call our function which modifies the clipping
		// volume and viewport
		ChangeSize(LOWORD(lParam), HIWORD(lParam));
		break;
		// The painting function.  This message sent by Windows 
		// whenever the screen needs updating.
	case WM_PAINT: {
		hDC = BeginPaint(hWnd, &ps);
		// Call OpenGL drawing code
		RenderScene();
		SwapBuffers(hDC);
		// Validate the newly painted client area
		ValidateRect(hWnd, NULL);
		EndPaint(hWnd, &ps);
	} break;
	// Windows is telling the application that it may modify
	// the system palette.  This message in essance asks the 
	// application for a new palette.
	case WM_QUERYNEWPALETTE:
		// If the palette was created.
		if (hPalette) {
			int nRet;
			// Selects the palette into the current device context
			SelectPalette(hDC, hPalette, FALSE);
			// Map entries from the currently selected palette to
			// the system palette.  The return value is the number 
			// of palette entries modified.
			nRet = RealizePalette(hDC);
			// Repaint, forces remap of palette in current window
			InvalidateRect(hWnd, NULL, FALSE);

			return nRet;
		} break;
		// This window may set the palette, even though it is not the 
		// currently active window.
	case WM_PALETTECHANGED:
		// Don't do anything if the palette does not exist, or if
		// this is the window that changed the palette.
		if ((hPalette != NULL) && ((HWND)wParam != hWnd)) {
			// Select the palette into the device context
			SelectPalette(hDC, hPalette, FALSE);
			// Map entries to system palette
			RealizePalette(hDC);
			// Remap the current colors to the newly realized palette
			UpdateColors(hDC);
			return 0;
		} break;
		// Key press, check for arrow keys to do cube rotation.

	case WM_TIMER:
		machine.tic();
		break;
	case WM_KEYDOWN: {
		GLfloat da = 1.0f;
		GLfloat dh = 1.0f;
		GLfloat dangle = 0.5f;
		GLfloat dr = 0.5f;
		if (wParam == VK_UP)
			CameraAngle.x += da;
		if (wParam == VK_DOWN)
			CameraAngle.x -= da;
		if (wParam == VK_LEFT)
			CameraAngle.y += da;
		if (wParam == VK_RIGHT)
			CameraAngle.y -= da;
		if (wParam == VK_OEM_4)
			CameraAngle.z += da;
		if (wParam == VK_OEM_6)
			CameraAngle.z -= da;
		if (wParam == VK_SPACE) {
			if (machine.getPause())
				machine.setPause(false);
			else
				machine.setPause(true);
		}
		if (wParam == 'W')
			machine.lowerArm(dh);
		if (wParam == 'S')
			machine.lowerArm(-dh);
		if (wParam == 'A')
			machine.rotateArm(dangle);
		if (wParam == 'D')
			machine.rotateArm(-dangle);
		if (wParam == 'X')
			machine.retractArm(dr);
		if (wParam == 'Z')
			machine.retractArm(-dr);

		if (wParam == 'J') {
			machine.doJobs();
		}

		if (wParam == 'R') {
			machine.resetClock();
		}

		if (wParam == 'M')
			machine.moveTo(draw::Point3D(rand(-60, 60), rand(-40, 40), rand(0, 60)), draw::Point3D(1, 1, 1), mt);

		CameraAngle.x = (int)CameraAngle.x % 360;
		CameraAngle.y = (int)CameraAngle.y % 360;
		CameraAngle.z = (int)CameraAngle.z % 360;

		InvalidateRect(hWnd, NULL, FALSE);
	} break;
	// A menu command
	case WM_COMMAND: {
		switch (LOWORD(wParam)) {
		case ID_FILE_EXIT:
			DestroyWindow(hWnd);
			break;
		}
	} break;
	default:   // Passes it on if unproccessed
		return (DefWindowProc(hWnd, message, wParam, lParam));
	}

	return (0L);
}

void printHUD(HDC hDC)
{
	draw::Point3D p = machine.getPos();
	std::string s = "Pos: x: " + std::to_string(p.x) + " y: " + std::to_string(p.y) + " z: " + std::to_string(p.z) +
		" B: " + std::to_string(machine.getBusy()) + " W: " + std::to_string(machine.getWorking());

	TextOut(hDC, 0, 0, s.c_str(), s.length());
}

void MainTw()
{
	TwBar *TwMain;
	TwMain = TwNewBar("Main");
	TwDefine(" Main position='20 40' size='200 200' color='96 216 224' ");
	TwAddButton(TwMain, "Reset", ResetBtn, NULL, " label='Reset camera angle' ");
	TwAddVarCB(TwMain, "Block multipier", TW_TYPE_FLOAT, setBlockMult, getBlockMult, NULL, " min=1 max=30 step=1 ");
	TwAddVarCB(TwMain, "Height multipier", TW_TYPE_FLOAT, setBlockHMult, getBlockHMult, NULL, " min=1 max=100 step=1 ");
	TwAddButton(TwMain, "NewBlock", NewBlock, NULL, " label='Reset block' ");
	TwAddButton(TwMain, "ChangeView", ChangeView, NULL, " label='Change disp mode' ");
	TwAddVarRW(TwMain, "Rotation", TW_TYPE_QUAT4F, &CameraAngleV, "");
}

void MoveStatsTw()
{
	TwBar *TwMoveStats;
	TwMoveStats = TwNewBar("MoveStats");
	TwDefine(" MoveStats position='20 470' size='200 200' color='96 216 224' ");
	TwAddSeparator(TwMoveStats, NULL, " group='Position' ");
	TwAddVarCB(TwMoveStats, "Pos.x:", TW_TYPE_FLOAT, NULL, getPosX, NULL, " precision=6 group='Position' ");
	TwAddVarCB(TwMoveStats, "Pos.y:", TW_TYPE_FLOAT, NULL, getPosY, NULL, " precision=6 group='Position' ");
	TwAddVarCB(TwMoveStats, "Pos.z:", TW_TYPE_FLOAT, NULL, getPosZ, NULL, " precision=6 group='Position' ");
	TwAddSeparator(TwMoveStats, NULL, " group='Destination' ");
	TwAddVarCB(TwMoveStats, "Des.x:", TW_TYPE_FLOAT, NULL, getDesX, NULL, " precision=6  group='Desination'");
	TwAddVarCB(TwMoveStats, "Des.y:", TW_TYPE_FLOAT, NULL, getDesY, NULL, " precision=6  group='Desination'");
	TwAddVarCB(TwMoveStats, "Des.z:", TW_TYPE_FLOAT, NULL, getDesZ, NULL, " precision=6  group='Desination'");
}

void MoveTw()
{
	TwBar *TwMove;
	TwMove = TwNewBar("Move");
	TwDefine(" Move position='1300 40' size='200 300' color='96 216 224' ");
	TwEnumVal movesEV[] = { { MV_SAFE, "Safe" },{ MV_DIRECT, "Direct" },{ MV_SEPARATE, "Separate" } };
	TwType moveType = TwDefineEnum("MoveType", movesEV, 3);
	TwAddSeparator(TwMove, NULL, " group='Position' ");
	TwAddVarRW(TwMove, "Pos.x:", TW_TYPE_FLOAT, &movePoint.x, " min=-70 max=70 step=1 group=Position ");
	TwAddVarRW(TwMove, "Pos.y:", TW_TYPE_FLOAT, &movePoint.y, " min=-50 max=50 step=1 group=Position ");
	TwAddVarRW(TwMove, "Pos.z:", TW_TYPE_FLOAT, &movePoint.z, " min=0 max=80 step=1 group=Position ");
	TwAddVarRW(TwMove, "MoveType", moveType, &mt, NULL);
	TwAddButton(TwMove, "AddMove", AddMove, NULL, " label='Add move to queue' ");
	TwAddVarCB(TwMove, "Queue:", TW_TYPE_FLOAT, NULL, getQueue, NULL, "");
	TwAddButton(TwMove, "ResetQueue", ResetQueue, NULL, " label='Reset queue' ");
	TwAddButton(TwMove, "OpenCreator", OpenCreator, NULL, " label='Open creator' ");
}

void MachineTw()
{
	TwBar *TwMachine;
	TwMachine = TwNewBar("Machine");
	TwDefine(" Machine position='20 250' size='200 200' color='96 216 224' ");
	TwAddVarCB(TwMachine, "Drill size:", TW_TYPE_FLOAT, setDrillR, getDrillR, NULL, " min=0.25 max=50 step=0.25 precision=3 ");
	TwAddVarCB(TwMachine, "Speed rate:", TW_TYPE_INT16, setSpeedRate, getSpeedRate, NULL, " min=1 max=100 step=1 ");
}

void TW_CALL ResetBtn(void *clientData)
{
	CameraAngleV[0] = 0;
	CameraAngleV[1] = 0;
	CameraAngleV[2] = 0;
}

void TW_CALL NewBlock(void *clientData)
{
	machine.allocBlock();
}

void TW_CALL ChangeView(void *clientData)
{
	view_mode = view_mode == 1 ? 2 : 1;
}

void TW_CALL AddMove(void *clientData)
{
	machine.moveTo(movePoint, draw::Point3D(1, 1, 1), mt);
}

void TW_CALL ResetQueue(void * clientData)
{
	machine.resetQueue();
}

void TW_CALL OpenCreator(void * clientData)
{
	machine.OpenCreator();
}

void TW_CALL getBlockMult(void * value, void * clientData)
{
	*(GLfloat*)value = machine.getBlockMult();
}

void TW_CALL setBlockMult(const void *value, void * clientData)
{
	machine.setBlockMult(*(const GLfloat*)value);
}

void TW_CALL getBlockHMult(void * value, void * clientData)
{
	*(GLfloat*)value = machine.getBlockHMult();
}

void TW_CALL setBlockHMult(const void * value, void * clientData)
{
	machine.setBlockHMult(*(const GLfloat*)value);
}

void TW_CALL getDrillR(void * value, void * clientData)
{
	*(GLfloat*)value = machine.getDrillR();
}

void TW_CALL setDrillR(const void * value, void * clientData)
{
	machine.setDrillR(*(const GLfloat*)value);
}

void TW_CALL getSpeedRate(void * value, void * clientData)
{
	*(GLint*)value = machine.getSpeedRate();
}

void TW_CALL setSpeedRate(const void * value, void * clientData)
{
	machine.setSpeedRate(*(const GLint*)value);
}

void TW_CALL getPosX(void *value, void *clientData) {
	*(GLfloat*)value = machine.getPos().x;
}

void TW_CALL getPosY(void *value, void *clientData) {
	*(GLfloat*)value = machine.getPos().y;
}

void TW_CALL getPosZ(void *value, void *clientData) {
	*(GLfloat*)value = machine.getPos().z;
}

void TW_CALL getDesX(void *value, void *clientData)
{
	*(GLfloat*)value = machine.getDes().x;
}

void TW_CALL getDesY(void *value, void *clientData)
{
	*(GLfloat*)value = machine.getDes().y;
}

void TW_CALL getDesZ(void *value, void *clientData)
{
	*(GLfloat*)value = machine.getDes().z;
}

void TW_CALL getQueue(void * value, void * clientData)
{
	*(GLfloat*)value = machine.getQueue();
}

GLfloat rand(GLint beg, GLint end)
{
	static bool init = false;
	if (!init) {
		srand(time(NULL));
		init = true;
	}

	return ::rand() % GLint(end - beg + 1) + beg;
}