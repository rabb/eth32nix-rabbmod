// ETH32 - an Enemy Territory cheat for windows
// Copyright (c) 2007 eth32 team
// www.cheatersutopia.com & www.nixcoders.org

#include "../eth32.h"

CWindow::CWindow(const char *wlabel, int wx, int wy, int ww, int wh)
: CControl(wlabel, wx, wy, ww, wh)
{
	showTitle = true;						// will determine what origin ctrls are oriented to
	maximized = true;						// windows maximized by default
	locked = false;							// windows unlocked by default

	controlHead = controlTail = NULL;		// init our control list to NULL

	xOffset = yOffset = 0;					// because windows all use abs coords to screen origin
											// offsets are for controls in relation to parent window

	lastclick = 0;							// init last click time to 0
}

CWindow::~CWindow(void)
{
	DeleteAllControls();
}

void CWindow::Display(void)
{
	if (!show) return;

	vec4_t titleColor = { 0.3, 0.3, 0.6, 0.85 };
	vec4_t winColor = { 0.1, 0.1, 0.2, 0.85 };

	int yOff = 0;

	if (showTitle) {
		Draw.RawPic(x, y, GUI_TITLEBAR_SIZE, GUI_TITLEBAR_SIZE, eth32.guiAssets.titleLeft);
		Draw.RawPic(x+GUI_TITLEBAR_SIZE, y, w - (GUI_TITLEBAR_SIZE << 1), GUI_TITLEBAR_SIZE, eth32.guiAssets.titleCenter);
		Draw.RawPicST(x2-GUI_TITLEBAR_SIZE, y, GUI_TITLEBAR_SIZE, GUI_TITLEBAR_SIZE, 1.f, 0.f, 0.f, 1.f, eth32.guiAssets.titleLeft);

		Draw.TextCenter(x + (w >> 1), y + 3, GUI_FONTSCALE, label, GUI_TITLECOLOR, qtrue, qfalse, GUI_FONT, true);

		yOff += GUI_TITLEBAR_SIZE;
	}
	else {
		Draw.RawPic(x, y, GUI_ESIZE, GUI_ESIZE, eth32.guiAssets.winTopLeft);
		Draw.RawPic(x + GUI_ESIZE, y, w - (GUI_ESIZE+GUI_ESIZE), GUI_ESIZE, eth32.guiAssets.winTop);
		Draw.RawPicST(x2 - GUI_ESIZE, y, GUI_ESIZE, GUI_ESIZE, 1.f, 0.f, 0.f, 1.f, eth32.guiAssets.winTopLeft);

		yOff += GUI_ESIZE;
	}

	if (maximized) {  // all windows without title are maximized anyway
		Draw.RawPic(x, y + yOff, GUI_ESIZE, h - GUI_ESIZE - yOff, eth32.guiAssets.winLeft);
		Draw.RawPic(x + GUI_ESIZE, y + yOff, w - (GUI_ESIZE+GUI_ESIZE), h - GUI_ESIZE - yOff, eth32.guiAssets.winCenter);
		Draw.RawPicST(x2 - GUI_ESIZE, y + yOff, GUI_ESIZE, h - GUI_ESIZE - yOff, 1.f, 0.f, 0.f, 1.f, eth32.guiAssets.winLeft);

		Draw.RawPicST(x, y2 - GUI_ESIZE, GUI_ESIZE, GUI_ESIZE, 0.f, 1.f, 1.f, 0.f, eth32.guiAssets.winTopLeft);
		Draw.RawPicST(x + GUI_ESIZE, y2 - GUI_ESIZE, w - (GUI_ESIZE+GUI_ESIZE), GUI_ESIZE, 0.f, 1.f, 1.f, 0.f, eth32.guiAssets.winTop);
		Draw.RawPicST(x2 - GUI_ESIZE, y2 - GUI_ESIZE, GUI_ESIZE, GUI_ESIZE, 1.f, 1.f, 0.f, 0.f, eth32.guiAssets.winTopLeft);

		CControl *ctrl = controlHead;
		while (ctrl) {
			ctrl->Display();
			ctrl = ctrl->next;
		}
	}
}

