// ETH32 - an Enemy Territory cheat for windows
// Copyright (c) 2007 eth32 team
// www.cheatersutopia.com & www.nixcoders.org

#include "../eth32.h"

CRadarWindow::CRadarWindow(const char *wlabel, int wx, int wy, int ww, int wh)
: CWindow(wlabel, wx, wy, ww, ww) // set width and height to ww to keep it square
{
	showTitle = false;

	numPoints = 0;
	memset(points, 0, sizeof(points));

	cx = x + (w >> 1);
	cy = y + (h >> 1);
}

void CRadarWindow::Display(void)
{
	if (!show || eth32.settings.guiOriginal) return;

	CWindow::Display();

	vec4_t colorTeam, colorEnemy;
	vec3_t pt, my_ang, pt_ang;
	int px, py;
	float delta_ang;
	
	float distFactor = (cx - x - 5)/eth32.settings.radarRange;

	float scaleX = eth32.cg.screenXScale;
	float scaleY = eth32.cg.screenYScale;

	player_t *player;
	
	Vector4Set(colorTeam, eth32.settings.colorTeam[0]/255.f, eth32.settings.colorTeam[1]/255.f, eth32.settings.colorTeam[2]/255.f, 1);
	Vector4Set(colorEnemy, eth32.settings.colorEnemy[0]/255.f, eth32.settings.colorEnemy[1]/255.f, eth32.settings.colorEnemy[2]/255.f, 1);

	Tools.VectorAngles(eth32.cg.refdef->viewaxis[0], my_ang);
	
	// Radar icon (self)
	Syscall.R_SetColor(colorWhite);
	Draw.Pic((cx/scaleX)-3.5, (cy/scaleY)-3.5, 7, 7, eth32.cg.media.radaric);
	Syscall.R_SetColor(NULL);

	for (int i=0 ; i<numPoints ; i++) 
	{
		vec4_t color;	// Color of radar icons
		color[3] = 1;
		player = points[i];
		
		pt[0] = player->orHead.origin[0] - eth32.cg.refdef->vieworg[0];
		pt[1] = player->orHead.origin[1] - eth32.cg.refdef->vieworg[1];

		Tools.VectorAngles(pt, pt_ang);
		delta_ang = pt_ang[YAW] - my_ang[YAW] + 90.f;

		px = cx + cos(delta_ang * M_PI/180.f) * player->distance * distFactor;
		py = cy - sin(delta_ang * M_PI/180.f) * player->distance * distFactor;
	
		if (player->invulnerable) {
			VectorCopy(eth32.settings.colorInvulnerable, color);
			for(int i=0; i<3; i++)
				color[i] /= 255.f;		
		} else {
			if (player->visible)		
				player->friendly ? VectorCopy(colorTeam, color) : VectorCopy(colorEnemy, color);
			else {
				player->friendly ? VectorCopy(eth32.settings.colorTeamHidden, color) : VectorCopy(eth32.settings.colorEnemyHidden, color);
				for(int i=0; i<3; i++)
					color[i] /= 255.f;
			}
		}
		Syscall.R_SetColor(color);
		Draw.Pic((px/scaleX)-5, (py/scaleY)-5, 10, 10, eth32.cg.media.classIcons[player->Class]);
		Syscall.R_SetColor(NULL);
	}
	numPoints = 0;

	if (Gui.InputActive()) {
		char buf[64];
		sprintf(buf, "%i", (int)eth32.settings.radarRange);
		Draw.Text(x+GUI_SPACING, y2-GUI_FONTHEIGHT-GUI_SPACING, GUI_FONTSCALE, "Range", GUI_FONTCOLOR1, qtrue, qfalse, GUI_FONT, true);
		Draw.Text(x2-GUI_SPACING-TEXTWIDTH(buf), y2-GUI_FONTHEIGHT-GUI_SPACING, GUI_FONTSCALE, buf, GUI_FONTCOLOR2, qtrue, qfalse, GUI_FONT, true);
	}
}

int CRadarWindow::MouseWheel(int mx, int my, int wheel)
{
	if (!show) return 1; // not visible, can't process

	if (INBOUNDS(mx,my)) {
		eth32.settings.radarRange += -wheel * 100.f;

		if (eth32.settings.radarRange > 10000.f)
			eth32.settings.radarRange = 10000.f;
		else if (eth32.settings.radarRange < 100.f)
			eth32.settings.radarRange = 100.f;

		return 0;
	}
	return 1;
}

void CRadarWindow::SetOrigin(int ox, int oy)
{
	CControl::SetOrigin(ox, oy);

	cx = x + (w >> 1);
	cy = y + (h >> 1);
}

void CRadarWindow::Reposition(int ox, int oy)
{
	CWindow::Reposition(ox, oy);

	cx = x + (w >> 1);
	cy = y + (h >> 1);
}

void CRadarWindow::AddPlayer(player_t *player)
{
	if (numPoints < MAX_CLIENTS && player->distance < eth32.settings.radarRange) {
		points[numPoints] = player;
		numPoints++;
	}
}
