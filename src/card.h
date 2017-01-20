#ifndef STUDIO_H
#define STUDIO_H

//	classes that contain data about studio files and what's in them

#include <string>
#include <fstream>

class Card {
	std::string pathname;
	std::string filename;
	unsigned int image_length;		//	size of image data, in bytes
	unsigned int file_length;		//	size of entire file, in bytes
	
	std::string hash;
	
	bool process_image(std::ifstream &);			//	handle PNG portion of file
//	bool process_card(std::ifstream &);			//	handle studio portion of file
public:
	Card(const std::string & FileName);
	bool process();
	std::string get_hash() { return hash; };
};

#endif