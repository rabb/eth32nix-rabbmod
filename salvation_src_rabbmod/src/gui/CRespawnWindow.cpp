// ETH32 - an Enemy Territory cheat for windows
// Copyright (c) 2007 eth32 team
// www.cheatersutopia.com & www.nixcoders.org

#include "../eth32.h"

CRespawnWindow::CRespawnWindow(const char *label, int wx, int wy, int ww, int wh)
: CWindow(label, wx, wy, ww, wh)
{
	showTitle = false;

	fontScale = GUI_FONTSCALE * 2.6;
	Resize(w, Draw.sizeTextHeight(fontScale, GUI_FONT) + 6);
}

void CRespawnWindow::Display(void)
{
	static char buffer[64];
	vec4_t color;
	int div = w / 3;

	if (!eth32.settings.respawnTimers)
		return;

	CWindow::Display();

	/*Vector4Set(color, eth32.settings.colorTeam[0]/255.f, eth32.settings.colorTeam[1]/255.f, eth32.settings.colorTeam[2]/255.f, 1.0);
	sprintf(buffer, "%i", eth32.cg.spawnTimes[TEAM_FRIEND]);
	Draw.TextCenter(x + div, y+3, fontScale, buffer, color, qtrue, qfalse, GUI_FONT, true);*/

	Vector4Set(color, eth32.settings.colorEnemy[0]/255.f, eth32.settings.colorEnemy[1]/255.f, eth32.settings.colorEnemy[2]/255.f, 1.0);
	sprintf(buffer, "%i", eth32.cg.spawnTimes[TEAM_ENEMY]);
	Draw.TextCenter(x + div + 8, y+3, fontScale, buffer, color, qtrue, qfalse, GUI_FONT, true);
}
