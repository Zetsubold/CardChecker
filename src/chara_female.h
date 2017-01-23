#pragma once

#include "chara.h" 		// Base class: Chara

//	contain data about an Female object

class Female : public Chara
{
public:
	Female(std::ifstream &f);
	~Female();
	bool process();
};
