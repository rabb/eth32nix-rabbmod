// ETH32 - an Enemy Territory cheat for windows
// Copyright (c) 2007 eth32 team
// www.cheatersutopia.com & www.nixcoders.org

class CSyscall
{
public:
	void		Cvar_Set(const char *var_name, const char *value);
	void		Cvar_VariableStringBuffer(const char *var_name, char *buffer, int bufsize);
	int			FS_FOpenFile(const char *path, fileHandle_t *f, fsMode_t mode);
	void		FS_FCloseFile(fileHandle_t f);
	void		FS_Read(void *buffer, int len, fileHandle_t f);
	void		FS_Write(const void *buffer, int len, fileHandle_t f);
	void		R_AddRefEntityToScene(const refEntity_t *re);
	void		R_DrawRotatedPic(float x, float y, float w, float h, float s1, float t1, float s2, float t2, qhandle_t hShader, float angle);
	void		R_DrawStretchPic(float x, float y, float w, float h, float s1, float t1, float s2, float t2, qhandle_t hShader);
	void		R_SetColor(const float *rgba);
	void		Cmd_AddCommand(const char *cmdName);
	void		Cmd_RemoveCommand(const char *cmdName);
	qhandle_t	R_RegisterShader(const char *name);
	qhandle_t	RegisterModel( const char *name );
	qhandle_t	R_RegisterShaderNoMip(const char *name);
	void		S_FadeAllSound( float targetvol, int time, qboolean stopsounds);
	sfxHandle_t	S_RegisterSound(const char *sample, qboolean compressed);
	void		R_RegisterFont(const char *fontName, int pointSize, fontInfo_t *font);
	void		S_StartLocalSound(sfxHandle_t sfx, int channelNum);
	int			R_LerpTag(orientation_t *tag, const refEntity_t *refent, const char *tagName, int startIndex);
	void		R_ModelBounds(clipHandle_t model, vec3_t mins, vec3_t maxs);
	void		R_AddLightToScene(const vec3_t org, float radius,float intensity, float r, float g, float b, qhandle_t hShader, int flags);
	void		R_ProjectDecal(qhandle_t hShader,int numPoints, vec3_t *points,vec4_t projection, vec4_t color,int lifeTime, int fadeTime);
	void		CG_SendConsoleCommand(const char *text);
	bool		CG_Key_IsDown(int keynum);
	const char	*UI_Argv(int);
	int			UI_Argc();
	void		UI_ExecuteText(const char *buf, ...);
	const char	*CG_Argv(int);
	int			CG_Argc();
	void		CG_Print(const char *txtFmt, ...);
	void		UI_Print(const char *txtFmt, ...);
	void		UI_Popup(int arg0);
	bool		isKeyActionDown(const char *action);
	void		CG_RemoveChatEscapeChar(char *text);
	char		*CG_TranslateString(const char *string);
	const char	*CG_LocalizeServerCommand(const char *buf);
	void		SendClientCommand(const char *fmt, ...);
	void		CM_BoxTrace( trace_t *results, const vec3_t start, const vec3_t end, const vec3_t mins, const vec3_t maxs, clipHandle_t model, int brushmask ) ;
};

extern CSyscall Syscall;
