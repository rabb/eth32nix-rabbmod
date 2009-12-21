// ETH32 - an Enemy Territory cheat for windows
// Copyright (c) 2007 eth32 team
// www.cheatersutopia.com & www.nixcoders.org

#include "eth32.h"
#include "guidefs.h"

CGui Gui;

CGui::CGui(void)
{
	currentView = views;				// init current view to our view array (GUI_MAINVIEW)

	memset(views, 0, sizeof(views));	// clear our views

	mouseFocus = keybdFocus = NULL;		// clear mouse & keybd focus
	inputActive = false;				// set gui to no input
	init = false;
	wndprocModified = false;

	loadStyleThisFrame = false;

	memset(styleNames, 0, sizeof(styleNames));
	numStyles = 0;
	selectedStyle = currentStyle = 0;
}

CGui::~CGui(void)
{
	DestroyAllWindows();
}

void CGui::Init(void)
{
	this->mutex_cp = new pthread_mutex_t;
	pthread_mutex_init(mutex_cp,NULL);

	this->mutex_cc = new pthread_mutex_t;
	pthread_mutex_init(mutex_cc,NULL);

	// set font scale
	eth32.guiAssets.fontScale = 2.0;
	eth32.guiAssets.fontHeight = GUI_FONTPIXELHEIGHT * (eth32.game.glconfig->vidWidth / 640.f);
	while (Draw.sizeTextHeight(eth32.guiAssets.fontScale, GUI_FONT) > eth32.guiAssets.fontHeight)
		eth32.guiAssets.fontScale -= 0.0005;

	if (init) {
		LoadStyle();  // reload style specific assets, rest should be fine
		return;
	}

	int numWindows = sizeof(windows) / sizeof(windows[0]);

	const windef_t *wd;
	const ctrldef_t *cd;

	CWindow *win;

#ifdef ETH32_DEBUG
	Debug.Log("CGui.Init() - %i windows defined.", numWindows);
#endif

	for (int i=0 ; i < numWindows ; i++) {
		wd = &windows[i];

#ifdef ETH32_DEBUG
		Debug.Log("  Creating \"%s\" window...", wd->title);
#endif

		// create our window and add our window
		win = CreateWin(wd->title, wd->x, wd->y, wd->w, wd->h, wd->type, wd->view);

		for (int j=0 ; j < wd->numCtrls ; j++) {
			cd = &wd->ctrls[j];
			CreateCtrl(win, cd);
		}
	}

			//View			Toggle		Mouse		Close		Open Func
	SetViewConfig(	GUI_MAINVIEW,		0,		XK_F11,		XK_Escape,	NULL	);
	SetViewConfig(	GUI_MENUVIEW,		XK_F9,		0,		XK_Escape,	NULL	);
	SetViewConfig(	GUI_CLIENTVIEW,		XK_F10,		0,		XK_Escape,	NULL	);

	winRadar = (CRadarWindow*)GetWinByTitle("Radar");

	InitStyles();

	CWindowPicker *winPicker = new CWindowPicker("navbar", 5, 5, 630, 26);
	/** AIMBOT **/
	winPicker->AddWindow("Aimbot", CAT_AIMBOT);
	winPicker->AddWindow("Grenade Bot", CAT_AIMBOT);
	winPicker->AddWindow("Human Aim", CAT_AIMBOT);
	winPicker->AddWindow("Weapon Config", CAT_AIMBOT);
	/** AIMBOT EXTRA **/
	winPicker->AddWindow("Corrections", CAT_AIMBOT2);
	winPicker->AddWindow("Predictions", CAT_AIMBOT2);
	winPicker->AddWindow("Hitbox", CAT_AIMBOT2);
	/** VISUALS **/
	winPicker->AddWindow("Visuals", CAT_VISUAL);
	winPicker->AddWindow("Hitbox Display", CAT_VISUAL);
	winPicker->AddWindow("ESP", CAT_VISUAL);
	winPicker->AddWindow("Camera Settings", CAT_VISUAL);
	/** VISUALS EXTRA **/
	winPicker->AddWindow("Chams", CAT_VISUAL2);
	winPicker->AddWindow("Custom Models", CAT_VISUAL2);
	winPicker->AddWindow("Crosshair", CAT_VISUAL2);
	winPicker->AddWindow("GUI Style", CAT_VISUAL2);
	/** COLOR PICKER**/
	winPicker->AddWindow("Colors", CAT_COLOR);
	/** MISC **/
	winPicker->AddWindow("Misc", CAT_MISC);
	winPicker->AddWindow("Sound", CAT_MISC);
	winPicker->AddWindow("Name Stealer", CAT_MISC);
	winPicker->AddWindow("Spam", CAT_MISC);
	winPicker->ClearAll();
	AddWindow((CWindow*)winPicker, &views[GUI_MENUVIEW]);

	CColorPicker *colorPicker = (CColorPicker*)GetWinByTitle("Colors")->GetControlByLabel("Picker");
	colorPicker->AddColor("Team", eth32.settings.colorTeam);
	colorPicker->AddColor("Team Outline", eth32.settings.colorTeamOut);
	colorPicker->AddColor("Team Hidden", eth32.settings.colorTeamHidden);
	colorPicker->AddColor("Team Weapons", eth32.settings.colorTeamWeapon);
	colorPicker->AddColor("Team Weapons Outline", eth32.settings.TeamWeaponOutline);
	colorPicker->AddColor("Team Class ESP", eth32.settings.clsTeam);
	colorPicker->AddColor("Enemy", eth32.settings.colorEnemy);
	colorPicker->AddColor("Enemy Outline", eth32.settings.colorEnemyOut);
	colorPicker->AddColor("Enemy Hidden", eth32.settings.colorEnemyHidden);
	colorPicker->AddColor("Enemy Weapons", eth32.settings.colorEnemyWeapon);
	colorPicker->AddColor("Enemy Weapons Outline", eth32.settings.EnemyWeaponOutline);
	colorPicker->AddColor("Enemy Class ESP", eth32.settings.clsEnemy);
	colorPicker->AddColor("Invulnerable", eth32.settings.colorInvulnerable);
	colorPicker->AddColor("Health", eth32.settings.colorHealth);
	colorPicker->AddColor("Health Outline", eth32.settings.colorHealthOutline);
	colorPicker->AddColor("Ammo", eth32.settings.colorAmmo);
	colorPicker->AddColor("Ammo Outline", eth32.settings.colorAmmoOutline);
	colorPicker->AddColor("World Weapons", eth32.settings.colorWorldWeapons);
	colorPicker->AddColor("World Weapons Outline", eth32.settings.colorWorldWeaponsOutline);
	colorPicker->AddColor("Missiles ESP", eth32.settings.missileEsp);
	colorPicker->AddColor("Crosshair", eth32.settings.xhairColor);
	colorPicker->AddColor("Head Hitbox", eth32.settings.colorHeadHitbox);
	colorPicker->AddColor("Body Hitbox", eth32.settings.colorBodyHitbox);
	colorPicker->AddColor("Bullet Rail", eth32.settings.colorBulletRail);
	colorPicker->AddColor("X-Axis", eth32.settings.colorXAxis);
	colorPicker->AddColor("Y-Axis", eth32.settings.colorYAxis);
	colorPicker->AddColor("Z-Axis", eth32.settings.colorZAxis);
	colorPicker->AddColor("Gui Title Color", eth32.guiAssets.titleColor);
	colorPicker->AddColor("Gui Text Color 1", eth32.guiAssets.textColor1);
	colorPicker->AddColor("Gui Text Color 2", eth32.guiAssets.textColor2);
	colorPicker->AddColor("Health Bar High", eth32.settings.gui_healthHi);
	colorPicker->AddColor("Health Bar Med", eth32.settings.gui_healthMd);
	colorPicker->AddColor("Health Bar Low", eth32.settings.gui_healthLo);
	colorPicker->AddColor("Stamina Bar", eth32.settings.gui_stamina);
	colorPicker->AddColor("Charge Bar", eth32.settings.gui_charge);
	colorPicker->AddColor("Overheat Bar", eth32.settings.gui_overheat);
	colorPicker->AddColor("Reload Bar", eth32.settings.gui_reload);

	char settingsFile[MAX_PATH];
	sprintf(settingsFile, "%s/%s", eth32.path, ETH32_SETTINGS);

	init = true;
}

