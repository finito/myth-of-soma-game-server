#pragma once

class LOGIC;
class EXEC;
class LOGIC_ELSE;

typedef CTypedPtrArray <CPtrArray, LOGIC*> LogicArray;
typedef CTypedPtrArray <CPtrArray, EXEC*> ExecArray;
typedef CTypedPtrArray <CPtrArray, LOGIC_ELSE*> LogicElseArray;

class EVENT_DATA
{
public:
	int m_EventNum;
	LogicArray m_arLogic;
	ExecArray m_arExec;
	LogicElseArray m_arLogicElse;

	EVENT_DATA();
	virtual ~EVENT_DATA();

};
