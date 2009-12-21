// ETH32 - an Enemy Territory cheat for windows
// Copyright (c) 2007 eth32 team
// www.cheatersutopia.com & www.nixcoders.org

#include "../eth32.h"

/********************
* CIntSlider        *
********************/

CIntSlider::CIntSlider(const char *clabel, int cx, int cy, int cw, int ch, int cmin, int cmax, int *ctarget, const char *ctxt[])
: CControl(clabel, cx, cy, cw, ch)
{
	target = ctarget;
	txtlist = ctxt;
	min = cmin;
	max = cmax;

	btnHeight = GUI_FONTHEIGHT;
	btnWidth = btnHeight * 1.4;

	sliderRange = max - min;
	sliderUnit = sliderRange / (float)(w - btnWidth);


}

void CIntSlider::Display(void)
{
	if (!target || !show)
		return;

	Refresh();

	char buf[16];
	sprintf(buf, "%i", *target);

	Draw.Text(x, y, GUI_FONTSCALE, label, GUI_FONTCOLOR1, qtrue, qfalse, GUI_FONT, true);

	if (txtlist)
		Draw.Text(x2 - Draw.sizeTextWidth(GUI_FONTSCALE, txtlist[*target - min], GUI_FONT), y, GUI_FONTSCALE, txtlist[*target - min], GUI_FONTCOLOR2, qtrue, qfalse, GUI_FONT, true);
	else
		Draw.Text(x2 - Draw.sizeTextWidth(GUI_FONTSCALE, buf, GUI_FONT), y, GUI_FONTSCALE, buf, GUI_FONTCOLOR2, qtrue, qfalse, GUI_FONT, true);

	// draw track
	Draw.RawPicST(x + (btnWidth >> 1), y2 - btnHeight, btnWidth, btnHeight, 0, 0, 0.25, 1, eth32.guiAssets.sliderTrack);
	Draw.RawPicST(x + btnWidth + (btnWidth >> 1), y2 - btnHeight, w - (btnWidth * 3), btnHeight, 0.25, 0, 0.75, 1, eth32.guiAssets.sliderTrack);
	Draw.RawPicST(x2 - btnWidth - (btnWidth >> 1), y2 - btnHeight, btnWidth, btnHeight, 0.75, 0, 1, 1, eth32.guiAssets.sliderTrack);

	// draw btn
	Draw.RawPic(sliderPos - (btnWidth >> 1), y2 - btnHeight, btnWidth, btnHeight, eth32.guiAssets.sliderBtn);
}

int CIntSlider::ProcessMouse(int mx, int my, uint32 event, CControl **mhook, CControl **khook)
{
	if (!target)
		return 1;

	if (INBOUNDS(mx,my)) {
		if (event == WM_LBUTTONDOWN && INBOUNDS2(mx,my,x,y2-btnHeight,x2,y2)) {	// is mouse in slider region
			*mhook = this;
			*target = min + (int)((mx - x - (btnWidth >> 1)) * sliderUnit + 0.5);	// update our target

			Refresh();

			return 0;
		}
		*mhook = NULL;
		// mouse was in bounds, event handled
		return 0;
	}
	return 1;
}

void CIntSlider::MouseMove(int mx, int my)
{
	if (mx < x) mx = x;
	if (mx > x2) mx = x2;

	*target = min + (int)((mx - x - (btnWidth >> 1)) * sliderUnit + 0.5); // fix for snapping target to right value based on position
	Refresh();

	return;
}

void CIntSlider::SetOrigin(int ox, int oy)
{
	CControl::SetOrigin(ox,oy);
	Refresh();
}

void CIntSlider::Refresh(void)
{
	if (target) {
		if (*target < min) *target = min;
		if (*target > max) *target = max;
		sliderPos = x + (btnWidth >> 1) + (*target - min) / sliderUnit;
	}
}

void CIntSlider::SetTarget(int *ctarget)
{
	target = ctarget;
	Refresh();
}

/********************
* CFloatSlider      *
********************/

CFloatSlider::CFloatSlider(const char *clabel, int cx, int cy, int cw, int ch, float cmin, float cmax, float *ctarget)
: CControl(clabel, cx, cy, cw, ch)
{
	target = ctarget;
	min = cmin;
	max = cmax;

	btnHeight = GUI_FONTHEIGHT;
	btnWidth = btnHeight * 1.4;

	sliderRange = max - min;
	sliderUnit = sliderRange / (float)(w - btnWidth);

	Refresh();
}

