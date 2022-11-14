#pragma once

class CIni
{
public:
	CIni();
	virtual ~CIni();

	int GetProfileInt(char* lpAppName, char* lpKeyName, int nDefault);
	char* GetProfileString(char* lpAppName, char* lpKeyName, char* lpDefault);
	int SetProfileInt(char* lpAppName, char* lpKeyName, int nDefault);
	int SetProfileString(char* lpAppName, char* lpKeyName, char* lpDefault);
	bool SetPath(const char* filename);

	char m_szFileName[200];
	int m_nError;
	char m_strError[200];
};