void CGui::Shutdown(void)
{
	delete this->mutex_cp;
	delete this->mutex_cc;

	//DestroyAllWindows();
	if (mouseFocus)
		mouseFocus->SetMouseFocus(false);
	if (keybdFocus)
		keybdFocus->SetKeybdFocus(false);

	mouseFocus = keybdFocus = NULL;
	inputActive = false;
}

bool CGui::InputActive(void)
{
	return (inputActive || keybdFocus);
}

void CGui::PreFrame(void)
{
	UpdateViewState();

	// default the limbopanel to open, will use one of the 2D funcs we hooked to flag it as off
	limbopanel = true;

	if (inputActive)
		orig_syscall(CG_KEY_SETCATCHER, KEYCATCH_UI | orig_syscall(CG_KEY_GETCATCHER));

	if (loadStyleThisFrame)
		LoadStyle();
}

void CGui::PostFrame(void)
{
	// if any of these are up or if it is intermission, we don't want to draw UI
	// NOTE: removed limbopanel check for now, it is interfering too much...need a different filter
	if (scoreboard || limbopanel || statspanel || topshots )
		return;

	if (eth32.cg.snap->ps.pm_type == PM_INTERMISSION) {
		// small hack to restore text display during intermission
		if (eth32.cg.cg_teamchatheight)
			eth32.cg.cg_teamchatheight->integer = 8;
		SetView(GUI_MAINVIEW);
		return;
	}

	//DrawCenterPrint();

	CWindow *win = currentView->tail;

	// draw our current view
	while (win) {
		win->Display();
		win = win->prev;	// draw in reverse order, head should be drawn last and on top of all others
	}
}