void CFloatSlider::Display(void)
{
	if (!target || !show)
		return;

	Refresh();

	char buf[16];
	sprintf(buf, "%.3f", *target);

	Draw.Text(x, y, GUI_FONTSCALE, label, GUI_FONTCOLOR1, qtrue, qfalse, GUI_FONT, true);
	Draw.Text(x2 - Draw.sizeTextWidth(GUI_FONTSCALE, buf, GUI_FONT), y, GUI_FONTSCALE, buf, GUI_FONTCOLOR2, qtrue, qfalse, GUI_FONT, true);

	// draw track
	Draw.RawPicST(x + (btnWidth >> 1), y2 - btnHeight, btnWidth, btnHeight, 0, 0, 0.25, 1, eth32.guiAssets.sliderTrack);
	Draw.RawPicST(x + btnWidth + (btnWidth >> 1), y2 - btnHeight, w - (btnWidth * 3), btnHeight, 0.25, 0, 0.75, 1, eth32.guiAssets.sliderTrack);
	Draw.RawPicST(x2 - btnWidth - (btnWidth >> 1), y2 - btnHeight, btnWidth, btnHeight, 0.75, 0, 1, 1, eth32.guiAssets.sliderTrack);

	// draw btn
	Draw.RawPic(sliderPos - (btnHeight >> 1), y2 - btnHeight, btnWidth, btnHeight, eth32.guiAssets.sliderBtn);

	//Draw.RawRect(x, y2, w, 0, colorYellow, 1);
	//Draw.RawRect(sliderPos, y2-4, 0, 5, colorYellow, 1);
}


int CFloatSlider::ProcessMouse(int mx, int my, uint32 event, CControl **mhook, CControl **khook)
{
	if (!target)
		return 1;

	if (INBOUNDS(mx,my)) {
		if (event == WM_LBUTTONDOWN && INBOUNDS2(mx,my,x,y2-btnHeight,x2,y2)) {
			*mhook = this;

			if (mx < (x + (btnWidth >> 1)))
				mx = x + (btnWidth >> 1);
			else if (mx > (x2 - (btnWidth >> 1)))
				mx = x2 - (btnWidth >> 1);

			sliderPos = mx;
			*target = min + (sliderPos - x - (btnWidth >> 1)) * sliderUnit;
			return 0;
		}
		*mhook = NULL;
		return 0;
	}
	return 1;
}

void CFloatSlider::MouseMove(int mx, int my)
{
	if (mx < (x + (btnWidth >> 1)))
		mx = x + (btnWidth >> 1);
	else if (mx > (x2 - (btnWidth >> 1)))
		mx = x2 - (btnWidth >> 1);

	sliderPos = mx;
	*target = min + (sliderPos - x - (btnWidth >> 1)) * sliderUnit;
	return;
}

void CFloatSlider::SetOrigin(int ox, int oy)
{
	CControl::SetOrigin(ox,oy);
	Refresh();
}

void CFloatSlider::Refresh(void)
{
	if (target) {
		if (*target < min) *target = min;
		if (*target > max) *target = max;
		sliderPos = x + (btnWidth >> 1) + (*target - min) / sliderUnit;
	}
}

void CFloatSlider::SetTarget(float *ctarget)
{
	target = ctarget;
	Refresh();
}

/********************
* CDropbox          *
********************/

CDropbox::CDropbox(const char *clabel, int cx, int cy, int cw, int ch, int cmin, int cmax, int *ctarget, const char *ctxt[])
: CControl(clabel, cx, cy, cw, ch)
{
	target = ctarget;
	min = cmin;
	max = cmax;
	txtlist = ctxt;

	if (target) {
		if (*target < min)
			*target = min;
		if (*target > max)
			*target = max;
	}

	pointTarget = -1;

	lyOffset = GUI_FONTHEIGHT + 3;
	ly = y + lyOffset;

	lxOffset = (w - (GUI_FONTHEIGHT + 6)) >> 1;
	lx = x + lxOffset;
}

