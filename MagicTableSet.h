#pragma once

class CMagicTableSet : public CRecordset
{
public:
	CMagicTableSet(CDatabase* pDatabase = NULL);
	DECLARE_DYNAMIC(CMagicTableSet)

// Field/Param Data
	//{{AFX_FIELD(CMagicTableSet, CRecordset)
	CString m_sMid;
	CString m_tClass;
	CString m_strName;
	CString m_tType01;
	CString m_tType02;
	CString m_sStartTime;
	CString m_tTarget;
	CString m_sMpdec;
	CString m_tEValue;
	CString m_sEDist;
	CString m_sRange;
	CString m_sDamage;
	CString m_iTime;
	CString m_iTerm;
	CString m_sMinLevel;
	CString m_sMinMxp;
	CString m_sMinInt;
	CString m_sMinMoral;
	CString m_sMaxMoral;
	CString m_strInform;
	CString m_strGetInfo;
	CString m_sRate;
	CString m_sPlusExpRate;
	CString m_sPlusType;
	//}}AFX_FIELD


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMagicTableSet)
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

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
