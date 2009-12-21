// ETH32 - an Enemy Territory cheat for windows
// Copyright (c) 2007 eth32 team
// www.cheatersutopia.com & www.nixcoders.org

// the nature of member function pointers and C function pointers
// prevent this from being really C++. Oh well, so long it works, right?

class CXCommands
{
public:
	CXCommands(void);

	void RegisterCommands();

private:
	bool cmdsRegistered;
};

extern CXCommands CCommands;
