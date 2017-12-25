#include "Creator.h"

HINSTANCE Creator::_hInstance = NULL;
HWND Creator::_hWnd = NULL;
HWND Creator::_parent_hWnd = NULL;
Creator* Creator::_this = nullptr;
GLboolean Creator::_WCInit = false;

Creator::Creator(draw::Point3D size, std::vector<GLfloat> enter_state, GLfloat h_max) : _size(size), lpszAppName("Creator"),
				_h_max(h_max), _h_slider_val(0), _pen_slider_val(0), _job_ready(false), _pen(SQUARE),
				 _controlsSize(250), _border(10), _mouse_button_down(false)
{
	_this = this;

	_windowHeight = 800;
	_windowWidth = 1200 + _controlsSize + 2 * _border;

	_dsize.x = floor((_windowWidth - 2 * _border - _controlsSize) / (_size.x + 1));
	_dsize.y = floor((_windowHeight - 2 * _border) / (_size.y + 1));

	_global_rect.left = _border;
	_global_rect.right = _global_rect.left + _size.x * _dsize.x;
	_global_rect.top = _border;
	_global_rect.bottom = _global_rect.top + _size.y * _dsize.y;

	InitRects(enter_state);

	if (!_WCInit) {
		WNDCLASSEX wcex;
		wcex.cbSize = sizeof(WNDCLASSEX);
		wcex.style = CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc = WndProc;
		wcex.cbClsExtra = 0;
		wcex.cbWndExtra = 0;
		wcex.hInstance = _hInstance;
		wcex.hIcon = NULL;
		wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
		wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW);
		wcex.lpszMenuName = NULL;
		wcex.lpszClassName = lpszAppName;
		wcex.hIconSm = NULL;

		if (!RegisterClassEx(&wcex))
			throw "Cannot register window class";
		_WCInit = true;
	}
	_hWnd = CreateWindow(lpszAppName, lpszAppName, WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_CHILD,
		CW_USEDEFAULT, CW_USEDEFAULT, _windowWidth, _windowHeight, _parent_hWnd, NULL, _hInstance, NULL);

	if (_hWnd == NULL)
		exit(1);
	
	InitControls();
	ShowWindow(_hWnd, SW_SHOW);
	UpdateWindow(_hWnd);
}


Creator::~Creator()
{
	_hWnd = NULL;
}

LRESULT Creator::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hDC;

	switch (message)
	{
	case WM_PAINT:
		hDC = BeginPaint(hWnd, &ps);
		
		_this->Paint(hDC);

		EndPaint(hWnd, &ps);
		break;

	case WM_MOUSEMOVE:
		_this->onMouse(wParam, lParam);
		break;

	case WM_LBUTTONDOWN:
		_this->_mouse_button_down = true;
		_this->onMouse(wParam, lParam);
		break;

	case WM_LBUTTONUP:
		_this->_mouse_button_down = false;
		break;

	case WM_RBUTTONDOWN:
		_this->onRightClick(wParam, lParam);
		break;

	case WM_VSCROLL: 
		_this->_h_slider_val = SendMessage(_this->cHSlider.hWnd, TBM_GETPOS, 0, 0);
		SetWindowText(_this->cHSliderStatic.hWnd, ("Height: " + std::to_string(_this->_h_slider_val)).c_str());
		InvalidateRect(_hWnd, &_this->_h_slider_color, false);
		break;

	case WM_HSCROLL:
		_this->_pen_slider_val = SendMessage(_this->cPenSlider.hWnd, TBM_GETPOS, 0, 0);
		SetWindowText(_this->cPenSliderStatic.hWnd, ("Pen size: " + std::to_string(_this->_pen_slider_val)).c_str());
		break;

	case WM_COMMAND:
		switch (LOWORD(wParam)) {
			case IDC_ADD_BUTTON:
				_this->prepareJob();
				SendMessage(_hWnd, WM_DESTROY, 0, 0);
			break;

			case IDC_SELECT_ALL_BUTTON:
				_this->selectAll();
				break;

			case IDC_REVERSE_ALL_BUTTON:
				_this->reverseAll();
				break;

			case IDC_CHECK_SQ:
				_this->_pen = SQUARE;
					break;
			case IDC_CHECK_CI:
				_this->_pen = CIRCLE;
				break;
		}
		break;

	case WM_DESTROY:
		DestroyWindow(_hWnd);
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
		break;
	}

	return 0;
}

void Creator::setHInstance(HINSTANCE hInstance)
{
	_hInstance = hInstance;
}

void Creator::setParentHWnd(HWND hWnd)
{
	_parent_hWnd = hWnd;
}

HWND Creator::getHWnd()
{
	return _hWnd;
}

