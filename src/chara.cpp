#include "chara.h"
#include "logging.h"

using namespace std;

Chara::Chara(std::ifstream & f, const std::string & ct) : Thing(f), class_name(ct)
{
}

Chara::~Chara()
{
}

int Chara::ignore_length()
{
	logstream << get_segment_length(37) << ", " << get_segment_length(38) << ", " << get_segment_length(39) << endl;
	return get_segment_length(37) + get_segment_length(38) + get_segment_length(39);
}

bool Chara::read_string(const int & count)
{
	store_data(count);
	if (thing_file->eof()) {
		logstream << class_name <<"process -- ERROR in reading text string from file.\n\t-- tried reading ";
		logstream << count << " bytes from offset " << offset << " but reached end of file." << endl;
		return false;
	}
	else
		return true;
}

ostream& operator<< (ostream & out, const Chara * in)
{
	unsigned int index = 0;
	
	out << in->class_name << " -- offsets and stuff.\n";
	
	while ( index < in->data.size() ) {
		switch (index) {
			case 4:
			case 10:
			case 16:
			case 22:
			case 28:
				out << (char *)(in->data[index]) << "\n";
				break;
				
//			case 55:
//			case 56:
//			case 57:
//				out << *(float *)(in.data[index]) << " -- " << internal2degrees(*(float *)(in.data[index])) << "\n";
//				break;
				
			default:
				in->data_output(out, index);
		}
		index++;
	}
    out << endl;
	return out;
}

bool Chara::write(std::ofstream & out, int limit)
{
	//  write data array to disk, up to limit entries ( defaults to data.size() )
	if ( limit == -1 )
		limit = data.size();
	
	ofstream::streampos pos = out.tellp();
	
	for (int i=0; i<limit; i++)
		data_save(out, i);
	
	logstream << class_name << "::write -- wrote " << out.tellp() - pos << " bytes." << endl;
	
	return true;
}
