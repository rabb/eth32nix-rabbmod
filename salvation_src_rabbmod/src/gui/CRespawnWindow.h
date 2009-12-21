// ETH32 - an Enemy Territory cheat for windows
// Copyright (c) 2007 eth32 team
// www.cheatersutopia.com & www.nixcoders.org

#pragma once

#include "CWindow.h"

class CRespawnWindow : public CWindow
{
public:
	CRespawnWindow(const char *label, int wx, int wy, int ww, int wh);
	void Display(void);
private:
	float fontScale;
};
