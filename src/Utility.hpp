#ifndef UTILITY_HPP
#define UTILITY_HPP

#include <string>
#include <list>

#include <iostream>
#include <algorithm>
#include <sstream>

namespace nmt::Utils
{

	

	

	namespace String {
		void split(const std::string& str, std::list<std::string>& list, char delim);
		std::string toLower(std::string str);

	}


	

}

#endif