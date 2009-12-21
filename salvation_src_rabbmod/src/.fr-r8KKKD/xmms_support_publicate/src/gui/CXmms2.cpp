// ETH32 - an Enemy Territory cheat for windows
// Copyright (c) 2007 eth32 team
// www.cheatersutopia.com & www.nixcoders.org

#include "../eth32.h"

#define BSCALE GUI_FONTSCALE*eth32.settings.BannerScale

CXmms2_GUI::CXmms2_GUI(const char *label, int wx, int wy, int ww, int wh)
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
 * [a] - Artist
 * [A] - Album
 * [t] - Title
 * [d] - Duration
 * [c] - Current Time
 * [p] - progressBar
 *
 */

 // special multi-line care
 // if more lines remaining, returns true, else false
bool CXmms2_GUI::generateMessage()
{
	char buf[255];
	char b[2];

    int progress;
    int i;
    int progressBarSize = 4;
    std::string temp;

	line[cline][0] = b[1] = '\0';
	char *l = line[cline];

	while (*bfmt) {
		if (*bfmt == '&' && *(bfmt+1) == '&') {
			bfmt+=2;
			return true;
		}

		if ( *bfmt == '[' && *(bfmt+1) && *(bfmt+2) == ']' ) {
			switch (*(bfmt+1)) {
			    case 'a':
                    try{
                        sprintf(buf, "%s", (xmms2->getInfo<std::string>("artist")).c_str());
                    }catch(...){
                        sprintf(buf, "%s", "Unknown");
                    }

                    strcat(l, buf);
                    bfmt+=3;
                    continue;
                case 'A':
                    try{
                        sprintf(buf, "%s", (xmms2->getInfo<std::string>("album")).c_str());
                    }catch(...){
                        sprintf(buf, "%s", "Unknown");
                    }

                    strcat(l, buf);
                    bfmt+=3;
                    continue;
                case 't':
                    try{
                        sprintf(buf, "%s", (xmms2->getInfo<std::string>("title")).c_str());
                    }catch(...){
                        sprintf(buf, "%s", "Unknown");
                    }

                    strcat(l, buf);
                    bfmt+=3;
                    continue;
                case 'd':
                    try{
                        sprintf(buf, "%i", xmms2->getInfo<int>("duration"));
                    }catch(...){
                        sprintf(buf, "%s", "Unknown");
                    }

                    strcat(l, buf);
                    bfmt+=3;
                    continue;
                case 'c':
                    try{
                        sprintf(buf, "%i", xmms2->getCurrentTime());
                    }catch(...){
                        sprintf(buf, "%s", "Unknown");
                    }

                    strcat(l, buf);
                    bfmt+=3;
                    continue;
                case 'p':
                    progress = ((xmms2->getCurrentTime()*100)/xmms2->getInfo<int>("duration")) / progressBarSize;
                    temp = "[";

                    for(i = 1; i < (100/progressBarSize); i++)
                    {
                        if(i <= progress){
                            temp.append(".");
                        }else{
                            temp.append(" ");
                        }
                    }

                    temp.append("]");

                    strcat(l, temp.c_str());
                    bfmt+=3;
                    continue;
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

void CXmms2_GUI::Display(void)
{
	if (!eth32.settings.guiBanner)
		return;

	CWindow::Display();

	if (eth32.cg.time - lastUpd > updInterval){
		nlines = cline = 0;

		bfmt = eth32.settings.Xmms2Fmt;
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
