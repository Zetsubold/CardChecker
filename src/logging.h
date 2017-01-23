#ifndef LOGGING_H
#define LOGGING_H

//	keeps the implementation of a program log file in one place

#include <string>
#include <fstream>
#include <ostream>
#include <ios>

class Log_Stream
{
public:
	enum logging{
		error,				//	only output error messages
		standard,			//	errors and standard program messages
		debug,				//	adds debug information
		full,				//	now with internal state data used during development
		extra				//	internal state of no longer current interest, but might be needed later
	};
	
private:
	std::ofstream base_stream;
	logging logging_level;
	logging current_level;
	
public:
	Log_Stream() : logging_level(standard), current_level(standard) {};
	~Log_Stream();
	
	void open(const std::string & filename, const logging level, const std::string & starttext);

	void set_level(const logging & lt) { current_level = lt; }
	void reset_level() { current_level = logging_level; }
	inline bool test_level(const logging & l) { return logging_level >= l; }

	template<typename T>
	Log_Stream & operator<<(const T & n)
	{
		if ( current_level <= logging_level)
			base_stream << n;
		return *this;
	}
	
	Log_Stream & operator<<(const logging & n)
	{
		current_level = n;
		return *this;
	}

	//	handle the manipulators (structure taken from <ostream> header file) 
	
	Log_Stream & operator<< (std::ostream::__ostream_type& (*pf)(std::ostream::__ostream_type&))
	{
		if ( current_level <= logging_level)
			pf(base_stream);
		return *this;
	}
	
	Log_Stream & operator<<(std::ostream::__ios_type& (*pf)(std::ostream::__ios_type&))
	{
		if ( current_level <= logging_level)
			pf(base_stream);
		return *this;
	}

	Log_Stream & operator<<(std::ios_base& (*pf) (std::ios_base&))
	{
		if ( current_level <= logging_level)
			pf(base_stream);
		return *this;
	}

};

extern Log_Stream logstream;

#endif