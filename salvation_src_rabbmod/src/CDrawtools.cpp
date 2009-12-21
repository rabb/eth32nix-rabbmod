// ETH32 - an Enemy Territory cheat for windows
// Copyright (c) 2007 eth32 team
// www.cheatersutopia.com & www.nixcoders.org

#include "eth32.h"

CDraw Draw;

vec4_t eth32WindowBg = { 0.28f, 0.25f, 0.19f, 0.75f };
vec4_t eth32WindowBd = { 0.9f, 0.9f, 0.9f, 0.9f };

vec4_t gameBackColor	= { 0.16f,	0.2f,	0.17f,	0.9f };
vec4_t gameBorderColor	= { 0.5f,	0.5f,	0.5f,	0.5f };
vec4_t gameFontColor	= { 0.625f,	0.625f,	0.6f,	1.0f };

int CDraw::CG_Text_Width_Ext(const char *text, float scale, int limit, fontInfo_t* font)
{
	int count, len;
	glyphInfo_t *glyph;
	const char *s = text;
	float out, useScale = scale * font->glyphScale;
	
	out = 0;
	if( text ) {
		len = (int) strlen( text );
		if (limit > 0 && len > limit) {
			len = limit;
		}
		count = 0;
		while (s && *s && count < len) {
			if ( Q_IsColorString(s) ) {
				s += 2;
				continue;
			} else {
				glyph = &font->glyphs[(unsigned char)*s];
				out += glyph->xSkip;
				s++;
				count++;
			}
		}
	}

	return out * useScale;
}

void CDraw::CG_Text_PaintChar_Ext(float x, float y, float w, float h, float scalex, float scaley, float s, float t, float s2, float t2, qhandle_t hShader, bool raw)
{
	w *= scalex;
	h *= scaley;

	if (!raw)
		adjustFrom640(&x, &y, &w, &h);

	Syscall.R_DrawStretchPic(x, y, w, h, s, t, s2, t2, hShader);
}

void CDraw::CG_Text_Paint_Ext(float x, float y, float scalex, float scaley, vec4_t color, const char *text, float adjust, int limit, int style, fontInfo_t* font, bool raw)
{
	int len, count;
	vec4_t newColor;
	glyphInfo_t *glyph;

	scalex *= font->glyphScale;
	scaley *= font->glyphScale;

	if (text) {
		const char *s = text;
		Syscall.R_SetColor( color );
		memcpy(&newColor[0], &color[0], sizeof(vec4_t));
		len = (int) strlen(text);
		if (limit > 0 && len > limit) {
			len = limit;
		}
		count = 0;
		while (s && *s && count < len) {
			glyph = &font->glyphs[(unsigned char)*s];
			if ( Q_IsColorString( s ) ) {
				if( *(s+1) == COLOR_NULL ) {
					memcpy( newColor, color, sizeof(newColor) );
				} else {
					memcpy( newColor, g_color_table[ColorIndex(*(s+1))], sizeof( newColor ) );
					newColor[3] = color[3];
				}
				Syscall.R_SetColor(newColor);
				s += 2;
				continue;
			} else {
				float yadj = scaley * glyph->top;
				yadj -= (font->glyphs['['].top + 3) * scaley;	// eth: Add y correction
				if (style == ITEM_TEXTSTYLE_SHADOWED || style == ITEM_TEXTSTYLE_SHADOWEDMORE) {
					int ofs = style == ITEM_TEXTSTYLE_SHADOWED ? 1 : 2;
					colorBlack[3] = newColor[3];
					Syscall.R_SetColor( colorBlack );
					CG_Text_PaintChar_Ext(x + (glyph->pitch * scalex) + ofs, y - yadj + ofs, glyph->imageWidth, glyph->imageHeight, scalex, scaley, glyph->s, glyph->t, glyph->s2, glyph->t2, glyph->glyph, raw);
					colorBlack[3] = 1.0;
					Syscall.R_SetColor( newColor );
				}
				CG_Text_PaintChar_Ext(x + (glyph->pitch * scalex), y - yadj, glyph->imageWidth, glyph->imageHeight, scalex, scaley, glyph->s, glyph->t, glyph->s2, glyph->t2, glyph->glyph, raw);
				x += (glyph->xSkip * scalex) + adjust;
				s++;
				count++;
			}
		}
		Syscall.R_SetColor( NULL );
	}

}

