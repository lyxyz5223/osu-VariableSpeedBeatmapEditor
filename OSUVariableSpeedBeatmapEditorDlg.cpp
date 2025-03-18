
// OSUVariableSpeedBeatmapEditorDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "OSUVariableSpeedBeatmapEditor.h"
#include "OSUVariableSpeedBeatmapEditorDlg.h"
#include "afxdialogex.h"
//导入谱面管理器类
#include  "BeatmapManager.h"
using namespace Excel;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

void CoUninitAndExit(int code)
{
	CoUninitialize();
	exit(code);
}

std::wstring HResultToString(HRESULT hr) {
	WCHAR* lpBuffer = new WCHAR[sizeof(WCHAR*) * 8];
	// 获取错误消息的长度
	DWORD messageSize = FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		hr,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		lpBuffer,
		0,
		NULL);
	delete[] lpBuffer;
	lpBuffer = new WCHAR[messageSize];
	MessageBox(0, std::to_wstring(messageSize).c_str(), L"messageSize", 0);
	// 将错误消息复制到字符串
	FormatMessage(
		FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		hr,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		lpBuffer,
		messageSize,
		NULL);
	std::wstring message = lpBuffer;
	delete[] lpBuffer;
	return message;
}
// COSUVariableSpeedBeatmapEditorDlg 对话框



COSUVariableSpeedBeatmapEditorDlg::COSUVariableSpeedBeatmapEditorDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_OSUVARIABLESPEEDBEATMAPEDITOR_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
#ifdef _DEBUG
	{
		AllocConsole();
		FILE* stream = nullptr;
		freopen_s(&stream, "CONOUT$", "w", stdout);
		freopen_s(&stream, "CONOUT$", "w", stderr);
	}
#endif // !_DEBUG
	beatmapManager = new BeatmapManager(); //创建谱面管理器类对象
	HRESULT hr = CoInitialize(0);
	if (FAILED(hr))
	{
		MessageBox(HResultToString(hr).c_str(), L"error", MB_ICONERROR);
		CoUninitAndExit(hr);
	}
}

COSUVariableSpeedBeatmapEditorDlg::~COSUVariableSpeedBeatmapEditorDlg()
{
	this->DeleteTmpFile();//删除临时文件
	CoUninitialize();
}

void COSUVariableSpeedBeatmapEditorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BTNOPENFILE, m_OpenFile);
	DDX_Control(pDX, IDC_EDITFILEPATH, m_FilePathEdit);
}

BEGIN_MESSAGE_MAP(COSUVariableSpeedBeatmapEditorDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTNOPENFILE, &COSUVariableSpeedBeatmapEditorDlg::OnBnClickedOpenFile)
	ON_BN_CLICKED(IDC_OPENEXCEL, &COSUVariableSpeedBeatmapEditorDlg::OnBnClickedOpenExcel)
	ON_MESSAGE(WM_USER, &COSUVariableSpeedBeatmapEditorDlg::OnMsgUser)
	ON_BN_CLICKED(IDC_BTNSAVEFILE, &COSUVariableSpeedBeatmapEditorDlg::OnBnClickedBtnSaveFile)
	ON_BN_CLICKED(IDC_BTNSAVEFILEAS, &COSUVariableSpeedBeatmapEditorDlg::OnBnClickedBtnSaveFileAs)
	ON_WM_GETMINMAXINFO()
	ON_EN_CHANGE(IDC_EDITFILEPATH, &COSUVariableSpeedBeatmapEditorDlg::OnEnChangeEditfilepath)
	ON_WM_EXITSIZEMOVE()
	ON_WM_SIZE()
END_MESSAGE_MAP()

