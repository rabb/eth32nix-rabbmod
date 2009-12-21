// ETH32 - an Enemy Territory cheat for windows
// Copyright (c) 2007 eth32 team
// www.cheatersutopia.com & www.nixcoders.org

#pragma once

#include "CWindow.h"
#include "CCheckbox.h"
#include <list>

typedef enum {
	CAT_AIMBOT,
	CAT_AIMBOT2,
	CAT_VISUAL,
	CAT_VISUAL2,
	CAT_COLOR,
	CAT_MISC,
	CAT_MAX,
} wincat_t;

class CWindowPicker;

class CWindowButton : public CControl
{
public:
	CWindowButton(const char *clabel, int cx, int cy, int cw, int ch, wincat_t cat, CWindowPicker *pwin);
	void Display(void);
	int ProcessMouse(int mx, int my, uint32 mevent, CControl **mhook, CControl **khook);
	void MouseMove(int mx, int my);
	bool WindowActive(void) { return winActive; }
	void ShowWindows(bool show);
	void AddWindow(CWindow *win);
private:
	wincat_t category;
	list<CWindow*> winList;
	CWindowPicker *windowPicker;
	bool winActive;
};

class CWindowPicker : public CWindow
{
public:
	CWindowPicker(const char *wlabel, int wx, int wy, int ww, int wh);
	~CWindowPicker(void);
	void Display(void);
	int ProcessMouse(int mx, int my, uint32 mevent, CControl **mhook, CControl **khook);
	void AddWindow(const char *winTitle, wincat_t cat);
	void ClearAll(void);	
	void WindowActiveCallback(wincat_t cat);
	void SetProps(const char *props, float xScale, float yScale) {   }
private:
	CWindowButton *buttons[CAT_MAX];

	void ReorganizeButtons(void);
	CCheckBox *ctrlMulti;
	wincat_t lastActive;
	bool allowMultiWindows;
	int buttonCount;
	int activeWinCount;
};
