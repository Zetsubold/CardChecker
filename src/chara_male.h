#pragma once

#include "chara.h" 		// Base class: Chara

//	contain data about an Male object

class Male : public Chara
{
public:
	Male(std::ifstream &f);
	~Male();
	bool process();
};