void CGui::CheckClose(int vkey)
{
	if (vkey == XK_Escape && keybdFocus && keybdFocus->CaptureEscapeCharacter())
		return;

	if (currentView->vkeyClose == vkey) {  // keybdFocus check to avoid closing view if a control has focus
		currentView = &views[GUI_MAINVIEW];
		SetMouseFocus(NULL);
		SetKeybdFocus(NULL);
		inputActive = false;
		Engine.MenuCloseActions();
	}
}

void CGui::UpdateViewState(void)
{
	if (GetAsyncKeyState(currentView->vkeyMouseEnable) & 1) {	// view input key is down
		inputActive = !inputActive;
		return;
	}
	for (int i=0 ; i < GUI_MAXVIEWS ; i++)
	{
		if (GetAsyncKeyState(views[i].vkeyToggle) & 1) {
			SetMouseFocus(NULL);
			SetKeybdFocus(NULL);
			if (currentView == &views[i]) {				// toggle view was already active, user wants to close it
				SetView(GUI_MAINVIEW);
				GetAsyncKeyState(currentView->vkeyMouseEnable);  // clear out the key if it was pressed while other view was open
				inputActive = false;
				return;
			}
			else { // toggle view was not active, set current to it
				SetView((viewtype_t)i);
				//currentView = &views[i];
				inputActive = true;
				if (currentView->openfunc)
					currentView->openfunc();
				return;
			}
		}
	}

}

