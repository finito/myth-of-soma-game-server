#pragma once

class LOGIC_ELSE  
{
public:
	void Parse(char* pBuf);
	void Parse_and(char* pBuf);
	void Parse_or(char* pBuf);
	void Init();
	BYTE m_LogicElse;
	BOOL m_bAnd;
	int m_LogicElseInt[MAX_LOGIC_ELSE_INT];

	LOGIC_ELSE();
	virtual ~LOGIC_ELSE();

};
