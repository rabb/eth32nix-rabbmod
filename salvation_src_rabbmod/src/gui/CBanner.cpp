// ETH32 - an Enemy Territory cheat for windows
// Copyright (c) 2007 eth32 team
// www.cheatersutopia.com & www.nixcoders.org

#include "../eth32.h"

#define BSCALE GUI_FONTSCALE*eth32.settings.BannerScale

CBanner::CBanner(const char *label, int wx, int wy, int ww, int wh)
: CWindow(label, wx, wy, ww, wh)
{
	showTitle = false;
	updInterval = 500;
	lastUpd = 0;
	nlines = cline = 0;
	memset(line, 0, sizeof(line));
	bannerTextWidth = 0;
	
	currWidth = ww;
	currHeight = wh;  
}

/* 
 *	banner format keys
 * 
 * [n] - our name
 * [h] - our health
 * [a] - our ammo
 * [s] - no. of PB screenshots taken
 * [k] - kills
 * [d] - deaths
 * [r] - kill/death ratio
 * [P] - ping
 * [p] - longest spree
 * [S] - current spree
 * [N] - servername
 * [w] - worst enemy
 * [l] - last killer
 * 
 * [.] - bullets fired
 * [,] - bullets hit 
 * [/] - headshots
 * [\] - accuracy
 * [<] - headshots per kill
 * [>] - headshots per hits
 * [?] - headshots per shots 
 * 
 * [L] - no. of teammates in limbo
 * 
 */
 
 // special multi-line care 
 // if more lines remaining, returns true, else false
bool CBanner::generateMessage()
{	
	char buf[64];
	char b[2];
	
	line[cline][0] = b[1] = '\0';
	char *l = line[cline];
	
	while (*bfmt) {
		if (*bfmt == '&' && *(bfmt+1) == '&') {
			bfmt+=2;
			return true;	
		}
		
		if ( *bfmt == '[' && *(bfmt+1) && *(bfmt+2) == ']' ) {
			switch (*(bfmt+1)) {
				case 'n':
					strcat(l, eth32.cg.self->name); 
					bfmt+=3;
					continue;
				case 'h':
					sprintf(buf, "%i", eth32.cg.snap->ps.stats[STAT_HEALTH]);
					//sprintf(buf, "%i", *eth32.cg.self->health);
					strcat(l, buf); 
					bfmt+=3;
					continue;
				case 'a':
					if (getAmmo()) {
						sprintf(buf, "%i/%i", clip, ammo);
						strcat(l, buf);
					}
					bfmt+=3;
					continue;
				case 's':
					sprintf(buf, "%i", eth32.cg.pbss);
					strcat(l, buf); 
					bfmt+=3;
					continue;
				case 'P':
					sprintf(buf, "%i", eth32.cg.snap->ping);
					strcat(l, buf); 
					bfmt+=3;
					continue;	
				case 'k':
					sprintf(buf, "%i", eth32.cg.self->stats.kills);
					strcat(l, buf);
					bfmt+=3;
					continue;
				case 'd':
					sprintf(buf, "%i", eth32.cg.self->stats.deaths);
					strcat(l, buf);
					bfmt+=3;
					continue;
				case 'p':
					sprintf(buf, "%i", eth32.cg.self->stats.longestSpree);
					strcat(l, buf);
					bfmt+=3;
					continue;
				case 'S':
					sprintf(buf, "%i", eth32.cg.self->stats.spree);
					//sprintf(buf, "%i", eth32.killSpreeCount);
					strcat(l, buf);
					bfmt+=3;
					continue;
				case 'r':
					sprintf(buf, "%.1f", eth32.cg.self->stats.kdRatio);
					strcat(l, buf);
					bfmt+=3;
					continue;
				case 'N':
					strcat(l, eth32.cg.serverName);
					bfmt+=3;
					continue;																																	
				case 'l':
					if (eth32.cg.self->stats.killer >=0)
						strcat(l, eth32.cg.players[eth32.cg.self->stats.killer].name);
					bfmt+=3;
					continue;
				case '.': {
					sprintf(buf, "%i", eth32.cg.self->stats.bulletsFiredWS);
					strcat(l, buf);
					bfmt+=3;
					continue;					
				}
				case ',': {
					sprintf(buf, "%i", eth32.cg.self->stats.bulletsHitWS);
					strcat(l, buf);
					bfmt+=3;
					continue;					
				} 
				case '/': {
					sprintf(buf, "%i", eth32.cg.self->stats.headshotsWS);
					strcat(l, buf);
					bfmt+=3;
					continue;					
				} 
				case '<': {
					sprintf(buf, "%.1f", eth32.cg.self->stats.hr_kills);
					strcat(l, buf);
					bfmt+=3;
					continue;					
				}
				case '\\': {
					sprintf(buf, "%.1f", eth32.cg.self->stats.bulletAcc);
					strcat(l, buf);
					bfmt+=3;
					continue;					
				}				
				case '>': {
					sprintf(buf, "%.1f", eth32.cg.self->stats.hr_hits);
					strcat(l, buf);
					bfmt+=3;
					continue;					
				} 
				case '?': {
					sprintf(buf, "%.1f", eth32.cg.self->stats.hr_shots);
					strcat(l, buf);
					bfmt+=3;
					continue;					
				}					
				case 'L': {
					sprintf(buf, "%i", eth32.cg.limbo);
					strcat(l, buf);
					bfmt+=3;
					continue;					
				}	
				default:
					bfmt+=3;
					continue;				
			}
		} else {
			b[0] = *bfmt;
			strcat(l, b);
			bfmt++;
		}
	}
	
	return false;
}