void CGui::MouseEvent(uint32 mevent)
{
	if (mouseFocus) {
		mouseFocus->ProcessMouse(mouseX, mouseY, mevent, &mouseFocus, &keybdFocus);
		// to be safe... any time left button goes up, we should release mouse focus
		if (mevent == WM_LBUTTONUP)
			mouseFocus = NULL;
		return;
	}

	CWindow *win = currentView->head;
	while (win && win->ProcessMouse(mouseX, mouseY, mevent, &mouseFocus, &keybdFocus)) {
		win = win->next;
	}
	if (win && (win != currentView->head)) {	// if a window processed event and it is not on top of the view
		win->prev->next = win->next;
		if (win->next)
			win->next->prev = win->prev;
		else currentView->tail = win->prev;
		// win is now detached, add it to the appropriate list at front
		AddWindow(win, currentView);

		// update current list to new list head
		currentView->head = win;
	}
}

void CGui::MouseMove(void)
{
	if (mouseFocus)
		mouseFocus->MouseMove(mouseX, mouseY);
}

void CGui::KeybdEvent(unsigned int key)
{
	if (keybdFocus)
		keybdFocus->ProcessKeybd(key, &keybdFocus);
}

void CGui::SetMouseFocus(CControl *ctrl)
{
	if (mouseFocus)
		mouseFocus->SetMouseFocus(false);

	mouseFocus = ctrl;

	if (mouseFocus)
		mouseFocus->SetMouseFocus(true);
}

void CGui::SetKeybdFocus(CControl *ctrl)
{
	if (keybdFocus)
		keybdFocus->SetKeybdFocus(false);

	keybdFocus = ctrl;

	if (keybdFocus)
		keybdFocus->SetKeybdFocus(true);
}

CWindow *CGui::CreateWin(const char *title, int x, int y, int w, int h, wintype_t wtype, viewtype_t vtype)
{
	if (vtype >= GUI_MAXVIEWS)
		return NULL;

	CWindow *win = NULL;

	// added wintypes so all windows could be defined in guidefs.h instead
	// of some custom creation instead of CGui::Init()
	switch (wtype)
	{
	case WIN_STANDARD:
		win = new CWindow(title, x, y, w, h);
		break;
	case WIN_STATUS:
		win = new CStatusWindow(title, x, y, w, h);
		break;
	case WIN_WEAPCONFIG:
		win = new CWeaponConfigWindow(title, x, y, w, h);
		break;
	case WIN_WEAPON:
		win = new CWeaponWindow(title, x, y, w, h);
		break;
	case WIN_CLIENTS:
		win = new CClientWindow(title, x, y, w, h);
		break;
	case WIN_RESPAWN:
		win = new CRespawnWindow(title, x, w, w, h);
		break;
	case WIN_RADAR:
		win = new CRadarWindow(title, x, y, w, h);
		break;
	case WIN_HITBOX:
		win = new CHitboxWindow(title, x, y, w, h);
		break;
	case WIN_CAMERA:
		win = new CCameraWindow(title, x, y, w, h);
		break;
	case WIN_CAMCONFIG:
		win = new CCameraConfigWindow(title, x, y, w, h);
		break;
	case WIN_ESPCONFIG:
		win = new CEspConfigWindow(title, x, y, w, h);
		break;
	case WIN_SPECTATOR:
		win = new CSpecWindow(title, x, y, w, h);
		break;
	case WIN_BANNER:
		win = new CBanner(title, x, y, w, h);
		break;
	default:
		break;
	}

	if (win)
		AddWindow(win, &views[vtype]);

	return win;
}

CWindow *CGui::GetWinByTitle(const char *title)
{
	CWindow *win = NULL;
	char buf[64];
	for (int i=0 ; i < GUI_MAXVIEWS ; i++)
	{
		win = views[i].head;
		while (win) {
			win->GetLabel(buf,sizeof(buf));
			if (strcmp(buf, title) == 0)
				return win;
			win = win->next;
		}
	}
	return NULL;
}

