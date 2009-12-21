// ETH32 - an Enemy Territory cheat for windows
// Copyright (c) 2007 eth32 team
// www.cheatersutopia.com & www.nixcoders.org

#pragma once

#include "CWindow.h"
#include "CSliders.h"
#include "CCheckbox.h"
#include "CButtons.h"
#include "CAimbot.h"
#include "../types.h"	// need weapon structs

class CWeaponConfigWindow : public CWindow
{
public:
	CWeaponConfigWindow(const char *wlabel, int wx, int wy, int ww, int wh);
	void Display(void);
	int ProcessMouse(int mx, int my, uint32 mevent, CControl **mhook, CControl **khook);
private:
	void SetWeaponFocus(void);

	//CIntSlider *headTraces, *bodyTraces, *range;
	//CFloatSlider *adjustx, *adjusty, *adjustz;
	CCheckBox *autofire;
	CIntSlider *delay;
	//CCheckBox *autoPoints;

	CButton *setCurrent;
};

// Helper Functions
void funcApplyToType(void);
void funcPrevWeapon(void);
void funcNextWeapon(void);
void funcViewCurrent(void);

typedef enum
{
	HB_STAND,
	HB_RUN,
	HB_CROUCH,
	HB_CRAWL,
	HB_PRONE,
	HB_MAX
} hitboxOffset_t;

extern const char *hbOffsetText[];

class CHitboxWindow : public CWindow
{
public:
	CHitboxWindow(const char *wlabel, int wx, int wy, int ww, int wh);
	void Display(void);
private:
	void AssignHitboxPart(void);
	hitbox_t *hitbox;
	//CFloatSlider *xOff, *yOff, *zOff;
	CFloatSlider *xOff, *xmmOff, *xffOff, *yOff, *zOff, *zmOff, *zfOff;
	int curPart;
};

class CWeaponWindow : public CWindow
{
public:
	CWeaponWindow(const char *wlabel, int wx, int wy, int ww, int wh);
	void Display(void);
private:
	bool GetCurrentAmmo(void);
	int ammo, clip, akimbo;
	bool dataValid;
	float ammoFontScale;
	int chargeBarSize;
};
