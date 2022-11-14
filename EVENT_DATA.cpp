#include "stdafx.h"
#include "EVENT_DATA.h"
#include "LOGIC.h"
#include "EXEC.h"
#include "LOGIC_ELSE.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

EVENT_DATA::EVENT_DATA()
{

}

EVENT_DATA::~EVENT_DATA()
{
	for (int i = 0; i < m_arLogic.GetSize(); i++)
	{
		if (m_arLogic[i])
		{
			delete m_arLogic[i];
		}
	}

	m_arLogic.RemoveAll();

	for (int i = 0; i < m_arExec.GetSize(); i++)
	{
		if (m_arExec[i])
		{
			delete m_arExec[i];
		}
	}

	m_arExec.RemoveAll();

	for (int i = 0; i < m_arLogicElse.GetSize(); i++)
	{
		if (m_arLogicElse[i])
		{
			delete m_arLogicElse[i];
		}
	}

	m_arLogicElse.RemoveAll();
}
