#pragma once

#define MAX_LENGTH 1024

class CNpcChat
{
public:
	CNpcChat();
	virtual ~CNpcChat();

	int	m_sCid;
	CString m_strTalk;
};
