#pragma once

#include "../minLib.h"

namespace minLib
{

class minRandom
{
public:
	minRandom(void);
	~minRandom(void);

	/**
	* Generate randim string
	* \param str 생성된 문자열을 받을 곳
	* \param Length 문자열길이
	* \param Small 
	* \param Capital 
	* \param Numbers 
	* \return true / false
	*/
	bool getRandomString(std::string& str, int Length, int Small, int Capital, int Numbers);
};

}