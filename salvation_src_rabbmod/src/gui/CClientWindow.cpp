// ETH32 - an Enemy Territory cheat for windows
// Copyright (c) 2007 eth32 team
// www.cheatersutopia.com & www.nixcoders.org

#include "../eth32.h"

/************************
* CClientWindow
************************/

#define CLIENT_SPACING		((int)(2 * eth32.cg.screenYScale))

CClientWindow::CClientWindow(const char *wlabel, int wx, int wy, int ww, int wh)
: CWindow(wlabel, wx, wy, ww, wh)
{
	locked = true;

	numTeam = numEnemy = numSpec = 0;
	selectedClientNum = -1;

	int totalUseableWidth = w - (GUI_SPACING * 3);
	leftPaneWidth = totalUseableWidth * 0.50;
	rightPaneWidth = totalUseableWidth - leftPaneWidth;

	InitClientButtons();

	lgFontScale = GUI_FONTSCALE * 2;
	lgFontSize = Draw.sizeTextHeight(lgFontScale, GUI_FONT);
}

CClientWindow::~CClientWindow(void)
{

}

void CClientWindow::Display(void)
{
	UpdateClients();

	CWindow::Display();

	if (selectedButton >= 0 && (eth32.cg.time - selectTime > 750))
		selectedButton = -1;

	DrawClientInfo();
	DrawClientButtons();
	DrawClTypeButtons();

	if (selectedClientNum >= 0 && selectedClientNum < MAX_CLIENTS && IS_INFOVALID(selectedClientNum)) {
		// TODO: friend checking for button display
	}
	else
		selectedClientNum = -1;
}

int CClientWindow::ProcessMouse(int mx, int my, uint32 mevent, CControl **mhook, CControl **khook)
{
	if (INBOUNDS(mx,my)) {
		if (mevent == WM_LBUTTONDOWN && (CheckClientSelect(mx,my) || CheckClTypeSelect(mx, my)))
			return 0;
	}
	return 1;
}

bool CClientWindow::CheckClientSelect(int mx, int my)
{
	for (int i=0 ; i<2 ; i++) {
		for (int j=0 ; j<32 ; j++) {
			clientButton_t *btn = &buttons[i][j];

			if (btn->valid && INBOUNDS2(mx,my,btn->x,btn->y,btn->x2,btn->y2)) {
				selectedClientNum = btn->clientNum;
				return true;
			}
		}
	}
	return false;
}

bool CClientWindow::CheckClTypeSelect(int mx, int my)
{
	for (int i=0 ; i<3 ; i++) {
		plTypeButton_t *btn = &typeButtons[i];

		if (INBOUNDS2(mx,my,btn->x,btn->y,btn->x2,btn->y2)) {
			selectedButton = i;
			selectTime = eth32.cg.time;
			if (selectedClientNum >= 0)
				eth32.cg.players[selectedClientNum].playerType = btn->type;
			return true;
		}
	}
	return false;
}

int CClientWindow::MouseWheel(int mx, int my, int wheel)
{
	return CWindow::MouseWheel(mx, my, wheel);
}

void CClientWindow::UpdateClients(void)
{
	numTeam = numEnemy = numSpec = 0;

	int cntCol1, cntCol2;
	cntCol1 = cntCol2 = 0;

	int specList[MAX_CLIENTS];
	clientButton_t *btn = NULL;

	ClearClients();

	for (int i=0 ; i<MAX_CLIENTS ; i++) {
		if (IS_INFOVALID(i)) {
			int team = *eth32.cg.players[i].team;

			if (IS_SPECTATOR(i)) {
				specList[numSpec] = i;
				numSpec++;
			} else if (IS_FRIENDLY(i)) {
				btn = &buttons[0][cntCol1];
				btn->clientNum = i;
				btn->valid = true;
				btn->type = CLIENT_TEAM;
				cntCol1++;
				numTeam++;
			} else {
				btn = &buttons[1][cntCol2];
				btn->clientNum = i;
				btn->valid = true;
				btn->type = CLIENT_ENEMY;
				cntCol2++;
				numEnemy++;
			}
		}
	}


	
	

	for (int i=0 ; i<numSpec ; i++) {
		btn = FindOpenSpecSlot();

		if (!btn)
			continue; // this shouldn't happen

		btn->clientNum = specList[i];
		btn->valid = true;
		btn->type = CLIENT_SPEC;
	}
}

