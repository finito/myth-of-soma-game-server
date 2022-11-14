#pragma once

class EVENT_DATA;

typedef CTypedPtrArray <CPtrArray, EVENT_DATA*> EventDataArray;

class EVENT
{
public:
	EVENT();
	virtual ~EVENT();

	void DeleteAll();
	void Parsing(char* pBuf);
	void Init();
	BOOL LoadEvent(int zone);

	int m_Zone;
	EventDataArray m_arEvent;
};