void CDraw::adjustFrom640(float *x, float *y, float *w, float *h)
{
	*x *= eth32.cg.screenXScale;
	*y *= eth32.cg.screenYScale;
	*w *= eth32.cg.screenXScale;
	*h *= eth32.cg.screenYScale;
}

void CDraw::adjustTo640(float *x, float *y, float *w, float *h)
{
	*x /= eth32.cg.screenXScale;
	*y /= eth32.cg.screenYScale;
	*w /= eth32.cg.screenXScale;
	*h /= eth32.cg.screenYScale;
}

void CDraw::Rect(float x, float y, float width, float height, vec4_t color, int borderSize)
{
	Syscall.R_SetColor(color);

	adjustFrom640(&x, &y, &width, &height);
	Syscall.R_DrawStretchPic(x, y, borderSize, height, 0, 0, 0, 0, eth32.cg.media.white);
	Syscall.R_DrawStretchPic(x + width - borderSize, y, borderSize, height, 0, 0, 0, 0, eth32.cg.media.white);
	Syscall.R_DrawStretchPic(x, y, width, borderSize, 0, 0, 0, 0, eth32.cg.media.white);
	Syscall.R_DrawStretchPic(x, y + height - borderSize, width, borderSize, 0, 0, 0, 0, eth32.cg.media.white);

	Syscall.R_SetColor(NULL);
}

void CDraw::FillRect(float x, float y, float width, float height, const float *color)
{
	Syscall.R_SetColor(color);

    adjustFrom640(&x, &y, &width, &height);
    Syscall.R_DrawStretchPic(x, y, width, height, 0, 0, 0, 1, eth32.cg.media.white);

	Syscall.R_SetColor(NULL);
}

void CDraw::RotatedPic( float x, float y, float width, float height, qhandle_t hShader, float angle ) {

	adjustFrom640( &x, &y, &width, &height );

	Syscall.R_DrawRotatedPic(x, y, width, height, 0, 0, 1, 1, hShader, angle);
}

void CDraw::HitIndicatorArrow( float x, float y, float w, float h, vec3_t origin, vec3_t dest, char *str, qhandle_t shader ) 
{
	float angle, pi2 = M_PI * 2;
	vec3_t v1, angles;

	VectorCopy( dest, v1 );
	VectorSubtract( origin, v1, v1 );
	VectorNormalize( v1 );
	vectoangles( v1, angles );

	if ( v1[0] == 0 && v1[1] == 0 && v1[2] == 0 )
		return;

		
	angles[YAW] = AngleSubtract( eth32.cg.refdefViewAngles[YAW], angles[YAW] );

	angle = ( ( angles[YAW] + 180.f ) / 360.f - ( 0.50 / 2.f ) ) * pi2;

	w /= 2;
	h /= 2;

	x += w;
	y += h;

	w = sqrt( ( w * w ) + ( h * h ) ) / 3.f * 2.f * 0.9f;

	x = x + ( cos( angle ) * w );
	y = y + ( sin( angle ) * w );

	// Alexplay: Added pic rotation according to icon position.
	float qangle;
	if (sin(angle) < 0.f)
		//imangle = (-1.5 + cos(angle))/4;
		qangle = (-1 + cos(angle))/4;
	else 
		//imangle = (0.5 + (-cos(angle)))/4;
		qangle = (1 + (-cos(angle)))/4;

	RotatedPic( x - (eth32.settings.dmgArrSize/2), y - (eth32.settings.dmgArrSize/2), eth32.settings.dmgArrSize, eth32.settings.dmgArrSize, shader, qangle);

	if(str)
		TextCenter(x, y + 14, 0.16, str, colorWhite, qfalse, qtrue, &eth32.cg.media.fontArial);
}

void CDraw::Pic(float x, float y, float width, float height, qhandle_t hShader)
{
	adjustFrom640(&x, &y, &width, &height);
	Syscall.R_DrawStretchPic(x, y, width, height, 0, 0, 1, 1, hShader);
}

// raw funcs for use by GUI
void CDraw::RawRect(int x, int y, int width, int height, vec4_t color, int borderSize)
{
	Syscall.R_SetColor(color);

	Syscall.R_DrawStretchPic(x, y, borderSize, height, 0, 0, 0, 0, eth32.cg.media.white);
	Syscall.R_DrawStretchPic(x + width - borderSize, y, borderSize, height, 0, 0, 0, 0, eth32.cg.media.white);
	Syscall.R_DrawStretchPic(x, y, width, borderSize, 0, 0, 0, 0, eth32.cg.media.white);
	Syscall.R_DrawStretchPic(x, y + height - borderSize, width, borderSize, 0, 0, 0, 0, eth32.cg.media.white);

	Syscall.R_SetColor(NULL);
}

