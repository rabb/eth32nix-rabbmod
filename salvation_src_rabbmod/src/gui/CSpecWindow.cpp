// ETH32 - an Enemy Territory cheat for windows
// Copyright (c) 2007 eth32 team
// www.cheatersutopia.com & www.nixcoders.org

#include "../eth32.h"

CSpecWindow::CSpecWindow(const char *wlabel, int wx, int wy, int ww, int wh)
	: CWindow(wlabel, wx, wy, ww, wh)
{
	showTitle = true;
}

void CSpecWindow::Display(void)
{
	if (!Gui.InputActive())
		if ((!eth32.settings.getSpeclist || !eth32.cg.spectators[0][0]))
			return;

	int i, cnt, yPos = y + 22;
	char name[128];

	if (!Gui.InputActive() || eth32.cg.spectators[0][0]) {
		for (i=0; i<MAX_CLIENTS; i++) {
			if (!eth32.cg.spectators[i][0])
				break;

			memset(name, 0, sizeof(name));
			cnt = Draw.BreakStringByWidth(eth32.cg.spectators[i], w-10, NULL, GUI_FONT, false);
			if (cnt > 0 && cnt < sizeof(name))
				strncpy(name, eth32.cg.spectators[i], cnt);
			else
				continue;

			Draw.Text(x + 5, yPos, GUI_FONTSCALE, name, GUI_FONTCOLOR2, qfalse, qfalse, GUI_FONT, true);
			yPos += GUI_FONTHEIGHT + GUI_TEXTSPACING;
		}
	} else {
		Draw.Text(x + 5, yPos, GUI_FONTSCALE, "Spectator 1", GUI_FONTCOLOR2, qfalse, qfalse, GUI_FONT, true);
		yPos += GUI_FONTHEIGHT + GUI_TEXTSPACING;

		Draw.Text(x + 5, yPos, GUI_FONTSCALE, "Spectator 2", GUI_FONTCOLOR2, qfalse, qfalse, GUI_FONT, true);
		yPos += GUI_FONTHEIGHT + GUI_TEXTSPACING;

		i = 2;
	}

	Resize(w, (GUI_FONTHEIGHT * (i+1)) + (GUI_TEXTSPACING * i) + 15);

	CWindow::Display();
}
