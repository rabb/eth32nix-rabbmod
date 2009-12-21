// ETH32 - an Enemy Territory cheat for windows
// Copyright (c) 2007 eth32 team
// www.cheatersutopia.com & www.nixcoders.org

#pragma once

#include "gui/CBanner.h"
#include "gui/CButtons.h"
#include "gui/CCameraWindow.h"
//#include "gui/CChatWindow.h"
#include "gui/CCheckbox.h"
#include "gui/CClientWindow.h"
#include "gui/CColorPicker.h"
#include "gui/CControl.h"
//#include "gui/CEspWindow.h"
#include "gui/CEspConfigWindow.h"
//#include "gui/CIrcWindow.h"
//#include "gui/CPlayerWindow.h"
#include "gui/CRadarWindow.h"
#include "gui/CRespawnWindow.h"
//#include "gui/CScrollBar.h"
#include "gui/CSliders.h"
#include "gui/CSpecWindow.h"
#include "gui/CStatusWindow.h"
//#include "gui/CStatsWindow.h"
//#include "gui/CTextControls.h"
//#include "gui/CUsersWindow.h"
#include "gui/CWeaponWindow.h"
#include "gui/CWindow.h"
#include "gui/CWindowPicker.h"

#define INBOUNDS(xx,yy)					((xx >= x) && (xx <= x2) && (yy >= y) && (yy <= y2))
#define INBOUNDS2(xx,yy,x1,y1,x2,y2)	((xx >= x1) && (xx <= x2) && (yy >= y1) && (yy <= y2))
#define INBOUNDS3(xx,yy,cx,cy,ww,hh)	((xx >= (cx-ww)) && (xx <= (cx+ww)) && (yy >= (cy-hh)) && (yy <= (cy+hh)))

// these should eventually be part of gui itself
#define GUI_FONT				&eth32.cg.media.fontArial
#define GUI_FONTSCALE			eth32.guiAssets.fontScale
#define GUI_SMALLFONTSCALE		GUI_FONTSCALE * 0.75f
#define GUI_FONTPIXELHEIGHT		8 // in 640x480 resolution, will scale to other resolutions
#define GUI_FONTHEIGHT			eth32.guiAssets.fontHeight//11//(int)(GUI_FONTPIXELHEIGHT * eth32.cg.screenYScale)
#define GUI_TITLEBAR_SIZE		(GUI_FONTHEIGHT + 6)
#define GUI_ESIZE				GUI_TITLEBAR_SIZE
#define GUI_TEXTSPACING			2
#define GUI_SPACING				(5 * eth32.cg.screenXScale)
#define GUI_TITLECOLOR			eth32.guiAssets.titleColor
#define GUI_FONTCOLOR1			eth32.guiAssets.textColor1
#define GUI_FONTCOLOR2			eth32.guiAssets.textColor2

#define TEXTWIDTH(str)			Draw.sizeTextWidth(GUI_FONTSCALE, str, GUI_FONT)

#define GUI_SCROLLBAR_WIDTH		10
#define GUI_SCROLL_BTN_MIN		10

#define GUI_DBLCLK_TIME			500		// time between clicks in msec

#define GUI_MAXWINCTRLS			16
#define GUI_CURFLASHTIME		400		// time in ms of text cursor on / off flashing

/*#define GUI_MAXCONSOLECMDS		16		// maximum number of console cmds we can buffer
#define GUI_MAXCONSOLEMSGS		32
#define GUI_CONSOLEMSGSIZE		1024	// maximum length of any given buffered cmd*/

#define GUI_MAXSTYLES			20
#define GUI_MAXVALUESIZE		1024

#define GUI_STYLES_FILE			"styles.ini"

typedef enum {
	GUI_MAINVIEW,
	GUI_MENUVIEW,
	GUI_CLIENTVIEW,
	GUI_USERVIEW,
	GUI_MAXVIEWS
} viewtype_t;

typedef struct view_s {
	CWindow *head;
	CWindow *tail;

	int	vkeyToggle;			// key to open/close a view
	int vkeyMouseEnable;	// key that must be pressed to allow keybd input
	int vkeyClose;			// key that will close view and restore default (GUI_MAINVIEW)

	void (*openfunc)(void);	// function to call when this view is opened
} view_t;

typedef enum {
	CTRL_CHECKBOX,
	CTRL_INTDISPLAY,
	CTRL_FLOATDISPLAY,
	CTRL_INTSLIDER,
	CTRL_FLOATSLIDER,
	CTRL_DROPBOX,
	CTRL_COLORPICKER,
	CTRL_MAX
} ctrltype_t;

typedef enum {
	//WIN_CHAT,
	WIN_USERS,
	WIN_CLIENTS,
	WIN_RADAR,
	WIN_HITBOX,
	WIN_STANDARD,
	WIN_STATUS,
	//WIN_STATS,
	WIN_WEAPCONFIG,
	WIN_WEAPON,
	//WIN_XHAIRINFO,
	WIN_CAMERA,
	WIN_CAMCONFIG,
	WIN_ESPCONFIG,
	WIN_RESPAWN,
 	WIN_SPECTATOR,
 	WIN_BANNER,
	WIN_MAX
} wintype_t;

typedef struct {
	ctrltype_t type;					// used to determine how to cast arg0-arg4
	const char *label;						// common to all controls
	int x, y, w, h;						// common to all controls
	int arg0, arg1, arg2, arg3, arg4;	// used for different types of controls, will be casted on creation
} ctrldef_t;

typedef struct {
	const char *title;		// window title
	wintype_t type;		// window type to create
	viewtype_t view;	// which view we want to add it to
	int x, y, w, h;		// position and size
	int numCtrls;		// num controls
	ctrldef_t ctrls[GUI_MAXWINCTRLS];	// array of ctrls to be added
} windef_t;

typedef enum {
	ASSET_VEC3,
	ASSET_VEC4,
	ASSET_SHADERNOMIP,
	ASSET_SHADER,
	MAX_ASSETS,
} assettype_t;

typedef struct {
	const char *key;
	assettype_t type;
	void *target;
} assetdef_t;
