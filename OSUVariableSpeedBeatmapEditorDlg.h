
// OSUVariableSpeedBeatmapEditorDlg.h: 头文件
//

#pragma once
#include "stringProcess.h"
#include "MyEdit.h"
#include "BeatmapManager.h"
#undef RGB
#undef DialogBoxW
#import "C:\Program Files\Microsoft Office\Root\VFS\ProgramFilesCommonX64\Microsoft Shared\OFFICE16\MSO.DLL"
#import "C:\Program Files\Microsoft Office\Root\VFS\ProgramFilesCommonX86\Microsoft Shared\VBA\VBA6\VBE6EXT.OLB"
#import "C:\Program Files\Microsoft Office\root\Office16\EXCEL.EXE"
#define MSG_WORKBOOK_AFTER_SAVE 2911
#define MSG_WORKBOOK_BEFORE_CLOSE 0x622

// COSUVariableSpeedBeatmapEditorDlg 对话框
class COSUVariableSpeedBeatmapEditorDlg : public CDialogEx
{
// 构造
public:
	COSUVariableSpeedBeatmapEditorDlg(CWnd* pParent = nullptr);	// 标准构造函数
	~COSUVariableSpeedBeatmapEditorDlg();
// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_OSUVARIABLESPEEDBEATMAPEDITOR_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	// 打开文件选择窗口的按钮
	CButton m_OpenFile;
	afx_msg void OnBnClickedOpenFile();
	CEdit m_FilePathEdit;
	afx_msg void OnBnClickedOpenExcel();


	//谱面管理器对象（指针）
	BeatmapManager* beatmapManager = nullptr;

	CComPtr<Excel::_Application> app;
	Excel::_ApplicationPtr appPtr;

    BOOL hasAskedIfCoverOldBeatmapBackupFile = FALSE;
    BOOL CoverOldBeatmapBackupFile = FALSE;

protected:
    afx_msg LRESULT OnMsgUser(WPARAM wParam, LPARAM lParam);
public:
    afx_msg void OnBnClickedBtnSaveFile();
    afx_msg void OnBnClickedBtnSaveFileAs();
    afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
    void DeleteTmpFile();
    void CreateExcelApp();
private:
    CSize defaultSize;
    bool excelHasOpen = false;
public:
    afx_msg void OnEnChangeEditfilepath();
};


// 定义事件接收器类
class CExcelEventSink : public IDispatch
{
public:
    // IUnknown 方法
    STDMETHOD(QueryInterface)(REFIID riid, void** ppvObject) override
    {
        if (riid == IID_IUnknown || riid == IID_IDispatch)
        {
            *ppvObject = static_cast<IDispatch*>(this);
            AddRef();
            return S_OK;
        }
        *ppvObject = nullptr;
        return E_NOINTERFACE;
    }

    STDMETHOD_(ULONG, AddRef)() override
    {
        return InterlockedIncrement(&_refCount);
    }

    STDMETHOD_(ULONG, Release)() override
    {
        ULONG count = InterlockedDecrement(&_refCount);
        if (count == 0)
        {
            delete this;
        }
        return count;
    }

    // IDispatch 方法
    STDMETHOD(GetTypeInfoCount)(UINT* pctinfo) override
    {
        *pctinfo = 0;
        return S_OK;
    }

    STDMETHOD(GetTypeInfo)(UINT iTInfo, LCID lcid, ITypeInfo** ppTInfo) override
    {
        *ppTInfo = nullptr;
        return E_NOTIMPL;
    }

    STDMETHOD(GetIDsOfNames)(REFIID riid, LPOLESTR* rgszNames, UINT cNames, LCID lcid, DISPID* rgDispId) override
    {
        return E_NOTIMPL;
    }

    STDMETHOD(Invoke)(DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS* pDispParams, VARIANT* pVarResult, EXCEPINFO* pExcepInfo, UINT* puArgErr) override
    {
        std::cout << "dispIdMember: " << dispIdMember << std::endl;
        //工作簿保存完成
        if (dispIdMember == /*Excel::DISPID_WORKBOOKOPEN*/MSG_WORKBOOK_AFTER_SAVE/*Excel::AppEvents::WorkbookAfterSave*/)
        {
            std::cout << "Workbook After Save Event Triggered!" << std::endl;
            SendMessage(parent, WM_USER, MSG_WORKBOOK_AFTER_SAVE, MSG_WORKBOOK_AFTER_SAVE);
            return S_OK;
        }
        else if (dispIdMember == MSG_WORKBOOK_BEFORE_CLOSE)
            //Excel::AppEvents::WorkbookBeforeClose
        {
            //MessageBox(0, L"WorkbookBeforeClose", L"", 0);
            std::cout << "Workbook Before Close Event Triggered!" << std::endl;
            PostMessage(parent, WM_USER, MSG_WORKBOOK_BEFORE_CLOSE, MSG_WORKBOOK_BEFORE_CLOSE);
            return S_OK;
        }
        else if (dispIdMember == 0x621)
            //Excel::AppEvents::WorkbookBeforeClose
        {
            //MessageBox(0, L"WorkbookDeactivate", L"", 0);
        }
        else if (dispIdMember == 0x615)
            //Excel::AppEvents::WorkbookBeforeClose
        {
            //MessageBox(0, L"WindowDeactivate", L"", 0);
        }
        return E_NOTIMPL;
    }
    CExcelEventSink(HWND parent) {
        this->parent = parent;
    }
private:
    ULONG _refCount = 1;
    HWND parent = NULL;
};