void CDraw::RawFillRect(int x, int y, int width, int height, const float *color)
{
	Syscall.R_SetColor(color);

    Syscall.R_DrawStretchPic(x, y, width, height, 0, 0, 0, 1, eth32.cg.media.white);

	Syscall.R_SetColor(NULL);
}

void CDraw::RawPic(int x, int y, int width, int height, qhandle_t hShader)
{
	Syscall.R_DrawStretchPic(x, y, width, height, 0, 0, 1, 1, hShader);
}

void CDraw::RawPicST(int x, int y, int width, int height, float s0, float s1, float t0, float t1, qhandle_t hShader)
{
	Syscall.R_DrawStretchPic(x, y, width, height, s0, s1, t0, t1, hShader);
}

void CDraw::Statbar(int x, int y, int width, int height, float frac, const float *color)
{
	if (frac > 1.0)
		frac = 1.0;
	else if (frac < 0.0)
		frac = 0.0;

	int cutWidth = width * frac;

	Syscall.R_SetColor(color);
	RawPicST(x, y, cutWidth, height, 0.f, 0.f, frac, 1.f, eth32.cg.media.statbar);
	Syscall.R_SetColor(colorLtGrey);
	RawPicST(x + cutWidth, y, width - cutWidth, height, frac, 0.f, 1.f, 1.f, eth32.cg.media.statbar);
	Syscall.R_SetColor(NULL);
}

void CDraw::Statbar2(int x, int y, int width, int height, float frac, const float *color)
{
	if (frac > 1.0)
		frac = 1.0;
	else if (frac < 0.0)
		frac = 0.0;

	int cutWidth = width * frac;

	Syscall.R_SetColor(color);
	RawPicST(x, y, cutWidth, height, 0.f, 0.f, frac, 1.f, eth32.cg.media.statbar2);
	Syscall.R_SetColor(colorLtGrey);
	RawPicST(x + cutWidth, y, width - cutWidth, height, frac, 0.f, 1.f, 1.f, eth32.cg.media.statbar2);
	Syscall.R_SetColor(NULL);
}

void CDraw::ColorForHealth(int health, vec4_t color) {
	if ( health <= 0 ) {
		VectorClear(color);	// black
		color[3] = 1;
		return;
	}

	float frac = health / 100.f;

	if (frac >= 0.8)
		//Vector4Set(color, 0, 1, 0, 1);
		Vector4Set(color, eth32.settings.gui_healthHi[0],eth32.settings.gui_healthHi[1],eth32.settings.gui_healthHi[2],eth32.settings.gui_healthHi[3]);
	else if (frac >= 0.3)
		//Vector4Set(color, 1, 1, 0, 1);
		Vector4Set(color, eth32.settings.gui_healthMd[0],eth32.settings.gui_healthMd[1],eth32.settings.gui_healthMd[2],eth32.settings.gui_healthMd[3]);
	else
		//Vector4Set(color, 1, 0, 0, 1);
		Vector4Set(color, eth32.settings.gui_healthLo[0],eth32.settings.gui_healthLo[1],eth32.settings.gui_healthLo[2],eth32.settings.gui_healthLo[3]);
}

/**********************************
Credits to OGC for world2screen
**********************************/

inline float VectorAngle(const vec3_t a, const vec3_t b)
{
    float length_a = VectorLength(a);
	float length_b = VectorLength(b);
	float length_ab = length_a*length_b;
	if( length_ab==0.0 ){ return 0.0; }
	else  { return (float) (acos(DotProduct(a,b)/length_ab) * (180.f/M_PI)); }
}

void MakeVector(const vec3_t ain, vec3_t vout)
{
	float pitch;
	float yaw;
	float tmp;		
	
	pitch = (float) (ain[0] * M_PI/180);
	yaw = (float) (ain[1] * M_PI/180);
	tmp = (float) cos(pitch);
	
	vout[0] = (float) (-tmp * -cos(yaw));
	vout[1] = (float) (sin(yaw)*tmp);
	vout[2] = (float) -sin(pitch);
}

