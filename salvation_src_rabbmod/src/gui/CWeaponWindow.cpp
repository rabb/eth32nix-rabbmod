// ETH32 - an Enemy Territory cheat for windows
// Copyright (c) 2007 eth32 team
// www.cheatersutopia.com & www.nixcoders.org

#include "../eth32.h"

/*********************
* CWeaponConfigWindow
*********************/

//#define VIRT_WEAP_SIZE		100
//#define REAL_WEAP_SIZE		(VIRT_WEAP_SIZE * eth32.cg.screenXScale)

CWeaponConfigWindow::CWeaponConfigWindow(const char *wlabel, int wx, int wy, int ww, int wh)
: CWindow(wlabel, wx, wy, ww, wh)
{
	CControl *ctrl;

	int yPos = (GUI_FONTPIXELHEIGHT * 2);

	/*autoPoints = new CCheckBox("Auto Aimpoints", 5, yPos, ww-10, 8, NULL);
	AddControl((CControl*)autoPoints);
	yPos += 28;*/

	/*headTraces = new CIntSlider("Head Points", 5, yPos, ww-10, 20, 0, 100, NULL);
	AddControl((CControl*)headTraces);
	yPos += 25;

	bodyTraces = new CIntSlider("Body Points", 5, yPos, ww-10, 20, 0, 100, NULL);
	AddControl((CControl*)bodyTraces);
	yPos += 25;

	range = new CIntSlider("Range", 5, yPos, ww-10, 20, 0, 10000, NULL);
	AddControl((CControl*)range);
	yPos += 25;*/

	delay = new CIntSlider("Delay", 5, yPos, ww-10, 20, 0, 1000, NULL);
	AddControl((CControl*)delay);
	yPos += 25;

	autofire = new CCheckBox("Autofire", 5, yPos, ww-10, 8, NULL);
	AddControl((CControl*)autofire);
	yPos += 13;

	ctrl = new CButton("Apply to all similar type", 5, yPos, ww-10, 12, funcApplyToType);
	AddControl(ctrl);
	yPos += 17;

	ctrl = new CButton("<<", 5, yPos, (ww-20)/3 , 12, funcPrevWeapon);
	AddControl(ctrl);
	setCurrent = new CButton("Current", (ww-20)/3+10, yPos, (ww-20)/3, 12, funcViewCurrent);
	AddControl((CControl*)setCurrent);
	ctrl = new CButton(">>", 2*(ww-20)/3+15, yPos, (ww-20)/3, 12, funcNextWeapon);
	AddControl(ctrl);
	yPos += 17;

	Resize(w, GUI_TITLEBAR_SIZE + (yPos * eth32.cg.screenYScale));
}

void CWeaponConfigWindow::Display(void)
{
	if (!eth32.cg.weaponList || !show)	// no valid weaponlist, so nothing to modify
		return;

	if (!eth32.cg.modifyWeapon) {	// find our weapon to modify if not set yet
		if (eth32.cg.snap->ps.weapon) {
			eth32.cg.modifyWeapNum = eth32.cg.snap->ps.weapon;
			eth32.cg.modifyWeapon = &eth32.cg.weaponList[eth32.cg.snap->ps.weapon];
			SetWeaponFocus();
		}
		else {
			int findweap = 0;
			// search for user defined weapon
			while (!IS_WEAPATTRIB(findweap, WA_USER_DEFINED) && findweap < eth32.cg.numWeapons)
				findweap++;
			// make sure we are still in weaponlist
			if (findweap < eth32.cg.numWeapons) {
				eth32.cg.modifyWeapNum = findweap;
				eth32.cg.modifyWeapon = &eth32.cg.weaponList[findweap];
				SetWeaponFocus();
			}
			else return;	// no user defined weapons found
		}
	}
	SetWeaponFocus();

	// Only show current button if current weapon is different than showing and current weapon is user defined
	if (eth32.cg.snap->ps.weapon && eth32.cg.modifyWeapNum != eth32.cg.snap->ps.weapon && IS_WEAPATTRIB(eth32.cg.snap->ps.weapon, WA_USER_DEFINED))
		setCurrent->Show(true);
	else setCurrent->Show(false);

	CWindow::Display();

	weapdef_t *weap = eth32.cg.modifyWeapon;

	Draw.TextCenter(x+(w>>1), y+GUI_TITLEBAR_SIZE+GUI_SPACING, GUI_FONTSCALE, weap->name, GUI_FONTCOLOR1, qtrue, qfalse, GUI_FONT, true);
}

