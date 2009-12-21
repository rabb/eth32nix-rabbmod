// ETH32 - an Enemy Territory cheat for windows
// Copyright (c) 2007 eth32 team
// www.cheatersutopia.com & www.nixcoders.org

#include "../eth32.h"

CEspConfigWindow::CEspConfigWindow(const char *wlabel, int wx, int wy, int ww, int wh)
: CWindow(wlabel, wx, wy, ww, wh)
{
		
	int yPos = (GUI_FONTPIXELHEIGHT * 2) - 10;
	
	pName = new CCheckBox("Player Names", 5, yPos, ww-10, 8, &eth32.settings.espName);
	AddControl(pName);
	yPos += 13;

	if(eth32.cgMod->type != MOD_TCE) 
	{
		AddControl(new CDropbox("Gentity Type", 5, yPos, ww-10, 23, 0, ESP_GENT_MAX-1, &eth32.settings.gentitySetting, gentityEspText));
		yPos += 28;
	}
	else
	{
		AddControl(new CDropbox("Gentity Type", 5, yPos, ww-10, 23, 0, ESP_GENT_TCE_MAX-1, &eth32.settings.gentitySetting, gentityEspTextTCE));
		yPos += 28;
	}
	gent = &eth32.settings.gentInfo[eth32.settings.gentitySetting];

	gCham = new CCheckBox("Chams", 5, yPos, ww-10, 8, &gent->chams);
	AddControl(gCham);
	yPos += 13;

	gText = new CCheckBox("Text", 5, yPos, ww-10, 8, &gent->text);
	AddControl(gText);
	yPos += 13;

	gIcon = new CCheckBox("Icon", 5, yPos, ww-10, 8, &gent->icon);
	AddControl(gIcon);
	yPos += 13;

	gRadarIcon = new CCheckBox("Radar Icon", 5, yPos, ww-10, 8, &gent->radarIcon);
	AddControl(gRadarIcon);
	yPos += 13;

	gDistance = new CIntSlider("Distance", 5, yPos, ww-10, 20, 0, 5000, &gent->distance);
	AddControl(gDistance);
	
}

void CEspConfigWindow::Display(void)
{
	UpdateOffsets();

	
	CWindow::Display();
}

void CEspConfigWindow::UpdateOffsets(void)
{
	gent = &eth32.settings.gentInfo[eth32.settings.gentitySetting];
	gCham->SetTarget(&gent->chams);
	gText->SetTarget(&gent->text);
	gIcon->SetTarget(&gent->icon);
	gRadarIcon->SetTarget(&gent->radarIcon);
	gDistance->SetTarget(&gent->distance);
}
