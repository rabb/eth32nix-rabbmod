// ETH32 - an Enemy Territory cheat for windows
// Copyright (c) 2007 eth32 team
// www.cheatersutopia.com & www.nixcoders.org

#pragma once

extern vec4_t eth32WindowBg;
extern vec4_t eth32WindowBd;

extern vec4_t gameBackColor;
extern vec4_t gameBorderColor;
extern vec4_t gameFontColor;

#define ETH32_PTR_TOGGLE_DELAY	150
#define ETH32_PTR_TOGGLE_KEY	XK_F8

class CDraw
{
public:
	// drawtools
	int CG_Text_Width_Ext(const char *text, float scale, int limit, fontInfo_t* font);
	void CG_Text_PaintChar_Ext(float x, float y, float w, float h, float scalex, float scaley, float s, float t, float s2, float t2, qhandle_t hShader, bool raw = false);
	void CG_Text_Paint_Ext(float x, float y, float scalex, float scaley, vec4_t color, const char *text, float adjust, int limit, int style, fontInfo_t* font, bool raw = false);
	void adjustFrom640(float *x, float *y, float *w, float *h);
	void adjustTo640(float *x, float *y, float *w, float *h);
	void Rect(float x, float y, float width, float height, vec4_t color, int borderSize);
	void FillRect(float x, float y, float width, float height, const float *color);
	void RotatedPic( float x, float y, float width, float height, qhandle_t hShader, float qangle);
	void HitIndicatorArrow( float x, float y, float w, float h, vec3_t origin, vec3_t dest, char *str, qhandle_t shader );
	void Pic(float x, float y, float width, float height, qhandle_t hShader);

	// added these for use by Gui to remove artifacts due to resolution calcs
	void RawRect(int x, int y, int width, int height, vec4_t color, int borderSize);
	void RawFillRect(int x, int y, int width, int height, const float *color);
	void RawPic(int x, int y, int width, int height, qhandle_t hShader);
	void RawPicST(int x, int y, int width, int height, float s0, float s1, float t0, float t1, qhandle_t hShader);

	bool worldToScreen(vec3_t worldCoord, int *x, int *y);
	void Text(float x, float y, float scale, const char *text, vec4_t color, qboolean forceColor, qboolean shadow, fontInfo_t *font, bool raw = false);
	void TextCenter(float x, float y, float scale, const char *text, vec4_t color, qboolean forceColor, qboolean shadow, fontInfo_t *font, bool raw = false);
	void MultiText(float x, float y, float scale, const char *text, fontInfo_t *font);
	int sizeTextHeight(float scale, fontInfo_t *font);
	int sizeTextWidth(float scale, const char *text, fontInfo_t *font, int limit = 0);
	void TextRect(float x, float y, float scale, char *text, vec4_t textColor, qboolean forceColor, qboolean textShadow, qboolean background, qboolean border, vec4_t backgroundColor, vec4_t borderColor, fontInfo_t *font);
	int BreakStringByWidth(char *str, float width, char *lastcolor, fontInfo_t *font, bool breakspaces);

	// et and our visuals
	//void drawCrosshair();
	void Statbar(int x, int y, int w, int h, float frac, const float *color);
	void Statbar2(int x, int y, int w, int h, float frac, const float *color);
	void ColorForHealth(int health, vec4_t color);
	bool CheckPic(float x, float y, float w, float h, float s1, float t1, float s2, float t2, qhandle_t hShader);
	void drawMortarTrace(player_t *player);
	void DrawCompassIcon( float x, float y, float w, float h, vec3_t origin, vec3_t dest, char *str, qhandle_t shader );
	void DrawSquareCompassIcon( float x, float y, float w, float h, vec3_t origin, vec3_t dest, qhandle_t shader );
};

extern CDraw Draw;