int CWeaponConfigWindow::ProcessMouse(int mx, int my, uint32 mevent, CControl **mhook, CControl **khook)
{
	int ret = CWindow::ProcessMouse(mx, my, mevent, mhook, khook);

	if (ret == 0) // a control or window handled event
		SetWeaponFocus();	// update controls, in case our mod weap has changed

	return ret;
}

void CWeaponConfigWindow::SetWeaponFocus(void)
{
	weapdef_t *weap = eth32.cg.modifyWeapon;

	if (weap) {
		// If modify weapon valid, set all our controls to point to that weapon definition
		/*headTraces->SetTarget(&weap->headTraces);
		bodyTraces->SetTarget(&weap->bodyTraces);
		range->SetTarget(&weap->range);*/
		delay->SetTarget(&weap->delay);
		autofire->SetTarget(&weap->autofire);
	}
}

// Helper Functions

void funcApplyToType(void)
{
	weapdef_t *weap = eth32.cg.modifyWeapon;
	weapdef_t *modweap = eth32.cg.weaponList;

	if (weap && modweap) { // sanity check
		for (int i=0 ; i<eth32.cg.numWeapons ; i++)
		{
			if (IS_WEAPATTRIB(i, WA_USER_DEFINED) && modweap[i].type == weap->type && weap != &modweap[i]) {
				//modweap[i].headTraces = weap->headTraces;
				//modweap[i].bodyTraces = weap->bodyTraces;
				//modweap[i].range = weap->range;
				modweap[i].delay = weap->delay;
				modweap[i].autofire = weap->autofire;
			}
		}
	}
}

void funcPrevWeapon(void)
{
	weapdef_t *weap = eth32.cg.modifyWeapon;
	int weapnum = eth32.cg.modifyWeapNum - 1;

	if (!weap)
		return;

	if (weapnum < 0) // this shouldn't happen, but to be safe
		weapnum = eth32.cg.numWeapons;

	while (!IS_WEAPATTRIB(weapnum, WA_USER_DEFINED))
	{
		weapnum--;
		if (weapnum < 0)
			weapnum = eth32.cg.numWeapons;

		// We went through the whole list and didn't find a valid weapon def
		if (weap == &eth32.cg.weaponList[weapnum])
			return;
	}
	eth32.cg.modifyWeapNum = weapnum;
	eth32.cg.modifyWeapon = &eth32.cg.weaponList[weapnum];
}

void funcNextWeapon(void)
{
	weapdef_t *weap = eth32.cg.modifyWeapon;
	int weapnum = eth32.cg.modifyWeapNum + 1;

	if (!weap)
		return;

	if (weapnum > eth32.cg.numWeapons) // current modify weapon is last in list
		weapnum = 0;

	while (!IS_WEAPATTRIB(weapnum, WA_USER_DEFINED))
	{
		weapnum++;
		if (weapnum > eth32.cg.numWeapons)
			weapnum = 0;

		// We went through the whole list and didn't find a valid weapon def
		if (weap == &eth32.cg.weaponList[weapnum])
			return;
	}
	eth32.cg.modifyWeapNum = weapnum;
	eth32.cg.modifyWeapon = &eth32.cg.weaponList[weapnum];
}

