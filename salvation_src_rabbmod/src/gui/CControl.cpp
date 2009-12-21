// ETH32 - an Enemy Territory cheat for windows
// Copyright (c) 2007 eth32 team
// www.cheatersutopia.com & www.nixcoders.org

#include "../eth32.h"

//**********************
// CControl
//**********************

CControl::CControl(const char *clabel, int cx, int cy, int cw, int ch) // x,y,w,h in 640x480 for layout purposes, convert to real res
{
	label = new char[strlen(clabel)+1];
	strcpy(label, clabel);
	
	x = xOffset = cx * eth32.cg.screenXScale;
	y = yOffset = cy * eth32.cg.screenYScale;
	lx = lxOffset = cx;
	ly = lyOffset = cy;
	w = cw * eth32.cg.screenXScale;
	h = ch * eth32.cg.screenYScale;
	x2 = x + w;
	y2 = y + h;

	show = true;
	selected = false;
}

char *CControl::GetLabel(char *s, size_t s_s)
{
	strcpy(s, label);
	return s;
}

CControl::~CControl(void)
{
	delete [] label;
}

void CControl::Display(void)
{
}

int CControl::ProcessMouse(int mx, int my, uint32 mevent, CControl **mhook, CControl **khook)
{
	if (INBOUNDS(mx,my))
		return 0;
	else
		return 1;
}

int CControl::ProcessKeybd(unsigned char key, CControl **khook)
{
	return 0;
}

int CControl::MouseWheel(int mx, int my, int wheel)
{
	return 1;
}

void CControl::MouseMove(int mx, int my)
{
}

void CControl::Refresh(void)
{
}

void CControl::ClearFocus(void)
{
}

void CControl::Reposition(int cx, int cy) // cx, cy in true resolution
{
	x = xOffset = cx;
	y = yOffset = cy;
	lx = x + lxOffset;
	ly = y + lyOffset;

	x2 = x + w;
	y2 = y + h;
}

void CControl::Resize(int cw, int ch)
{
	w = cw;
	h = ch;

	x2 = x + w;
	y2 = y + h;
}

void CControl::SetOrigin(int ox, int oy)
{
	x = ox + xOffset;
	y = oy + yOffset;
	x2 = x + w;
	y2 = y + h;
	lx = x + lxOffset;
	ly = y + lyOffset;
}

void CControl::SetMouseFocus(bool set)
{

}

void CControl::SetKeybdFocus(bool set)
{
	
}

bool CControl::CaptureEscapeCharacter(void)
{
	// default for most keybd hooking controls
	return true;
}

void CControl::Show(bool enable)
{
	show = enable;
}

void CControl::SetLabel(const char *newLabel)
{
	if (newLabel && *newLabel) {
		delete [] label;
		label = new char[strlen(newLabel)+1];
		strcpy(label, newLabel);
	}
}

//**********************
// CIntDisplay
//**********************

CIntDisplay::CIntDisplay(const char *clabel, int cx, int cy, int cw, int ch, int *ctarget)
: CControl(clabel, cx, cy, cw, ch)
{
	target = ctarget;
}

void CIntDisplay::Display(void)
{
	char txt[16];
	sprintf(txt, "%i", *target);

	Draw.Text(x, y, GUI_FONTSCALE, label, GUI_FONTCOLOR1, qtrue, qfalse, GUI_FONT, true);
	Draw.Text(x2 - Draw.sizeTextWidth(GUI_FONTSCALE, txt, GUI_FONT), y, GUI_FONTSCALE, txt, GUI_FONTCOLOR2, qtrue, qfalse, GUI_FONT, true);
}

//**********************
// CIntDisplay
//**********************

CFloatDisplay::CFloatDisplay(const char *clabel, int cx, int cy, int cw, int ch, float *ctarget)
: CControl(clabel, cx, cy, cw, ch)
{
	target = ctarget;
}

void CFloatDisplay::Display(void)
{
	char txt[16];
	sprintf(txt, "%.2f", *target);

	Draw.Text(x, y, GUI_FONTSCALE, label, GUI_FONTCOLOR1, qtrue, qfalse, GUI_FONT, true);
	Draw.Text(x2 - Draw.sizeTextWidth(GUI_FONTSCALE, txt, GUI_FONT), y, GUI_FONTSCALE, txt, GUI_FONTCOLOR2, qtrue, qfalse, GUI_FONT, true);
}