CControl *CGui::CreateCtrl(CWindow *win, const ctrldef_t *cd)
{
	CControl *ctrl = NULL;

	if (cd->type == CTRL_MAX) return NULL;
	if (win == NULL) return NULL;

	switch(cd->type)
	{
	case CTRL_CHECKBOX:
		ctrl = new CCheckBox(cd->label, cd->x, cd->y, cd->w, cd->h, (bool*)cd->arg0);
		break;
	case CTRL_INTDISPLAY:
		ctrl = new CIntDisplay(cd->label, cd->x, cd->y, cd->w, cd->h, (int*)cd->arg0);
		break;
	case CTRL_FLOATDISPLAY:
		ctrl = new CFloatDisplay(cd->label, cd->x, cd->y, cd->w, cd->h, (float*)cd->arg0);
		break;
	case CTRL_INTSLIDER:
		ctrl = new CIntSlider(cd->label, cd->x, cd->y, cd->w, cd->h, cd->arg0, cd->arg1, (int*)cd->arg2, (const char**)cd->arg3);
		break;
	case CTRL_FLOATSLIDER:
		ctrl = new CFloatSlider(cd->label, cd->x, cd->y, cd->w, cd->h, GETFLOAT(cd->arg0), GETFLOAT(cd->arg1), (float*)cd->arg2);
		break;
	case CTRL_DROPBOX:
		ctrl = new CDropbox(cd->label, cd->x, cd->y, cd->w, cd->h, cd->arg0, cd->arg1, (int*)cd->arg2, (const char**)cd->arg3);
		break;
	case CTRL_COLORPICKER:
		ctrl = new CColorPicker(cd->label, cd->x, cd->y, cd->w, cd->h);
		break;
	default:
		break;
	}

	win->AddControl(ctrl);
	return ctrl;
}

void CGui::SetView(viewtype_t type)
{
	// we could add more views eventually if need be...
	if (type == GUI_MAXVIEWS) return;

	currentView = &views[type];

	if (mouseFocus) {
		mouseFocus->SetMouseFocus(false);
		mouseFocus = NULL;
	}

	if (keybdFocus) {
		keybdFocus->SetKeybdFocus(false);
		keybdFocus = NULL;
	}

	inputActive = false;
}

void CGui::SetViewConfig(viewtype_t view, int toggle, int mouse, int close, void (*ofunc)(void))
{
	views[view].vkeyToggle = toggle;
	views[view].vkeyMouseEnable = mouse;
	views[view].vkeyClose = close;
	views[view].openfunc = ofunc;
}

void CGui::SetMouse(float *mx, float *my, int *mouseShader)
{
	if (inputActive && eth32.cgMod->crc32 == 0xE6396FB8 ) { // TCE 0.49b letterbox correction
		*my = (*my  - (60.f * eth32.cg.screenYScale)) * 4.f / 3.f;
	}

	mouseX = *mx;
	mouseY = *my;

	if (init)
		*mouseShader = eth32.guiAssets.mousePtr;
}

void CGui::AddWindow(CWindow *win, view_t *view)
{
	win->next = view->head;
	win->prev = NULL;
	if (view->head != NULL) {
		view->head->prev = win;
		view->head = win;
	}
	else view->head = view->tail = win;
}

void CGui::DestroyAllWindows(void)
{
	CWindow *win;
	// destroy all the windows in each of our views
	for (int i=0 ; i < GUI_MAXVIEWS ; i++) {
		win = views[i].head;
		while (win) {
			views[i].head = win->next;
			delete win;
			win = views[i].head;
		}
	}
	// pointers should be NULL, but memset them anyway
	memset(views, 0, sizeof(views));
	currentView = views;
	init = false;
}

void CGui::AddRadarPlayer(player_t *player)
{
	if (winRadar)
		winRadar->AddPlayer(player);
}