void CDropbox::Display(void)
{
	if (!target)
		return;

	Draw.Text(x, y, GUI_FONTSCALE, label, GUI_FONTCOLOR1, qtrue, qfalse, GUI_FONT, true);

	lyOffset = GUI_FONTHEIGHT + 3;
	ly = y + lyOffset;

	lxOffset = (w - (GUI_FONTHEIGHT + 6)) >> 1;
	lx = x + lxOffset;

	int drawY = ly;

	char buf[16];

	if (selected) {
		int height = (GUI_FONTHEIGHT + GUI_TEXTSPACING) * (max - min + 1) + GUI_TEXTSPACING;

		if (height > GUI_TITLEBAR_SIZE*2) {
			/*Draw.RawPic(x, ly, GUI_TITLEBAR_SIZE, GUI_TITLEBAR_SIZE, eth32.guiAssets.winTopLeft);
			Draw.RawPic(x+GUI_TITLEBAR_SIZE, ly, w-(GUI_TITLEBAR_SIZE * 2), GUI_TITLEBAR_SIZE, eth32.guiAssets.winTop);
			Draw.RawPicST(x2-GUI_TITLEBAR_SIZE, ly, GUI_TITLEBAR_SIZE, GUI_TITLEBAR_SIZE, 1.f, 0.f, 0.f, 1.f, eth32.guiAssets.winTopLeft);

			Draw.RawPic(x, ly+GUI_TITLEBAR_SIZE, GUI_TITLEBAR_SIZE, height-(GUI_TITLEBAR_SIZE*2), eth32.guiAssets.winLeft);
			Draw.RawPic(x+GUI_TITLEBAR_SIZE, ly+GUI_TITLEBAR_SIZE, w-(GUI_TITLEBAR_SIZE*2), height-(GUI_TITLEBAR_SIZE*2), eth32.guiAssets.winCenter);
			Draw.RawPicST(x2-GUI_TITLEBAR_SIZE, ly+GUI_TITLEBAR_SIZE, GUI_TITLEBAR_SIZE, height-(GUI_TITLEBAR_SIZE*2), 1.f, 0.f, 0.f, 1.f, eth32.guiAssets.winLeft);

			Draw.RawPicST(x, ly+height-GUI_TITLEBAR_SIZE, GUI_TITLEBAR_SIZE, GUI_TITLEBAR_SIZE, 0.f, 1.f, 1.f, 0.f, eth32.guiAssets.winTopLeft);
			Draw.RawPicST(x+GUI_TITLEBAR_SIZE, ly+height-GUI_TITLEBAR_SIZE, w-(GUI_TITLEBAR_SIZE*2), GUI_TITLEBAR_SIZE, 0.f, 1.f, 1.f, 0.f, eth32.guiAssets.winTop);
			Draw.RawPicST(x2-GUI_TITLEBAR_SIZE, ly+height-GUI_TITLEBAR_SIZE, GUI_TITLEBAR_SIZE, GUI_TITLEBAR_SIZE, 1.f, 1.f, 0.f, 0.f, eth32.guiAssets.winTopLeft);*/
			Draw.RawPicST(x, drawY, GUI_TITLEBAR_SIZE, GUI_TITLEBAR_SIZE >> 1, 0.f, 0.f, 1.f, 0.5f, eth32.guiAssets.txtinputLeft);
			Draw.RawPicST(x+GUI_TITLEBAR_SIZE, drawY, w-(GUI_TITLEBAR_SIZE * 2), GUI_TITLEBAR_SIZE >> 1, 0.f, 0.f, 1.f, 0.5f, eth32.guiAssets.txtinputCenter);
			Draw.RawPicST(x2-GUI_TITLEBAR_SIZE, drawY, GUI_TITLEBAR_SIZE, GUI_TITLEBAR_SIZE >> 1, 0.f, 0.f, 1.f, 0.5f, eth32.guiAssets.txtinputRight);
			drawY += GUI_TITLEBAR_SIZE >> 1;

			Draw.RawPicST(x, drawY, GUI_TITLEBAR_SIZE, height-GUI_TITLEBAR_SIZE, 0.f, 0.25f, 1.f, 0.75f, eth32.guiAssets.txtinputLeft);
			Draw.RawPicST(x+GUI_TITLEBAR_SIZE, drawY, w-(GUI_TITLEBAR_SIZE*2), height-GUI_TITLEBAR_SIZE, 0.f, 0.25f, 1.f, 0.75f, eth32.guiAssets.txtinputCenter);
			Draw.RawPicST(x2-GUI_TITLEBAR_SIZE, drawY, GUI_TITLEBAR_SIZE, height-GUI_TITLEBAR_SIZE, 0.f, 0.25f, 1.f, 0.75f, eth32.guiAssets.txtinputRight);
			drawY = ly + height - (GUI_TITLEBAR_SIZE >> 1);

			Draw.RawPicST(x, drawY, GUI_TITLEBAR_SIZE, GUI_TITLEBAR_SIZE >> 1, 0.f, 0.5f, 1.f, 1.f, eth32.guiAssets.txtinputLeft);
			Draw.RawPicST(x+GUI_TITLEBAR_SIZE, drawY, w-(GUI_TITLEBAR_SIZE*2), GUI_TITLEBAR_SIZE >> 1, 0.f, 0.5f, 1.f, 1.f, eth32.guiAssets.txtinputCenter);
			Draw.RawPicST(x2-GUI_TITLEBAR_SIZE, drawY, GUI_TITLEBAR_SIZE, GUI_TITLEBAR_SIZE >> 1, 0.f, 0.5f, 1.f, 1.f, eth32.guiAssets.txtinputRight);
		}
		else {
			int size = height >> 1;
			Draw.RawPicST(x, ly, size, size, 0.f, 0.f, 0.5f, 0.5f, eth32.guiAssets.txtinputLeft);
			Draw.RawPicST(x+size, ly, w-(size*2), size, 0.f, 0.f, 1.f, 0.5f, eth32.guiAssets.txtinputCenter);
			Draw.RawPicST(x2-size, ly, size, size, 0.5f, 0.f, 1.f, 0.5f, eth32.guiAssets.txtinputRight);

			Draw.RawPicST(x, ly+size, size, size, 0.f, 0.5f, 0.5f, 1.f, eth32.guiAssets.txtinputLeft);
			Draw.RawPicST(x+size, ly+size, w-(size*2), size, 0.f, 0.5f, 1.f, 1.f, eth32.guiAssets.txtinputCenter);
			Draw.RawPicST(x2-size, ly+size, size, size, 0.5f, 0.5f, 1.f, 1.f, eth32.guiAssets.txtinputRight);
		}

		height = ly + GUI_TEXTSPACING;
		for (int i=min  ; i<max+1 ; i++)
		{
			sprintf(buf, "%i",i);
			Draw.TextCenter(x+(w>>1), height, GUI_FONTSCALE, txtlist && txtlist[i-min] ? txtlist[i-min] : buf, pointTarget == i ? GUI_FONTCOLOR2 : GUI_FONTCOLOR1, qtrue, qfalse, GUI_FONT, true);
			height += GUI_FONTHEIGHT + GUI_TEXTSPACING;
		}
	}
	else {
		Draw.RawPic(x, ly, GUI_FONTHEIGHT + 6, GUI_FONTHEIGHT + 6, eth32.guiAssets.txtinputLeft);
		Draw.RawPic(x + GUI_FONTHEIGHT + 6, ly, w - (GUI_FONTHEIGHT + GUI_FONTHEIGHT + 12), GUI_FONTHEIGHT + 6, eth32.guiAssets.txtinputCenter);
		Draw.RawPic(x2 - (GUI_FONTHEIGHT + 6), ly, GUI_FONTHEIGHT + 6, GUI_FONTHEIGHT + 6, eth32.guiAssets.dropboxArrow);

		sprintf(buf, "%i",*target);
		Draw.TextCenter(lx, ly + 3, GUI_FONTSCALE, txtlist && txtlist[*target-min] ? txtlist[*target-min] : buf, GUI_FONTCOLOR2, qtrue, qfalse, GUI_FONT, true);
	}
}

