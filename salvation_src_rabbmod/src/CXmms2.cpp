// ETH32 - an Enemy Territory cheat for windows
// Copyright (c) 2007 eth32 team
// www.cheatersutopia.com & www.nixcoders.org

// Added - 2009 by Avoloos

#include "eth32.h"
#include "settings.h"
#include <boost/lexical_cast.hpp>

/*
 * TODO: Errorhandling... Sry but at the moment quick and dirty...
 */

CXmms2::CXmms2() : client("EnemyTerritory"){
    this->Init();
}

Xmms::PropDict CXmms2::getCurrentMediaInfo( )
{
    return static_cast<Xmms::PropDict>(this->client.medialib.getInfo( this->client.playback.currentID() ));
}

void CXmms2::connect(){
    if(!this->connected){
        this->client.connect( std::getenv("XMMS_PATH") );
        this->connected = true;
    }
}

template <class myType>
myType CXmms2::getInfo(const char* key ){
    Xmms::PropDict info = this->getCurrentMediaInfo();
    myType temp;

    try {
        temp = boost::get< myType >(info[key]);
	}
	catch( Xmms::no_such_key_error& err ) {
	    return NULL;
	}

    return temp;
}

int CXmms2::getCurrentTime(){
    return this->client.playback.getPlaytime();
}

void CXmms2::printSongInfo(){
    std::string output;

    output.append("^2");
	output.append("Artist = ");
	output.append(this->getInfo<std::string>("artist"));
	output.append("\n^2");
	output.append("Title = ");
	output.append(this->getInfo<std::string>("title"));
	output.append("\n");
	output.append("^2Duration = ");
	output.append(boost::lexical_cast<std::string>(this->getInfo<int>("duration")));
	output.append("\n");

    Tools.Print(output.c_str());
}

void CXmms2::play()
{
    if(this->client.playback.getStatus() == XMMS_PLAYBACK_STATUS_STOP || this->client.playback.getStatus() == XMMS_PLAYBACK_STATUS_PAUSE){
        this->client.playback.start();
    }else{
        this->client.playback.pause();
    }
}

void CXmms2::stop()
{
    this->client.playback.stop();
}

void CXmms2::next()
{
    this->stop();
    this->client.playlist.setNextRel(1);
    this->play();
}

void CXmms2::prev()
{
    this->stop();
    this->client.playlist.setNextRel(-1);
    this->play();
}

void CXmms2::Init(){
    this->connected = false;
    this->connect();
}


/*
 * Not nice... but we _need_ functions to work.
 */
CXmms2* xmms2;

void CXmms2_ET_Play(){
    xmms2->play();
}

void CXmms2_ET_Prev(){
    xmms2->prev();
}

void CXmms2_ET_Next(){
    xmms2->next();
}
void CXmms2_ET_Stop(){
    xmms2->stop();
}
void CXmms2_ET_Info(){
    xmms2->printSongInfo();
}

void CXmms2_ET_Init(){
    xmms2 = new CXmms2();

    orig_Cmd_AddCommand("xmms2_play", &CXmms2_ET_Play);
    orig_Cmd_AddCommand("xmms2_stop", &CXmms2_ET_Stop);
    orig_Cmd_AddCommand("xmms2_next", &CXmms2_ET_Next);
    orig_Cmd_AddCommand("xmms2_prev", &CXmms2_ET_Prev);
    orig_Cmd_AddCommand("xmms2_info", &CXmms2_ET_Info);
}

// Added - 2009 by Avoloos
