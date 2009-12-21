// ETH32 - an Enemy Territory cheat for windows
// Copyright (c) 2007 eth32 team
// www.cheatersutopia.com & www.nixcoders.org

#pragma once

#include "CControl.h"

class CWindow : public CControl
{
public:
	CWindow(const char *wlabel, int wx, int wy, int ww, int wh);
	~CWindow(void);
	virtual int ProcessMouse(int mx, int my, uint32 mevent, CControl **mhook, CControl **khook);
	virtual int ProcessKeybd(unsigned char key, CControl **khook);
	virtual int MouseWheel(int mx, int my, int wheel);
	virtual void MouseMove(int mx, int my);
	virtual void Display(void);
	virtual void Reposition(int cx, int cy);
	void AddControl(CControl *ctrl);
	void Refresh(void);

	virtual char* GetProps(char *s, size_t s_s);
	virtual void SetProps(const char *props, float xScale, float yScale);

	CControl* GetControlByLabel(const char *label);

	CWindow *next, *prev;
protected:
	void	CheckPosition(void);			// make sure window is not going off screen
	void	DeleteAllControls(void);		// destroy all child controls

	bool	showTitle;
	bool	maximized;
	bool	locked;
	uint32	lastclick;						// for timing doubleclicks
	int		prevx, prevy;					// used for moving the window
	CControl *controlHead, *controlTail;	
};