void Creator::InitRects(std::vector<GLfloat> enter_state)
{
	draw::Point3D d(_global_rect.left, _global_rect.top, 0);

	GLint len = GLint(_size.x * _size.y);

	for (int i = 0; i < len; i++) {
		RECT rec;
		rec.left = d.x;
		rec.top = d.y;
		rec.right = rec.left + _dsize.x;
		rec.bottom = rec.top + _dsize.y;
		_rects.push_back(CRect(rec, calcColor(enter_state[i]), enter_state[i]));

		d.x += _dsize.x;
		if (d.x >= _global_rect.right) {
			d.x = _global_rect.left;
			d.y += _dsize.y;
		}
	}
}

void Creator::InitControls()
{
	cStaticPos.rect.left = _windowWidth - _controlsSize;
	cStaticPos.rect.right = cStaticPos.rect.left + 100;
	cStaticPos.rect.top = _border;
	cStaticPos.rect.bottom = cStaticPos.rect.top + 40;
	cStaticPos.hWnd = CreateWindow("static", "Pos:",
		WS_VISIBLE | WS_CHILD | SS_RIGHT,
		cStaticPos.rect.left, cStaticPos.rect.top,
		cStaticPos.rect.right - cStaticPos.rect.left, cStaticPos.rect.bottom - cStaticPos.rect.top,
		_hWnd, NULL,
		_hInstance, NULL);

	cHSlider.rect.left = _windowWidth - _controlsSize;
	cHSlider.rect.right = cStaticPos.rect.left + 50;
	cHSlider.rect.top = cStaticPos.rect.bottom + 20;
	cHSlider.rect.bottom = cStaticPos.rect.top + 200;
	cHSlider.hWnd = CreateWindowEx(
		0, TRACKBAR_CLASS, "Height",
		WS_CHILD | WS_VISIBLE | TBS_VERT,
		cHSlider.rect.left, cHSlider.rect.top, cHSlider.rect.right - cHSlider.rect.left, cHSlider.rect.bottom - cHSlider.rect.top,
		_hWnd, NULL,
		_hInstance, NULL);

	SendMessage(cHSlider.hWnd, TBM_SETRANGE, (WPARAM)TRUE, (LPARAM)MAKELONG(0, _h_max));

	cHSliderStatic.rect.left = cHSlider.rect.right + 20;
	cHSliderStatic.rect.right = cHSliderStatic.rect.left + 100;
	cHSliderStatic.rect.top = cHSlider.rect.top + (cHSlider.rect.bottom - cHSlider.rect.top) / 2 - 15;
	cHSliderStatic.rect.bottom = cHSliderStatic.rect.top + 20;
	cHSliderStatic.hWnd = CreateWindow("static", ("Height: " + std::to_string(_h_slider_val)).c_str(),
		WS_VISIBLE | WS_CHILD | SS_RIGHT,
		cHSliderStatic.rect.left, cHSliderStatic.rect.top,
		cHSliderStatic.rect.right - cHSliderStatic.rect.left, cHSliderStatic.rect.bottom - cHSliderStatic.rect.top,
		_hWnd, NULL,
		_hInstance, NULL);

	cPenSlider.rect.left = _windowWidth - _controlsSize;
	cPenSlider.rect.right = cPenSlider.rect.left + 200;
	cPenSlider.rect.top = cHSlider.rect.bottom + 20;
	cPenSlider.rect.bottom = cPenSlider.rect.top + 50;
	cPenSlider.hWnd = CreateWindowEx(
		0, TRACKBAR_CLASS, "Pen size:",
		WS_CHILD | WS_VISIBLE | TBS_HORZ,
		cPenSlider.rect.left, cPenSlider.rect.top,
		cPenSlider.rect.right - cPenSlider.rect.left, cPenSlider.rect.bottom - cPenSlider.rect.top,
		_hWnd, NULL,
		_hInstance, NULL);

	SendMessage(cPenSlider.hWnd, TBM_SETRANGE, (WPARAM)TRUE, (LPARAM)MAKELONG(0, 400));

	cPenSliderStatic.rect.left = _windowWidth - _controlsSize;
	cPenSliderStatic.rect.right = cPenSliderStatic.rect.left + 100;
	cPenSliderStatic.rect.top = cPenSlider.rect.bottom + 5;
	cPenSliderStatic.rect.bottom = cPenSliderStatic.rect.top + 20;
	cPenSliderStatic.hWnd = CreateWindow("static", ("Pen size: " + std::to_string(_pen_slider_val)).c_str(),
		WS_VISIBLE | WS_CHILD | SS_RIGHT,
		cPenSliderStatic.rect.left, cPenSliderStatic.rect.top,
		cPenSliderStatic.rect.right - cPenSliderStatic.rect.left, cPenSliderStatic.rect.bottom - cPenSliderStatic.rect.top,
		_hWnd, NULL,
		_hInstance, NULL);

	cPenRadio.rect.left = _windowWidth - _controlsSize;
	cPenRadio.rect.right = cPenRadio.rect.left + 100;
	cPenRadio.rect.top = cPenSliderStatic.rect.bottom + 10;
	cPenRadio.rect.bottom = cPenRadio.rect.top + 80;
	cPenRadio.hWnd = CreateWindowEx(WS_EX_WINDOWEDGE,
		"BUTTON",
		"Brush type:",
		WS_VISIBLE | WS_CHILD | BS_GROUPBOX,
		cPenRadio.rect.left, cPenRadio.rect.top,
		cPenRadio.rect.right - cPenRadio.rect.left, cPenRadio.rect.bottom - cPenRadio.rect.top,
		_hWnd,
		(HMENU)IDC_GRPBUTTONS,
		_hInstance, NULL);
	 HWND squareRadio = CreateWindowEx(WS_EX_WINDOWEDGE,
		"BUTTON",
		"Square",
		WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON | WS_GROUP,
		cPenRadio.rect.left + 10, cPenRadio.rect.top + 25,
		80, 20,
		_hWnd,
		(HMENU)IDC_CHECK_SQ,
		_hInstance, NULL);
	CreateWindowEx(WS_EX_WINDOWEDGE,
		"BUTTON",
		"Circle",
		WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON,
		cPenRadio.rect.left + 10, cPenRadio.rect.top + 50,
		80, 20,
		_hWnd,
		(HMENU)IDC_CHECK_CI,
		_hInstance, NULL);

	Button_SetCheck(squareRadio, BST_CHECKED);

	cSelAllButton.rect.left = _windowWidth - _controlsSize;
	cSelAllButton.rect.right = cSelAllButton.rect.left + 100;
	cSelAllButton.rect.top = cPenRadio.rect.bottom + 20;
	cSelAllButton.rect.bottom = cSelAllButton.rect.top + 30;
	cSelAllButton.hWnd = CreateWindow(
		"BUTTON", "Select all",
		WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
		cSelAllButton.rect.left, cSelAllButton.rect.top,
		cSelAllButton.rect.right - cSelAllButton.rect.left, cSelAllButton.rect.bottom - cSelAllButton.rect.top,
		_hWnd, (HMENU)IDC_SELECT_ALL_BUTTON,
		_hInstance, NULL);

	cReverseAllButton.rect.left = cSelAllButton.rect.right + 10;
	cReverseAllButton.rect.right = cReverseAllButton.rect.left + 100;
	cReverseAllButton.rect.top = cSelAllButton.rect.top;
	cReverseAllButton.rect.bottom = cReverseAllButton.rect.top + 30;
	cReverseAllButton.hWnd = CreateWindow(
		"BUTTON", "Reverse all",
		WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
		cReverseAllButton.rect.left, cReverseAllButton.rect.top,
		cReverseAllButton.rect.right - cReverseAllButton.rect.left, cReverseAllButton.rect.bottom - cReverseAllButton.rect.top,
		_hWnd, (HMENU)IDC_REVERSE_ALL_BUTTON,
		_hInstance, NULL);

	cAddButton.rect.left = _windowWidth - _controlsSize;
	cAddButton.rect.right = cAddButton.rect.left + 210;
	cAddButton.rect.top = cSelAllButton.rect.bottom + 20;
	cAddButton.rect.bottom = cAddButton.rect.top + 30;
	cAddButton.hWnd = CreateWindow(
		"BUTTON", "DO IT!",
		WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
		cAddButton.rect.left, cAddButton.rect.top,
		cAddButton.rect.right - cAddButton.rect.left, cAddButton.rect.bottom - cAddButton.rect.top,
		_hWnd, (HMENU)IDC_ADD_BUTTON,
		_hInstance, NULL);
}

