
// MiniscopeSDCardReaderDlg.cpp : implementation file
//

#include "stdafx.h"
#include "MiniscopeSDCardReader.h"
#include "MiniscopeSDCardReaderDlg.h"
#include "afxdialogex.h"
#include "definitions.h"
#include <opencv2\opencv.hpp>
#include <opencv\highgui.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CMiniscopeSDCardReaderDlg dialog



CMiniscopeSDCardReaderDlg::CMiniscopeSDCardReaderDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_MINISCOPESDCARDREADER_DIALOG, pParent)
	, mDrive(_T(""))
	, mStartSector(0)
	, mNumFrames(0)
	, mNumFramesToClear(0)
	, mGain(0)
	, mLED(0)
	, mNumFramesToRecord(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMiniscopeSDCardReaderDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_DRIVE, mDrive);
	DDX_Control(pDX, IDC_LIST1, mInfoList);
	DDX_Text(pDX, IDC_EDIT2, mStartSector);
	DDX_Text(pDX, IDC_EDIT3, mNumFrames);
	DDX_Text(pDX, IDC_EDIT1, mNumFramesToClear);
	DDX_Text(pDX, IDC_EDIT4, mGain);
	DDX_Text(pDX, IDC_EDIT6, mLED);
	DDX_Text(pDX, IDC_EDIT5, mNumFramesToRecord);
}

BEGIN_MESSAGE_MAP(CMiniscopeSDCardReaderDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_READ, &CMiniscopeSDCardReaderDlg::OnBnClickedRead)
	ON_BN_CLICKED(IDC_DELETE, &CMiniscopeSDCardReaderDlg::OnBnClickedDelete)
	ON_BN_CLICKED(IDC_SETPARAMETERS, &CMiniscopeSDCardReaderDlg::OnBnClickedSetparameters)
	ON_BN_CLICKED(IDC_SAVE, &CMiniscopeSDCardReaderDlg::OnBnClickedSave)
END_MESSAGE_MAP()


// CMiniscopeSDCardReaderDlg message handlers

BOOL CMiniscopeSDCardReaderDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	mDrive = DEFAULT_DRIVE;
	mStartSector = START_SECTOR;
	mNumFrames = NUM_FRAMES;
	mNumFramesToClear = NUM_FRAMES_TO_CLEAR;
	
	mGain = GAIN;
	mLED = LED_POWER;
	mNumFramesToRecord = NUM_FRAMES;

	UpdateData(FALSE);
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CMiniscopeSDCardReaderDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CMiniscopeSDCardReaderDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CMiniscopeSDCardReaderDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void  CMiniscopeSDCardReaderDlg::AddListText(CString str)
{ //Used to add text to the Infor Box List
	int index = mInfoList.GetCount();
	mInfoList.InsertString(index, str);
	mInfoList.SetTopIndex(mInfoList.GetCount() - 1);
	//m_InfoList.EnsureVisible(index, FALSE);
}