void VectorRotateX(const vec3_t in, float angle, vec3_t out)
{
	float a,c,s;

	a = (float) (angle * M_PI/180);
	c = (float) cos(a);
	s = (float) sin(a);
	out[0] = in[0];
	out[1] = c*in[1] - s*in[2];
	out[2] = s*in[1] + c*in[2];	
}

void VectorRotateY(const vec3_t in, float angle, vec3_t out)
{
	float a,c,s;

	a = (float) (angle * M_PI/180);
	c = (float) cos(a);
	s = (float) sin(a);
	out[0] = c*in[0] + s*in[2];
	out[1] = in[1];
	out[2] = -s*in[0] + c*in[2];
}

void VectorRotateZ(const vec3_t in, float angle, vec3_t out)
{
	float a,c,s;

	a = (float) (angle * M_PI/180);
	c = (float) cos(a);
	s = (float) sin(a);
	out[0] = c*in[0] - s*in[1];
	out[1] = s*in[0] + c*in[1];
	out[2] = in[2];
}


bool CDraw::worldToScreen(vec3_t worldCoord, int *x, int *y)
{
	vec3_t aim;
	vec3_t newaim;
	vec3_t view;
	vec3_t tmp;
	float num;

	VectorCopy(eth32.cg.refdef->vieworg, tmp);
	VectorSubtract(worldCoord, tmp, aim);	
	MakeVector(eth32.cg.refdefViewAngles, view);	

	if (VectorAngle(view, aim) > (eth32.cg.refdef->fov_x / 1.8))
		return false;
		
	VectorRotateZ(aim, -eth32.cg.refdefViewAngles[1], newaim);
	VectorRotateY(newaim, -eth32.cg.refdefViewAngles[0], tmp);
	VectorRotateX(tmp, -eth32.cg.refdefViewAngles[2], newaim);
	
	if (newaim[0] <= 0)
		return false;

	num = (float) ((320.0f / newaim[0]) * (120.0 / eth32.cg.refdef->fov_x - 1.0 / 3.0));	

	*x = (int) (320 - (num * newaim[1]));
	*y = (int) (240 - (num * newaim[2]));

	BOUND_VALUE(*x, 0, 640);
	BOUND_VALUE(*y, 0, 480);

	return true;
}

void CDraw::Text(float x, float y, float scale, const char *text, vec4_t color, qboolean forceColor, qboolean shadow, fontInfo_t *font, bool raw)
{
	char *finalText = (char*)text;

	if (forceColor) {
		finalText = strdup(text);
		Q_CleanStr(finalText);
	}

	CG_Text_Paint_Ext(x, y, scale, scale, color, finalText, 0.0f, 0, shadow ? ITEM_TEXTSTYLE_SHADOWED : 0, font, raw);

	if (forceColor)
		free(finalText);
}

void CDraw::MultiText(float x, float y, float scale, const char *text, fontInfo_t *font)
{
	if (!text)
		return;	
	
	std::string stxt = text;
	StringTokenizer lines(stxt, "::");
	
	int maxwidth = -1;
	float theight = y;
	int height = sizeTextHeight(scale, font);
	
	while (lines.hasMoreTokens()) {
		char *txt = (char *)lines.nextToken().c_str();
		int width = sizeTextWidth(scale, txt, font);
		maxwidth = width > maxwidth ? width : maxwidth;
		theight += (float)height+1.0;
	}	
	
	float borderSize = font->glyphs['['].pitch * scale * 3;
	FillRect(x - borderSize, y - borderSize, maxwidth + (2 * borderSize), (theight-y) + (2 * borderSize), gameBackColor);
	Rect(x - borderSize, y - borderSize, maxwidth + (2 * borderSize),  (theight-y) + (2 * borderSize), gameBorderColor, 1);		
			
	StringTokenizer lines2(stxt, "::");
	
	float y0 = y;
		
	while (lines2.hasMoreTokens()) {		
		char *txt = (char *)lines2.nextToken().c_str();
		Text(x, y, scale, txt, gameFontColor, qfalse, qfalse, font, qfalse);
		y += (float)height+1.0;
	}		
	

}

void CDraw::TextCenter(float x, float y, float scale, const char *text, vec4_t color, qboolean forceColor, qboolean shadow, fontInfo_t *font, bool raw)
{
	// Just centered horizontally, not sure we need vertical centering also
	Text(x - (CG_Text_Width_Ext(text, scale, 0, font) >> 1), y, scale, text, color, forceColor, shadow, font, raw);
}

