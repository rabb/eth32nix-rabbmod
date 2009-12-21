//eth32nix UI Shaders 
// Credits: *nixCoders Team 2008

eth32logo
{
	nopicmip
	{
		map gfx/2d/eth32_logo.tga
		blendfunc blend
		alphaGen const 0.5
	}
}

eth32logo2
{
	nomipmaps
	nopicmip
	{
		clampmap gfx/2d/eth32_logo.tga
		blendfunc blend
		alphaGen const 0.5
	}
}

nullshader
{
	nomipmaps
	nopicmip
	{
		map gfx/2d/nullshader.tga
		blendfunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen vertex
	}
}
