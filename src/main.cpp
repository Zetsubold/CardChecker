#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>

#include "dir_list.h"
#include "card.h"
#include "CardFolder.h"
#include "logging.h"

const std::string PROGRAM_NAME = "CardChecker";
const std::string VERSION = "1.1";

/*	CardChecker
 * 	-----------
 * 
 * 	A program to find cards with duplicate character data.
 * 
 * 	Use   - drag a folder onto the program, it will then check that directory and all subdirectories for 
 * 			character cards, and then hash the character data, then search for duplicates
 * 
 *	v0.1		- development version
 * 	v1.0		- initial release
 *	v1.1		- solving the waifu problem
 * 
 */

using namespace std;

// my standard "do things to files" template


bool process(vector<string> & paths, bool save_chara)
{
	bool result = true;
	
	CardFolder cards;
	
	for (vector<string>::iterator iter=paths.begin(); iter!=paths.end(); iter++) {
		
		vector<FileData> file_listing;
		get_filtered_files(file_listing, *iter, ".png", cout, true);
		
		result = result && cards.construct(file_listing, save_chara);
	}
	
	
	string outname = PROGRAM_NAME + " results.txt";
	cards.process(outname);
	
	return result;
}

int main(int argc, char *argv[])
{
	ostringstream logheader;
	
	logheader << PROGRAM_NAME << " v" << VERSION;
	
	string logname = PROGRAM_NAME + " log.txt";
	
	logstream.open(logname, Log_Stream::standard, logheader.str());
	
	if ( logstream.test_level(Log_Stream::extra) )
		logstream << "why me show up?" << endl;

	//	do we even have input?
    if (argc==1) {
		logstream <<	"ERROR - no input folder given, session aborted." << endl;
		cout << "NO INPUT FOLDER DETECTED" << endl;
		cout << "Try dragging a folder onto the program. :)" << endl;
		system("PAUSE");
		return EXIT_FAILURE;
	}
	
	vector<string> paths;
	bool save_chara = false;
       
    //	iterate through all files on command line

    for (int i=1; i<argc; i++) {
        string argstring = argv[i];
		
		if (argstring == "DEBUG") {
			cout << "Extra debugging info will be saved to " << logname << endl;
			logstream.set_level(Log_Stream::debug);
			continue;
		}
		
		if (argstring == "CHARA") {
			cout << "Basic Character card data will be extracted as .chara files" << endl;
			logstream.set_level(Log_Stream::debug);
			save_chara = true;
			continue;
		}
		

		bool isfolder = is_folder(argstring);
       
        if (!isfolder) {
			logstream <<	"ERROR - not given a folder, session aborted." << endl;
			cout << "HELP! NO FOLDER DETECTED" << endl;
			cout << "Whatever you gave me wasn't a folder. Please try again:)" << endl;
			system("PAUSE");
			return EXIT_FAILURE;
		}
        else
			paths.push_back(argstring);

    }
	
	
	//	Process the list of paths
	if ( !process(paths, save_chara) )
		cout << "Strange things happened. Please check " << logname << " for details" << endl;

    system("PAUSE");
    return EXIT_SUCCESS;

}
