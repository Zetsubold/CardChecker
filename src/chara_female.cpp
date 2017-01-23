#include <iostream>
#include <string>
#include <iomanip>

#include "logging.h"
#include "chara_female.h"

using namespace std;

Female::Female(ifstream &f) : Chara(f, "Female")
{
}

Female::~Female()
{
}

/*  Females
 * 	------- 
 * 
 *  0	| uint32		|	|	???
 *  1	| uint8			| N	|	length of text
 * 	2	| N bytes 		|	|	text - "[HoneySelectCharaFemale]"
 *  3	| uint32		|	|	???
 *  4	| uint32		|	|	???
 * 
 *  [5..10] [11..16] [17..22] [23..28] [29..34]
 *  	| 128 bytes		|	|	text label
 *  	| uint32		|	|	???
 *  	| uint32		|	|	offset ? [O1 to O5]
 *  	| uint32		|	|	???
 *  	| uint32		|	|	length ? [L1 to L5]
 *  	| uint32		|	|	???
 * 
 * 	[35..39]
 *  	| Lx bytes		|	|	data segments
 * 
 *  40	| 16 bytes		|	|   ???
 * 
 */

bool Female::process()
{
	int count;
	
	vector<uint32_t> data_length;
	
	//	index 1 & 2 - [HoneySelectCharaFemale]
	count = store_uint8();
	store_data(count);

	store_uint32();
	store_uint32();
	
	//	index 5...34 - 5 groups of label data + offse
	//	"Preview information"
	//	"Custom information"
	//	"Coordination information"
	//	"Status information"
	//	"Parameter information"
	for (count=0; count<5; count++) {
		store_data(128, true);
		
//		logfile << (char *)(data[data.size()-1]) << endl;
		
		store_uint32();
		store_uint32();								//	offset
		store_uint32();
		data_length.push_back( store_uint32() );	//	length
		store_uint32();
	}
	
	//	store data according to _datastore_
	
	for (count=0; count<5; count++) {
		store_data(data_length[count], true);
	}
	
	store_data(16, true);

	return true;
}

