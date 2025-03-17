#pragma once
#include <afxwin.h>
class MyEdit :
    public CEdit
{
public:
    DECLARE_MESSAGE_MAP()
    afx_msg void OnPaint();
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
};