void COSUVariableSpeedBeatmapEditorDlg::CreateExcelApp()
{
	// 创建Excel应用程序对象
	HRESULT hr = appPtr.CreateInstance(__uuidof(Excel::Application));
	if (FAILED(hr))
	{
		MessageBox(HResultToString(hr).c_str(), L"error", MB_ICONERROR);
		CoUninitAndExit(hr);
	}
	// 获取Excel事件连接点容器
	CComPtr<IConnectionPointContainer> icpc;
	hr = appPtr.QueryInterface(IID_IConnectionPointContainer, &icpc);
	if (FAILED(hr))
	{
		MessageBox(HResultToString(hr).c_str(), L"error", MB_ICONERROR);
		CoUninitAndExit(hr);
	}
	// 获取Excel事件连接点
	CComPtr<IConnectionPoint> icp;
	hr = icpc->FindConnectionPoint(__uuidof(Excel::AppEvents), &icp);
	if (FAILED(hr))
	{
		MessageBox(HResultToString(hr).c_str(), L"error", MB_ICONERROR);
		CoUninitAndExit(hr);
	}
	// 将事件接收器绑定到连接点
	DWORD dwCookie;
	CExcelEventSink* pEventSink = new CExcelEventSink(GetSafeHwnd());
	hr = icp->Advise(pEventSink, &dwCookie);
	if (FAILED(hr))
	{
		MessageBox(HResultToString(hr).c_str(), L"error", MB_ICONERROR);
		CoUninitAndExit(hr);
	}
}

// COSUVariableSpeedBeatmapEditorDlg 消息处理程序

BOOL COSUVariableSpeedBeatmapEditorDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 主窗口初始化
	//SetWindowPos(NULL, 0, 0, 600, 450, SWP_NOMOVE);

	//打开按钮图标
	SHSTOCKICONINFO sii = { 0 };
	sii.cbSize = sizeof(sii);
	SHGetStockIconInfo(SIID_FOLDEROPEN, SHGSI_ICON | SHGSI_SMALLICON, &sii);
	m_OpenFile.SetIcon(sii.hIcon);// AfxGetApp()->LoadIcon(IDC_ICON);

	CString mainText;
	GetDlgItem(IDC_STATIC)->GetWindowText(mainText);
	GetDlgItem(IDC_STATIC)->SetWindowText(mainText);
	"cyf的osu!变速谱面编辑器\ncyf专用，开源免费";

	//编辑框垂直居中
	CRect rect;
	GetDlgItem(IDC_EDITFILEPATH)->GetClientRect(&rect);
	LOGFONT lf;
	CFont* font = GetDlgItem(IDC_EDITFILEPATH)->GetFont();
	font->GetLogFont(&lf);
	//rect.OffsetRect(0, (rect.Height() - (lf.lfHeight < 0 ? -lf.lfHeight : lf.lfHeight)) / 2); //设置内容的左边距与上边距，大小自适取
	rect.OffsetRect(0, (rect.Height() - (lf.lfHeight < 0 ? -lf.lfHeight : lf.lfHeight)) / 2  - (lf.lfHeight < 0 ? -lf.lfHeight : lf.lfHeight) / 8); //设置内容的左边距与上边距，大小自适取
	//((CEdit*)GetDlgItem(IDC_EDITFILEPATH))->SetPasswordChar(_T('*')); //设置字符显示为密码模式
	::SendMessage(((CEdit*)GetDlgItem(IDC_EDITFILEPATH))->m_hWnd, EM_SETRECT, 0, (LPARAM)&rect);
#ifdef _DEBUG
	m_FilePathEdit.SetWindowText(L"D:\\1Downloads\\test.osu");
