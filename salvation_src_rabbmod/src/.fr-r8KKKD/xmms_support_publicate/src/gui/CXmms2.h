// ETH32 - an Enemy Territory cheat for windows
// Copyright (c) 2007 eth32 team
// www.cheatersutopia.com & www.nixcoders.org

#ifndef CXMMS2_GUI_H_INCLUDED
#define CXMMS2_GUI_H_INCLUDED

#pragma once

#define MAX_BANNER_LINES	32

#include "CXmms2.h"
#include "../CXmms2.h"

class CXmms2_GUI : public CWindow
{
public:
	CXmms2_GUI(const char *label, int wx, int wy, int ww, int wh);
	void Display(void);

private:
	bool generateMessage();

	char *bfmt;
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

extern CXmms2* xmms2;

#endif // CXMMS2_H_INCLUDED