int CDraw::sizeTextHeight(float scale, fontInfo_t *font)
{
	return (font->glyphs['['].height + 6) * font->glyphScale * scale;
}

int CDraw::sizeTextWidth(float scale, const char *text, fontInfo_t *font, int limit)
{
	return CG_Text_Width_Ext(text, scale, limit, font) + 1;
}

void CDraw::TextRect(float x, float y, float scale, char *text, vec4_t textColor, qboolean forceColor, qboolean textShadow, qboolean background, qboolean border, vec4_t backgroundColor, vec4_t borderColor, fontInfo_t *font)
{
	int width = sizeTextWidth(scale, text, font);
	int height = sizeTextHeight(scale, font);
	float borderSize = font->glyphs['['].pitch * scale * 3;
	if (background)
		FillRect(x - borderSize, y - borderSize, width + (2 * borderSize), height + (2 * borderSize), backgroundColor);
	if (border)
		Rect(x - borderSize, y - borderSize, width + (2 * borderSize), height + (2 * borderSize), borderColor, 1);

	Text(x, y, scale, text, textColor, forceColor, textShadow, font);
}

int CDraw::BreakStringByWidth(char *str, float width, char *lastcolor, fontInfo_t *font, bool breakspaces)
{
	if (*str == 0) return 0;

	char *p = str;
	char *ls = NULL;	// last space
	char *lc = NULL;	// last color
	float textWidth = 0.f;
	
	while (textWidth < width && *p) {
		if (*p == ' ' && breakspaces) 
			ls = p;
		if (Q_IsColorString(p)) {
			lc = (p+1);
			p += 2;
			continue;
		}
		textWidth += font->glyphs[(unsigned char)*p].xSkip * GUI_FONTSCALE * font->glyphScale;
		p++;		
	}
	if (textWidth > width) {
		if (*(p-1) == '^')
			p -= 2;
		else if (*(p-2) == '^')
			p -= 3;
		else p--;
	}

	if (lc && lastcolor) 
		*lastcolor = *lc;

    if (*p && ls && (p-ls) < 13) // if a space is close, break there
		return (ls-str);
	else return (p-str);  // otherwise use the whole piece
}

bool CDraw::CheckPic(float x, float y, float w, float h, float s1, float t1, float s2, float t2, qhandle_t hShader)
{
	this->adjustTo640(&x, &y, &w, &h);

	if (hShader == eth32.cg.media.white) {

		if (eth32.cgMod->type == MOD_TCE && (int) w == 646)//Flash Removal
			return false;
		// sniper blackout
		if (!eth32.settings.blackout && (int) y == 0 && (int) w == 80)
			return false;
	}

	if (!eth32.settings.blackout && (hShader == eth32.cg.media.binoc || hShader == eth32.cg.media.reticleSimple))
		return false;

	this->adjustFrom640(&x, &y, &w, &h);

	return true;
}

void CDraw::drawMortarTrace(player_t *player)
{
	bool once = false;

	vec3_t nextPos, lastPos;
	vec3_t angles, forward;

	AxisToAngles(player->orHead.axis, angles);

	angles[PITCH] -= 60.f;
	AngleVectors(angles, forward, NULL, NULL);
	forward[0] *= 3000 * 1.1f;
	forward[1] *= 3000 * 1.1f;
	forward[2] *= 1500 * 1.1f;

	trajectory_t mortarTrajectory;
	mortarTrajectory.trType = TR_GRAVITY;
	mortarTrajectory.trTime = eth32.cg.time;
	VectorCopy(player->lerpOrigin, mortarTrajectory.trBase);
	VectorCopy(forward, mortarTrajectory.trDelta);

	// Calculate mortar impact
	int timeOffset = 0;
	trace_t mortarTrace;
	vec3_t mortarImpact;
	VectorCopy(mortarTrajectory.trBase, mortarImpact);
	#define TIME_STEP 20
	while (timeOffset < 10000) {
		timeOffset += TIME_STEP;
		BG_EvaluateTrajectory(&mortarTrajectory, eth32.cg.time + timeOffset, nextPos, qfalse, 0);
		orig_CG_Trace(&mortarTrace, mortarImpact, 0, 0, nextPos, eth32.cg.snap->ps.clientNum, MASK_MISSILESHOT);
		if ((mortarTrace.fraction != 1)
				// Stop if we hit sky
				&& !((mortarTrace.surfaceFlags & SURF_NODRAW) || (mortarTrace.surfaceFlags & SURF_NOIMPACT))
				&& (mortarTrace.contents != 0)) {
			break;
		}
		if(!once)
		{
			VectorCopy(mortarTrajectory.trBase, lastPos);
			once = true;
		}

		Engine.MakeRailTrail( lastPos, nextPos, false, colorMagenta, 350 ); // buffalo: colorYellow for now till I add in controls
		VectorCopy(nextPos, lastPos);
	}
}

