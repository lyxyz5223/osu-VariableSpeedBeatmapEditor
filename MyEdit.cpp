#include "pch.h"
#include "MyEdit.h"
BEGIN_MESSAGE_MAP(MyEdit, CEdit)
	ON_WM_PAINT()
    ON_WM_CREATE()
END_MESSAGE_MAP()


void MyEdit::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: �ڴ˴������Ϣ����������
	// ��Ϊ��ͼ��Ϣ���� CEdit::OnPaint()
    CEdit::OnPaint();
    //CRect rect;
    //GetClientRect(&rect); // ��ȡ�ؼ��Ŀͻ�����
    //CString strText;
    //GetWindowText(strText); // ��ȡ�༭���е��ı�
    //// �������ֵĴ�С
    //CSize textSize = dc.GetTextExtent(strText);
    //// ���㴹ֱ���е�ƫ����
    //int yOffset = (rect.Height() - textSize.cy) / 2;
    //// ��������
    //dc.TextOut(rect.left, yOffset, strText);
    
}


int MyEdit::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CEdit::OnCreate(lpCreateStruct) == -1)
        return -1;

    // TODO:  �ڴ������ר�õĴ�������

    return 0;
}
