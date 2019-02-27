#include "Utility.hpp"

namespace nmt::Utils
{

	namespace String {

		void split(const std::string& str, std::list<std::string>& list, char delim = ' ')
		{
			std::stringstream ss(str);
			std::string token;
			while (std::getline(ss, token, delim)) {
				list.push_back(token);
			}
		}

		std::string toLower(std::string str)
		{
			std::for_each(str.begin(), str.end(), [](char &c){ c = std::tolower(static_cast<unsigned char>(c)); });
			return str;
		}
	}

}