void CDraw::DrawCompassIcon( float x, float y, float w, float h, vec3_t origin, vec3_t dest, char *str, qhandle_t shader ) 
{
	float angle, pi2 = M_PI * 2, len;
	vec3_t v1, angles;

	VectorCopy( dest, v1 );
	VectorSubtract( origin, v1, v1 );
	len = VectorLength( v1 );
	VectorNormalize( v1 );
	vectoangles( v1, angles );

	if ( v1[0] == 0 && v1[1] == 0 && v1[2] == 0 )
		return;

		
	angles[YAW] = AngleSubtract( eth32.cg.refdefViewAngles[YAW], angles[YAW] );

	angle = ( ( angles[YAW] + 180.f ) / 360.f - ( 0.50 / 2.f ) ) * pi2;

	w /= 2;
	h /= 2;

	x += w;
	y += h;

	w = sqrt( ( w * w ) + ( h * h ) ) / 3.f * 2.f * 0.9f;

	x = x + ( cos( angle ) * w );
	y = y + ( sin( angle ) * w );

	len = 1 - min( 1.f, len / 2000.f );
	
	Pic( x - (14 * len + 4)/2, y - (14 * len + 4)/2, 14 * len + 8, 14 * len + 8, shader );

	if(str)
		TextCenter(x, y + 14, 0.16, str, colorWhite, qfalse, qtrue, &eth32.cg.media.fontArial);
}


void CDraw::DrawSquareCompassIcon( float x, float y, float w, float h, vec3_t origin, vec3_t dest, qhandle_t shader ) 
{
	int iconWidth, iconHeight;
	float angle, pi2 = M_PI * 2, len;
	vec3_t angles;

	angles[YAW] = AngleSubtract( eth32.cg.snap->ps.viewangles[YAW], angles[YAW] );

	angle = ( ( angles[YAW] + 180.f ) / 360.f - ( 0.50 / 2.f ) ) * pi2;

	w /= 2;
	h /= 2;

	x += w;
	y += h;

	w = sqrt( ( w * w ) + ( h * h ) ) / 3.f * 2.f * 0.9f;

	x = x + ( cos( angle ) * w );
	y = y + ( sin( angle ) * w );

	len = 1 - min( 1.f, len / 2000.f );

	while (angle < 0)
		angle += pi2;

	while (angle >= pi2)
		angle -= pi2;


	x = x + w/2;
	y = y + h/2;
	w /= 2;// = sqrt( ( w * w ) + ( h * h ) ) / 3.f * 2.f * 0.9f;

	if ( (angle >= 0) && (angle < M_PI/4.0))
	{
		x += w;
		y += w * tan(angle);

	}
	else if ( (angle >= M_PI/4.0) && (angle < 3.0 * M_PI / 4.0) )
	{
		x += w / tan(angle);
		y += w;
	}
	else if ( (angle >= 3.0 * M_PI / 4.0) && (angle < 5.0 * M_PI / 4.0) )
	{
		x -= w;
		y -= w * tan(angle);
	}
	else if ( (angle >= 5.0 * M_PI / 4.0) && (angle < 7.0 * M_PI / 4.0) )
	{
		x -= w / tan(angle);
		y -= w;
	}
	else
	{
		x += w;
		y += w * tan(angle);

	}

	len = 1 - min( 1.f, len / 2000.f );
	iconWidth = 14 * len + 4; // where did this calc. come from?
	iconHeight = 14 * len + 4; 

	// adjust so that icon is always outside of the map
	if ( (angle > 5.0*M_PI/4.0) && (angle < 2*M_PI) )
		y -= iconHeight;
	 

	if ((angle >= 3.0*M_PI/4.0) && (angle <= 5.0*M_PI/4.0) )
		x -= iconWidth;

	Pic( x, y, iconWidth, iconHeight, shader );

}
