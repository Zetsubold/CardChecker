#pragma once

#include "thing.h" // Base class: Thing

class Chara : protected Thing
{
private:
	std::string class_name;
	
protected:

	Chara(std::ifstream & f, const std::string &);
	bool read_string(const int &);
		
public:
	~Chara();
	
	virtual bool process() = 0;

	bool write(std::ofstream &, int limit = -1 );

	friend std::ostream& operator<< (std::ostream &, const Chara *);
	
	int ignore_length();
	
	using Thing::add_ignore;
	using Thing::clear_ignore;
	
	using Thing::get_composed_length;
	using Thing::compose;
	
	inline void dump_segments() { Thing::dump_segments(); };
};

