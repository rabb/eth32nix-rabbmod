// ETH32 - an Enemy Territory cheat for windows
// Copyright (c) 2007 eth32 team
// www.cheatersutopia.com & www.nixcoders.org

#include "../eth32.h"

/*********************
* CWindowButton
*********************/

CWindowButton::CWindowButton(const char *clabel, int cx, int cy, int cw, int ch, wincat_t cat, CWindowPicker *pwin)
: CControl(clabel, cx, cy, cw, ch)
{
	category = cat;
	windowPicker = pwin;
	winActive = false;
	winList.clear();
}

void CWindowButton::Display(void)
{
	if (!show)
		return;

	if (selected || winActive) {
		Draw.RawPic(x, y, h, h, eth32.guiAssets.btnselLeft);
		Draw.RawPic(x + h, y, w-(h+h), h, eth32.guiAssets.btnselCenter);
		Draw.RawPic(x2 - h, y, h, h, eth32.guiAssets.btnselRight);
	}
	else {
		Draw.RawPic(x, y, h, h, eth32.guiAssets.btnLeft);
		Draw.RawPic(x + h, y, w-(h+h), h, eth32.guiAssets.btnCenter);
		Draw.RawPic(x2 - h, y, h, h, eth32.guiAssets.btnRight);
	}

	Draw.TextCenter(x + (w >> 1), y + ((h - GUI_FONTHEIGHT) >> 1), GUI_FONTSCALE, label, GUI_FONTCOLOR1, qtrue, qfalse, GUI_FONT, true);
}

int CWindowButton::ProcessMouse(int mx, int my, uint32 mevent, CControl **mhook, CControl **khook)
{
	if (!show)
		return 1;

	if (INBOUNDS(mx,my)) {
		if (mevent == WM_LBUTTONDOWN) {
			selected = true;
			*mhook = this;
		}
		else if (mevent == WM_LBUTTONUP && selected) {
			winActive = !winActive;
			ShowWindows(winActive);

			selected = false;
			*mhook = NULL;
		}
		else {
			selected = false;
			*mhook = NULL;
		}
		return 0;
	}
	return 1;
}

void CWindowButton::MouseMove(int mx, int my)
{
	selected = INBOUNDS(mx,my);
}

void CWindowButton::ShowWindows(bool show)
{
	winActive = show;

	if (show)
		windowPicker->WindowActiveCallback(category);

	list<CWindow*>::iterator win_iter = winList.begin();

	while (win_iter != winList.end()) {
		(*win_iter)->Show(show);
		win_iter++;
	}
}

void CWindowButton::AddWindow(CWindow *win)
{
	winList.push_front(win);
}

/*********************
* CWindowPicker
*********************/

CWindowPicker::CWindowPicker(const char *wlabel, int wx, int wy, int ww, int wh)
: CWindow(wlabel, wx, wy, ww, wh)
{
	showTitle = false;
	locked = true;

	allowMultiWindows = false;
	activeWinCount = buttonCount = 0;
	lastActive = CAT_AIMBOT;

	Reposition(GUI_SPACING, GUI_SPACING);
	Resize(eth32.game.glconfig->vidWidth - (GUI_SPACING*2), h);

	buttons[CAT_AIMBOT] = new CWindowButton("Aimbot", 5, 5, 76, 16, CAT_AIMBOT, this);
	buttons[CAT_AIMBOT2] = new CWindowButton("Aimbot Extra", 5, 5, 76, 16, CAT_AIMBOT2, this);
	buttons[CAT_VISUAL] = new CWindowButton("Visuals", 5, 5, 76, 16, CAT_VISUAL, this);
	buttons[CAT_VISUAL2] = new CWindowButton("Visuals Extra", 5, 5, 76, 16, CAT_VISUAL2, this);
	buttons[CAT_COLOR] = new CWindowButton("Color Picker", 5, 5, 76, 16, CAT_COLOR, this);
	buttons[CAT_MISC] = new CWindowButton("Misc", 5, 5, 76, 16, CAT_MISC, this);

	ReorganizeButtons();
}

CWindowPicker::~CWindowPicker(void)
{
	for (int i=0 ; i<CAT_MAX ; i++)
	{
		if (buttons[i])
			delete buttons[i];
	}
}

void CWindowPicker::Display(void)
{
	CWindow::Display();

	for (int i=0 ; i<CAT_MAX ; i++)
	{
		if (buttons[i]) {
			if (!allowMultiWindows && i != lastActive)
				buttons[i]->ShowWindows(false);
			buttons[i]->Display();
		}
	}
}

int CWindowPicker::ProcessMouse(int mx, int my, uint32 mevent, CControl **mhook, CControl **khook)
{
	// a hidden window can't process any input
	if (!show) return 1;

	if (INBOUNDS(mx,my)) {
		for (int i=0 ; i<CAT_MAX ; i++) {
			if (buttons[i] && !buttons[i]->ProcessMouse(mx, my, mevent, mhook, khook))
				return 0;
		}
		return 0;
	}
	// mouse event outside of this window
	return 1;
}

void CWindowPicker::AddWindow(const char *winTitle, wincat_t cat)
{
	CWindow *win = Gui.GetWinByTitle(winTitle);

	if (!win || cat == CAT_MAX)
		return;

	buttons[cat]->AddWindow(win);
}

void CWindowPicker::ClearAll(void)
{
	for (int i=0 ; i<CAT_MAX ; i++)
	{
		buttons[i]->ShowWindows(false);
	}
}

// callback is used to close other open windows when user only wants 1 open at a time
void CWindowPicker::WindowActiveCallback(wincat_t cat)
{
	lastActive = cat;

	if (allowMultiWindows)
		return;

	for (int i=0 ; i<CAT_MAX ; i++)
	{
		if (i != cat && buttons[i])
			buttons[i]->ShowWindows(false);
	}
}

void CWindowPicker::ReorganizeButtons(void)
{
	int xPos = GUI_SPACING;
	int yPos = GUI_SPACING;
	int btnWidth = (w - ((CAT_MAX + 1) * GUI_SPACING)) / CAT_MAX;

	for (int i=0 ; i<CAT_MAX ; i++)
	{
		buttons[i]->Reposition(xPos, yPos);
		buttons[i]->Resize(btnWidth, buttons[i]->Height());
		buttons[i]->SetOrigin(x, y);

		xPos += (btnWidth + GUI_SPACING);
	}
}
