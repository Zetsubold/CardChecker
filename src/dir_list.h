#ifndef DIR_LIST_H
#define DIR_LIST_H

#include <string>
#include <ctime>
#include <iostream>
#include <vector>

class FileData {
	// For storing the details about a particular file
public:
    std::string name;
    std::time_t date;
    FileData(std::string & s, std::time_t t);
	bool check_extension(std::string ext);
};

std::ostream& operator<< (std::ostream & out, const FileData & in);

bool is_folder (std::string &);

void get_dir_files(std::vector<FileData> & file_list, const std::string & path_name, std::ostream & out = std::cout, bool recursion = false);

void get_filtered_files(std::vector<FileData> & file_list, const std::string & path_name, const std::string & filter, std::ostream & out = std::cout, bool recursion = false);

bool check_extension(std::string);

#endif
