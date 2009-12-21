// ETH32 - an Enemy Territory cheat for windows
// Copyright (c) 2007 eth32 team
// www.cheatersutopia.com & www.nixcoders.org

#ifndef CXMMS2_H_INCLUDED
#define CXMMS2_H_INCLUDED

#pragma once

class CXmms2
{
public:
        CXmms2(void);
        void Init();

        void play();
        void stop();
        void next();
        void prev();

        template <class myType>
        myType getInfo(const char*);

        void printSongInfo();
        int getCurrentTime();

private:
        Xmms::Client client;
        bool connected;

        void connect();
        Xmms::PropDict getCurrentMediaInfo();
};

void CXmms2_ET_Init();
void CXmms2_ET_Play();
void CXmms2_ET_Prev();
void CXmms2_ET_Next();
void CXmms2_ET_Stop();
void CXmms2_ET_Info();

#endif
// Added - 2009 by Avoloos
