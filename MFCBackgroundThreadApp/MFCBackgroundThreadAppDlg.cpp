
// MFCBackgroundThreadAppDlg.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include "MFCBackgroundThreadApp.h"
#include "MFCBackgroundThreadAppDlg.h"
#include "afxdialogex.h"
#include <chrono>
#include <thread>
#include <future>
#include <mutex>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace std;

struct ThreadData
{
    ThreadData(DWORD threadId, HWND hMainWnd, long nEditID)
        : _threadId(threadId), _hMainWnd(hMainWnd), _nEditID(nEditID), _run(false)
    {
        _threadDoneEvent.ResetEvent();
    }
    ThreadData() : _run(false) 
    {
        _threadDoneEvent.ResetEvent();
    }
    DWORD _threadId;
    HWND _hMainWnd;
    long _nEditID;
    bool _run;
    CEvent _threadDoneEvent;
    mutex _mutex;
};

ThreadData g_threadData[3];


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


// CMFCBackgroundThreadAppDlg dialog



CMFCBackgroundThreadAppDlg::CMFCBackgroundThreadAppDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MFCBACKGROUNDTHREADAPP_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMFCBackgroundThreadAppDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_EDIT1, m_ouputEdit1);
    DDX_Control(pDX, IDC_EDIT2, m_outputEdit2);
    DDX_Control(pDX, IDC_EDIT3, m_outputEdit3);
}

static UINT MSG_UPDATE_VALUE = ::RegisterWindowMessageA("MSG_UPDATE_VALUE");



BEGIN_MESSAGE_MAP(CMFCBackgroundThreadAppDlg, CDialogEx)
    ON_WM_SYSCOMMAND()
    ON_WM_PAINT()
    ON_WM_DESTROY()
	ON_WM_QUERYDRAGICON()
    ON_BN_CLICKED(IDC_START_BTN1, &CMFCBackgroundThreadAppDlg::OnBnClickedStartBtn1)
    ON_BN_CLICKED(IDC_START_BTN2, &CMFCBackgroundThreadAppDlg::OnBnClickedStartBtn2)
    ON_BN_CLICKED(IDC_START_BTN3, &CMFCBackgroundThreadAppDlg::OnBnClickedStartBtn3)
    ON_BN_CLICKED(IDC_CANCEL_BUTTON1, &CMFCBackgroundThreadAppDlg::OnBnClickedCancelButton1)
    ON_BN_CLICKED(IDC_CANCEL_BUTTON2, &CMFCBackgroundThreadAppDlg::OnBnClickedCancelButton2)
    ON_BN_CLICKED(IDC_CANCEL_BUTTON3, &CMFCBackgroundThreadAppDlg::OnBnClickedCancelButton3)
    ON_REGISTERED_MESSAGE(MSG_UPDATE_VALUE, &CMFCBackgroundThreadAppDlg::OnUpdateOutputEdit)
END_MESSAGE_MAP()


// CMFCBackgroundThreadAppDlg message handlers

BOOL CMFCBackgroundThreadAppDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
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

	// TODO: Add extra initialization here

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CMFCBackgroundThreadAppDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CMFCBackgroundThreadAppDlg::OnPaint()
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
HCURSOR CMFCBackgroundThreadAppDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void ThreadFunc(DWORD threadId, HWND hMainWnd, long nEditID) // used by std::async (cannot call MFC windows)
{
    for (int i = 0; i < 10; ++i)
    {
        //::SendMessage(hMainWnd, MSG_UPDATE_VALUE, nEditID, i); //::PostThreadMessage()
        ::PostMessage(hMainWnd, MSG_UPDATE_VALUE, nEditID, i); //::PostThreadMessage()
        //::PostThreadMessage(threadId, MSG_UPDATE_VALUE, nEditID, 99);
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}


UINT ThreadFunc2(LPVOID pVoid) // used by AfxBeginThread - CAN call MFC windows
{
    TRACE("%s - started\n", __FUNCTION__);
    ThreadData* pData = (ThreadData*)pVoid;
    for (int i = 0; i < 100; ++i)
    {
        std::unique_lock<mutex> ul(pData->_mutex);
        if (!pData->_run)
        {
            break;
        }

        //::SendMessage(hMainWnd, MSG_UPDATE_VALUE, nEditID, i); //::PostThreadMessage()
        ::PostMessage(pData->_hMainWnd, MSG_UPDATE_VALUE, pData->_nEditID, i); //::PostThreadMessage()
        ul.unlock();

        //::PostThreadMessage(threadId, MSG_UPDATE_VALUE, nEditID, 99);
        //std::this_thread::sleep_for(std::chrono::milliseconds(50));
        Sleep(100);
    }

    TRACE("%s - signalling thread done event\n", __FUNCTION__);
    {
        std::unique_lock<mutex> ul(pData->_mutex);
        pData->_run = false;
        pData->_threadDoneEvent.SetEvent();
    }

    TRACE("%s - done\n", __FUNCTION__);
    return 0;
}


LRESULT CMFCBackgroundThreadAppDlg::OnUpdateOutputEdit(WPARAM editCtrlId, LPARAM nValue)
{
    CWnd* pEdit = this->GetDlgItem(editCtrlId);
    if (pEdit)
    {
        CString sVal;
        sVal.Format(_T("%d"), nValue);
        pEdit->SetWindowText(sVal);
    }

    return 0;
}

void CMFCBackgroundThreadAppDlg::OnBnClickedStartBtn1()
{
    //auto res = std::async(std::launch::async, ThreadFunc, ::GetCurrentThreadId(), m_hWnd, IDC_EDIT1);
    //res.get();
    ThreadData& data = g_threadData[0];
    if (data._run)
    {
        return; // already running
    }

    std::unique_lock<mutex> ul(data._mutex);
    data._threadDoneEvent.ResetEvent();
    data._run = true;
    data._hMainWnd = m_hWnd;
    data._nEditID = IDC_EDIT1;
    data._threadId = ::GetCurrentThreadId();
    ::AfxBeginThread(ThreadFunc2, (LPVOID)&data);
}

void CMFCBackgroundThreadAppDlg::OnBnClickedStartBtn2()
{
    ThreadData& data = g_threadData[1];
    if (data._run)
    {
        return; // already running
    }

    std::unique_lock<mutex> ul(data._mutex);
    data._threadDoneEvent.ResetEvent();
    data._run = true;
    data._hMainWnd = m_hWnd;
    data._nEditID = IDC_EDIT2;
    data._threadId = ::GetCurrentThreadId();
    ::AfxBeginThread(ThreadFunc2, (LPVOID)&data);
}


void CMFCBackgroundThreadAppDlg::OnBnClickedStartBtn3()
{
    ThreadData& data = g_threadData[2];
    if (data._run)
    {
        return; // already running
    }

    std::unique_lock<mutex> ul(data._mutex);
    data._threadDoneEvent.ResetEvent();
    data._run = true;
    data._hMainWnd = m_hWnd;
    data._nEditID = IDC_EDIT3;
    data._threadId = ::GetCurrentThreadId();
    ::AfxBeginThread(ThreadFunc2, (LPVOID)&data);
}


void CMFCBackgroundThreadAppDlg::OnBnClickedCancelButton1()
{
    ThreadData& data = g_threadData[0];
    if (data._run)
    {
        data._run = false;
        WaitForSingleObject(data._threadDoneEvent.m_hObject, INFINITE);
    }
}


void CMFCBackgroundThreadAppDlg::OnBnClickedCancelButton2()
{
    ThreadData& data = g_threadData[1];
    if (data._run)
    {
        data._run = false;
        WaitForSingleObject(data._threadDoneEvent.m_hObject, INFINITE);
    }
}


void CMFCBackgroundThreadAppDlg::OnBnClickedCancelButton3()
{
    ThreadData& data = g_threadData[2];
    if (data._run)
    {
        data._run = false;
        WaitForSingleObject(data._threadDoneEvent.m_hObject, INFINITE);
    }
}

void CMFCBackgroundThreadAppDlg::OnDestroy()
{
    TRACE("%s - started\n", __FUNCTION__);
    for (int i = 0; i < 3; ++i)
    {
        ThreadData& data = g_threadData[i];
        std::unique_lock<mutex> ul(data._mutex);
        if (data._run)
        {
            data._run = false;            
            ul.unlock();
            TRACE("%s[%d] - waiting for thread done event\n", __FUNCTION__, i);
            WaitForSingleObject(data._threadDoneEvent.m_hObject, INFINITE);
            TRACE("%s[%d] - thread done event was signalled\n", __FUNCTION__, i);
            //std::cout << "Thread " << i << " finished" << endl;
        }
        else
        {
            TRACE("%s[%d] - thread is not active\n", __FUNCTION__, i);
        }

    }

    TRACE("%s - done\n", __FUNCTION__);
}
