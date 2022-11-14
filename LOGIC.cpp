#include "stdafx.h"
#include "LOGIC.h"
#include "EXEC.h"
#include <map>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

LOGIC::LOGIC()
{

}

LOGIC::~LOGIC()
{

}

void LOGIC::Parse(char *pBuf)
{
	int index = 0, i = 0;
	char temp[1024];

	index += ParseSpace(temp, pBuf+index);

	UnparsedInfo unparsedInfo;
	unparsedInfo.evtCommand = temp;
	unparsedInfo.lineNumber = 0;
	unparsedInfo.evtNumber = 0;
}

void LOGIC::Init()
{
	for (int i = 0; i < MAX_LOGIC_INT; i++)
	{
		m_LogicInt[i] = -1;
	}
}