#endif

	CRect winRect;
	GetWindowRect(&winRect);
	defaultSize = rect.Size();

	CreateExcelApp();

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void COSUVariableSpeedBeatmapEditorDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR COSUVariableSpeedBeatmapEditorDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void COSUVariableSpeedBeatmapEditorDlg::OnBnClickedOpenFile()
{
	// TODO: 在此添加控件通知处理程序代码
	this->DeleteTmpFile();//先删除临时文件
	excelHasOpen = false;


	//过滤的文件扩展名
	const TCHAR extsFilter[] = _T("Osu! Beatmap Files (*.osu)|*.osu|")
		_T("文本文档 (*.txt)|*.txt|")
		_T("All Files (*.*)|*.*||");//';'号间隔多个后缀名

	//获取当前工作路径作为文件打开的默认路径
//#define buffer_len max_path
//	tchar* defaultdir = new tchar[buffer_len];
//	dword needlen = getcurrentdirectory(buffer_len, defaultdir);
//	if (needlen > buffer_len)
//	{
//		delete[] defaultdir;
//		defaultdir = new tchar[needlen];
//		getcurrentdirectory(needlen, defaultdir);
//	}
	//下文中记得要delete[] defaultdir;释放内存!

	CFileDialog FileSelectionDlg(TRUE/*TRUE为"打开"窗口,FALSE为"另存为"窗口*/,
		NULL/*默认自动追加的文件扩展名*/,
		L"cyf.osu"/*"文件名"一栏默认填充文本*/,
		NULL/*自定义属性*/,
		extsFilter/*文件扩展名过滤*/,
		this/*父窗口*/,
		0/*文件选择窗口的版本*/,
		TRUE/*是否启用新样式文件选择窗口*/
	);
	INT_PTR result = FileSelectionDlg.DoModal();//打开窗口并等待返回
	//delete[] defaultDir;
	if (result != IDOK)
		return;
	//else
	//点击了 "打开"/"保存" 按钮
	CString filePath = FileSelectionDlg.GetPathName();
	m_FilePathEdit.SetWindowText(filePath);
	std::wstring fileName = FileSelectionDlg.GetFileName();
	beatmapManager->setFileName(fileName);

	hasAskedIfCoverOldBeatmapBackupFile = FALSE;

}


void COSUVariableSpeedBeatmapEditorDlg::OnBnClickedOpenExcel()
{
	// TODO: 处理打开Excel的代码
	using namespace std;
	//获取输入框中的文件路径
	CString fileWithPathCStr;
	GetDlgItem(IDC_EDITFILEPATH)->GetWindowText(fileWithPathCStr);
	wstring fileWithPath = fileWithPathCStr;

	//判断excel是否已经打开，如已打开则直接返回
	try {
		::SetForegroundWindow((HWND)appPtr->Application->GetHwnd());
		appPtr->ActiveWindow->Activate();
		return;
	}
	catch (...) {

	}
	if (excelHasOpen)
		return;
	//创建谱面管理器对象，并打开谱面
	if (!beatmapManager->openBeatmap(fileWithPath))
	{
		MessageBox(L"无法打开谱面", L"Error", MB_ICONERROR);
		return;
	}
	vector<wstring> variableSpeedVector = beatmapManager->getVariableSpeedVector();//获取变速谱面内容
	wstring tmpFileNameWithPath = fileWithPath + L".cyf";
	fstream tmpFile(tmpFileNameWithPath, ios::out);
	if (!tmpFile.is_open())
	{
		MessageBox(L"临时文件创建/打开失败", L"Error", MB_ICONERROR);
		return;
	}
	wstring outputTmpFileWStr;//输出临时文件
	for (size_t i = 1; i < variableSpeedVector.size(); i++)
		outputTmpFileWStr += variableSpeedVector[i] + L"\n";
	if (outputTmpFileWStr != L"")
		outputTmpFileWStr.pop_back();
	tmpFile << wstr2str_2ANSI(outputTmpFileWStr);
	tmpFile.close();
	try {
		appPtr->Quit();
	}
	catch (...) {

	}
	try {
		appPtr->Workbooks->Open(_com_util::ConvertStringToBSTR(wstr2str_2ANSI(tmpFileNameWithPath).c_str()), 0, 0, 2, "", "", 0, Excel::xlWindows, ", ", false, true, 0, true, true, Excel::xlNormalLoad);//0,0,6,0,0,0,0,", ", 1,0,0,0,0,0,0);)
		//app->Workbooks->Open("D:\\测试文本.txt", 0, 0, 2, "", "", 0, Excel::xlWindows, ", ", false, true, 0, true, true, Excel::xlNormalLoad);//0,0,6,0,0,0,0,", ", 1,0,0,0,0,0,0);)
		//app->PutVisible(0, Excel::xlNormal);
		appPtr->PutVisible(0, Excel::xlMaximized);
		excelHasOpen = true;
	}
	catch(...) {
		try {
			CreateExcelApp();
			appPtr->Workbooks->Open(_com_util::ConvertStringToBSTR(wstr2str_2ANSI(tmpFileNameWithPath).c_str()), 0, 0, 2, "", "", 0, Excel::xlWindows, ", ", false, true, 0, true, true, Excel::xlNormalLoad);//0,0,6,0,0,0,0,", ", 1,0,0,0,0,0,0);)
			//app->Workbooks->Open("D:\\测试文本.txt", 0, 0, 2, "", "", 0, Excel::xlWindows, ", ", false, true, 0, true, true, Excel::xlNormalLoad);//0,0,6,0,0,0,0,", ", 1,0,0,0,0,0,0);)
			//app->PutVisible(0, Excel::xlNormal);
			appPtr->PutVisible(0, Excel::xlMaximized);
			excelHasOpen = true;
		}
		catch (...) {
			cerr << "Error open Excel!" << endl;
			MessageBox(L"无法打开Excel！", L"Error", MB_ICONERROR);
			CoUninitAndExit(9);
		}
	}

}


