#pragma once
#include "main.h"
#include "shapes.h"

class Creator
{
	static HINSTANCE _hInstance;
	static HWND _parent_hWnd;
	static Creator *_this;
	static GLboolean _WCInit;
	static HWND _hWnd;
	LPCTSTR lpszAppName;
	GLint _windowHeight, _windowWidth, _controlsSize;

	draw::Point3D _size;
	draw::Point3D _dsize;
	GLfloat _h_max;
	GLfloat _border;

	std::vector <draw::Point3D> _interp_nodes;

	GLboolean _mouse_button_down;

	RECT _global_rect;

	struct Control
	{
		HWND hWnd;
		RECT rect;
		Control(HWND hWnd, RECT rect)
		{
			this->hWnd = hWnd;
			this->rect = rect;
		}
		Control()
		{
			this->hWnd = NULL;
			this->rect.left = this->rect.right = this->rect.top = this->rect.bottom = 0;
		}
	};

	Control cStaticPos;
	Control cHSlider;
	Control cHSliderStatic;
	GLint _h_slider_val;
	RECT _h_slider_color;
	Control cPenSlider;
	Control cPenSliderStatic;
	GLint _pen_slider_val;
	Control cPenRadio;
	enum {
		SQUARE,
		CIRCLE
	}_pen;
	Control cSelAllButton;
	Control cReverseAllButton;
	Control cAddButton;

	struct CRect
	{
		RECT rect;
		draw::Point3D color;
		GLfloat h;
		const GLfloat hmax;
		CRect(RECT rect, draw::Point3D color, GLfloat hmax) : hmax(hmax)
		{
			this->rect = rect;
			this->color = color;
			this->h = hmax;
		}
		GLboolean contains(draw::Point3D pos, GLfloat r)
		{
			return rect.left <= pos.x + r && pos.x - r <= rect.right &&
				rect.top <= pos.y + r && pos.y - r <= rect.bottom;
		}

		GLboolean intersects(draw::Point3D pos, GLfloat r)
		{
			GLfloat a = rect.right - rect.left;
			GLfloat b = rect.bottom - rect.top;
			draw::Point3D circleDist(abs(pos.x - rect.left - 0.5 * a), abs(pos.y - rect.top - 0.5 * b), 0);

			if (circleDist.x > (a / 2 + r)) return false;
			if (circleDist.y > (b / 2 + r)) return false;

			if (circleDist.x <= (a / 2)) return true;
			if (circleDist.y <= (b / 2)) return true;

			GLfloat x = circleDist.x - a / 2;
			GLfloat y = circleDist.y - b / 2;
			GLfloat cornerDist_sq = x * x + y * y;

			return (cornerDist_sq <= r * r);
		}
	};
	std::vector<CRect> _rects;
	std::vector<draw::Point3D> _job;
	GLboolean _job_ready;

public:
	Creator(draw::Point3D size, std::vector<GLfloat> enter_state, GLfloat h_max);
	~Creator();

private:
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
public: 
	static void setHInstance(HINSTANCE hInstance);
	static void setParentHWnd(HWND hWnd);
	static HWND getHWnd();
private:
	void InitRects(std::vector<GLfloat> enter_state);
	void InitControls();
	draw::Point3D calcColor(GLfloat h);
	GLboolean Contains(RECT rect, draw::Point3D p);
	void Paint(HDC hDC);
	void onMouse(WPARAM wParam, LPARAM lParam);
	void onLeftClick(draw::Point3D pos);
	void onRightClick(WPARAM wParam, LPARAM lParam);
	void prepareJob();
	void selectAll();
	void reverseAll();
	void Interpolate();
public:
	std::vector<draw::Point3D> getJob();
	GLboolean getJobReady();
};

