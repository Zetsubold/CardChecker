#include <cstdio>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <iomanip>
#include <sstream>

#include "card.h"
#include "chara_female.h"
#include "chara_male.h"

#include "logging.h"

#include "mhash.h"

using namespace std;

//	test data should hash to
//	1a77fcf7b35367d0d390e554bda9703af83a4597
	
string hash2string (unsigned char * hash, int count) {
	ostringstream build;
	
	for (int i=0; i<count; i++) 
		build << uppercase << hex << setfill('0') << setw(2) << static_cast<unsigned int>(hash[i]);
	
	string result = build.str();
	
	return result;
}

/*  Character Cards
 * 	---------------
 * 
 *  0	| uint8			| N	|	length of text
 * 	1	| N bytes 		|	|	text - "[HoneySelectCharaFemale]" or "[HoneySelectCharaMale]"
 *  2	| uint32		|	|	???
 *  3	| uint32		|	|	???
 * 
 *  [4..9] [10..15] [16..21] [22..27] [28..33]
 *  	| 128 bytes		|	|	text label
 *  	| uint32		|	|	???
 *  	| uint32		|	|	offset ? [O1 to O5]
 *  	| uint32		|	|	???
 *  	| uint32		|	|	length ? [L1 to L5]
 *  	| uint32		|	|	???
 * 
 * 	[34..38]
 *  	| Lx bytes		|	|	data segments
 * 							|	"Preview information"
 *							|	"Custom information"
 *							|	"Coordination information"
 *							|	"Status information"
 *							|	"Parameter information"
 * 
 * 
 *  39	| 16 bytes		|	|   ???
 * 
 */


Card::Card( const string & FileName, bool sc ) : save_chara(sc)
{
	string::size_type pos = FileName.find_last_of("\\");
	pathname = FileName.substr( 0, pos+1 );
	filename = FileName.substr( pos+1, FileName.length() );
	
	image_length = 0;
	file_length = 0;
	data_length = 0;
	
	logstream << Log_Stream::extra << "Card::Card\n-- [PATH] " << pathname << "\n-- [FILE] " << filename << endl;
	logstream.reset_level()	;
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
		logstream << "Card::process_image -- invalid .png file\n-- [FILE] " << filename << endl;
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
		
		if ( logstream.test_level(Log_Stream::extra) ) {
			logstream << showbase << hex;
			logstream << setw(10) << chunk_length << " -- ";
			logstream << setw(10) << chunk_type << " -- ";
			logstream << setw(10) << chunk_chksum << " - ";
			logstream << noshowbase << dec;
			logstream << "\tp = " << setw(15) << file.tellg() << "\tr = " << setw(15) << running << endl;
		}
		
	}
	
	//	did we reach the end of the file unexpectedly?
	if (file.eof()) {
		logstream << "Card::process_image -- ERROR unexpected end of file reached.\n-- [FILE] " << pathname + filename << endl;
		return false;
	}
	
	image_length = file.tellg();
	file.seekg(0,ios::end);		//	find the actual end of the file
	file_length = file.tellg();
	
	if ( image_length == file_length ) {
		logstream << "Card::process_image -- file is a normal PNG.\n-- [FILE] " << pathname + filename << endl;
		return false;
	}
	
	file.seekg(image_length); 	//	put us back at the end of the image
	return true;
}

bool Card::process_data(ifstream & file)
{
	bool on_fire = false;
	
	int chara_code = file.peek();
	
	if ( chara_code == 0x1c )
		chara_data = new Female(file);
	else if ( chara_code == 0x1a )
		chara_data = new Male(file);
	else {
		logstream << "Card::process_card -- ERROR in reading card file. Might not be a proper card\n-- [FILE] " << filename << endl;
		return false;
	}
		
	bool result = chara_data->process();
	
	if (!result) {
		logstream << "Card::process_card -- ERROR in processing card." << endl;
		on_fire = true;
	}
	
	if ( ( logstream.test_level(Log_Stream::debug) ) || on_fire )
		logstream << filename << "\n" << chara_data;
	
	if (on_fire){
		return false;
	}
	
	data_length = file.tellg() - image_length;
	
	if ( file.tellg() == file_length ) {
		logstream << Log_Stream::full << "Read in card file successfully." << endl;
		
		if (save_chara) {
			
			string outname = pathname + filename + ".chara";
			ofstream out(outname, ios::binary);
			
			chara_data->write(out, 39);
			
			logstream << Log_Stream::standard << "saved to " << outname << endl;
		}
		
		logstream.reset_level();
	}
	else {
		logstream << Log_Stream::extra << "Card::process_card -- extra data found at end card of file. Is this one made by Waifu Ripper?\n";
		logstream << "\t-- at position " << file.tellg() << " of " << file_length << endl;
		logstream.reset_level();
	}
	

	
	return true;
		
}



bool Card::process()
{
	string fullname = pathname + filename;
	
	ifstream file(fullname, ios::binary);
	
	bool result = process_image(file);
	
	if ( result )
		result = process_data(file);
		
	//	set logging level for loop
	logstream << Log_Stream::full;
	
	if ( result ) {
		//	segments 37 to 39 are ignored when getting the data to compare
		
		chara_data->add_ignore(37);
		chara_data->add_ignore(38);
		chara_data->add_ignore(39);
		
		int usable_data = chara_data->get_composed_length();
		
		if ( logstream.test_level(Log_Stream::extra) )
			logstream << "using " << setw(8) << usable_data << " of " << setw(8) << data_length << endl;
		
		char * data = chara_data->compose();
		
		MHASH hash_thread = mhash_init(MHASH_SHA1);
		
		logstream << "trying to hash " << usable_data << " bytes from " << fullname << endl;
		
		mhash(hash_thread, data, usable_data);
		
		unsigned char * hashresult = new unsigned char[mhash_get_block_size(MHASH_SHA1)+1];
		
		hashresult = reinterpret_cast<unsigned char *>(mhash_end(hash_thread));
		
		hash = hash2string(hashresult, mhash_get_block_size(MHASH_SHA1));
		
		delete hashresult;
		delete data;

		logstream << "chara data in " << filename << " has hash " << hash << endl;
	}
	
	logstream.reset_level();
		
	file.close();
	
	return result;
}