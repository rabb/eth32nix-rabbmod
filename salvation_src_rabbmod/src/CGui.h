// ETH32 - an Enemy Territory cheat for windows
// Copyright (c) 2007 eth32 team
// www.cheatersutopia.com & www.nixcoders.org

#pragma once

#include "gui/gui_types.h"

class CGui
{
public:
	CGui(void);
	~CGui(void);

	void Init(void);						// load our windows/controls from struct (guidefs.h)
	void Shutdown(void);					// destroy all windows and reset gui states

	bool InputActive(void);					// true if we want keybd or mouse input to gui

	void PreFrame(void);					// called prior to DrawActiveFrame
	void PostFrame(void);					// called after DrawActiveFrame

	void UpdateViewState(void);				// check if we need to change view or input state
	void CheckClose(int vkey);				// if view has an assigned key to close it (generally escape)
	void MouseEvent(uint32 mevent);			// mouse event from wndproc
	void MouseMove(void);					// mouse move from wndproc
	void KeybdEvent(unsigned int key);		// keybd event from wndproc

	void SetMouseFocus(CControl *ctrl);		// manually set mouse focus
	void SetKeybdFocus(CControl *ctrl);		// manually set keybd focus

	CWindow *CreateWin(const char *title, int x, int y, int w, int h, wintype_t = WIN_STANDARD, viewtype_t vtype = GUI_MAINVIEW);	// create a window

	CWindow *GetWinByTitle(const char *title);		// return ptr to window if Gui can find it
	CControl *GetCtrlByLabel(const char *label);	// return ptr to control if Gui can find it

	void SetView(viewtype_t view);			// set view to main, menu, or none
	void SetViewConfig(viewtype_t view, int toggle, int mouse, int close, void (*ofunc)(void));	// set view key to check for
	void SetMouse(float *mx, float *my, int *mouseShader);	// grab our mouse position (in 640x480) and set mouse ptr if need be

	/*bool SetChatTarget(int type);
	chattype_t GetChatTarget(void);
	void CheckPrivMsg(const char *msg);
	void ReplyMessage(void);
	void ChatMessage(const char *msg);*/

	//void CenterPrint(const char *str, int priority);
	//void DrawCenterPrint(void);

	void AddRadarPlayer(player_t *player);

	// Set of functions to allow us to send commands in sync with game
	// (i.e. sending commands on keybd messages == bad)
	// So we buffer them, then send them all on PreFrame()
	/*void BufferConsoleCommand(const char *cmd);
	void PurgeConsoleCommands(void);
	// simple print to console, buffered for use by irc/portal threads
	void BufferConsoleMessage(const char *msg);
	void PurgeConsoleMessages(void);

	void PrivateMessage(const char *msg);*/

	void LoadStyle(void);
	void CreateStyleWindow(void);
	void DestroyAllWindows(void);

	bool loadStyleThisFrame;			// in order to keep this in sync with engine

	void SaveSettings(const char *filename);
	void LoadSettings(const char *filename);
	void ValidateCamPositions(void);

	void ScoreboardStatus(bool up);
	void LimbopanelStatus(bool up);
	bool IsLimboPanelUp(void) { return limbopanel; }
	void TopshotsStatus(bool up);
	void StatspanelStatus(bool up);
private:
	CControl *CreateCtrl(CWindow *win, const ctrldef_t *cd);	// create a control using ctrldef

	void AddWindow(CWindow *win, view_t *view);	// adds a window to a view

	void SetStyleByName(const char *name);
	void CleanStyleNames(void);

	void InitStyles(void);
	void LoadAsset(const char *stylename, const assetdef_t *asset);
	bool ValidateStyle(const char *stylename);
	bool ValidateAsset(const char *stylename, const assetdef_t *asset);
	bool ValidateFile(const char *file);

	view_t		views[GUI_MAXVIEWS];		// our different views (main, menu, etc)

	//CChatWindow		*winMainChat;			// allow access to chat window functions
	CRadarWindow	*winRadar;				// allow access to radar functions

	int			mouseX, mouseY;				// mouse position (in 640x480)
	bool		inputActive;				// are we looking for mouse or keybd input
	bool		menuOpen;					// menu open

	bool		init;						// so we don't Init() twice accidently
	bool		wndprocModified;			// safety for redirecting/restoring wndproc

	view_t		*currentView;				// active view

	CControl	*mouseFocus, *keybdFocus;	// when != NULL, we send mouse/keybd input directly to the control

	char		styleFile[MAX_PATH];

	//char		consoleCmdBuffer[GUI_MAXCONSOLECMDS][GUI_CONSOLEMSGSIZE];
	//int			numConsoleCmds;

	//char		consoleMsgBuffer[GUI_MAXCONSOLEMSGS][GUI_CONSOLEMSGSIZE];
	//int			numConsoleMsgs;

	int			selectedStyle;				// added so that we save actual current style, not just what control shows
	int			currentStyle;
	char		*styleNames[GUI_MAXSTYLES];
	int			numStyles;

	bool		scoreboard;					// disable drawing when any of these are flagged up
	bool		limbopanel;
	bool		topshots;
	bool		statspanel;

	/*char		centerPrintString[1024];
	int			centerPrintPriority;
	int			centerPrintTime;*/

	pthread_mutex_t *mutex_cp;
	pthread_mutex_t *mutex_cc;
};

extern CGui		Gui;

//******************
// Helper Functions
//******************

void funcSetStyle(void);
