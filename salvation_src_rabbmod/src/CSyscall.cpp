// ETH32 - an Enemy Territory cheat for windows
// Copyright (c) 2007 eth32 team
// www.cheatersutopia.com & www.nixcoders.org

#include "eth32.h"

CSyscall Syscall;

void CSyscall::Cvar_Set(const char *var_name, const char *value)
{
	orig_syscall(CG_CVAR_SET, var_name, value);
}

void CSyscall::Cvar_VariableStringBuffer(const char *var_name, char *buffer, int bufsize)
{
	orig_syscall(CG_CVAR_VARIABLESTRINGBUFFER, var_name, buffer, bufsize);
}

int CSyscall::FS_FOpenFile(const char *path, fileHandle_t *f, fsMode_t mode) 
{
	return orig_syscall(CG_FS_FOPENFILE, path, f, mode);
}

void CSyscall::FS_FCloseFile(fileHandle_t f) 
{
	orig_syscall(CG_FS_FCLOSEFILE, f);
}

void CSyscall::FS_Read(void *buffer, int len, fileHandle_t f) 
{
	orig_syscall(CG_FS_READ, buffer, len, f);
}

void CSyscall::FS_Write(const void *buffer, int len, fileHandle_t f) 
{
	orig_syscall(CG_FS_WRITE, buffer, len, f);
}

void CSyscall::R_AddRefEntityToScene(const refEntity_t *re)
{
	orig_syscall(CG_R_ADDREFENTITYTOSCENE, re);
}

void CSyscall::R_DrawRotatedPic(float x, float y, float w, float h, float s1, float t1, float s2, float t2, qhandle_t hShader, float angle)
{
	orig_syscall(CG_R_DRAWROTATEDPIC, PASSFLOAT(x), PASSFLOAT(y), PASSFLOAT(w), PASSFLOAT(h), PASSFLOAT(s1), PASSFLOAT(t1), PASSFLOAT(s2), PASSFLOAT(t2), hShader, PASSFLOAT(angle));
}

void CSyscall::R_DrawStretchPic(float x, float y, float w, float h, float s1, float t1, float s2, float t2, qhandle_t hShader)
{
	orig_syscall(CG_R_DRAWSTRETCHPIC, PASSFLOAT(x), PASSFLOAT(y), PASSFLOAT(w), PASSFLOAT(h), PASSFLOAT(s1), PASSFLOAT(t1), PASSFLOAT(s2), PASSFLOAT(t2), hShader);
}

void CSyscall::R_SetColor(const float *rgba)
{
	orig_syscall(CG_R_SETCOLOR, rgba);
}

void CSyscall::Cmd_AddCommand(const char *cmdName)
{
	orig_syscall(CG_ADDCOMMAND, cmdName);
}

void CSyscall::Cmd_RemoveCommand(const char *cmdName)
{
	orig_syscall(CG_REMOVECOMMAND, cmdName);
}


qhandle_t CSyscall::R_RegisterShader(const char *name)
{
	return orig_syscall(CG_R_REGISTERSHADER, name);
}

qhandle_t CSyscall::R_RegisterShaderNoMip(const char *name)
{
	return orig_syscall(CG_R_REGISTERSHADERNOMIP, name);
}

qhandle_t CSyscall::RegisterModel( const char *name )
{
	qhandle_t retvalue;

	retvalue = ( qhandle_t )orig_syscall( CG_R_REGISTERMODEL, name );
	return( retvalue );
}

void	CSyscall::S_FadeAllSound( float targetvol, int time, qboolean stopsounds) 
{
	orig_syscall( CG_S_FADEALLSOUNDS, PASSFLOAT(targetvol), time, stopsounds);
}

sfxHandle_t	CSyscall::S_RegisterSound(const char *sample, qboolean compressed)
{
	return orig_syscall(CG_S_REGISTERSOUND, sample, qfalse);
}

void CSyscall::R_RegisterFont(const char *fontName, int pointSize, fontInfo_t *font)
{
	orig_syscall(CG_R_REGISTERFONT, fontName, pointSize, font);
}

void CSyscall::S_StartLocalSound(sfxHandle_t sfx, int channelNum)
{
	orig_syscall(CG_S_STARTLOCALSOUND, sfx, channelNum, 230);
}

int CSyscall::R_LerpTag(orientation_t *tag, const refEntity_t *refent, const char *tagName, int startIndex)
{
	return orig_syscall(CG_R_LERPTAG, tag, refent, tagName, startIndex);
}

void CSyscall::R_ModelBounds(clipHandle_t model, vec3_t mins, vec3_t maxs)
{
	orig_syscall(CG_R_MODELBOUNDS, model, mins, maxs);
}

void CSyscall::R_AddLightToScene(const vec3_t org, float radius,float intensity, float r, float g, float b, qhandle_t hShader, int flags)
{
	orig_syscall(CG_R_ADDLIGHTTOSCENE, org, PASSFLOAT(radius), PASSFLOAT(intensity), PASSFLOAT(r), PASSFLOAT(g), PASSFLOAT(b), hShader, flags);
}