int CWindow::ProcessMouse(int mx, int my, uint32 mevent, CControl **mhook, CControl **khook)
{
	// a hidden window can't process any input
	if (!show) return 1;

	if (INBOUNDS(mx,my)) {
		// check if mouse was in titlebar region
		if (showTitle && !locked && my <= y + GUI_TITLEBAR_SIZE) {
			if (mevent == WM_LBUTTONDOWN) {
				uint32 click = Time();
				if ((click - lastclick) < GUI_DBLCLK_TIME) {	// minimize window if user double clicks titlebar
					maximized = !maximized;
					lastclick = 0;
				}
				else lastclick = click;
				prevx = mx;
				prevy = my;
				*mhook = this;
			}
			else *mhook = NULL;		// release mouse focus
			// event processed
			return 0;
		}
		// if maximized, check our control list
		if (maximized) {
			CControl *ctrl = controlHead;
			while (ctrl && ctrl->ProcessMouse(mx, my, mevent, mhook, khook)) {
				ctrl = ctrl->next;
			}

			if (ctrl)
				return 0;	// ctrl handled event

			if (!showTitle && mevent == WM_LBUTTONDOWN) {	// no title & no control claimed input & left button pressed
				prevx = mx;
				prevy = my;
				*mhook = this;
			}
			else *mhook = NULL;
			// mouse was within this window, event handled
			return 0;
		}
		else return 1; // window is minimized and mouse was not in titlebar region
	}
	// mouse event outside of this window
	return 1;
}

int CWindow::ProcessKeybd(unsigned char key, CControl **khook)
{
	// TODO: window function keys could be used here
	return 0;
}

int CWindow::MouseWheel(int mx, int my, int wheel)
{
	if (!show) return 1;

	if (INBOUNDS(mx,my)) {
		CControl *ctrl = controlHead;
		while (ctrl && ctrl->MouseWheel(mx, my, wheel)) {
			ctrl = ctrl->next;
		}
		return 0; // scroll event happened within our window region, handled
	}
	return 1;
}

void CWindow::MouseMove(int mx, int my)
{
	// NOTE: this is only called when the window/control has focus in gui (mouseFocus == this)
	SetOrigin(x + mx - prevx, y + my - prevy);

	prevx = mx;
	prevy = my;

	CheckPosition();

	CControl *ctrl = controlHead;
	while (ctrl) {
		if (showTitle)
			ctrl->SetOrigin(x, y + GUI_TITLEBAR_SIZE);		// control coords should be relative to top/left corner not including titlebar
		else ctrl->SetOrigin(x, y);
		ctrl = ctrl->next;
	}
}

void CWindow::Reposition(int cx, int cy)
{
	CControl::Reposition(cx, cy);

	xOffset = yOffset = 0;

	CheckPosition();

	CControl *ctrl = controlHead;
	while (ctrl) {
		if (showTitle)
			ctrl->SetOrigin(x, y + GUI_TITLEBAR_SIZE);		// control coords should be relative to top/left corner not including titlebar
		else ctrl->SetOrigin(x, y);
		ctrl = ctrl->next;
	}
}

void CWindow::AddControl(CControl *ctrl)
{
	if (ctrl == NULL) return;

	ctrl->next = controlHead;
	ctrl->prev = NULL;

	if (showTitle)
		ctrl->SetOrigin(x, y + GUI_TITLEBAR_SIZE);			// control coords should be relative to top/left corner not including titlebar
	else ctrl->SetOrigin(x, y);

	if (controlHead != NULL) {
		controlHead->prev = ctrl;
		controlHead = ctrl;
	}
	else {
		controlHead = controlTail = ctrl;
	}
}

void CWindow::Refresh(void)
{
	CControl *ctrl = controlHead;

	while (ctrl) {
		ctrl->Refresh();
		ctrl = ctrl->next;
	}
}

char *CWindow::GetProps(char *s, size_t s_s)
{
	sprintf(s, "%i %i %i %i", x, y, w, h);
	return s;
}

void CWindow::SetProps(const char *props, float xScale, float yScale)
{
	int px, py, pw, ph;

	sscanf(props, "%i %i %i %i", &px, &py, &pw, &ph);
	// not using width and height for regular windows, but will use for chat and radar windows
	Reposition(px * xScale, py * yScale);
}

CControl* CWindow::GetControlByLabel(const char *label)
{
	CControl *find = controlHead;

	char buf[64];
	while (find)
	{
		find->GetLabel(buf,sizeof(buf));
		if (!strcmp(label, buf))
			return find;

		find = find->next;
	}

	return NULL;
}

void CWindow::CheckPosition(void)
{
	// check and make sure user doesn't accidently move window off screen
	if (x2 > eth32.game.glconfig->vidWidth)
		x = eth32.game.glconfig->vidWidth - w;
	else if (x < 0)
		x = 0;

	x2 = x + w;

	if (y2 > eth32.game.glconfig->vidHeight)
		y = eth32.game.glconfig->vidHeight - h;
	else if (y < 0)					// dont't want titlebar to become hidden
		y = 0;

	y2 = y + h;
}

void CWindow::DeleteAllControls(void)
{
	CControl *del = controlHead;
	while (del) {
		controlHead = del->next;
		delete del;
		del = controlHead;
	}
	controlHead = controlTail = NULL;
}
