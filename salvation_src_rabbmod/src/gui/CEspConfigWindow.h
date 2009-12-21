// ETH32 - an Enemy Territory cheat for windows
// Copyright (c) 2007 eth32 team
// www.cheatersutopia.com & www.nixcoders.org

#pragma once

#include "CWindow.h"
#include "CCheckbox.h"
#include "CSliders.h"

class CEspConfigWindow : public CWindow
{
public:
	CEspConfigWindow(const char *wlabel, int wx, int wy, int ww, int wh);
	void Display(void);
private:
	void UpdateOffsets(void);
	gentityInfo_t *gent;

	int x, y, w, h;
	CCheckBox *pName, *gRadarIcon, *gCham, *gIcon, *gText;
	CIntSlider *gDistance;
};
