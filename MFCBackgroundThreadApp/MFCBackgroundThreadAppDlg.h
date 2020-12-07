
// MFCBackgroundThreadAppDlg.h : header file
//

#pragma once


// CMFCBackgroundThreadAppDlg dialog
class CMFCBackgroundThreadAppDlg : public CDialogEx
{
// Construction
public:
	CMFCBackgroundThreadAppDlg(CWnd* pParent = nullptr);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MFCBACKGROUNDTHREADAPP_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
    CEdit m_ouputEdit1;
    CEdit m_outputEdit2;
    CEdit m_outputEdit3;
    afx_msg LRESULT OnUpdateOutputEdit(WPARAM editCtrlId, LPARAM nValue);
    afx_msg void OnBnClickedStartBtn1();
    afx_msg void OnBnClickedStartBtn2();
    afx_msg void OnBnClickedStartBtn3();
    afx_msg void OnBnClickedCancelButton1();
    afx_msg void OnBnClickedCancelButton2();
    afx_msg void OnBnClickedCancelButton3();
    afx_msg void OnDestroy();
};
