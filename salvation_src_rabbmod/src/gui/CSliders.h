// ETH32 - an Enemy Territory cheat for windows
// Copyright (c) 2007 eth32 team
// www.cheatersutopia.com & www.nixcoders.org

#pragma once

#include "CControl.h"

class CIntSlider : public CControl
{
private:
	int *target;
	const char **txtlist;
	int max, min;
	int sliderPos;
	int sliderRange;
	float sliderUnit;
	int btnHeight;
	int btnWidth;
public:
	CIntSlider(const char *clabel, int cx, int cy, int cw, int ch, int cmin, int cmax, int *ctarget, const char *ctxt[] = NULL);
	void Display(void);
	int ProcessMouse(int mx, int my, uint32 mevent, CControl **mhook, CControl **khook);
	void MouseMove(int mx, int my);
	void SetOrigin(int ox, int oy);
	void Refresh(void);
	void SetTarget(int *ctarget);
};

class CFloatSlider : public CControl
{
protected:
	float *target;
	float max, min;
	int sliderPos;
	float sliderRange;
	float sliderUnit;
	int btnHeight;
	int btnWidth;
public:
	CFloatSlider(const char *clabel, int cx, int cy, int cw, int ch, float cmin, float cmax, float *ctarget);
	virtual void Display(void);
	int ProcessMouse(int mx, int my, uint32 mevent, CControl **mhook, CControl **khook);
	void MouseMove(int mx, int my);
	virtual void SetOrigin(int ox, int oy);
	virtual void Refresh(void);
	void SetTarget(float *ctarget);
};

// Using text like a intslider can, but more intuitive
class CDropbox : public CControl
{
public:
	CDropbox(const char *clabel, int cx, int cy, int cw, int ch, int cmin, int cmax, int *ctarget, const char *ctxt[] = NULL);
	void Display(void);
	int ProcessMouse(int mx, int my, uint32 mevent, CControl **mhook, CControl **khook);
	int MouseWheel(int mx, int my, int wheel);
	void MouseMove(int mx, int my);
	void SetTarget(int *ctarget);
	void SetText(const char **ctxt) { txtlist = ctxt; }
	void SetMin(int newMin);
	void SetMax(int newMax);
private:
	int *target;
	void (*func)(int);
	int pointTarget;
	int min, max;
	const char **txtlist;
};
