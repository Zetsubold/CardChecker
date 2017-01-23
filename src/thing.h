#ifndef THING_H
#define THING_H

#include <vector>
#include <set>
#include <fstream>
#include <cstdint>

//	a base thing class, be it a bed, sofa, or little green man.

enum DataType {
	DT_uint32,
	DT_uint16,
	DT_uint8,
	DT_data,
	DT_float,
	DT_data_noshow
};

class Thing {
	std::vector<DataType> datatype;
	std::vector<int> datacount;
	std::vector<unsigned int> dataoffset;
	unsigned int starting_offset;
	
	std::set<int> ignore; 	//	contains a list of indexes to ignore when composing the data
	
protected:
	std::ifstream * thing_file;
	inline int get_segment_length(const unsigned int & index) { return ( index >= data.size() ) ? 0 : datacount[index]; };
public:
	std::vector<void *> data;
	
	Thing(std::ifstream &);
	Thing(const int &);
	~Thing();
	
	void update_offset (std::ifstream &);
	
	unsigned int offset;		// the input file offset at the time of the last call to a store function
	
	uint32_t 	store_uint32	();
	uint16_t 	store_uint16	();
	uint8_t 	store_uint8		();
	char * 		store_data		(int count, bool = false);
	float		store_float		();
	
	void data_output (std::ostream &, const int &) const;
	void data_save (std::ostream &, const int &) const;
	
	inline void add_ignore(const int & i) { ignore.insert(i); }
	inline void clear_ignore() { ignore.clear(); }
	
	int get_composed_length();
	char * compose(char * comp = nullptr);

	void dump_segments();
	
};




#endif // THING_H
