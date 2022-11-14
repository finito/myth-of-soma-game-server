#pragma once

/////////////////////////////////////////////////////////////////////////////
// CNpcPosSet recordset

class CNpcPosSet : public CRecordset
{
public:
	CNpcPosSet(CDatabase* pDatabase = NULL);
	DECLARE_DYNAMIC(CNpcPosSet)

// Field/Param Data
	//{{AFX_FIELD(CNpcPosSet, CRecordset)
	CString m_sSid;
	CString m_sZone;
	CString m_sMinX;
	CString m_sMinY;
	CString m_sMaxX;
	CString m_sMaxY;
	CString m_sNum;
	CString m_sTableNum;
	CString m_sRegenType;
	CString m_sRegenTime;
	CString m_sRegenEvent;
	CString m_sGroup;
	CString m_sGuild;
	CString m_sGuildOpt;
	CString m_sChat01;
	CString m_sChat02;
	CString m_sChat03;
	CString m_sMaxItemNum;
	CString m_sItem01;
	CString m_sItem01Rand;
	CString m_sItem02;
	CString m_sItem02Rand;
	CString m_sItem03;
	CString m_sItem03Rand;
	CString m_sItem04;
	CString m_sItem04Rand;
	CString m_sItem05;
	CString m_sItem05Rand;
	CString m_sItem06;
	CString m_sItem06Rand;
	CString m_sItem07;
	CString m_sItem07Rand;
	CString m_sItem08;
	CString m_sItem08Rand;
	CString m_sItem09;
	CString m_sItem09Rand;
	CString m_sItem10;
	CString m_sItem10Rand;
	CString m_sItem11;
	CString m_sItem11Rand;
	CString m_sItem12;
	CString m_sItem12Rand;
	CString m_sItem13;
	CString m_sItem13Rand;
	CString m_sItem14;
	CString m_sItem14Rand;
	CString m_sItem15;
	CString m_sItem15Rand;
	CString m_sEventNum;
	CString m_sMoneyType;
	CString m_sMoneyMin;
	CString m_sMoneyMax;
	//}}AFX_FIELD


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNpcPosSet)
	public:
	virtual CString GetDefaultConnect();    // Default connection string
	virtual CString GetDefaultSQL();    // Default SQL for Recordset
	virtual void DoFieldExchange(CFieldExchange* pFX);  // RFX support
	//}}AFX_VIRTUAL

// Implementation
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
};
