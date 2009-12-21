// ETH32 - an Enemy Territory cheat for windows
// Copyright (c) 2007 eth32 team
// www.cheatersutopia.com & www.nixcoders.org

#pragma once

#include "CSliders.h"

typedef struct
{
	uchar *bColor;
	float *fColor;
} colorptr_t;

class CColorPicker : public CControl
{
public:
	CColorPicker(const char *clabel, int cx, int cy, int cw, int ch);
	~CColorPicker(void);
	void Display(void);
	virtual int ProcessMouse(int mx, int my, uint32 event, CControl **mhook, CControl **khook);
	virtual void MouseMove(int mx, int my);
	void Reposition(int cx, int cy);
	void SetOrigin(int ox, int oy);
	void AddColor(const char *colorName, uchar *color);
	void AddColor(const char *colorName, float *color);
protected:
	void CreateNewEntry(const char *colorName, uchar *bColor, float *fColor);
	int currentColor;
	int numColors;
	char **colorText;
	colorptr_t *colorList;
	CDropbox *ctrlDropbox;
};