void CGui::InitStyles(void)
{
	char styleName[64];
	char key[16];

	numStyles = 0;
	CleanStyleNames();

	sprintf(styleFile, "%s/%s", eth32.path, GUI_STYLES_FILE);

#ifdef ETH32_DEBUG
	Debug.Log("Gui.InitStyles() - initializing style definitions");
#endif

	for (int i=0 ; i<GUI_MAXSTYLES ; i++) {
		sprintf(key,"style%i", i);
		Tools.GetPrivateProfileString("Styles", key, "\0", styleName, 64, styleFile);
		if (*styleName && ValidateStyle(styleName)) {
			styleNames[numStyles] = new char [strlen(styleName)+1];
			strcpy(styleNames[numStyles], styleName);
			numStyles++;
		}
	}

	if (numStyles > 1)
		CreateStyleWindow();

	LoadStyle();
}

bool CGui::ValidateStyle(const char *stylename)
{
	int numAssetDefs = sizeof(assetDefs) / sizeof(assetDefs[0]);
	const assetdef_t *asset;

	for (int i=0 ; i<numAssetDefs ; i++)
	{
		asset = &assetDefs[i];
		if (!ValidateAsset(stylename, asset)) {
			#ifdef ETH32_DEBUG
				Debug.Log("style %s validation failed for %s", stylename, asset->key);
			#endif
			return false;
		}
	}

#ifdef ETH32_DEBUG
	Debug.Log("  Validated style: \"%s\"", stylename);
#endif

	return true;
}

bool CGui::ValidateAsset(const char *stylename, const assetdef_t *asset)
{
	char value[64];
	Tools.GetPrivateProfileString((char*)stylename, asset->key, "\0", value, 64, (char*)styleFile);

	switch (asset->type)
	{
	case ASSET_VEC3:
	case ASSET_VEC4:
		return (*value != 0);	// because we already know there is a value for this key, p != NULL
	case ASSET_SHADER:
	case ASSET_SHADERNOMIP:
		return ValidateFile(value);
	}
	return false;
}

bool CGui::ValidateFile(const char *file)
{
	if (!file || !(*file))
		return false;

	fileHandle_t f;
	int len = Syscall.FS_FOpenFile(file, &f, FS_READ);

	if (!f)
		return false;

	Syscall.FS_FCloseFile(f);

	if (len > 0)
		return true;
	else return false;
}

void CGui::SetStyleByName(const char *name)
{
	for (int i=0 ; i<numStyles ; i++)
	{
		if (!strcmp(name, styleNames[i])) {
			selectedStyle = i;
			return;
		}
	}
}

void CGui::CleanStyleNames(void)
{
	for (int i=0 ; i<GUI_MAXSTYLES ; i++)
	{
		if (styleNames[i])
			delete [] styleNames[i];
		styleNames[i] = NULL;
	}
}

void CGui::LoadStyle(void)
{
	loadStyleThisFrame = false;

	if (selectedStyle < 0 || selectedStyle >= numStyles || !numStyles)
		return;

	char *name = styleNames[selectedStyle];

	int numAssetDefs = sizeof(assetDefs) / sizeof(assetDefs[0]);
	const assetdef_t *asset;

	for (int i=0 ; i<numAssetDefs ; i++)
	{
		asset = &assetDefs[i];
		LoadAsset(name, asset);
	}

	currentStyle = selectedStyle;
}

void CGui::LoadAsset(const char *stylename, const assetdef_t *asset)
{
	char value[64];
	Tools.GetPrivateProfileString((char*)stylename, asset->key, "\0", value, 64, (char*)styleFile);

	if (*value == 0)
		return;

	switch (asset->type)
	{
	/*case ASSET_VEC4:
		Tools.CharToVec4(value, (float*)asset->target);
		break;*/
	case ASSET_SHADERNOMIP:
		*(qhandle_t *)asset->target = Syscall.R_RegisterShaderNoMip(value);
		break;
	}
}

