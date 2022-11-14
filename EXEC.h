#pragma once

struct UnparsedInfo
{
	int lineNumber;
	int evtNumber;
	CString evtCommand;
};

class EXEC
{
public:
	EXEC();
	virtual ~EXEC();

	void Init();
	void Parse(char* pBuf);

	BYTE m_Exec;
	int m_ExecInt[MAX_EXEC_INT];
};