bool CBanner::getAmmo()
{
	ammo = clip = -1;

	if (!orig_BG_FindClipForWeapon || !orig_BG_FindAmmoForWeapon)
		return false;

	if (!eth32.cg.snap || !eth32.cg.snap->ps.weapon)
		return false;

	if (eth32.cg.snap->ps.eFlags & EF_MG42_ACTIVE || eth32.cg.snap->ps.eFlags & EF_MOUNTEDTANK)
		return false;

	int weap = eth32.cg.snap->ps.weapon;

	if (IS_WEAPATTRIB(weap, WA_NO_AMMO))
		return false;

	clip = eth32.cg.snap->ps.ammoclip[orig_BG_FindClipForWeapon(weap)];

	if (IS_WEAPATTRIB(weap, WA_ONLY_CLIP))
		return true;

	ammo = eth32.cg.snap->ps.ammo[orig_BG_FindAmmoForWeapon(weap)];

	return true;	
}
 
void CBanner::Display(void)
{
	if (!eth32.settings.guiBanner)
		return;
		
	CWindow::Display();	
	
	if (eth32.cg.time - lastUpd > updInterval){
		nlines = cline = 0;
		bfmt = eth32.settings.BannerFmt;
		bannerTextWidth = 0;
		bannerTextHeight = 0;
		
		while (1) {
			bool more = generateMessage();
			
			int lineWidth = Draw.sizeTextWidth(BSCALE, line[cline], GUI_FONT);
			bannerTextWidth = lineWidth > bannerTextWidth ? lineWidth : bannerTextWidth;
			
			cline++;
			nlines++;	
			
			if (!more)
				break;	
		}
		bannerTextHeight = (Draw.sizeTextHeight(BSCALE, GUI_FONT)  + GUI_TEXTSPACING)*nlines;
		
		Resize( bannerTextWidth+17, bannerTextHeight+10 );
		
		lastUpd = eth32.cg.time;	
	}

	int yPos = y + 5;
	for (int i=0; i<nlines; i++) {
		if (line[i][0]) {
			Draw.Text(x + 5, yPos, BSCALE, line[i], GUI_FONTCOLOR1, qfalse, qfalse, GUI_FONT, true);			
			yPos += Draw.sizeTextHeight(BSCALE, GUI_FONT)  + GUI_TEXTSPACING;
		}
	}	
}
