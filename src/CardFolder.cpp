#include <fstream>
#include <iomanip>

#include "CardFolder.h"
#include "card.h"
#include "logging.h"

using namespace std;

CardFolder::CardFolder() {
	dupes_found = 0;
	dupe_groups = 0;
}

CardFolder::~CardFolder()
{
}

bool CardFolder::construct(std::vector<FileData> & file_list, bool save_chara)
{
	//  builds the card_hashes set from the file list provided

	bool result = true;
	
	cout << "Found " << file_list.size() << " candidate files.  Starting scanning..." << endl;

	logstream << "scanning ";
	logstream << file_list.size();
	logstream << " files.";
	logstream << endl;
	
	//	for each entry in file_list
	for (vector<FileData>::iterator iter=file_list.begin(); iter!=file_list.end(); iter++) {
		
		//	skip file if the extension is not .png
		//		(file_list may contain .png.bonemod.txt and other files)
		if (!iter->check_extension(".png"))
			continue;

		//  1. create Card object from file
		Card card(iter->name, save_chara);
		
		if ( card.process() ) {
		//	2. Get hash from card
			string hash = card.get_hash();
		//	3. insert <hash, name> into the map
			card_hashes.insert(make_pair(hash, iter->name));
		}
	
	}
		
	return result;
}

void CardFolder::process(const std::string & filename)
{
	//	check for duplicates

	ofstream out_file(filename);
	
	bool clear = true;
	
	string lasthash = "hash";
	
	// for each hash stored
	for (map_iterator iter=card_hashes.begin(); iter!=card_hashes.end(); iter++) {
		
		logstream << Log_Stream::debug << iter->first << " -- " << iter->second << endl;
		
		if (card_hashes.count(iter->first) > 1) {
			dupes_found++;

			if ((lasthash != iter->first) || (clear)) {
				if (lasthash != iter->first)
					out_file << "\n";
					
				out_file << "These " << card_hashes.count(iter->first) << " files might be duplicates\n";
				
				logstream << Log_Stream::debug << "These " << card_hashes.count(iter->first) << " files might be duplicates\n";

				dupe_groups++;
				lasthash = iter->first;
			}
			
			out_file << "\t" << iter->second << endl;
			clear = false;
		}
		else
			if (!clear) {
				clear = true;
				out_file << "\n";
			}
	}
	
	logstream.reset_level();
	
	//	catch that the last entries might be groups
	if (!clear)
		out_file << "\n";
	
	//	send result to various outputs
	out_file << "in total " << dupes_found << " duplicates, in " << dupe_groups << " groups." << endl;
	if ( (dupe_groups+dupes_found) == 0 )
		cout << "No duplicates found" << endl;
	else {
		cout << "Found " << dupes_found << " duplicates, in " << dupe_groups << " groups." << endl;
		cout << "Check " << filename << " for details." << endl;
	}
	logstream << "Found " << dupes_found << " duplicates, in " << dupe_groups << " groups." << endl;
}