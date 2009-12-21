// ETH32 - an Enemy Territory cheat for windows
// Copyright (c) 2007 eth32 team
// www.cheatersutopia.com & www.nixcoders.org

#include "../eth32.h"

CButton::CButton(const char *clabel, int cx, int cy, int cw, int ch, void (*cfunc)(void))
: CControl(clabel, cx, cy, cw, ch)
{
	func = cfunc;
}

void CButton::Display(void)
{
	if (!show)
		return;

	if (selected) {
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

int CButton::ProcessMouse(int mx, int my, uint32 mevent, CControl **mhook, CControl **khook)
{
	if (!show)
		return 1;

	if (INBOUNDS(mx,my)) {
		if (mevent == WM_LBUTTONDOWN) {
			selected = true;
			*mhook = this;
		}
		else if (mevent == WM_LBUTTONUP && selected) {
			if (func)
				func();
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

void CButton::MouseMove(int mx, int my)
{
	selected = INBOUNDS(mx,my);
}
