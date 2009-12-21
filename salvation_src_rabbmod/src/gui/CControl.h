// ETH32 - an Enemy Territory cheat for windows
// Copyright (c) 2007 eth32 team
// www.cheatersutopia.com & www.nixcoders.org

#pragma once

class CControl
{
public:
	CControl(const char *clabel, int cx, int cy, int cw, int ch);
	~CControl(void);

	// Draw function
	virtual void Display(void);
	// Default handlers for mouse & keybd input
	virtual int ProcessMouse(int mx, int my, uint32 mevent, CControl **mhook, CControl **khook);
	virtual int ProcessKeybd(unsigned char key, CControl **khook);
	virtual int MouseWheel(int mx, int my, int wheel);
	virtual void MouseMove(int mx, int my);
	// Utility functions
	int Width(void) { return w; }
	int Height(void) { return h; }
	virtual void Refresh(void);
	virtual void ClearFocus(void);
	virtual void Reposition(int cx, int cy);
	virtual void Resize(int cw, int ch);
	virtual void SetOrigin(int ox, int oy);

	virtual void SetMouseFocus(bool set);
	virtual void SetKeybdFocus(bool set);
	// Used by text controls, tells GUI if they will process ESC or let GUI use it
	virtual bool CaptureEscapeCharacter(void);

	void SetLabel(const char *newLabel);
	char *GetLabel(char *s, size_t s_s);
	void Show(bool enable);
	// Our links to other controls
	CControl *prev, *next;
protected:
	char *label;			// text to be displayed with control (windows derived from this will use label as title)

	int x, y;				// abs control position
	int x2, y2;				// bottom right corner
	int xOffset, yOffset;	// relative position to parent

	int w, h;				// width and height of control

	int lx, ly;				// abs label position
	int lxOffset, lyOffset; // rel label position

	bool selected;
	bool show;
};

class CIntDisplay : public CControl
{
private:
	int *target;
public:
	CIntDisplay(const char *clabel, int cx, int cy, int cw, int ch, int *ctarget);
	void Display(void);
};

class CFloatDisplay : public CControl
{
private:
	float *target;
public:
	CFloatDisplay(const char *clabel, int cx, int cy, int cw, int ch, float *ctarget);
	void Display(void);
};
