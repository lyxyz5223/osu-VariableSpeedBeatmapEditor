#include "pch.h"
#include "MyEdit.h"
BEGIN_MESSAGE_MAP(MyEdit, CEdit)
	ON_WM_PAINT()
    ON_WM_CREATE()
END_MESSAGE_MAP()


void MyEdit::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 在此处添加消息处理程序代码
	// 不为绘图消息调用 CEdit::OnPaint()
    CEdit::OnPaint();
    //CRect rect;
    //GetClientRect(&rect); // 获取控件的客户区域
    //CString strText;
    //GetWindowText(strText); // 获取编辑框中的文本
    //// 计算文字的大小
    //CSize textSize = dc.GetTextExtent(strText);
    //// 计算垂直居中的偏移量
    //int yOffset = (rect.Height() - textSize.cy) / 2;
    //// 绘制文字
    //dc.TextOut(rect.left, yOffset, strText);
    
}


int MyEdit::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CEdit::OnCreate(lpCreateStruct) == -1)
        return -1;

    // TODO:  在此添加您专用的创建代码

    return 0;
}
