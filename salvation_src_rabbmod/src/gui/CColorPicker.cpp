// ETH32 - an Enemy Territory cheat for windows
// Copyright (c) 2007 eth32 team
// www.cheatersutopia.com & www.nixcoders.org

#include "../eth32.h"

CColorPicker::CColorPicker(const char *clabel, int cx, int cy, int cw, int ch)
: CControl(clabel, cx, cy, cw, ch)
{
	currentColor = 0;
	numColors = 0;
	colorText = NULL;
	colorList = NULL;

	ctrlDropbox = new CDropbox("Current", 0, 0, cw - ch - 5, 23, 0, 0, &currentColor);
	ctrlDropbox->SetOrigin(x, y);
}

void CColorPicker::Display(void)
{
	if (!numColors)
		return;

	vec4_t targetColor;

	colorptr_t *color = &colorList[currentColor];

	if (color->bColor)
		VectorSet(targetColor, color->bColor[0]/255.f, color->bColor[1]/255.f, color->bColor[2]/255.f);
	else 
		VectorCopy(color->fColor, targetColor);

	targetColor[3] = 1.f;

	int drawY = 28 * eth32.cg.screenYScale;
	
	Draw.RawFillRect(x2 - h, y, h, h, targetColor);
	Draw.RawPic(x, y + drawY, w - h - GUI_SPACING, h - drawY, eth32.cg.media.colorTable);

	ctrlDropbox->Display();
}

int CColorPicker::ProcessMouse(int mx, int my, uint32 mevent, CControl **mhook, CControl **khook)
{
	if (INBOUNDS(mx, my)) {
		if (!ctrlDropbox->ProcessMouse(mx, my, mevent, mhook, khook))
			return 0;

		mx -= 2;
		my -= 1;
		if (INBOUNDS2(mx + 1, my, x, y + (28 * eth32.cg.screenYScale), x + (w - h - GUI_SPACING), y2)) {
			if (mevent == WM_LBUTTONDOWN) {
				if (colorList[currentColor].bColor)
					glReadPixels(mx, eth32.game.glconfig->vidHeight - my, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, colorList[currentColor].bColor);
				else 
					glReadPixels(mx, eth32.game.glconfig->vidHeight - my, 1, 1, GL_RGB, GL_FLOAT, colorList[currentColor].fColor);
					
				*mhook = this;
				return 0;
			}
		}
		return 0;
	}
	return 1;
}

void CColorPicker::MouseMove(int mx, int my)
{
	mx -= 1;
	my -= 1;
	if (INBOUNDS2(mx + 1, my, x, y + (28 * eth32.cg.screenYScale), x + (w - h - GUI_SPACING), y2)) {
		if (colorList[currentColor].bColor)
			glReadPixels(mx, eth32.game.glconfig->vidHeight - my, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, colorList[currentColor].bColor);
		else 
			glReadPixels(mx, eth32.game.glconfig->vidHeight - my, 1, 1, GL_RGB, GL_FLOAT, colorList[currentColor].fColor);
	}
}

void CColorPicker::Reposition(int cx, int cy)
{
	CControl::Reposition(cx, cy);
	ctrlDropbox->SetOrigin(x, y);
}

void CColorPicker::SetOrigin(int ox, int oy)
{
	CControl::SetOrigin(ox, oy);
	ctrlDropbox->SetOrigin(x, y);
}

void CColorPicker::AddColor(const char *colorName, uchar *color)
{
	if (!colorName || !*colorName || !color)
		return;

	CreateNewEntry(colorName, color, NULL);
}

void CColorPicker::AddColor(const char *colorName, float *color)
{
	if (!colorName || !*colorName || !color)
		return;

	CreateNewEntry(colorName, NULL, color);
}

void CColorPicker::CreateNewEntry(const char *colorName, uchar *bColor, float *fColor)
{
	colorptr_t *tempList = new colorptr_t[numColors+1];
	char **tempText = new char*[numColors+1];

	if (numColors) {
		memcpy(tempList, colorList, sizeof(colorptr_t) * numColors);
		memcpy(tempText, colorText, sizeof(char*) * numColors);

		delete [] colorList;
		delete [] colorText;
	}

	colorList = tempList;
	colorText = tempText;

	colorList[numColors].bColor = bColor;
	colorList[numColors].fColor = fColor;
	colorText[numColors] = new char[strlen(colorName)+1];
	strcpy(colorText[numColors], colorName);

	ctrlDropbox->SetText((const char**)colorText);
	ctrlDropbox->SetMin(0);
	ctrlDropbox->SetMax(numColors);

	numColors++;
}
