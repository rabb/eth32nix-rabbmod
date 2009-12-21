// ETH32 - an Enemy Territory cheat for windows
// Copyright (c) 2007 eth32 team
// www.cheatersutopia.com & www.nixcoders.org

#pragma once

#define MAX_BANNER_LINES	32

#include "CWindow.h"

class CBanner : public CWindow
{
public:
	CBanner(const char *label, int wx, int wy, int ww, int wh);
	void Display(void);
	
private:
	bool generateMessage();
	bool getAmmo();
	
	char *bfmt;
	int ammo;
	int clip;
	int lastOffset;
	int updInterval;
	int lastUpd;
	
	int nlines;
	int cline;
	
	int currWidth;
	int currHeight;
	
	int bannerTextHeight;
	int bannerTextWidth;
	
	char line[MAX_BANNER_LINES][MAX_STRING_CHARS];
};
