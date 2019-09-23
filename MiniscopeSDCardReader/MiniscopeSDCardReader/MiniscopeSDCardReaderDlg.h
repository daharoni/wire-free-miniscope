
// MiniscopeSDCardReaderDlg.h : header file
//

#pragma once
#include "afxwin.h"


// CMiniscopeSDCardReaderDlg dialog
class CMiniscopeSDCardReaderDlg : public CDialogEx
{
// Construction
public:
	CMiniscopeSDCardReaderDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MINISCOPESDCARDREADER_DIALOG };
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
	CString mDrive;
	afx_msg void OnBnClickedRead();
	void AddListText(CString);
	CListBox mInfoList;
	UINT mStartSector;
	UINT mNumFrames;
	UINT mNumFramesToClear;
	afx_msg void OnBnClickedDelete();
	UINT mGain;
	double mLED;
	UINT mNumFramesToRecord;
	afx_msg void OnBnClickedSetparameters();
	afx_msg void OnBnClickedSave();
};
