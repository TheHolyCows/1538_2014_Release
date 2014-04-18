#include "CowLexer.h"

namespace CowLib
{
	CowLexer::CowLexer()
	{
	}
	
	void CowLexer::AddToken(std::string name, char* pattern)
	{
		st_PrivateToken tok;
		tok.name = name;
	
		const char* error;
		int erroroffset;
		tok.pattern = pcre_compile(pattern, 0, &error, &erroroffset, 0);
	
		if(tok.pattern == NULL)
		{
			printf("Unable to add token \"%s\"\nError: %s (offset %d)\n    %s\n", name.c_str(), error, erroroffset, pattern);
			for(int i = 0; i < erroroffset+4; i++)
			{
				printf("-");
			}
			printf("^\n");
			return;
		}
		
		tok.resultBeginOffset = 0;
		tok.resultEndOffset = 0;
		
		m_Tokens.push_back(tok);
	}
	
	std::vector<CowLib::CowLexer::st_Token> CowLexer::TokenizeString(std::string str)
	{
		unsigned int offset = 0;
		std::vector<st_PrivateToken>::iterator it = m_Tokens.begin();
		st_PrivateToken* candidate = NULL;
		std::vector<st_Token> resultList;
		while(offset < str.length())
		{
			while(it != m_Tokens.end())
			{
				it->resultBeginOffset = 0;
				it->resultEndOffset = 0;
				int ovec[3];
				int result = pcre_exec(it->pattern, 0, str.c_str(), str.length(), offset, 0, ovec, 3);
	
				if(result > 0)
				{
					it->resultBeginOffset = ovec[0];
					it->resultEndOffset = ovec[1];
					if(candidate == NULL)
					{
						candidate = &(*it);
					}
					if(it->resultBeginOffset < candidate->resultBeginOffset)
					{
						// Choose the match that occurs the earliest
						candidate = &(*it);
					}
					else if(it->resultBeginOffset == candidate->resultBeginOffset)
					{
						// If two matches occur at the same offset, choose the longer match
						if(it->resultEndOffset > candidate->resultEndOffset)
						{
							candidate = &(*it);
						}
						// Don't do anything if lengths are equal
						// The match that was added to m_Tokens first will take precedence
					}
				}
				it++;
			}
			if(candidate == NULL)
			{
				// No more matches in string
				return resultList;
			}
			// Push match to token list
			st_Token match;
			match.type = candidate->name;
			match.value = str.substr(candidate->resultBeginOffset, candidate->resultEndOffset - candidate->resultBeginOffset);
			if(match.type.length() > 0)
			{
				resultList.push_back(match);
			}
			offset = candidate->resultEndOffset;
			candidate = NULL;
			it = m_Tokens.begin();
		}
		return resultList;
	}
}
