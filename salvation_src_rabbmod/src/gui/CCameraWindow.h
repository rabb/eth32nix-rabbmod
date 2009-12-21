// ETH32 - an Enemy Territory cheat for windows
// Copyright (c) 2007 eth32 team
// www.cheatersutopia.com & www.nixcoders.org

#pragma once

#include "CWindow.h"
#include "CSliders.h"
#include "CCheckbox.h"

#define CAM_MIN_SIZE	80

typedef enum {
	CORNER_TL,
	CORNER_TR,
	CORNER_BL,
	CORNER_BR
} corner_t;

class CCameraWindow : public CWindow
{
public:
	CCameraWindow(const char *wlabel, int wx, int wy, int ww, int wh);
	void Display(void);
	int ProcessMouse(int mx, int my, uint32 mevent, CControl **mhook, CControl **khook);
	int MouseWheel(int mx, int my, int wheel);
	void MouseMove(int mx, int my);
private:
	bool cameraActive;
	int cornerActive;
	int type;
};

class CCameraConfigWindow : public CWindow
{
public:
	CCameraConfigWindow(const char *wlabel, int wx, int wy, int ww, int wh);
	void Display(void);
private:
	void UpdateOffsets(void);
	camInfo_t *cam;

	int x, y, w, h;
	CCheckBox *show;
	CFloatSlider *distance, *angle, *fov;
};