void CMiniscopeSDCardReaderDlg::OnBnClickedRead()
{
	UpdateData(TRUE);
	DWORD lDrives = GetLogicalDrives();
	CString drive, str;
	uint8_t buff[NUM_SECTOR_PER_FRAME*BYTE_PER_SECTOR];
	uint32_t buffSize = NUM_SECTOR_PER_FRAME*BYTE_PER_SECTOR;
	uint32_t *footer = (uint32_t*)(&buff[buffSize-FOOTER_WORD_LENGTH*4]);

	DWORD dwRead;
	cv::Mat frame;
	drive.Format(L"\\\\.\\%s", mDrive); 

	HANDLE hDisk = CreateFile(drive, GENERIC_READ, FILE_SHARE_READ| FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
	if (hDisk == INVALID_HANDLE_VALUE) {
		str.Format(L"Problem opening drive:%s", drive);
		AddListText(str);
		CloseHandle(hDisk);
	}
	else {
		cv::namedWindow("window", 1);
		str.Format(L"Opening drive:%s.", drive);
		AddListText(str);
		SetFilePointer(hDisk, (mStartSector)*512, 0, FILE_BEGIN); // which sector to read

		for (UINT frameNum = 0; frameNum < mNumFrames; frameNum++) {
			
			ReadFile(hDisk, buff, NUM_SECTOR_PER_FRAME*BYTE_PER_SECTOR, &dwRead, 0);  // read sector

			str.Format(L"Overflow = %u | LineCount = %u | Time = %u | FrameNum = %u", footer[0], footer[1], footer[2], footer[3]);
			AddListText(str);
			frame = cv::Mat(HEIGHT, WIDTH, CV_8UC1, buff);
			cv::imshow("window", frame);
			//cv::waitKey(60);
		}
		CloseHandle(hDisk);
	}
		


}


void CMiniscopeSDCardReaderDlg::OnBnClickedDelete()
{
	UpdateData(TRUE);
	CString drive, str;
	uint8_t buff[NUM_SECTOR_PER_FRAME*BYTE_PER_SECTOR] = { 0 };
	uint32_t buffSize = NUM_SECTOR_PER_FRAME*BYTE_PER_SECTOR;
	uint8_t headerTemp[HEADER_LENGTH] = { 0 };
	uint32_t *header = (uint32_t *)headerTemp;

	DWORD dwRead;
	drive.Format(L"\\\\.\\%s", mDrive);

	HANDLE hDisk = CreateFile(drive, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
	if (hDisk == INVALID_HANDLE_VALUE) {
		str.Format(L"Problem opening drive:%s", drive);
		AddListText(str);
		CloseHandle(hDisk);
	}
	else {
		SetFilePointer(hDisk, (HEADER_SECTOR)*512, 0, FILE_BEGIN); // which sector to write
		ReadFile(hDisk, headerTemp, HEADER_LENGTH, &dwRead, 0);  // read header
		
		if (header[0] != WRITE_KEY0 || header[1] != WRITE_KEY1 || header[2] != WRITE_KEY2 || header[3] != WRITE_KEY3) {
			str.Format(L"Write key in header (0x%08X, 0x%08X, 0x%08X, 0x%08X) does not match defined WRITE_KEY (0x%08X, 0x%08X, 0x%08X, 0x%08X)", 
				header[0], header[1], header[2], header[3], WRITE_KEY0, WRITE_KEY1, WRITE_KEY2, WRITE_KEY3);
			AddListText(str);
			CloseHandle(hDisk);
			
		}
		else {
			CloseHandle(hDisk);

			HANDLE hDisk = CreateFile(drive, GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
			SetFilePointer(hDisk, (mStartSector) * 512, 0, FILE_BEGIN); // which sector to write
			

			str.Format(L"Write key match! Clearing %u frames from drive:%s", mNumFramesToClear, drive);
			AddListText(str);

			for (UINT frameNum = 0; frameNum < mNumFramesToClear; frameNum++) {

				WriteFile(hDisk, buff, NUM_SECTOR_PER_FRAME*BYTE_PER_SECTOR, &dwRead, 0);  // read sector
			}
			str.Format(L"Clearing complete.");
			AddListText(str);
			CloseHandle(hDisk);
		}
	}
}


void CMiniscopeSDCardReaderDlg::OnBnClickedSetparameters()
{
	UpdateData(TRUE);

	CString drive, str;

	uint8_t headerTemp[HEADER_LENGTH] = { 0 };
	uint32_t *header = (uint32_t *)headerTemp;

	DWORD dwRead;
	drive.Format(L"\\\\.\\%s", mDrive);
	
	HANDLE hDisk = CreateFile(drive, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);

	if (hDisk == INVALID_HANDLE_VALUE) {
		str.Format(L"Problem opening drive:%s", drive);
		AddListText(str);
		CloseHandle(hDisk);
	}
	else {
		SetFilePointer(hDisk, (HEADER_SECTOR) * 512, 0, FILE_BEGIN); // which sector to write
		ReadFile(hDisk, headerTemp, HEADER_LENGTH, &dwRead, 0);  // read header

		if (header[0] != WRITE_KEY0 || header[1] != WRITE_KEY1 || header[2] != WRITE_KEY2 || header[3] != WRITE_KEY3) {
			str.Format(L"Write key in header (0x%08X, 0x%08X, 0x%08X, 0x%08X) does not match defined WRITE_KEY (0x%08X, 0x%08X, 0x%08X, 0x%08X)",
				header[0], header[1], header[2], header[3], WRITE_KEY0, WRITE_KEY1, WRITE_KEY2, WRITE_KEY3);
			AddListText(str);
			CloseHandle(hDisk);

		}
		else {
			CloseHandle(hDisk);

			header[HEADER_GAIN_POS] = mGain;
			header[HEADER_LED_POS] = uint32_t(mLED * 255 / 100);
			header[HEADER_NUM_FRAMES_POS] = mNumFramesToRecord;

			HANDLE hDisk = CreateFile(drive, GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
			SetFilePointer(hDisk, (HEADER_SECTOR) * 512, 0, FILE_BEGIN); // which sector to write


			str.Format(L"Write key match! Saving recording parameters on drive:%s", drive);
			AddListText(str);
			WriteFile(hDisk, headerTemp, 512, &dwRead, 0);  // read sector
			CloseHandle(hDisk);
		}
	}
}


void CMiniscopeSDCardReaderDlg::OnBnClickedSave()
{
	UpdateData(TRUE);
	DWORD lDrives = GetLogicalDrives();
	CString drive, str;
	uint8_t buff[NUM_SECTOR_PER_FRAME*BYTE_PER_SECTOR] = { 0 };
	uint32_t buffSize = NUM_SECTOR_PER_FRAME*BYTE_PER_SECTOR;
	uint32_t *footer = (uint32_t*)(&buff[buffSize - FOOTER_WORD_LENGTH * 4]);
	uint32_t msCamFileNumber = 1;

	CTime time = CTime::GetCurrentTime();
	CreateDirectory(L"data", NULL);
	str.Format(L"data\\%u_%u_%u", time.GetMonth(), time.GetDay(), time.GetYear());
	CreateDirectory(str, NULL);
	str.Format(L"data\\%u_%u_%u\\H%u_M%u_S%u", time.GetMonth(), time.GetDay(), time.GetYear(), time.GetHour(), time.GetMinute(), time.GetSecond());
	CreateDirectory(str, NULL);


	std::ostringstream os;
	os << "data\\" << time.GetMonth() << "_" << time.GetDay() << "_" << time.GetYear() << "\\H" << time.GetHour() << "_M" << time.GetMinute() << "_S" << time.GetSecond() << "\\msCam";
	std::string msCamFileName = os.str() + std::to_string(msCamFileNumber) + ".avi";
	cv::VideoWriter msOutVid;
	msOutVid.open(msCamFileName, CV_FOURCC('D', 'I', 'B', ' '), 13, cv::Size(WIDTH, HEIGHT), false);

	DWORD dwRead;
	cv::Mat frame;
	drive.Format(L"\\\\.\\%s", mDrive);

	HANDLE hDisk = CreateFile(drive, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
	if (hDisk == INVALID_HANDLE_VALUE) {
		str.Format(L"Problem opening drive:%s", drive);
		AddListText(str);
		CloseHandle(hDisk);
	}
	else {
		cv::namedWindow("window", 1);
		str.Format(L"Opening drive:%s.", drive);
		AddListText(str);
		SetFilePointer(hDisk, (mStartSector) * 512, 0, FILE_BEGIN); // which sector to read

		for (UINT frameNum = 0; frameNum < mNumFrames; frameNum++) {
		
		//Changed mNumFrames to hold seconds not frames
		//while ((footer[2] < (mNumFrames*1000) && footer[2] != 0) || (footer[3] < 10)) {
			if (footer[3]%MAX_FRAMES_PER_FILE == 0 && footer[3] != 0) {
				msOutVid.release();
				msCamFileNumber++;
				msCamFileName = os.str() + std::to_string(msCamFileNumber) + ".avi";
				msOutVid.open(msCamFileName, CV_FOURCC('D', 'I', 'B', ' '), 13, cv::Size(WIDTH, HEIGHT), false);
			}

			ReadFile(hDisk, buff, NUM_SECTOR_PER_FRAME*BYTE_PER_SECTOR, &dwRead, 0);  // read sector

			str.Format(L"Overflow = %u | LineCount = %u | Time = %u | FrameNum = %u", footer[0], footer[1], footer[2], footer[3]);
			AddListText(str);
			frame = cv::Mat(HEIGHT, WIDTH, CV_8UC1, buff);
			msOutVid.write(frame);
			cv::imshow("window", frame);
			cv::waitKey(5);
		}
		CloseHandle(hDisk);
		msOutVid.release();
	}
}