void funcViewCurrent(void)
{
	if (eth32.cg.snap->ps.weapon && IS_WEAPATTRIB(eth32.cg.snap->ps.weapon, WA_USER_DEFINED)) {
		eth32.cg.modifyWeapNum = eth32.cg.snap->ps.weapon;
		eth32.cg.modifyWeapon = &eth32.cg.weaponList[eth32.cg.snap->ps.weapon];
	}
}

/*********************
* CHitboxWindow
*********************/

const char *hbOffsetText[] = {
	"Stand",
	"Run",
	"Crouch",
	"Crawl",
	"Prone",
};

CHitboxWindow::CHitboxWindow(const char *wlabel, int wx, int wy, int ww, int wh)
: CWindow(wlabel, wx, wy, ww, wh)
{
	curPart = HB_STAND;

	AddControl(new CDropbox("Offset", 5, 5, ww-10, 23, 0, HB_MAX-1, &curPart, hbOffsetText));

	xOff = new CFloatSlider("X Low",   5, 33, ww-10, 20, -10, 10, &eth32.settings.standlowX);
	xmmOff = new CFloatSlider("X Med", 5, 58, ww-10, 20, -10, 10, &eth32.settings.standmedX);
	xffOff = new CFloatSlider("X Far", 5, 88, ww-10, 20, -10, 10, &eth32.settings.standfarX);
	yOff = new CFloatSlider("Y",       5, 113, ww-10, 20, -10, 10, &eth32.settings.standY);
	zOff = new CFloatSlider("Z Low",   5, 143, ww-10, 20, -10, 10, &eth32.settings.standlowZ);
	zmOff = new CFloatSlider("Z Med",  5, 168, ww-10, 20, -10, 10, &eth32.settings.standmedZ);
	zfOff = new CFloatSlider("Z Far",  5, 193, ww-10, 20, -10, 10, &eth32.settings.standfarZ);

	AddControl(xOff);
	AddControl(xmmOff);
	AddControl(xffOff);
	AddControl(yOff);
	AddControl(zOff);
	AddControl(zmOff);
	AddControl(zfOff);
}

void CHitboxWindow::Display(void)
{
	if (eth32.settings.hitboxType != HITBOX_CUSTOM)
		return;

	AssignHitboxPart();

	CWindow::Display();
}

/*void CHitboxWindow::AssignHitboxPart(void)
{
	float *curOffset;

	if (curPart == HB_STAND)
		curOffset = eth32.settings.vecStand;
	else if (curPart == HB_RUN)
		curOffset = eth32.settings.vecRun;
	else if (curPart == HB_CROUCH)
		curOffset = eth32.settings.vecCrouch;
	else if (curPart == HB_CRAWL)
		curOffset = eth32.settings.vecCrawl;
	else if (curPart == HB_PRONE)
		curOffset = eth32.settings.vecProne;
	else // shouldn't ever get here
		curOffset = eth32.settings.vecStand;

	xOff->SetTarget(&curOffset[0]);
	yOff->SetTarget(&curOffset[1]);
	zOff->SetTarget(&curOffset[2]);
}*/