void CGui::SaveSettings(const char *filename)
{
	if (!filename || !init)
		return;

	view_t *view;
	CWindow *win;

	Tools.WritePrivateProfileString("GUI Settings", "style", styleNames[currentStyle], (char *)filename);
	// used to keep windows in same relative position regardless of resolution
	char temp[32];
	sprintf(temp, "%i", eth32.game.glconfig->vidWidth);
	Tools.WritePrivateProfileString("GUI Settings", "scrwidth", temp, (char *)filename);
	sprintf(temp, "%i", eth32.game.glconfig->vidHeight);
	Tools.WritePrivateProfileString("GUI Settings", "scrheight", temp, (char *)filename);

	char buf1[64], buf2[64];
	for (int i=0 ; i<GUI_MAXVIEWS ; i++)
	{
		view = &views[i];
		win = view->head;

		while (win)
		{
			win->GetProps(buf1, sizeof(buf1));
			win->GetLabel(buf2, sizeof(buf2));
			Tools.WritePrivateProfileString("GUI Settings", buf2, buf1, (char *)filename);
			win = win->next;
		}
	}

	for (int i=0 ; i<CAM_MAX ; i++)
	{
		camInfo_t *cam = &eth32.settings.cams[i];

		sprintf(buf1, "cam%i", i);
		sprintf(buf2, "%i %i %i %i %f %f %f %s", cam->x1, cam->y1, cam->x2, cam->y2, cam->distance, cam->angle, cam->fov, (cam->display)?"true":"false");
		Tools.WritePrivateProfileString("GUI Settings", buf1, buf2, (char *)filename);
	}

	for (int i=0 ; i<ESP_GENT_MAX ; i++)
	{
		gentityInfo_t *gent = &eth32.settings.gentInfo[i];

		sprintf(buf1, "gentity%i", i);
		sprintf(buf2, "%s %s %s %s %i", (gent->chams)?"true":"false", (gent->text)?"true":"false", (gent->icon)?"true":"false", (gent->radarIcon)?"true":"false", gent->distance);
		Tools.WritePrivateProfileString("Gentity Settings", buf1, buf2, (char *)filename);
	}

#ifdef ETH32_DEBUG
	Debug.Log("Saving GUI settings...");
#endif
}

void CGui::LoadSettings(const char *filename)
{
	if (!filename)
		return;

	view_t *view;
	CWindow *win;

	int savedWidth, savedHeight;
	char value[1024];

	Tools.GetPrivateProfileString("GUI Settings", "scrwidth", "0", value, sizeof(value), (char *)filename);
	savedWidth = atoi(value);
	if (!savedWidth)
		savedWidth = eth32.game.glconfig->vidWidth;

	Tools.GetPrivateProfileString("GUI Settings", "scrheight", "0", value, sizeof(value), (char *)filename);
	savedHeight = atoi(value);
	if (!savedHeight)
		savedHeight = eth32.game.glconfig->vidHeight;

#ifdef ETH32_DEBUG
	Debug.Log("CGui::LoadSettings(): vidWidth %i vidHeight %i", eth32.game.glconfig->vidWidth, eth32.game.glconfig->vidHeight);
#endif

	float xScale = eth32.game.glconfig->vidWidth / (float)savedWidth;
	float yScale = eth32.game.glconfig->vidHeight / (float)savedHeight;

	char buf[64];
	for (int i=0 ; i<GUI_MAXVIEWS ; i++)
	{
		view = &views[i];
		win = view->head;

		while (win)
		{
			win->GetLabel(buf,sizeof(buf));
			Tools.GetPrivateProfileString("GUI Settings", buf, "\0", value, sizeof(value), (char *)filename);
			if (*value)
				win->SetProps(value, xScale, yScale);
			win = win->next;
		}
	}

	for (int i=0 ; i<CAM_MAX ; i++)
	{
		char display[32];

		camInfo_t *cam = &eth32.settings.cams[i];

		sprintf(buf, "cam%i", i);
		Tools.GetPrivateProfileString("GUI Settings", buf, "10 10 210 210 200.00 90.00 90.00 false", value, sizeof(value), (char *)filename);

		sscanf(value, "%i %i %i %i %f %f %f %s", &cam->x1, &cam->y1, &cam->x2, &cam->y2, &cam->distance, &cam->angle, &cam->fov, display);
		cam->display = (strcmp(display, "false") != 0);

		cam->x1 *= xScale;
		cam->x2 *= xScale;
		cam->y1 *= yScale;
		cam->y2 *= yScale;
	}
	ValidateCamPositions();

	for (int i=0 ; i<ESP_GENT_MAX ; i++)
	{
		int t;
		char set[32], set1[32], set2[32], set3[32];

		gentityInfo_t *gent = &eth32.settings.gentInfo[i];

		sprintf(buf, "gentity%i", i);
		Tools.GetPrivateProfileString("Gentity Settings", buf, "true true false true 3000", value, sizeof(value), (char *)filename);

		sscanf(value, "%s %s %s %s %i", &set, &set1, &set2, &set3, &gent->distance);

		gent->chams = (strcmp(set, "false") != 0);
		gent->text = (strcmp(set1, "false") != 0);
		gent->icon = (strcmp(set2, "false") != 0);
		gent->radarIcon = (strcmp(set3, "false") != 0);
	}

	Tools.GetPrivateProfileString("GUI Settings", "style", "\0", value, sizeof(value), (char *)filename);
	SetStyleByName(value);

	LoadStyle();
#ifdef ETH32_DEBUG
	Debug.Log("Loading GUI settings...");
#endif
}

