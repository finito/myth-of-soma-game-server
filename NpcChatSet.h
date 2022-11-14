#pragma once

class CNpcChatSet : public CRecordset
{
public:
	CNpcChatSet(CDatabase* pDatabase = NULL);
	DECLARE_DYNAMIC(CNpcChatSet)

// Field/Param Data
	//{{AFX_FIELD(CNpcChatSet, CRecordset)
	CString	m_sCid;
	CString	m_strTalk;
	//}}AFX_FIELD

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNpcChatSet)
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
