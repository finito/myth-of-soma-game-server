#pragma once

class CTownPortal : public CRecordset
{
public:
	CTownPortal(CDatabase* pDatabase = NULL);
	DECLARE_DYNAMIC(CTownPortal)

// Field/Param Data

	int	m_sid;
	int	m_sitemnum;
	int	m_smovezone;
	int	m_sx;
	int	m_sy;
	int	m_susezone1;
	int	m_susezone2;
	int	m_susezone3;
	int	m_susezone4;

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