void CSyscall::R_ProjectDecal(qhandle_t hShader,int numPoints, vec3_t *points,vec4_t projection, vec4_t color,int lifeTime, int fadeTime)
{
	orig_syscall(CG_R_PROJECTDECAL, hShader, numPoints, points, projection, color, lifeTime, fadeTime);

}

void CSyscall::CG_SendConsoleCommand(const char *text)
{
	orig_syscall(CG_SENDCONSOLECOMMAND, text);
}

bool CSyscall::CG_Key_IsDown(int keynum)
{
	return orig_syscall(CG_KEY_ISDOWN, keynum);
}

const char *CSyscall::UI_Argv(int arg)
{
	static char buffer[MAX_STRING_CHARS];
	orig_syscall(UI_ARGV, arg, buffer, sizeof(buffer));
	return buffer;
}

int CSyscall::UI_Argc()
{
	return orig_syscall(UI_ARGC);
}

void CSyscall::UI_ExecuteText(const char *buf, ...)
{
	char text[1024];

	va_list arglist;
	va_start(arglist, buf );
	vsprintf(text, buf, arglist);
	va_end(arglist);

	orig_syscall(UI_CMD_EXECUTETEXT, EXEC_APPEND, text);
}

const char *CSyscall::CG_Argv(int arg)
{
	static char buffer[MAX_STRING_CHARS];
	orig_syscall(CG_ARGV, arg, buffer, sizeof buffer);
	return buffer;
}

int CSyscall::CG_Argc()
{
	return orig_syscall(CG_ARGC);
}

void CSyscall::CG_Print(const char *txtFmt, ...)
{
	char text[1024];

	va_list arglist;
	va_start(arglist, txtFmt );
	vsprintf(text, txtFmt, arglist);
	va_end(arglist);

	orig_syscall(CG_PRINT, text);
};

void CSyscall::UI_Print(const char *txtFmt, ...)
{
	char text[1024];

	va_list arglist;
	va_start(arglist, txtFmt );
	vsprintf(text, txtFmt, arglist);
	va_end(arglist);

	orig_syscall(UI_PRINT, text);
};

void CSyscall::UI_Popup(int arg0) 
{
	orig_syscall(CG_INGAME_POPUP, arg0);
}

bool CSyscall::isKeyActionDown(const char *action)
{
	int key1, key2;
	orig_syscall(CG_KEY_BINDINGTOKEYS, action, &key1, &key2);

	if (Syscall.CG_Key_IsDown(key1) || Syscall.CG_Key_IsDown(key2))
		return true;
	else
		return false;
}

void CSyscall::CG_RemoveChatEscapeChar(char *text) {
	int i, l;

	l = 0;
	for ( i = 0; text[i]; i++ ) {
		if (text[i] == '\x19')
			continue;
		text[l++] = text[i];
	}
	text[l] = '\0';
}

#define	MAX_VA_STRING		32000
char* CSyscall::CG_TranslateString( const char *string ) {
	static char staticbuf[2][MAX_VA_STRING];
	static int bufcount = 0;
	char *buf;

	// some code expects this to return a copy always, even
	// if none is needed for translation, so always supply
	// another buffer

	buf = staticbuf[bufcount++ % 2];
	orig_syscall( CG_TRANSLATE_STRING, string, buf );

	return buf;
}

const char* CSyscall::CG_LocalizeServerCommand( const char *buf ) {
	static char token[MAX_TOKEN_CHARS];
	char temp[MAX_TOKEN_CHARS];
	qboolean togloc = qtrue;
	const char *s;
	int i, prev;

	memset( token, 0, sizeof( token ) );
	s = buf;
	prev = 0;

	for ( i = 0; *s; i++, s++ ) {
    
		if (*s == '[' && (!strncmp(s, "[lon]", 5) || !strncmp(s, "[lof]", 5))) {

			if (togloc) {
				memset(temp, 0, sizeof(temp));
				strncpy(temp, buf + prev, i - prev);
				strcat(token, CG_TranslateString(temp));
			}
			else {
				strncat(token, buf + prev, i - prev);
			}

			if (s[3] == 'n')
				togloc = qtrue;
			else
				togloc = qfalse;

			i += 5;
			s += 5;
			prev = i;
		}
	}

	if (togloc) {
		memset(temp, 0, sizeof(temp));
		strncpy(temp, buf + prev, i - prev);
		strcat(token, CG_TranslateString(temp));
	}
	else {
		strncat(token, buf + prev, i - prev);
	}

	return token;
}

void CSyscall::SendClientCommand(const char *fmt, ...)
{
	char text[1024];

	va_list arglist;
	va_start(arglist, fmt );
	vsprintf(text, fmt, arglist);
	va_end(arglist);
	
	orig_syscall(CG_SENDCLIENTCOMMAND, text);
}

void CSyscall::CM_BoxTrace( trace_t *results, const vec3_t start, const vec3_t end, const vec3_t mins, const vec3_t maxs, clipHandle_t model, int brushmask ) 
{
	orig_syscall( CG_CM_BOXTRACE, results, start, end, mins, maxs, model, brushmask );
}
