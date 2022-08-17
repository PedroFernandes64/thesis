#ifndef __CSVReader__h
#define __CSVReader__h

#include <iostream>
#include <fstream>
#include <vector>
#include <iterator>
#include <string>
#include <algorithm>
#include <boost/algorithm/string.hpp>

/************************************************
 * This class implements a reader of .csv files. 
 * It is used for reading the input files.
 ************************************************/
class CSVReader{
private:
	std::string fileName; 	/**< The file to be read. **/
	std::string delimeter;	/**< The delimiter used for separating data. **/
public:
	/** Constructor. @param filepath The path of the file to be read. @param delm The delimiter to be used. **/
	CSVReader(std::string filepath, std::string delm = ";"): fileName(filepath), delimeter(delm){}

	/** Function to fetch data from a CSV File. It goes through the .csv file, line by line, and returns the data in a vector of vector of strings. **/
	std::vector<std::vector<std::string> > getData();
};

/****************************************************************
 * These are other useful methods for treating strings.
 * *************************************************************/

/** Returns the substring between the first and last delimiters. @param str The string to be examined. @param firstDelimiter The first delimiter. @param lastDelimiter The last delimiter. **/
std::string getInBetweenString(std::string str, std::string firstDelimiter, std::string lastDelimiter);

/** Splits a given string by a delimiter and returns a vector of strings. @param str The string to split. @param delimiter The delimiter. For instance, "1;2;3" becomes vector {1, 2, 3} if delimiter is ";". **/
std::vector<std::string> splitBy(std::string str, std::string delimiter);


#endif