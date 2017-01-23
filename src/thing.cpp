#include "thing.h"

#include <iomanip>
#include <cstring>

#include "logging.h"


using namespace std;

Thing::Thing(std::ifstream & f) : starting_offset(f.tellg()), thing_file(&f)
{
}

Thing::Thing(const int & o) : starting_offset(o)
{
}

void Thing::update_offset (std::ifstream & f)
{
	thing_file = &f;
	starting_offset = f.tellg();
}

Thing::~Thing()
{
}

uint32_t Thing::store_uint32()
{
	uint32_t * d = new uint32_t;
	
	datatype.push_back(DT_uint32);
	datacount.push_back(sizeof(uint32_t));
	
	offset = thing_file->tellg();
	dataoffset.push_back( offset - starting_offset );
	
	thing_file->read(reinterpret_cast<char *>(d), sizeof(uint32_t));
	data.push_back(d);
	
//	logfile << "uint32 - " << hex << *d << dec << endl;
	
	return *d;
}

uint16_t Thing::store_uint16()
{
	uint16_t * d = new uint16_t;
	
	datatype.push_back(DT_uint16);
	datacount.push_back(sizeof(uint16_t));
	
	offset = thing_file->tellg();
	dataoffset.push_back( offset - starting_offset );
	
	thing_file->read(reinterpret_cast<char *>(d), sizeof(uint16_t));
	data.push_back(d);
	
//	logfile << "uint16 - " << hex << *d << dec << endl;
	
	return *d;
}

uint8_t Thing::store_uint8()
{
	uint8_t * d = new uint8_t;
	
	datatype.push_back(DT_uint8);
	datacount.push_back(sizeof(uint8_t));
	
	offset = thing_file->tellg();
	dataoffset.push_back( offset - starting_offset );
	
	thing_file->read(reinterpret_cast<char *>(d), sizeof(uint8_t));
	data.push_back(d);
	
//	logfile << "uint8 - " << hex << (unsigned int)(*d) << dec << endl;
	
	return *d;
}

char * Thing::store_data(int count, bool suppress)
{
	datatype.push_back( (suppress) ? DT_data_noshow : DT_data );
	datacount.push_back(count);
		
	offset = thing_file->tellg();
	dataoffset.push_back( offset - starting_offset );
	
	//  allow for a zero-length data segment
	
	char * d = (count==0) ? nullptr : new char[count];
	
	if ( count > 0 )
		thing_file->read(d, count);
		
	data.push_back(d);
	
//	string s = (suppress || (count==0) ) ? ">>not shown<<" : string(d, count);
//	logfile << "store - read " << count << " bytes -- " << s << endl;

	return d;
}

float Thing::store_float()
{
	float * d = new float;
	
	datatype.push_back(DT_float);
	datacount.push_back(4);


	offset = thing_file->tellg();
	dataoffset.push_back( offset - starting_offset );
	
	thing_file->read(reinterpret_cast<char *>(d), 4);
	data.push_back(d);
	
//	logfile << "float - " << showpoint << (float)(*d) << noshowpoint << endl;
	
	return *d;
}

void Thing::data_output (std::ostream & out, const int & index) const
{
	out << setw(4) << index << "|" << setw(10) << dataoffset[index] << " | ";
	
	switch (datatype[index]) {
		case DT_uint32:
			out << *(unsigned int *)(data[index]) << "\n";
			break;
		case DT_uint16:
			out << *(unsigned short *)(data[index]) << "\n";
			break;
		case DT_uint8:
			out << (unsigned int)*(unsigned char *)(data[index]) << "\n";
			break;
		case DT_data:
		{
			string s = (datacount[index]==0) ? ">>EMPTY<<" : string((char *)(data[index]), datacount[index]);
			out << s << "\n";
			break;
		}
		case DT_data_noshow:
		{
			out << ">> not shown - " << datacount[index] << " bytes <<" << "\n";
			break;
		}
		case DT_float:
			out << showpoint << *(float *)(data[index]) << "\n";
			break;
		
	}
}

void Thing::data_save (std::ostream & out, const int & index) const
{
	out.write((char *)(data[index]), datacount[index]);
}

void Thing::dump_segments()
{
	logstream << setw(8) << data.size() << " | ";
	for (unsigned int i=0; i<data.size(); i++)
		logstream << setw(5) << datacount[i] << ", ";
	logstream << endl;
}

int Thing::get_composed_length()
{
	int result = 0;
	for (unsigned int i=0; i<data.size(); i++)
		if (ignore.find(i)==ignore.end())
			result += datacount[i];

	return result;
}

char * Thing::compose(char * comp)
{
	if (comp == nullptr)
		comp = new char[get_composed_length()];

	int count = 0;
	for (unsigned int i=0; i<data.size(); i++)
		if (ignore.find(i)==ignore.end()) {
			memcpy(comp+count, static_cast<char *>(data[i]), datacount[i]);
			count += datacount[i];
		}
	
	return comp;
}
