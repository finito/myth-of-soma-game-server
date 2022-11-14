#pragma once

class CStoreSet : public CRecordset
{
public:
	CStoreSet(CDatabase* pDatabase = NULL);
	DECLARE_DYNAMIC(CStoreSet)

// Field/Param Data
	//{{AFX_FIELD(CStoreSet, CRecordset)
	int		m_sStoreID;
	int		m_sType;
	int		m_sItemNum;
	CByteArray m_Items;
	CByteArray m_SellType;
	//}}AFX_FIELD

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CStoreSet)
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
