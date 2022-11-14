#pragma once

class CSpecialTableSet : public CRecordset
{
public:
	CSpecialTableSet(CDatabase* pDatabase = NULL);
	DECLARE_DYNAMIC(CSpecialTableSet)

// Field/Param Data
	//{{AFX_FIELD(CSpecialTableSet, CRecordset)
	CString m_sMid;
	CString m_tClass;
	CString m_strName;
	CString m_tType;
	CString m_tTarget;
	CString m_sHpdec;
	CString m_sMpdec;
	CString m_sStmdec;
	CString m_tEValue;
	CString m_sEDist;
	CString m_sDamage;
	CString m_iTime;
	CString m_iTerm;
	CString m_iDecTerm;
	CString m_sTHp;
	CString m_sTMp;
	CString m_sMinExp;
	CString m_strInform;
	CString m_strGetInfo;
	CString m_sRate;
	CString m_sPlusExpRate;
	CString m_sPlusType;
	//}}AFX_FIELD


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSpecialTableSet)
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
