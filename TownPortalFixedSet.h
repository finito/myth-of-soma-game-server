#pragma once

class CTownPortalFixed : public CRecordset
{
public:
	CTownPortalFixed(CDatabase* pDatabase = NULL);
	DECLARE_DYNAMIC(CTownPortalFixed)

// Field/Param Data

	int	m_sid;
	int	m_szone;
	int	m_smovezone;
	int	m_sx;
	int	m_sy;

// Overrides
	// Wizard generated virtual function overrides
	public:
	virtual CString GetDefaultConnect();	// Default connection string

	virtual CString GetDefaultSQL(); 	// default SQL for Recordset
	virtual void DoFieldExchange(CFieldExchange* pFX);	// RFX support

// Implementation
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
};
