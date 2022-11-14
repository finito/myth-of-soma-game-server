#pragma once

/////////////////////////////////////////////////////////////////////////////
// CTraderExchangeTableSet recordset

class CTraderExchangeTableSet : public CRecordset
{
public:
	CTraderExchangeTableSet(CDatabase* pDatabase = NULL);
	DECLARE_DYNAMIC(CTraderExchangeTableSet)

// Field/Param Data
	//{{AFX_FIELD(CTraderExchangeTableSet, CRecordset)
	int m_sId;
	int m_sType;
	int m_sItem;
	int m_sMoney;
	int m_sNeedItem[5];
	int m_sNeedItemNum[5];
	//}}AFX_FIELD
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTraderExchangeTableSet)
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