draw::Point3D Creator::calcColor(GLfloat h)
{
	GLfloat hhmax = 0.5 * _h_max;
	GLfloat R = 255 * (h >= hhmax ? (2 - h / hhmax) : 1);
	GLfloat G = 255 * (h <= hhmax ? (h / hhmax) : 1);
	return draw::Point3D(R, G, 0);
}

GLboolean Creator::Contains(RECT rect, draw::Point3D p)
{
	return (rect.left <= p.x && p.x <= rect.right && rect.top <= p.y && p.y <= rect.bottom);
}

void Creator::Paint(HDC hDC)
{
	HBRUSH hbr;
	
	draw::Point3D p;
	for (CRect &r : _rects) {
		hbr = CreateSolidBrush(RGB(r.color.x, r.color.y, r.color.z));
		FillRect(hDC, &r.rect, hbr);
		DeleteObject(hbr);
	}
	hbr = CreateSolidBrush(RGB(0, 0, 0));
	FrameRect(hDC, &_global_rect, hbr);
	DeleteObject(hbr);

	_h_slider_color.left = cHSlider.rect.right + 20;
	_h_slider_color.right = _h_slider_color.left + 30;
	_h_slider_color.top = cHSlider.rect.top;
	_h_slider_color.bottom = _h_slider_color.top + 30;

	p = calcColor(_h_slider_val);
	hbr = CreateSolidBrush(RGB(p.x, p.y, p.z));
	FillRect(hDC, &_h_slider_color, hbr);
	DeleteObject(hbr);
}

