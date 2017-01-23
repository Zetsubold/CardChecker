#include <cstdlib>
#include <ctime>
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <fstream>

using namespace std;

#include "logging.h"

Log_Stream logstream;

Log_Stream::~Log_Stream()
{
	if (base_stream.is_open()) {
		base_stream << '\n' << setw(80) << setfill('=') << "=" << "\n";
		base_stream.close();
	}
}

void Log_Stream::open(const string & filename, const logging level, const string & starttext)
{
    base_stream.open(filename, ios_base::app);

	time_t runtime = time(NULL);

	base_stream << starttext << "\t\t - session run at " << ctime(&runtime) << endl;
	
	logging_level = level;
	current_level = level;
}