void CClientWindow::ClearClients(void)
{
	for (int i=0 ; i<2 ; i++) {
		for (int j=0 ; j<32 ; j++) {
			clientButton_t *btn = &buttons[i][j];

			btn->valid = false;
			btn->clientNum = -1;
			btn->type = CLIENT_NONE;
		}
	}
}

void CClientWindow::InitClientButtons(void)
{
	int btnWidth = (rightPaneWidth - GUI_SPACING) / 2;
	int btnHeight = (h - GUI_TITLEBAR_SIZE - (GUI_SPACING * 2) - (CLIENT_SPACING * 31)) / 32;
	int xPos = x + GUI_SPACING + leftPaneWidth + GUI_SPACING;

	for (int i=0 ; i<2 ; i++) {
		int yPos = y + GUI_TITLEBAR_SIZE + GUI_SPACING;
		for (int j=0 ; j<32 ; j++) {
			clientButton_t *btn = &buttons[i][j];

			btn->clientNum = -1;
			btn->type = CLIENT_NONE;
			btn->valid = false;
			btn->x = xPos;
			btn->y = yPos;
			btn->w = btnWidth;
			btn->h = btnHeight;
			btn->x2 = xPos + btnWidth;
			btn->y2 = yPos + btnHeight;

			yPos += btnHeight + CLIENT_SPACING;
		}
		xPos += btnWidth + GUI_SPACING;
	}

	strcpy(typeButtons[0].caption, "Normal");
	typeButtons[0].type = PLAYER_NORMAL;
	strcpy(typeButtons[1].caption, "Friend");
	typeButtons[1].type = PLAYER_FRIEND;
	strcpy(typeButtons[2].caption, "Enemy");
	typeButtons[2].type = PLAYER_ENEMY;

	typeButtons[0].w = typeButtons[1].w = typeButtons[2].w = btnWidth/2;
	typeButtons[0].h = typeButtons[1].h = typeButtons[2].h = btnHeight;

	xPos = 20;
	int yPos = y+h-100;

	for(int i=0; i<3; i++) {
		typeButtons[i].x = xPos; typeButtons[i].x2 = typeButtons[i].x + typeButtons[i].w;
		typeButtons[i].y = yPos; typeButtons[i].y2 = typeButtons[i].y + typeButtons[i].h;
		xPos += GUI_SPACING + typeButtons[i].w;
	}
}

void CClientWindow::DrawClTypeButtons(void)
{
	for (int i=0 ; i<3 ; i++) {
		plTypeButton_t *btn = &typeButtons[i];

		if (selectedButton == i)
			Syscall.R_SetColor(colorMagenta);
		else {
			if (selectedButton < 0 && selectedClientNum >= 0 && eth32.cg.players[selectedClientNum].playerType == btn->type)
				Syscall.R_SetColor(colorGreen);
			else
				Syscall.R_SetColor(colorLtGrey);
		}

		qhandle_t shader = eth32.cg.media.combtnLeft;

		Draw.RawPicST(btn->x, btn->y, 12, 6, 0, 0, 1, 0.5, shader);
		Draw.RawPicST(btn->x, btn->y+6, 12, btn->h-12, 0, .4, 1, 0.6, shader);
		Draw.RawPicST(btn->x, btn->y+btn->h-6, 12, 6, 0, 0.5, 1, 1, shader);

		shader = eth32.cg.media.combtnCenter;

		Draw.RawPicST(btn->x + 12, btn->y, btn->w - 24, 6, 0, 0, 1, 0.5, shader);
		Draw.RawPicST(btn->x + 12, btn->y+6, btn->w - 24, btn->h-12, 0, 0.4, 1, 0.6, shader);
		Draw.RawPicST(btn->x + 12, btn->y+btn->h-6, btn->w - 24, 6, 0, 0.5, 1, 1, shader);

		shader = eth32.cg.media.combtnRight;

		Draw.RawPicST(btn->x + btn->w - 12, btn->y, 12, 6, 0, 0, 1, 0.5, shader);
		Draw.RawPicST(btn->x + btn->w - 12, btn->y+6, 12, btn->h-12, 0, 0.4, 1, 0.6, shader);
		Draw.RawPicST(btn->x + btn->w - 12, btn->y+btn->h-6, 12, 6, 0, 0.5, 1, 1, shader);

		Syscall.R_SetColor(NULL);

		int textX = btn->x + (btn->w >> 1);
		int textY = btn->y + ((btn->h - GUI_FONTHEIGHT) / 2.f);

		Draw.TextCenter(textX, textY, GUI_FONTSCALE, btn->caption, colorWhite, qtrue, qfalse, GUI_FONT, true);
	}
}

