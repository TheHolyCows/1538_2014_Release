//==================================================
// Copyright (C) 2014 Team 1538 / The Holy Cows
//==================================================

#ifndef __COW_LEXER_H__
#define __COW_LEXER_H__

#include <stdio.h>
#include <string>
#include <vector>
#include <pcre.h>

namespace CowLib
{
	class CowLexer
	{
	private:
		typedef struct
		{
			std::string name;
			pcre* pattern;
			// Result info is stored for sorting
			int resultBeginOffset;
			int resultEndOffset;
		} st_PrivateToken;
	
		std::vector<st_PrivateToken> m_Tokens;
	public:
		typedef struct
		{
			std::string type;
			std::string value;
		} st_Token;
		CowLexer();
		void AddToken(std::string name, char* pattern);
		std::vector<CowLib::CowLexer::st_Token> TokenizeString(std::string str);
	};
}

#endif