void CHitboxWindow::AssignHitboxPart(void)
{
	float *curOffset1, *curOffset2, *curOffset3, *curOffset4, *curOffset5, *curOffset6, *curOffset7;

	switch(curPart) {
		case HB_STAND: {
			curOffset1 = &eth32.settings.standlowX;
			curOffset2 = &eth32.settings.standmedX;
			curOffset3 = &eth32.settings.standfarX;
			curOffset4 = &eth32.settings.standY;
			curOffset5 = &eth32.settings.standlowZ;
			curOffset6 = &eth32.settings.standmedZ;
			curOffset7 = &eth32.settings.standfarZ;
			break;
		}
		case HB_RUN: {
			curOffset1 = &eth32.settings.runlowX;
			curOffset2 = &eth32.settings.runmedX;
			curOffset3 = &eth32.settings.runfarX;
			curOffset4 = &eth32.settings.runY;
			curOffset5 = &eth32.settings.runlowZ;
			curOffset6 = &eth32.settings.runmedZ;
			curOffset7 = &eth32.settings.runfarZ;
			break;
		}
		case HB_CROUCH: {
			curOffset1 = &eth32.settings.crouchlowX;
			curOffset2 = &eth32.settings.crouchmedX;
			curOffset3 = &eth32.settings.crouchfarX;
			curOffset4 = &eth32.settings.crouchY;
			curOffset5 = &eth32.settings.crouchlowZ;
			curOffset6 = &eth32.settings.crouchmedZ;
			curOffset7 = &eth32.settings.crouchfarZ;
			break;
		}
		case HB_CRAWL: {
			curOffset1 = &eth32.settings.crawllowX;
			curOffset2 = &eth32.settings.crawlmedX;
			curOffset3 = &eth32.settings.crawlfarX;
			curOffset4 = &eth32.settings.crawlY;
			curOffset5 = &eth32.settings.crawllowZ;
			curOffset6 = &eth32.settings.crawlmedZ;
			curOffset7 = &eth32.settings.crawlfarZ;
			break;
		}
		case HB_PRONE: {
			curOffset1 = &eth32.settings.pronelowX;
			curOffset2 = &eth32.settings.pronemedX;
			curOffset3 = &eth32.settings.pronefarX;
			curOffset4 = &eth32.settings.proneY;
			curOffset5 = &eth32.settings.pronelowZ;
			curOffset6 = &eth32.settings.pronemedZ;
			curOffset7 = &eth32.settings.pronefarZ;
			break;
		}
	}

	xOff->SetTarget(curOffset1);
	xmmOff->SetTarget(curOffset2);
	xffOff->SetTarget(curOffset3);
	yOff->SetTarget(curOffset4);
	zOff->SetTarget(curOffset5);
	zmOff->SetTarget(curOffset6);
	zfOff->SetTarget(curOffset7);
}

/*********************
* CWeaponWindow
*********************/

CWeaponWindow::CWeaponWindow(const char *wlabel, int wx, int wy, int ww, int wh)
: CWindow(wlabel, wx, wy, ww, wh)
{
	showTitle = false;
	chargeBarSize = 10 * eth32.cg.screenYScale;

	int fontSize = h - GUI_FONTHEIGHT - chargeBarSize - (4 * GUI_SPACING);
	ammoFontScale = 2.f;

	while (Draw.sizeTextHeight(ammoFontScale, GUI_FONT) > fontSize)
		ammoFontScale -= 0.001;
}