void CGui::ValidateCamPositions(void)
{
	for (int i=0 ; i<CAM_MAX ; i++) {
		camInfo_t *cam = &eth32.settings.cams[i];

		if ((cam->x2 - cam->x1) < CAM_MIN_SIZE)
			cam->x2 = cam->x1 + CAM_MIN_SIZE;

		if ((cam->y2 - cam->y1) < CAM_MIN_SIZE)
			cam->y1 = cam->y1 + CAM_MIN_SIZE;

		if ((cam->x2 - cam->x1) > (eth32.game.glconfig->vidWidth - 8))
			cam->x2 = cam->x1 + (eth32.game.glconfig->vidWidth - 8);

		if ((cam->y2 - cam->y1) > (eth32.game.glconfig->vidHeight-8))
			cam->y2 = cam->y1 + (eth32.game.glconfig->vidHeight - 8);

		int width = cam->x2 - cam->x1;
		int height = cam->y2 - cam->y1;

		if (cam->x1 < 4) {
			cam->x1 = 4;
			cam->x2 = cam->x1 + width;
		} else if (cam->x2 > (eth32.game.glconfig->vidWidth - 4)) {
			cam->x2 = eth32.game.glconfig->vidWidth - 4;
			cam->x1 = cam->x2 - width;
		}

		if (cam->y1 < 4) {
			cam->y1 = 4;
			cam->y2 = cam->y1 + height;
		} else if (cam->y2 > (eth32.game.glconfig->vidHeight - 4)) {
			cam->y2 = eth32.game.glconfig->vidHeight - 4;
			cam->y1 = cam->y2 - height;
		}
	}
}

void CGui::CreateStyleWindow(void)
{
	CControl *ctrl;
	CWindow *win = CreateWin("GUI Style", 535, 400, 100, 65, WIN_STANDARD, GUI_MENUVIEW);

	ctrl = new CDropbox("Style", 5, 5, 90, 23, 0, numStyles-1, &selectedStyle, (const char**)styleNames);
	win->AddControl(ctrl);
	ctrl = new CButton("Apply", 15, 35, 70, 12, funcSetStyle);
	win->AddControl(ctrl);
}

void CGui::ScoreboardStatus(bool up)
{
	scoreboard = up;
}

void CGui::LimbopanelStatus(bool up)
{
	limbopanel = up;
}

void CGui::TopshotsStatus(bool up)
{
	topshots = up;
}

void CGui::StatspanelStatus(bool up)
{
	statspanel = up;
}

//******************
// Helper Functions
//******************

void funcSetStyle(void)
{
	Gui.loadStyleThisFrame = true;
}