int CDropbox::ProcessMouse(int mx, int my, uint32 mevent, CControl **mhook, CControl **khook)
{
	if (!target)
		return 1;

	if (selected) {
		if (mevent == WM_LBUTTONUP && INBOUNDS2(mx, my, x, ly, x2, ly + (GUI_FONTHEIGHT + GUI_TEXTSPACING) * (max - min + 1) + GUI_TEXTSPACING)) {
			*target = min + (my - ly) / (GUI_FONTHEIGHT + GUI_TEXTSPACING);
		}
		*mhook = NULL;
		selected = false;
		pointTarget = -1;
		return 0;
	}
	if (mevent == WM_LBUTTONDOWN && INBOUNDS2(mx, my, x, ly, x2, ly+GUI_FONTHEIGHT+6)) {
		*mhook = this;
		selected = true;
		pointTarget = min;
		return 0;
	}
	return 1;
}

int CDropbox::MouseWheel(int mx, int my, int wheel)
{
	if (!target)
		return 1;

	if (INBOUNDS(mx,my)) {
		*target -= wheel;

		if (*target < min)
			*target = min;
		else if (*target > max)
			*target = max;

		return 0;
	}
	return 1;
}

void CDropbox::MouseMove(int mx, int my)
{
	if (selected) {
		if (INBOUNDS2(mx, my, x, ly, x2, ly + (GUI_FONTHEIGHT + GUI_TEXTSPACING) * (max - min + 1) + GUI_TEXTSPACING))
			pointTarget = min + (my - ly) / (GUI_FONTHEIGHT + GUI_TEXTSPACING);
	}
}

void CDropbox::SetTarget(int *ctarget)
{
	target = ctarget;

	if (target) {
		if (*target < min)
			*target = min;
		if (*target > max)
			*target = max;
	}
}

void CDropbox::SetMin(int newMin)
{
	min = newMin;

	if (*target < min)
		*target = min;
}

void CDropbox::SetMax(int newMax)
{
	max = newMax;

	if (*target > max)
		*target = max;
}