void CClientWindow::DrawClientButtons(void)
{
	vec4_t btnColor;

	for (int i=0 ; i<2 ; i++) {
		for (int j=0 ; j<32 ; j++) {
			clientButton_t *btn = &buttons[i][j];

			if (btn->valid) {
				bool selected = (btn->clientNum == selectedClientNum);

				if (eth32.cg.players[btn->clientNum].playerType == PLAYER_NORMAL) {
					if (btn->type == CLIENT_TEAM)
						Vector4Set(btnColor, eth32.settings.colorTeam[0] / 255.f, eth32.settings.colorTeam[1] / 255.f, eth32.settings.colorTeam[2] / 255.f, 1.0);
					else if (btn->type == CLIENT_ENEMY)
						Vector4Set(btnColor, eth32.settings.colorEnemy[0] / 255.f, eth32.settings.colorEnemy[1] / 255.f, eth32.settings.colorEnemy[2] / 255.f, 1.0);
					else
						Vector4Copy(colorLtGrey, btnColor);
				} else if (eth32.cg.players[btn->clientNum].playerType == PLAYER_FRIEND)
					Vector4Copy(colorGreen, btnColor);
				else if (eth32.cg.players[btn->clientNum].playerType == PLAYER_ENEMY)
					Vector4Copy(colorOrange, btnColor);
				else
					Vector4Copy(colorLtGrey, btnColor);

				Syscall.R_SetColor(btnColor);

				qhandle_t shader = eth32.cg.media.combtnLeft;

				Draw.RawPicST(btn->x, btn->y, 12, 6, 0, 0, 1, 0.5, shader);
				Draw.RawPicST(btn->x, btn->y+6, 12, btn->h-12, 0, .4, 1, 0.6, shader);
				Draw.RawPicST(btn->x, btn->y+btn->h-6, 12, 6, 0, 0.5, 1, 1, shader);

				shader = eth32.cg.media.combtnCenter;

				Draw.RawPicST(btn->x + 12, btn->y, btn->w - 24, 6, 0, 0, 1, 0.5, shader);
				Draw.RawPicST(btn->x + 12, btn->y+6, btn->w - 24, btn->h-12, 0, 0.4, 1, 0.6, shader);
				Draw.RawPicST(btn->x + 12, btn->y+btn->h-6, btn->w - 24, 6, 0, 0.5, 1, 1, shader);

				shader = eth32.cg.media.combtnRight;

				Draw.RawPicST(btn->x + btn->w - 12, btn->y, 12, 6, 0, 0, 1, 0.5, shader);
				Draw.RawPicST(btn->x + btn->w - 12, btn->y+6, 12, btn->h-12, 0, 0.4, 1, 0.6, shader);
				Draw.RawPicST(btn->x + btn->w - 12, btn->y+btn->h-6, 12, 6, 0, 0.5, 1, 1, shader);

				Syscall.R_SetColor(NULL);

				if (selectedClientNum == btn->clientNum) {
					Syscall.R_SetColor(colorMagenta);

					qhandle_t shader = eth32.cg.media.comselLeft;

					Draw.RawPicST(btn->x, btn->y, 12, 6, 0, 0, 1, 0.5, shader);
					Draw.RawPicST(btn->x, btn->y+6, 12, btn->h-12, 0, .4, 1, 0.6, shader);
					Draw.RawPicST(btn->x, btn->y+btn->h-6, 12, 6, 0, 0.5, 1, 1, shader);

					shader = eth32.cg.media.comselCenter;

					Draw.RawPicST(btn->x + 12, btn->y, btn->w - 24, 6, 0, 0, 1, 0.5, shader);
					Draw.RawPicST(btn->x + 12, btn->y+6, btn->w - 24, btn->h-12, 0, 0.4, 1, 0.6, shader);
					Draw.RawPicST(btn->x + 12, btn->y+btn->h-6, btn->w - 24, 6, 0, 0.5, 1, 1, shader);

					shader = eth32.cg.media.comselRight;

					Draw.RawPicST(btn->x + btn->w - 12, btn->y, 12, 6, 0, 0, 1, 0.5, shader);
					Draw.RawPicST(btn->x + btn->w - 12, btn->y+6, 12, btn->h-12, 0, 0.4, 1, 0.6, shader);
					Draw.RawPicST(btn->x + btn->w - 12, btn->y+btn->h-6, 12, 6, 0, 0.5, 1, 1, shader);

					Syscall.R_SetColor(NULL);
				}

				player_t *player = &eth32.cg.players[btn->clientNum];

				int textX = btn->x + (btn->w >> 1);
				int textY = btn->y + ((btn->h - GUI_FONTHEIGHT) / 2.f);

				if (TEXTWIDTH(player->name) > (btn->w - 10)) {
					char tempName[256];
					int cnt = Draw.BreakStringByWidth(player->name, btn->w-10, NULL, GUI_FONT, false);
					strncpy(tempName, player->name, cnt);
					Draw.TextCenter(textX, textY, GUI_FONTSCALE, tempName, colorWhite, qtrue, qfalse, GUI_FONT, true);
				} else {
					Draw.TextCenter(textX, textY, GUI_FONTSCALE, player->name, colorWhite, qtrue, qfalse, GUI_FONT, true);
				}
			}
		}
	}
}

