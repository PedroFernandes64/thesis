#include "CSVReader.h"

/* Function to fetch data from a CSV File. */
std::vector<std::vector<std::string> > CSVReader::getData()
{
	std::ifstream file(fileName.c_str());
	std::vector<std::vector<std::string> > dataList;
	if (file.is_open()) {
		std::string line = "";
		// Iterate through each line and split the content using delimeter
		while (getline(file, line))
		{
			std::vector<std::string> vec;
			boost::algorithm::split(vec, line, boost::is_any_of(delimeter));
			dataList.push_back(vec);
		}
		// Close the File
		file.close();
	}
	else {
		std::cout << "ERROR: Unable to open file " << fileName << "." << std::endl;
		exit(0);
	}
	return dataList;
}

/* Returns the substring of str between the first and last delimiters */
std::string getInBetweenString(std::string str, std::string firstDelimiter, std::string lastDelimiter)
{
	size_t first = str.find_last_of(firstDelimiter)+1;
	size_t last = str.find_last_of(lastDelimiter);
	std::string strNew = str.substr(first, last - first);
	return strNew;
}

/* Splits a given string into a vector by a given delimiter. */
std::vector<std::string> splitBy(std::string str, std::string delimiter)
{
	std::vector<std::string> vec;
	size_t pos = 0;
	std::string token;
	std::string s = str;
	while ((pos = s.find(delimiter)) != std::string::npos) {
		token = s.substr(0, pos);
		vec.push_back(token);
		s.erase(0, pos + delimiter.length());
	}
	vec.push_back(s);
	return vec;
}

