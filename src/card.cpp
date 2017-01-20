#include <cstdio>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <iomanip>
#include <sstream>

#include "card.h"
#include "logging.h"

#include "mhash.h"

using namespace std;

//	test data should hash to
//	1a77fcf7b35367d0d390e554bda9703af83a4597
	
string hash2string (unsigned char * hash) {
	int count = mhash_get_block_size(MHASH_SHA1);
	
	ostringstream build;
	
	for (int i=0; i<count; i++) 
		build << hex << static_cast<unsigned int>(hash[i]);
	
	string result = build.str();
	
	return result;
}
Card::Card( const string & FileName )
{
	string::size_type pos = FileName.find_last_of("\\");
	pathname = FileName.substr( 0, pos+1 );
	filename = FileName.substr( pos+1, FileName.length() );
	
	image_length = 0;
	file_length = 0;
	
	if ( loglevel==logging::extra )
		logfile << "Card::Card\n-- [PATH] " << pathname << "\n-- [FILE] " << filename << endl;
}

unsigned int read_BE32 (ifstream & file)
{
	unsigned int data;
	file.read(reinterpret_cast<char *>(&data), 4);
	
	unsigned int result = 0;
	result |= (data & 0x000000ff) << 24;
	result |= (data & 0x0000ff00) << 8;
	result |= (data & 0x00ff0000) >> 8;
	result |= (data & 0xff000000) >> 24;
	
	return result;
}

bool Card::process_image(ifstream & file)
{
	unsigned int running = 0;

	//	check the PNG header
	unsigned char PNG_header[] = { 0x89, 'P', 'N', 'G', 0x0D, 0x0A, 0x1A, 0x0A };
	unsigned char * header = new unsigned char [8];
	file.read( reinterpret_cast<char *>(header), 8 );
	bool match;
	for (int i=0; i!=8; i++) {
		match = ( header[i]==PNG_header[i] );
		if (!match) break;
	}
	if (!match) {
		logfile << "Card::process_image -- invalid .png file\n-- [FILE] " << filename << endl;
	}
	
	running+=8;

	//	check that scene data exists
	
	bool more_data = true;
	
	//	process chunks until we reach IEND
	while (more_data) {
		unsigned int chunk_length = read_BE32(file);
		unsigned int chunk_type = read_BE32(file);
		
		running+=8;
	
		//	skip over the chunk data
		file.seekg(chunk_length, ios::cur);
		running+=chunk_length;
		
		unsigned int chunk_chksum = read_BE32(file);
		
		running+=4;
		
		more_data = ( file.eof() == false ) and ( chunk_type != 0x49454e44 ) ;		//	reached the end of file or IEND chunk?
		
		if (loglevel==logging::extra) {
			logfile << showbase << hex;
			logfile << setw(10) << chunk_length << " -- ";
			logfile << setw(10) << chunk_type << " -- ";
			logfile << setw(10) << chunk_chksum << " - ";
			logfile << noshowbase << dec;
			logfile << "\tp = " << setw(15) << file.tellg() << "\tr = " << setw(15) << running << endl;
		}
		
	}
	
	//	did we reach the end of the file unexpectedly?
	if (file.eof()) {
		logfile << "Card::process_image -- ERROR unexpected end of file reached.\n-- [FILE] " << pathname + filename << endl;
		return false;
	}
	
	image_length = file.tellg();
	file.seekg(0,ios::end);		//	find the actual end of the file
	file_length = file.tellg();
	
	if ( image_length == file_length ) {
		logfile << "Card::process_image -- file is a normal PNG.\n-- [FILE] " << pathname + filename << endl;
		return false;
	}
	
	file.seekg(image_length); 	//	put us back at the end of the image
	return true;
}


bool Card::process()
{
	string fullname = pathname + filename;
	
	ifstream file(fullname, ios::binary);
	
	bool result = process_image(file);
	
	if ( result ) {
		char * data = new char[file_length - image_length];
		
		file.read(data, file_length - image_length);
		
		MHASH hash_thread = mhash_init(MHASH_SHA1);
		
//		logfile << "trying to hash " << file_length - image_length << " bytes from " << fullname << endl;
		
		mhash(hash_thread, data, file_length - image_length);
		
		unsigned char * hashresult = new unsigned char[mhash_get_block_size(MHASH_SHA1)+1];
		
		hashresult = reinterpret_cast<unsigned char *>(mhash_end(hash_thread));
		
		hash = hash2string(hashresult);
		
		delete hashresult;
		delete data;

//		logfile << "chara data in " << filename << " has hash " << hash << endl;
	}
		
	file.close();
	
	return result;
}