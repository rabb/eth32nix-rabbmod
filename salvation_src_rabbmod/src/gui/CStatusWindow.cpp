// ETH32 - an Enemy Territory cheat for windows
// Copyright (c) 2007 eth32 team
// www.cheatersutopia.com & www.nixcoders.org

#include "../eth32.h"

CStatusWindow::CStatusWindow(const char *wlabel, int wx, int wy, int ww, int wh)
: CWindow(wlabel, wx, wy, ww, wh)
{
	showTitle = false;
	statbarSize = 10 * eth32.cg.screenYScale;
}

void CStatusWindow::Display(void)
{
	if (!IS_INFOVALID(eth32.cg.snap->ps.clientNum) || eth32.settings.guiOriginal)
		return;

	CWindow::Display();

	int maxHealth = eth32.cg.snap->ps.stats[STAT_MAX_HEALTH];
	int curHealth = eth32.cg.snap->ps.stats[STAT_HEALTH];

	if (eth32.cgMod->type != MOD_TCE && *eth32.cg.players[eth32.cg.snap->ps.clientNum].cls == PC_MEDIC)
		maxHealth *= 1.12;

	if (curHealth < 0)
		curHealth = 0;

	float healthFrac = curHealth / (float)maxHealth;
	char buffer[16];
	sprintf(buffer, "%i", curHealth);
	
	int drawX = x + GUI_SPACING;
	int drawY = y + GUI_SPACING;

	Draw.Text(drawX, drawY, GUI_FONTSCALE, "Health", eth32.guiAssets.textColor1, qtrue, qfalse, GUI_FONT, true);
	Draw.Text(x2-GUI_SPACING-Draw.sizeTextWidth(GUI_FONTSCALE, buffer, GUI_FONT), drawY, GUI_FONTSCALE, buffer, eth32.guiAssets.textColor2, qtrue, qfalse, GUI_FONT, true);
	drawY += GUI_FONTHEIGHT + GUI_SPACING;
	vec4_t healthColor;
	Draw.ColorForHealth(curHealth, healthColor);
	Draw.Statbar(drawX, drawY, w-GUI_SPACING-GUI_SPACING, statbarSize, healthFrac, healthColor);
	
	drawY += statbarSize + GUI_SPACING;

	if (eth32.cg.pmext) {
		float stamFrac = eth32.cg.pmext->sprintTime / (float)SPRINTTIME;
		sprintf(buffer, "%i", (int)(stamFrac * 100.f));

		Draw.Text(drawX, drawY, GUI_FONTSCALE, "Stamina", eth32.guiAssets.textColor1, qtrue, qfalse, GUI_FONT, true);
		Draw.Text(x2-GUI_SPACING-Draw.sizeTextWidth(GUI_FONTSCALE, buffer, GUI_FONT), drawY, GUI_FONTSCALE, buffer, eth32.guiAssets.textColor2, qtrue, qfalse, GUI_FONT, true);
		
		drawY += GUI_FONTHEIGHT + GUI_SPACING;
		
		vec4_t color;
		Vector4Copy(eth32.settings.gui_stamina, color);

		// Add white flash for adrenaline time.
		float msec = eth32.cg.snap->ps.powerups[PW_ADRENALINE] - eth32.cg.time;
		
		if( msec < 0 ) 
			msec = 0;
		else 
			Vector4Average( color, colorWhite, .5f + sin(.2f * sqrt(msec) * 2 * M_PI) * .5f, color);

		Draw.Statbar(drawX, drawY, w-GUI_SPACING-GUI_SPACING, statbarSize, stamFrac, color);
	}	
}
