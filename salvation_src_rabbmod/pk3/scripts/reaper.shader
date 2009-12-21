models/players/reaper/reaper
{	

	{	map models/players/reaper/specmap.tga
		rgbGen lightingDiffuse
	}
	{	map models/players/reaper/spec.jpg
		blendfunc GL_DST_COLOR GL_ZERO
		tcGen environment
	}
	{
		map models/players/reaper/reaper.jpg
		blendfunc GL_ONE GL_ONE
		rgbGen lightingDiffuse
	}
	{
		map models/players/reaper/eye_glow.jpg
		blendfunc GL_ONE GL_ONE
        }
}