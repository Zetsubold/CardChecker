#ifndef STUDIO_H
#define STUDIO_H

#include <string>
#include <fstream>

#include "thing.h"
#include "chara.h"

//	classes that contain data about character cards and what's in them

class Card {
private:
	std::string pathname;
	std::string filename;
	
	std::streamoff image_length;	//	size of image data, in bytes
	std::streamoff file_length;		//	size of entire file, in bytes
	std::streamoff data_length;		//  size of normal character block
	
	Chara * chara_data;
	
	std::string hash;
	
	bool process_image(std::ifstream &);			//	handle PNG portion of file
	bool process_data(std::ifstream &);			//	handle card portion of file

	bool save_chara;
	
public:
	Card(const std::string & FileName, bool sc = false);
	bool process();
	std::string get_hash() { return hash; };
};

#endif