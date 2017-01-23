#pragma once

#include <string>
#include <map>
#include <vector>

#include "dir_list.h"

class CardFolder
{
	std::multimap< std::string, std::string > card_hashes;

	typedef std::multimap< std::string, std::string >::iterator map_iterator;

	unsigned int dupes_found;
	unsigned int dupe_groups;
public:
	CardFolder();
	
	bool construct(std::vector<FileData> & file_list, bool save_chara = false);
	void process(const std::string & filename);

	unsigned int get_count() { return dupes_found; } ;
	~CardFolder();
};