afx_msg LRESULT COSUVariableSpeedBeatmapEditorDlg::OnMsgUser(WPARAM wParam, LPARAM lParam)
{
	switch (wParam)
	{
	case MSG_WORKBOOK_AFTER_SAVE:
	{
		OnBnClickedBtnSaveFile();
		break;
	}
	case MSG_WORKBOOK_BEFORE_CLOSE:
	{
		excelHasOpen = false;
		this->DeleteTmpFile();
		break;
	}
	default:
		break;
	}
	return 0;
}


void COSUVariableSpeedBeatmapEditorDlg::OnBnClickedBtnSaveFile()
{
	// TODO: 保存谱面
	using namespace std;
	wstring fileWithPath = beatmapManager->getFileWithPath();
	wstring tmpFileNameWithPath = fileWithPath + L".cyf";
	vector<wstring> variableSpeedVector = beatmapManager->getVariableSpeedVector();
	fstream tmpFile(tmpFileNameWithPath, ios::in);

	if (((CButton*)GetDlgItem(IDC_AUTOBACKUP))->GetCheck())
	{
		//备份文件
		if (!hasAskedIfCoverOldBeatmapBackupFile)
		{
			if (!CopyFile(fileWithPath.c_str(), (fileWithPath + L".bak").c_str(), TRUE))
			{
				int res = MessageBox(L"备份文件已存在,是否覆盖当前备份？\n注：此弹窗每次打开文件后仅显示一次", L"?", MB_ICONQUESTION | MB_YESNO);
				//if (res == IDOK)
				//	CoverOldBeatmapBackupFile = TRUE;
				//else
				//	CoverOldBeatmapBackupFile = FALSE;
				if (res == IDOK)
					CopyFile(fileWithPath.c_str(), (fileWithPath + L".bak").c_str(), FALSE);
			}
			hasAskedIfCoverOldBeatmapBackupFile = TRUE;
		}
		//CopyFile(fileWithPath.c_str(), (fileWithPath + L".bak").c_str(), !CoverOldBeatmapBackupFile);
	}
	if (!tmpFile.is_open())
	{
		MessageBox(L"临时文件打开失败", L"Error", MB_ICONERROR);
		return;
	}
	vector<wstring> newVariableSpeedBeatmapVector;
	string tmpNewBeatmap;
	while (getline(tmpFile, tmpNewBeatmap))
		newVariableSpeedBeatmapVector.push_back(str2wstr_2ANSI(tmpNewBeatmap));
	beatmapManager->changeVariableSpeedVector(newVariableSpeedBeatmapVector);
	tmpFile.close();
	beatmapManager->save();
}