void CWeaponWindow::Display(void)
{
	if (!eth32.cg.snap || !eth32.cg.currentWeapon || !eth32.cg.snap->ps.weapon || eth32.settings.guiOriginal)
		return;

	CWindow::Display();

	int drawX = x + GUI_SPACING;
	int drawY = y + GUI_SPACING;

	weapdef_t *weap = &eth32.cg.weaponList[eth32.cg.snap->ps.weapon];

#ifdef ETH32_DEBUG
	char buf[64];
	sprintf(buf, "%s", weap->name/*, eth32.cg.snap->ps.weapon*/);
	Draw.TextCenter(/*x + ((w-h) >> 1)*/x+(w/2), drawY, GUI_FONTSCALE, buf, GUI_FONTCOLOR2, qtrue, qfalse, GUI_FONT, true);
#else
	Draw.TextCenter(/*x + ((w-h) >> 1)*/x+(w/2), drawY, GUI_FONTSCALE, weap->name, GUI_FONTCOLOR2, qtrue, qfalse, GUI_FONT, true);
#endif

	// get current charge time (ripped and modified from SDK)
	int chargeTime;
	if( eth32.cg.snap->ps.stats[ STAT_PLAYER_CLASS ] == PC_ENGINEER ) {
		chargeTime = eth32.cg.chargeTimes.engineer[eth32.cg.snap->ps.persistant[PERS_TEAM]-1];
	} else if( eth32.cg.snap->ps.stats[ STAT_PLAYER_CLASS ] == PC_MEDIC ) {
		chargeTime = eth32.cg.chargeTimes.medic[eth32.cg.snap->ps.persistant[PERS_TEAM]-1];
	} else if( eth32.cg.snap->ps.stats[ STAT_PLAYER_CLASS ] == PC_FIELDOPS ) {
		chargeTime = eth32.cg.chargeTimes.lt[eth32.cg.snap->ps.persistant[PERS_TEAM]-1];
	} else if( eth32.cg.snap->ps.stats[ STAT_PLAYER_CLASS ] == PC_COVERTOPS ) {
		chargeTime = eth32.cg.chargeTimes.covops[eth32.cg.snap->ps.persistant[PERS_TEAM]-1];
	} else {
		chargeTime = eth32.cg.chargeTimes.soldier[eth32.cg.snap->ps.persistant[PERS_TEAM]-1];
	}

	float frac;
	if (chargeTime)
		frac = (float)(eth32.cg.time - eth32.cg.snap->ps.classWeaponTime) / chargeTime;
	else
		frac = 1.f;

	// Used 2000 as an average reload time for all weapons, not the best but does the job.
	float weapTime = (float)eth32.cg.snap->ps.weaponTime/2000;

	drawY += GUI_FONTHEIGHT + GUI_SPACING;
	// draw weapon charge
	Draw.Statbar(drawX, drawY, w-GUI_SPACING-GUI_SPACING, chargeBarSize, frac, eth32.settings.gui_charge);	// this will eventually show weapon charge
	// draw weapon reload
	if (eth32.cg.snap->ps.weaponstate == WEAPON_RELOADING)
		Draw.Statbar(drawX, drawY, w-GUI_SPACING-GUI_SPACING, chargeBarSize, weapTime, eth32.settings.gui_reload);
	// draw weapon heat
	if (eth32.cg.snap->ps.curWeapHeat)
		Draw.Statbar2(drawX, drawY, w-GUI_SPACING-GUI_SPACING, chargeBarSize, (float)eth32.cg.snap->ps.curWeapHeat/255.f, eth32.settings.gui_overheat);

	drawY += chargeBarSize + GUI_SPACING;

	if (GetCurrentAmmo()) {
		char ammobuf[64];
		if (akimbo >= 0)
			sprintf(ammobuf, "%i|%i/%i", akimbo, clip, ammo);
		else if (ammo >= 0)
			sprintf(ammobuf, "%i/%i", clip, ammo);
		else
			sprintf(ammobuf, "%i", clip);

		Draw.TextCenter(x+(w/2), drawY, ammoFontScale, ammobuf, GUI_FONTCOLOR2, qtrue, qfalse, GUI_FONT, true);
	}
}

bool CWeaponWindow::GetCurrentAmmo(void)
{
	ammo = clip = akimbo = -1;

	if (!orig_BG_FindClipForWeapon || !orig_BG_FindAmmoForWeapon)
		return false;

	if (!eth32.cg.snap || !eth32.cg.snap->ps.weapon)
		return false;

	if (eth32.cg.snap->ps.eFlags & EF_MG42_ACTIVE || eth32.cg.snap->ps.eFlags & EF_MOUNTEDTANK)
		return false;

	int weap = eth32.cg.snap->ps.weapon;

	if (IS_WEAPATTRIB(weap, WA_NO_AMMO))
		return false;

	clip = eth32.cg.snap->ps.ammoclip[orig_BG_FindClipForWeapon(weap)];

	if (IS_WEAPATTRIB(weap, WA_ONLY_CLIP))
		return true;

	ammo = eth32.cg.snap->ps.ammo[orig_BG_FindAmmoForWeapon(weap)];

	if (IS_WEAPATTRIB(weap, WA_AKIMBO))
		akimbo = eth32.cg.snap->ps.ammoclip[orig_BG_FindClipForWeapon(eth32.cg.weaponList[weap].otherAkimbo)];

	return true;
}