void Creator::onMouse(WPARAM wParam, LPARAM lParam)
{
	draw::Point3D pos(LOWORD(lParam), HIWORD(lParam), _h_slider_val);

	std::string text;
	if (Contains(_global_rect, pos)) {
		GLfloat x = pos.x - _global_rect.left;
		GLfloat y = pos.y - _global_rect.top;
		GLint ix = floor(x / _dsize.x);
		GLint iy = floor(y / _dsize.y);
		if (ix >= _size.x) ix = _size.x - 1;
		if (iy >= _size.y) iy = _size.y - 1;
		if (ix < 0) ix = 0;
		if (iy < 0) iy = 0;
		text = "Pos: " + std::to_string(ix - (GLint)floor(0.5f * _size.x)) + "x" + 
			std::to_string(iy - (GLint)floor(0.5f * _size.y)) + "\n\r h: " +
			std::to_string(GLint(_rects[GLint(iy * _size.x + ix)].h));
	} else {
		text = "Pos:";
	}
	SetWindowText(cStaticPos.hWnd, text.c_str());

	if (wParam == MK_LBUTTON &&  _mouse_button_down)
		onLeftClick(pos);
}

void Creator::onLeftClick(draw::Point3D pos)
{
	if (!Contains(_global_rect, pos)) return;

	for (CRect &r : _rects) {
		if (_pen == SQUARE ? r.contains(pos, _pen_slider_val) : r.intersects(pos, _pen_slider_val)) {
			r.h = pos.z > r.hmax ? r.hmax : pos.z;
			r.color = calcColor(r.h);
			InvalidateRect(_hWnd, &r.rect, false);
		}
	}
}

void Creator::onRightClick(WPARAM wParam, LPARAM lParam)
{
	draw::Point3D pos(LOWORD(lParam), HIWORD(lParam), _h_slider_val);
	onLeftClick(pos);
	_interp_nodes.push_back(pos);
	Interpolate();
}

void Creator::prepareJob()
{
	GLint len = _rects.size();
	for (int i = 0; i < len; i++) {
		if (_rects[i].h < _rects[i].hmax)
			_job.push_back(draw::Point3D(i % GLint(_size.x), floor(i / _size.x), _rects[i].h));
	}
	_job_ready = true;
}

void Creator::selectAll()
{
	for (CRect &r : _rects) {
		r.h = _h_slider_val;
		r.color = calcColor(r.h);
		InvalidateRect(_hWnd, &r.rect, false);
	}
}

void Creator::reverseAll()
{
	for (CRect &r : _rects) {
		r.h = r.hmax - r.h;
		r.color = calcColor(r.h);
		InvalidateRect(_hWnd, &r.rect, false);
	}
}

void Creator::Interpolate()
{
	if (_interp_nodes.size() != 2)
		return;

	draw::Point3D p1 = _interp_nodes[0];
	draw::Point3D p2 = _interp_nodes[1];
	_interp_nodes.clear();

	GLfloat dx = abs(p1.x - p2.x);
	GLfloat dy = abs(p1.y - p2.y);

	GLint nx = (GLint)ceil(dx / _dsize.x);
	GLint ny = (GLint)ceil(dy / _dsize.y);
	GLint n = nx > ny ? nx : ny;

	GLfloat x_inc = dx == 0 ? 0 : dx / n * ((p2.x - p1.x) / dx);
	GLfloat y_inc = dy == 0 ? 0 : dy / n * ((p2.y - p1.y) / dy);

	GLint dh = abs(p1.z - p2.z);
	GLfloat h_inc = dh == 0 ? 0 : (GLfloat)dh / n * ((p2.z - p1.z) / dh);
	GLfloat h_inc_min = floor(h_inc);
	GLfloat h_inc_max = ceil(h_inc);

	GLfloat x = p1.x;
	GLfloat y = p1.y;
	GLint h = p1.z;

	for (int i = 0; i < n; i++) {
		onLeftClick(draw::Point3D(x += x_inc, y += y_inc, h += abs(h) >= abs(p2.z) ? 0 : i % 2 == 0 ? h_inc_min : h_inc_max));
	}
}

std::vector<draw::Point3D> Creator::getJob()
{
	_job_ready = false;
	return _job;
}

GLboolean Creator::getJobReady()
{
	return _job_ready;
}
