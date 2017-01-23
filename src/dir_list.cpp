#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <cstdio>
#include <fstream>

#include "dir_list.h"
#include "logging.h"

/*
 * 	dir_list
 * 	========
 * 
 * 	finds files in a directory passed to it.
 *	Can filter out files that don't contain a given string in their names.
 * 	Can also recurse into sub-directories
 * 
 * 	change-log
 * 	---------
 * 
 * 	1.0		|	initial version
 * 	1.1		|	added support for path names that don't end with a path separation character
 * 			|	re-factored get_dir_files so it calls get_filtered_files with a blank filter
 * 			|	added check_extension to the FileData class
 * 
 */

using namespace std;

FileData::FileData(std::string & s, std::time_t t)
{
    name = s;
    date = t;
}

std::ostream& operator<< (std::ostream & out, const FileData & in)
{
    out << '>' << in.name << " | " << in.date;
	return out;
}

bool is_folder (std::string & path_name) {
	struct stat sbuf;
	if (stat(path_name.c_str(),&sbuf) == -1) {
		return false;
	}
    if (S_ISDIR(sbuf.st_mode))
		return true;
	else
		return false;
}


// walk through the specified path, storing the details in the provided FileData vector
// can recurse into subdirectories if needed
// also sends status messages to a provided ostream (defaults to cout)

void get_dir_files(std::vector<FileData> & file_list, const std::string & PathName, std::ostream & out, bool recursion)
{
	get_filtered_files(file_list, PathName, "", out, recursion);
}

static void convert_uppercase (std::string & s)
{
	for (string::iterator iter=s.begin(); iter!=s.end(); iter++)
		*iter = toupper(*iter);
}

bool FileData::check_extension(std::string ext)
{
	string testname = name;
	
	convert_uppercase(testname);
	convert_uppercase(ext);
	
	return testname.find(ext) == testname.size() - ext.size();
}

// same as get_dir_files, except that it only stores files that have filenames that contain _filter_
void get_filtered_files(std::vector<FileData> & file_list, const std::string & PathName, const std::string & Filter, std::ostream & out, bool recursion)
{
    out << "entering " << PathName << endl;
	
	string path_name = PathName;
	
	// check for path seperator at end of string, add it if required
	if ( path_name.find_last_of('\\') != PathName.size()-1 ) {
		path_name += '\\';
	}
	
	// check to see if the filter is an empty string - if not, set the internal filter.
	bool use_filter = Filter.size() > 0;
	
	string filter;
	
	if (use_filter) {
		filter = Filter;
		convert_uppercase(filter);
	}
	
	
    DIR * dir_stream = opendir(path_name.c_str());
    if (dir_stream) {
		
//		logfile << "opened " << path_name << " as directory." << endl;
		
        struct dirent * entry;
        while ( (entry = readdir (dir_stream)) ) {
			
//			logfile << "\tfound entry - ";
			
            string filename = path_name + entry->d_name;
			
//			logfile << filename << endl;
			
            struct stat sbuf;
            if (stat(filename.c_str(),&sbuf) == -1) {
                continue;
            }
            if (S_ISREG(sbuf.st_mode)){         // normal file
				//	only store file if it matches the filter (if we aren't using a filter, say it passed anyway.)
				bool passed_filter = !use_filter;
				
				if (use_filter) {
					string up_filename = filename;
					
					convert_uppercase(up_filename);
					
					passed_filter = (up_filename.find(filter) != string::npos);
				}
				
				if (passed_filter)
                    file_list.push_back(FileData(filename, sbuf.st_mtime));
					
            }
            if (S_ISDIR(sbuf.st_mode)) {        // directory
                //  Ignore the automatic '.' and '..' directories
                if (entry->d_name[0]=='.') continue;
                else if (recursion) {
					get_filtered_files(file_list, filename, filter, out, recursion);
				}
            }
        }
        closedir(dir_stream);
    }
	else
		logstream << "ERROR -- " << path_name << " not opened as a directory.";
}