void COSUVariableSpeedBeatmapEditorDlg::OnBnClickedBtnSaveFileAs()
{
	// TODO: 谱面另存为

	//过滤的文件扩展名
	const TCHAR extsFilter[] = _T("Osu! Beatmap Files (*.osu)|*.osu|")
		_T("文本文档 (*.txt)|*.txt|")
		_T("All Files (*.*)|*.*||");//';'号间隔多个后缀名

	CFileDialog FileSelectionDlg(FALSE/*TRUE为"打开"窗口,FALSE为"另存为"窗口*/,
		NULL/*默认自动追加的文件扩展名*/,
		beatmapManager->getFileName().c_str()/*"文件名"一栏默认填充文本*/,
		NULL/*自定义属性*/,
		extsFilter/*文件扩展名过滤*/,
		this/*父窗口*/,
		0/*文件选择窗口的版本*/,
		TRUE/*是否启用新样式文件选择窗口*/
	);
	INT_PTR result = FileSelectionDlg.DoModal();//打开窗口并等待返回
	if (result != IDOK)
		return;
	//else
	//点击了 "打开"/"保存" 按钮
	CString filePathCStr = FileSelectionDlg.GetPathName();
	std::wstring filePathWStr = filePathCStr;
	beatmapManager->saveAs(filePathWStr);
}


void COSUVariableSpeedBeatmapEditorDlg::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	//lpMMI->ptMinTrackSize = { 600, 450 };

	CDialogEx::OnGetMinMaxInfo(lpMMI);
}

void COSUVariableSpeedBeatmapEditorDlg::DeleteTmpFile()
{
	if (appPtr)
	{
		try {
			HRESULT hRes = appPtr->Quit();
			if (FAILED(hRes))
				MessageBox(L"Excel App关闭失败", L"Warning", MB_ICONWARNING);
			while (appPtr->Workbooks->Count) Sleep(100);
		}
		catch (...) {
			//MessageBox(L"Excel App关闭失败", L"Warning", MB_ICONWARNING);
		}
	}
	if (beatmapManager->getFileWithPath() != L"")
		DeleteFile((beatmapManager->getFileWithPath() + L".cyf").c_str());
}


void COSUVariableSpeedBeatmapEditorDlg::OnEnChangeEditfilepath()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialogEx::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
	excelHasOpen = false;
}


void COSUVariableSpeedBeatmapEditorDlg::OnExitSizeMove()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	//编辑框垂直居中
	CRect rect;
	GetDlgItem(IDC_EDITFILEPATH)->GetClientRect(&rect);
	LOGFONT lf;
	CFont* font = GetDlgItem(IDC_EDITFILEPATH)->GetFont();
	font->GetLogFont(&lf);
	//rect.OffsetRect(0, (rect.Height() - (lf.lfHeight < 0 ? -lf.lfHeight : lf.lfHeight)) / 2); //设置内容的左边距与上边距，大小自适取
	rect.OffsetRect(0, (rect.Height() - (lf.lfHeight < 0 ? -lf.lfHeight : lf.lfHeight)) / 2 - (lf.lfHeight < 0 ? -lf.lfHeight : lf.lfHeight) / 8); //设置内容的左边距与上边距，大小自适取
	//((CEdit*)GetDlgItem(IDC_EDITFILEPATH))->SetPasswordChar(_T('*')); //设置字符显示为密码模式
	::SendMessage(((CEdit*)GetDlgItem(IDC_EDITFILEPATH))->m_hWnd, EM_SETRECT, 0, (LPARAM)&rect);

	CDialogEx::OnExitSizeMove();
}


void COSUVariableSpeedBeatmapEditorDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	

	// TODO: 在此处添加消息处理程序代码
}