clientButton_t* CClientWindow::FindOpenSpecSlot(void)
{
	for (int i=31 ; i >= 0 ; i--) {
		if (!buttons[0][i].valid)
			return &buttons[0][i];

		if (!buttons[1][i].valid)
			return &buttons[1][i];
	}
	return NULL; // all full?? this should never happen
}

void CClientWindow::DrawClientInfo(void)
{
	if (selectedClientNum < 0 || selectedClientNum >= MAX_CLIENTS)
		return;

	if (!IS_INFOVALID(selectedClientNum)) {
		selectedClientNum = -1;
		return;
	}

	player_t *player = &eth32.cg.players[selectedClientNum];
	int xPos, yPos;
	char buffer[256];

	// draw name
	xPos = x + GUI_SPACING + (leftPaneWidth >> 1);
	yPos = y + GUI_TITLEBAR_SIZE + (GUI_SPACING*4);

	Draw.TextCenter(xPos, yPos, lgFontScale, player->name, GUI_FONTCOLOR1, qfalse, qfalse, GUI_FONT, true);
	yPos += lgFontSize + GUI_SPACING;

	Draw.TextCenter(xPos, yPos, GUI_FONTSCALE, "player name", GUI_FONTCOLOR2, qfalse, qfalse, GUI_FONT, true);
	yPos += GUI_FONTHEIGHT + (GUI_SPACING*5);

	// draw basic stats - line 1 (kills, deaths, k/d ratio)
	xPos = x + GUI_SPACING + (leftPaneWidth / 6);
	// kills
	sprintf(buffer, "%i", player->stats.kills);
	Draw.TextCenter(xPos, yPos, lgFontScale, buffer, GUI_FONTCOLOR1, qtrue, qfalse, GUI_FONT, true);
	Draw.TextCenter(xPos, yPos+lgFontSize+GUI_SPACING, GUI_FONTSCALE, "kills", GUI_FONTCOLOR2, qtrue, qfalse, GUI_FONT, true);

	xPos += leftPaneWidth / 3;
	// deaths
	sprintf(buffer, "%i", player->stats.deaths);
	Draw.TextCenter(xPos, yPos, lgFontScale, buffer, GUI_FONTCOLOR1, qtrue, qfalse, GUI_FONT, true);
	Draw.TextCenter(xPos, yPos+lgFontSize+GUI_SPACING, GUI_FONTSCALE, "deaths", GUI_FONTCOLOR2, qtrue, qfalse, GUI_FONT, true);

	xPos += leftPaneWidth / 3;
	// k/d ratio
	sprintf(buffer, "%.3f", player->stats.kdRatio);
	Draw.TextCenter(xPos, yPos, lgFontScale, buffer, GUI_FONTCOLOR1, qtrue, qfalse, GUI_FONT, true);
	Draw.TextCenter(xPos, yPos+lgFontSize+GUI_SPACING, GUI_FONTSCALE, "k/d ratio", GUI_FONTCOLOR2, qtrue, qfalse, GUI_FONT, true);

	yPos += lgFontSize + GUI_SPACING + GUI_FONTHEIGHT + (GUI_SPACING*4);

	// draw basic stats - line 2 (rnds fired, rnds hit, acc)
	xPos = x + GUI_SPACING + (leftPaneWidth / 6);
	// rnds fired
	sprintf(buffer, "%i", player->stats.bulletsFired);
	Draw.TextCenter(xPos, yPos, lgFontScale, buffer, GUI_FONTCOLOR1, qtrue, qfalse, GUI_FONT, true);
	Draw.TextCenter(xPos, yPos+lgFontSize+GUI_SPACING, GUI_FONTSCALE, "rounds fired", GUI_FONTCOLOR2, qtrue, qfalse, GUI_FONT, true);

	xPos += leftPaneWidth / 3;
	// rnds hit
	sprintf(buffer, "%i", player->stats.bulletsHit);
	Draw.TextCenter(xPos, yPos, lgFontScale, buffer, GUI_FONTCOLOR1, qtrue, qfalse, GUI_FONT, true);
	Draw.TextCenter(xPos, yPos+lgFontSize+GUI_SPACING, GUI_FONTSCALE, "rounds hit", GUI_FONTCOLOR2, qtrue, qfalse, GUI_FONT, true);

	xPos += leftPaneWidth / 3;
	// accuracy
	sprintf(buffer, "%.3f%%", player->stats.accuracy);
	Draw.TextCenter(xPos, yPos, lgFontScale, buffer, GUI_FONTCOLOR1, qtrue, qfalse, GUI_FONT, true);
	Draw.TextCenter(xPos, yPos+lgFontSize+GUI_SPACING, GUI_FONTSCALE, "accuracy", GUI_FONTCOLOR2, qtrue, qfalse, GUI_FONT, true);

	yPos += lgFontSize + GUI_SPACING + GUI_FONTHEIGHT + (GUI_SPACING*4);

	// draw basic stats - line 3 (suicides, current spree, longest spree)
	xPos = x + GUI_SPACING + (leftPaneWidth / 6);
	// suicides
	sprintf(buffer, "%i", player->stats.suicides);
	Draw.TextCenter(xPos, yPos, lgFontScale, buffer, GUI_FONTCOLOR1, qtrue, qfalse, GUI_FONT, true);
	Draw.TextCenter(xPos, yPos+lgFontSize+GUI_SPACING, GUI_FONTSCALE, "suicides", GUI_FONTCOLOR2, qtrue, qfalse, GUI_FONT, true);

	xPos += leftPaneWidth / 3;
	// current spree
	sprintf(buffer, "%i", player->stats.spree);
	Draw.TextCenter(xPos, yPos, lgFontScale, buffer, GUI_FONTCOLOR1, qtrue, qfalse, GUI_FONT, true);
	Draw.TextCenter(xPos, yPos+lgFontSize+GUI_SPACING, GUI_FONTSCALE, "cur spree", GUI_FONTCOLOR2, qtrue, qfalse, GUI_FONT, true);

	xPos += leftPaneWidth / 3;
	// max spree
	sprintf(buffer, "%i", player->stats.longestSpree);
	Draw.TextCenter(xPos, yPos, lgFontScale, buffer, GUI_FONTCOLOR1, qtrue, qfalse, GUI_FONT, true);
	Draw.TextCenter(xPos, yPos+lgFontSize+GUI_SPACING, GUI_FONTSCALE, "max spree", GUI_FONTCOLOR2, qtrue, qfalse, GUI_FONT, true);

	yPos += lgFontSize + GUI_SPACING + GUI_FONTHEIGHT + (GUI_SPACING*4);

	// draw basic stats - line 4 (threat)
	xPos = x + GUI_SPACING + (leftPaneWidth >> 1);
	// threat
	sprintf(buffer, "%.3f", player->stats.threat);
	Draw.TextCenter(xPos, yPos, lgFontScale, buffer, GUI_FONTCOLOR1, qtrue, qfalse, GUI_FONT, true);
	Draw.TextCenter(xPos, yPos+lgFontSize+GUI_SPACING, GUI_FONTSCALE, "threat", GUI_FONTCOLOR2, qtrue, qfalse, GUI_FONT, true);
	
	// draw basic stats - line 4 (Ping)
	xPos += leftPaneWidth / 3;
	// ping
	for (int i=0; i<eth32.cg.numScores; i++) {
		if (eth32.ocg.scores[i].client == selectedClientNum)
			sprintf(buffer, "%i", eth32.ocg.scores[i].ping);
	}
	Draw.TextCenter(xPos, yPos, lgFontScale, buffer, GUI_FONTCOLOR1, qtrue, qfalse, GUI_FONT, true);
	Draw.TextCenter(xPos, yPos+lgFontSize+GUI_SPACING, GUI_FONTSCALE, "ping", GUI_FONTCOLOR2, qtrue, qfalse, GUI_FONT, true);

}
