// This file needs to be included in the client pk3


// Helmet ranks

helmet/ranks/rank1
{
	{
		map models/players/temperate/common/rank1.tga
		alphaFunc GE128
		rgbGen lightingDiffuse
	}
}
helmet/ranks/rank2
{
	{
		map models/players/temperate/common/rank2.tga
		alphaFunc GE128
		rgbGen lightingDiffuse
	}
}
helmet/ranks/rank3
{
	{
		map models/players/temperate/common/rank3.tga
		alphaFunc GE128
		rgbGen lightingDiffuse
	}
}
helmet/ranks/rank4
{
	{
		map models/players/temperate/common/rank4.tga
		alphaFunc GE128
		rgbGen lightingDiffuse
	}
}
helmet/ranks/rank5
{
	{
		map models/players/temperate/common/rank5.tga
		alphaFunc GE128
		rgbGen lightingDiffuse
	}
}
helmet/ranks/rank6
{
	{
		map models/players/temperate/common/rank6.tga
		alphaFunc GE128
		rgbGen lightingDiffuse
	}
}
helmet/ranks/rank7
{
	{
		map models/players/temperate/common/rank7.tga
		alphaFunc GE128
		rgbGen lightingDiffuse
	}
}
helmet/ranks/rank8
{
	{
		map models/players/temperate/common/rank8.tga
		alphaFunc GE128
		rgbGen lightingDiffuse
	}
}
helmet/ranks/rank9
{
	{
		map models/players/temperate/common/rank9.tga
		alphaFunc GE128
		rgbGen lightingDiffuse
	}
}
helmet/ranks/rank10
{
	{
		map models/players/temperate/common/rank10.tga
		alphaFunc GE128
		rgbGen lightingDiffuse
	}
}
helmet/ranks/rank11
{
	{
		map models/players/temperate/common/rank11.tga
		alphaFunc GE128
		rgbGen lightingDiffuse
	}
}

// Weapon icons

icons/iconw_landmine_1_select
{
	nomipmaps
	nopicmip
	{
		map icons/iconw_landmine_1_select.tga
		blendFunc blend
		rgbGen vertex
	}
}

icons/iconw_m1_garand_1_select
{
	nomipmaps
	nopicmip
	{
		map icons/iconw_m1_garand_1_select.tga
		blendFunc blend
		rgbGen vertex
	}
}

icons/iconw_m1_garand_gren_1_select
{
	nomipmaps
	nopicmip
	{
		map icons/iconw_m1_garand_gren_1_select.tga
		blendFunc blend
		rgbGen vertex
	}
}

icons/iconw_kar98_1_select
{
	nomipmaps
	nopicmip
	{
		map icons/iconw_kar98_1_select.tga
		blendFunc blend
		rgbGen vertex
	}
}

icons/iconw_kar98_gren_1_select
{
	nomipmaps
	nopicmip
	{
		map icons/iconw_kar98_gren_1_select.tga
		blendFunc blend
		rgbGen vertex
	}
}

icons/iconw_fg42_1_select
{
	nomipmaps
	nopicmip
	{
		map icons/iconw_fg42_1_select.tga
		blendFunc blend
		rgbGen vertex
	}
}

icons/iconw_mg42_1_select
{
	nomipmaps
	nopicmip
	{
		map icons/iconw_mg42_1_select.tga
		blendFunc blend
		rgbGen vertex
	}
}

icons/iconw_mortar_1_select
{
	nomipmaps
	nopicmip
	{
		map icons/iconw_mortar_1_select.tga
		blendFunc blend
		rgbGen vertex
	}
}

icons/iconw_syringe_1_select
{
	nomipmaps
	nopicmip
	{
		map icons/iconw_syringe_1_select.tga
		blendFunc blend
		rgbGen vertex
	}
}

icons/iconw_syringe2_1_select
{
	nomipmaps
	nopicmip
	{
		map icons/iconw_syringe2_1_select.tga
		blendFunc blend
		rgbGen vertex
	}
}

icons/iconw_binoculars_1_select
{
	nomipmaps
	nopicmip
	{
		map icons/iconw_binoculars_1_select.tga
		blendFunc blend
		rgbGen vertex
	}
}

icons/iconw_satchel_1_select
{
	nomipmaps
	nopicmip
	{
		map icons/iconw_satchel_1_select.tga
		blendFunc blend
		rgbGen vertex
	}
}

icons/iconw_radio_1_select
{
	nomipmaps
	nopicmip
	{
		map icons/iconw_radio_1_select.tga
		blendFunc blend
		rgbGen vertex
	}
}

icons/iconw_silencer_1_select
{
	nomipmaps
	nopicmip
	{
		map icons/iconw_silencer_1_select.tga
		blendFunc blend
		rgbGen vertex
	}
}

// Lag icon

gfx/2d/net
{
        nocompress
        nopicmip
        {
                map gfx/2d/net.jpg
                blendfunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
        }
}

// Floating disguise icon

gfx/2d/friendlycross
{
        nocompress
        nopicmip
        {
                map gfx/2d/friendlycross.tga
                blendfunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
        }
}

// Machine gun bullet brass

models/weapons2/shells/m_shell
{
	{
		map models/weapons2/shells/M_shell.jpg
		blendFunc GL_ONE GL_ONE_MINUS_SRC_ALPHA
		rgbGen lightingDiffuse 
	}
	{
		map models/weapons2/shells/M_shell.jpg
		blendFunc GL_ZERO GL_ONE
		rgbGen lightingDiffuse 
	}
}

// Goldrush bank floor shader was missing, this gives a nice reflection.
textures/egypt_floor_sd/marblefloor_c04
{
	{
		map textures/goldrush-env.tga
		rgbGen identity
		tcGen environment
		tcMod scale 0.9 0.9
	}
	{
		map textures/egypt_floor_sd/marblefloor_c04.tga
		blendfunc blend
		rgbGen identity
	}
	{
		map $lightmap
		blendfunc filter
		rgbGen identity
	}
}

// forty - transparent gun models

textures/sfx/transgunRed
{ 
     noPicmip 
     surfaceparm trans 
     { 
          map textures/sfx/construction.tga
          blendFunc GL_SRC_ALPHA GL_ONE
          rgbgen const ( 0.35 0.10 0.10 )
          tcMod scroll 0.025 0.025 
     } 
}

textures/sfx/transgunGreen
{ 
     noPicmip 
     surfaceparm trans 
     { 
          map textures/sfx/construction.tga
          blendFunc GL_SRC_ALPHA GL_ONE
          rgbgen const ( 0.10 0.35 0.10 )
          tcMod scroll 0.025 0.025 
     } 
}

textures/sfx/transgunBlue
{ 
     noPicmip 
     surfaceparm trans 
     { 
          map textures/sfx/construction.tga
          blendFunc GL_SRC_ALPHA GL_ONE
          rgbgen const ( 0.10 0.10 0.35 )
          tcMod scroll 0.025 0.025 
     } 
}

textures/sfx/transgunWhite
{ 
     noPicmip 
     surfaceparm trans 
     { 
          map textures/sfx/construction.tga
          blendFunc GL_SRC_ALPHA GL_ONE
          rgbgen const ( 0.35 0.35 0.35 )
          tcMod scroll 0.025 0.025 
     } 
}

