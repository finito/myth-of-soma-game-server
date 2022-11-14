#pragma once

class LOGIC
{
public:
	LOGIC();
	virtual ~LOGIC();

	void Init();
	void Parse(char* pBuf);

	BYTE m_Logic;
	int m_LogicInt[MAX_LOGIC_INT];
};
