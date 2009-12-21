// ETH32 - an Enemy Territory cheat for windows
// Copyright (c) 2007 eth32 team
// www.cheatersutopia.com & www.nixcoders.org
// (ported to unix by kobj and blof)

static et_t etlist[] =
{
	{
		"2.55",                 // version
		0x21e60afb,             // crc32
		ET_255,					// ver

		0x8ffc06c,              // viewAngleX
		0x8ffc068,              // viewAngleY
  		0x8628a80,				// inButtons
		0x8e24360,				// glConfig
		0x91682bc,				// com_frameTime
 	 	0x9163d4c,				// consoleshader

		0x80b8e82,				// etroq video
		" ",					// etroq safe

  		0x80f4818,              // syscall
  		0x80bab60,              // Cvar_Get
		0x80bae24,				// Cvar_Set2
		0x87cd9c8,            	// fs_searchpaths
		0x80c1d68,            	// FS_PureServerSetLoadedPaks
  		0x80C0ba8,				// FS_AddGameDirectory
		0x0,					// FS_AddGameDirectory2
  		0x80C1598,				// FS_Startup
		0x0,					// FS_Startup2
  		0x80b561c,             	// Cmd_AddCommand
		0x0,					// VM_Create
		0x0,					// Sys_LoadDll
  		0x8069390,				// LoadTGA
		0x8f451d0,				// server_ip
  		0x80a7658,				// SCR_UpdateScreen				<---------- its CALL 20 in bad cgame system trap, NOT 17!
		0x80972b0,				// Con_DrawConsole
		0x821c020,				// currentVm
		0x80bba24,				// Cvar_VariableString
		// aimhack
		25, 0x0809faf6, "\xe8\x55\x1b\xff\xff\xe8\x58\x7b\x00\x00\xe8\x0f\x19\x03\x00\xe8\x5e\xa2\xff\xff\xe8\x31\xe9\xff\xff",
	},
	{
		"2.56",                 // version
  		0x3d59a703,       		// crc32
		ET_256,					// ver

		0x08ffc668,             // viewAngleX
		0x08ffc66c,             // viewAngleY
  		0x8629060,				// inButtons
  		0x8e24960,				// glConfig
		0x91688bc,				// com_frameTime
  		0x916434c,				// consoleshader

		0x80b88f6,				// etroq video
		" ",					// etroq safe

  		0x80f4a80,               // syscall
  		0x080ba5d8,             // Cvar_Get
		0x080ba89c,				// Cvar_Set2
		0x087cdfa8,             // fs_searchpaths
		0x080c17e0,             // FS_PureServerSetLoadedPaks
  		0x0804d1e8,				// FS_AddGameDirectory
		0x0,					// FS_AddGameDirectory2
  		0x080c1010,				// FS_Startup
		0x0,					// FS_Startup2
		0x080b5098,             // Cmd_AddCommand
		0x0,					// VM_Create
		0x0,					// Sys_LoadDll
  		0x08068f38,				// LoadTGA
		0x08f457d0,				// server_ip
		0x0,					// SCR_UpdateScreen
  		0x08096d3c,				// Con_DrawConsole
  		0x821c628,				// currentVm
  		0x080bb49c,				// Cvar_VariableString
  		// aimhack
  		0, 0, "",
	},
	{
		"2.60",                 // version
		0x3b18a889,             // crc32
		ET_260,					// ver

		0x0906c1cc,             // viewAngleX
		0x0906c1c8,             // viewAngleY
  		0x8cb1860,				// inButtons
		0x8e85500,				// glConfig
  		0x8e366b8,				// com_frameTime
  		0x925beec,				// consoleshader

		0x0806d1fc,				// etroq video
		" ",					// etroq safe

  		0x8084f50,              // syscall
  		0x0806fcb0,             // Cvar_Get
		0x08070010,				// Cvar_Set2
		0x0888c7c8,             // fs_searchpaths
		0x08076570,             // FS_PureServerSetLoadedPaks
  		0x080788d0,				// FS_AddGameDirectory
		0x0,					// FS_AddGameDirectory2
  		0x08078290,				// FS_Startup
		0x0,					// FS_Startup2
		0x08069240,             // Cmd_AddCommand
		0x0,					// VM_Create
		0x0,					// Sys_LoadDll
  		0x0810abd0,				// LoadTGA
		0x09036108,				// server_ip
  		0x08149ec0,				// SCR_UpdateScreen
		0x08138c30,				// Con_DrawConsole
		0x88b1920,				// CurrentVM
		0x080709f0,				// Cvar_VariableString
		// aimhack
		25, 0x08141424, "\xe8\x17\x0e\xff\xff\xe8\x92\x8a\x00\x00\xe8\xcd\xfb\x00\x00\xe8\x68\x9b\xff\xff\xe8\x53\xe6\xff\xff",
	},
	{
		"2.60b",                // version
  		0x6ab49f82,         	// crc32
		ET_260b,				// ver

		0x0907d28c,             // viewAngleX
		0x0907d288,             // ViewAngleY
  		0x8cc27c0,				// inButtons

		0x8e965c0,				// glConfig
  		0x8e47778,				// com_frameTime
		0x926cfa8,				// consoleshader

  		0x08071fcd,				// etroq video
		" ",					// etroq safe

  		0x08089590,        		// syscall
  		0x08073bb0,             // Cvar_Get
		0x08073160,				// Cvar_Set2
		0x088b1f18,             // fs_searchpaths
		0x0807cef0,             // FS_PureServerSetLoadedPaks
  		0x0,					// FS_AddGameDirectory
		0x0807bcd0,				// FS_AddGameDirectory2
  		0x0,					// FS_Startup
		0x0807c5a0,				// FS_Startup2
		0x0806b640,             // Cmd_AddCommand
		0x0,					// VM_Create
		0x0,					// Sys_LoadDll
  		0x0810ef30,				// LoadTGA
		0x090471c8,				// server_ip
  		0x08150b30,				// SCR_UpdateScreen
		0x0813e100,				// Con_DrawConsole
		0x88c2824,				// CurrentVM
		0x08072f00,				// Cvar_VariableString
		
		// aimhack
		28, 0x814db3e, "\xe8\x0d\xa7\xfe\xff\xe8\xe8\x2f\x00\x00\xe8\x13\xb8\x00\x00\x8d\x76\x00\xe8\xdb\x33\xff\xff\xe8\x96\x70\xff\xff",
		
	},
};
