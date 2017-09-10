#pragma once

//---------------------------------------------------------------------------//
//
// CtrlWnd.hpp
//  Wrappers for various controls
//   Copyright (C) 2015-2017 tapetums
//
//---------------------------------------------------------------------------//

#include <windows.h>
#include <windowsx.h>

#include <commctrl.h>
#pragma comment(lib, "comctl32.lib") // InitCommonControls

#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib") // StrToInt

#include "UWnd.hpp"

#ifdef min
  #undef min
#endif
#ifdef max
  #undef max
#endif

//---------------------------------------------------------------------------//
// Forward Declarations
//---------------------------------------------------------------------------//

namespace tapetums
{
    class CtrlWnd;
    class LabelWnd;
    class BtnWnd;
    class EditWnd;
    class ComboBox;
    class ListWnd;
    class TreeWnd;
    class TrackbarWnd;
    class UpDownWnd;
    class DateTimeWnd;
}

//---------------------------------------------------------------------------//
// Classes
//---------------------------------------------------------------------------//

class tapetums::CtrlWnd : public tapetums::UWnd
{
    using super = UWnd;

public:
    CtrlWnd() { static Init init; }
    ~CtrlWnd() = default;

    CtrlWnd(const CtrlWnd&)             = delete;
    CtrlWnd& operator =(const CtrlWnd&) = delete;

    CtrlWnd(CtrlWnd&&)             noexcept = default;
    CtrlWnd& operator =(CtrlWnd&&) noexcept = default;

private:
    struct Init
    {
        Init()
        {
            INITCOMMONCONTROLSEX icex;

            icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
            icex.dwICC  = ICC_WIN95_CLASSES | ICC_DATE_CLASSES | ICC_USEREX_CLASSES;
            ::InitCommonControlsEx(&icex);
        }
    };

public:
    INT16 id() const
    {
        return (INT16)::GetWindowLongPtr(m_hwnd, GWLP_ID);
    }

    void id(INT16 id)
    {
        ::SetWindowLongPtr(m_hwnd, GWLP_ID, LONG_PTR(id));
    }

public:
    HWND WINAPI Create
    (
        LPCTSTR class_name,
        DWORD   style,
        DWORD   styleEx,
        HWND    hwndParent,
        INT16   id
    )
    {
        if ( m_hwnd ) { return m_hwnd; }

        style |= WS_CHILD | WS_VISIBLE;

        const auto hwnd = ::CreateWindowEx
        (
            styleEx, class_name, nullptr, style,
            CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
            hwndParent, HMENU(id), ::GetModuleHandle(nullptr),
            reinterpret_cast<LPVOID>(this)
        );
        if ( hwnd == nullptr )
        {
            return nullptr;
        }

        const auto ret = ::SetWindowSubclass
        (
            hwnd, SubclassWndProc, id, DWORD_PTR(this)
        );
        if ( ret == false )
        {
            return nullptr;
        }

        const auto cs = CREATESTRUCT
        {
            this, ::GetModuleHandle(nullptr),
            HMENU(id), hwndParent,
            CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
            LONG(style), nullptr, class_name, 0
        };

        ::SendMessage(hwnd, WM_CREATE, 0, LPARAM(&cs));

        return hwnd;
    }

public:
    static LRESULT CALLBACK SubclassWndProc
    (
        HWND hwnd, UINT msg, WPARAM wp, LPARAM lp,
        UINT_PTR, DWORD_PTR dwRefData
    )
    {
        auto wnd = reinterpret_cast<CtrlWnd*>(dwRefData);
        if ( wnd == nullptr )
        {
            return ::DefSubclassProc(hwnd, msg, wp, lp);
        }

        if ( msg == WM_CREATE )
        {
            wnd->m_hwnd = hwnd;
        }

        return wnd->WndProc(hwnd, msg, wp, lp);
    }

public:
    LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) override
    {
        return ::DefSubclassProc(hwnd, msg, wp, lp);
    }
};

//---------------------------------------------------------------------------//

class tapetums::LabelWnd : public tapetums::CtrlWnd
{
    using super = CtrlWnd;

public:
    LabelWnd()  = default;
    ~LabelWnd() = default;

    LabelWnd(const LabelWnd&)             = delete;
    LabelWnd& operator =(const LabelWnd&) = delete;

    LabelWnd(LabelWnd&&)             noexcept = default;
    LabelWnd& operator =(LabelWnd&&) noexcept = default;

public:
    HWND WINAPI Create(DWORD style, DWORD styleEx, HWND hwndParent, INT16 id)
    {
        return super::Create(WC_STATIC, style, styleEx, hwndParent, id);
    }
};

//---------------------------------------------------------------------------//

class tapetums::BtnWnd : public tapetums::CtrlWnd
{
    using super = CtrlWnd;

public:
    BtnWnd()  = default;
    ~BtnWnd() = default;

    BtnWnd(const BtnWnd&)             = delete;
    BtnWnd& operator =(const BtnWnd&) = delete;

    BtnWnd(BtnWnd&&)             noexcept = default;
    BtnWnd& operator =(BtnWnd&&) noexcept = default;

public:
    HWND WINAPI Create(DWORD style, DWORD styleEx, HWND hwndParent, INT16 id)
    {
        return super::Create(WC_BUTTON, style, styleEx, hwndParent, id);
    }

    bool WINAPI IsChecked()
    {
        return Send(BM_GETCHECK, 0, 0) ? true : false;
    }

    void WINAPI Check(UINT32 state = BST_CHECKED)
    {
        Send(BM_SETCHECK, WPARAM(state), 0);
    }

    void WINAPI Uncheck()
    {
        Send(BM_SETCHECK, (WPARAM)BST_UNCHECKED, 0);
    }
};

//---------------------------------------------------------------------------//

class tapetums::EditWnd : public tapetums::CtrlWnd
{
    using super = CtrlWnd;

public:
    EditWnd()  = default;
    ~EditWnd() = default;

    EditWnd(const EditWnd&)             = delete;
    EditWnd& operator =(const EditWnd&) = delete;

    EditWnd(EditWnd&&)             noexcept = default;
    EditWnd& operator =(EditWnd&&) noexcept = default;

public:
    HWND WINAPI Create(DWORD style, DWORD styleEx, HWND hwndParent, INT16 id)
    {
        return super::Create(WC_EDIT, style, styleEx, hwndParent, id);
    }
};

//---------------------------------------------------------------------------//

class tapetums::ComboBox : public tapetums::CtrlWnd
{
    using super = CtrlWnd;

public:
    ComboBox()  = default;
    ~ComboBox() = default;

    ComboBox(const ComboBox&)             = delete;
    ComboBox& operator =(const ComboBox&) = delete;

    ComboBox(ComboBox&&)             noexcept = default;
    ComboBox& operator =(ComboBox&&) noexcept = default;

public:
    HWND WINAPI Create(DWORD style, DWORD styleEx, HWND hwndParent, INT16 id)
    {
        return super::Create(WC_COMBOBOX, style, styleEx, hwndParent, id);
    }

    void WINAPI AddString(LPCTSTR text)
    {
        Send(CB_ADDSTRING, 0, LPARAM(text));
    }

    INT32 WINAPI SelectedIndex()
    {
        return (INT32)Send(CB_GETCURSEL, 0, 0);
    }

    void WINAPI Select(INT32 index)
    {
        Send(CB_SETCURSEL, index, 0);
    }

    INT32 WINAPI Count()
    {
        return (INT32)Send(CB_GETCOUNT, 0, 0);
    }

    void Clear()
    {
        while ( Send(CB_GETCOUNT, 0, 0 ) )
        {
            Send(CB_DELETESTRING, 0, 0);
        }
    }
};

//---------------------------------------------------------------------------//

class tapetums::ListWnd : public tapetums::CtrlWnd
{
    using super = CtrlWnd;

public:
    ListWnd()  = default;
    ~ListWnd() = default;

    ListWnd(const ListWnd&)             = delete;
    ListWnd& operator =(const ListWnd&) = delete;

    ListWnd(ListWnd&&)             noexcept = default;
    ListWnd& operator =(ListWnd&&) noexcept = default;

public:
    HWND WINAPI Create(DWORD style, DWORD styleEx, HWND hwndParent, INT16 id)
    {
        super::Create(WC_LISTVIEW, style, 0, hwndParent, id);

        ListView_SetExtendedListViewStyle(m_hwnd, styleEx);

        return m_hwnd;
    }

    INT32 WINAPI InsertColumn(LPCTSTR text, INT32 width, INT32 index = 0)
    {
        LVCOLUMN col;
        col.mask     = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
        col.fmt      = LVCFMT_LEFT;
        col.cx       = width;
        col.iSubItem = index;
        col.pszText  = LPTSTR(text);

        return ListView_InsertColumn(m_hwnd, index , &col);
    }

    INT32 WINAPI InsertItem(LPCTSTR text, INT32 index)
    {
        return InsertItem(text, index, 0, 0) ? true : false;
    }

    INT32 WINAPI InsertItem(LPCTSTR text, INT32 index, LPARAM lp)
    {
        return InsertItem(text, index, 0, lp) ? true : false;
    }

    INT32 WINAPI InsertItem(LPCTSTR text, INT32 index, INT32 image, LPARAM lp)
    {
        LVITEM item;
        item.mask     = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM;
        item.pszText  = (LPTSTR)text;
        item.iItem    = index;
        item.iSubItem = 0;
        item.iImage   = image;
        item.lParam   = lp;

        return ListView_InsertItem(m_hwnd, &item) ? true : false;
    }

    INT32 WINAPI SetItem(LPCTSTR text, INT32 index, INT32 sub_index)
    {
        LVITEM item;
        item.mask     = LVIF_TEXT;
        item.pszText  = (LPTSTR)text;
        item.iItem    = index;
        item.iSubItem = sub_index;

        return ListView_SetItem(m_hwnd, &item) ? true : false;
    }

    void WINAPI DeleteItem(INT32 index)
    {
        ListView_DeleteItem(m_hwnd, index);
    }

    void WINAPI DeleteAllItems()
    {
        ListView_DeleteAllItems(m_hwnd);
    }

    void WINAPI SetImageList(HIMAGELIST list)
    {
        ListView_SetImageList(m_hwnd, list, LVSIL_NORMAL);
    }

    INT32 WINAPI Count()
    {
        return ListView_GetItemCount(m_hwnd);
    }

    void WINAPI Select(INT32 index)
    {
        ListView_SetItemState
        (
            m_hwnd, index,
            LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED
        );
    }

    void WINAPI ClearSelect()
    {
        ListView_SetItemState
        (
            m_hwnd, SelectedIndex(), 0, LVIS_SELECTED
        );
    }

    INT32 WINAPI SelectedIndex()
    {
        return ListView_GetNextItem(m_hwnd, -1, LVNI_ALL | LVNI_SELECTED);
    }

    void WINAPI GetItemText(INT32 index, INT32 sub_index, LPTSTR pszText, INT32 cchTextMax)
    {
        LVITEM item;
        item.mask       = LVIF_TEXT;
        item.iItem      = index;
        item.iSubItem   = sub_index;
        item.pszText    = pszText;
        item.cchTextMax = cchTextMax;
        ListView_GetItem(m_hwnd, &item);
    }

    INT32 WINAPI GetItemToInt(INT32 index, INT32 sub_index)
    {
        constexpr size_t cchTextMax { 16 };
        TCHAR buf[cchTextMax];

        LVITEM item;
        item.mask       = LVIF_TEXT;
        item.iItem      = index;
        item.iSubItem   = sub_index;
        item.pszText    = buf;
        item.cchTextMax = cchTextMax;
        ListView_GetItem(m_hwnd, &item);

        return ::StrToInt(buf);
    }

    UINT WINAPI GetItemSatte(INT32 index, INT32 sub_index)
    {
        LVITEM item;
        item.mask       = LVIF_TEXT;
        item.iItem      = index;
        item.iSubItem   = sub_index;
        ListView_GetItem(m_hwnd, &item);

        return item.state;
    }

    LPARAM WINAPI GetItemLPARAM(INT32 index)
    {
        LVITEM item;
        item.mask     = LVIF_PARAM;
        item.iItem    = index;
        item.iSubItem = 0;
        ListView_GetItem(m_hwnd, &item);

        return item.lParam;
    }

    bool WINAPI IsChecked(INT32 index)
    {
        return ListView_GetCheckState(m_hwnd, index) ? true : false;
    }

    void WINAPI Check(INT32 index)
    {
        ListView_SetCheckState(m_hwnd, index, true);
    }

    void WINAPI Uncheck(INT32 index)
    {
        ListView_SetCheckState(m_hwnd, index, false);
    }
};

//---------------------------------------------------------------------------//

class tapetums::TreeWnd : public tapetums::CtrlWnd
{
    using super = CtrlWnd;

public:
    TreeWnd()  = default;
    ~TreeWnd() = default;

    TreeWnd(const TreeWnd&)             = delete;
    TreeWnd& operator =(const TreeWnd&) = delete;

    TreeWnd(TreeWnd&&)             noexcept = default;
    TreeWnd& operator =(TreeWnd&&) noexcept = default;

public:
    HWND WINAPI Create(DWORD style, DWORD styleEx, HWND hwndParent, INT16 id)
    {
        return super::Create(WC_TREEVIEW, style, styleEx, hwndParent, id);
    }

    HTREEITEM WINAPI InsertItem(LPCTSTR text, HTREEITEM parent = TVI_ROOT)
    {
        TVINSERTSTRUCT tvis;
        tvis.hParent      = parent;
        tvis.hInsertAfter = TVI_LAST;
        tvis.item.mask    = TVIF_TEXT;
        tvis.item.pszText = (LPTSTR)text;

        return TreeView_InsertItem(m_hwnd, &tvis);
    }

    void WINAPI SetImageList(HIMAGELIST list)
    {
        TreeView_SetImageList(m_hwnd, list, TVSIL_NORMAL);
    }

    void WINAPI Select(HTREEITEM hitem)
    {
        TreeView_Select(m_hwnd, hitem, TVGN_CARET);
    }

    HTREEITEM WINAPI GetSelection()
    {
        return TreeView_GetSelection(m_hwnd);
    }

public:
    LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) override
    {
        if ( msg == WM_HSCROLL || msg == WM_VSCROLL )
        {
            return ::SendMessage(GetParent(), msg, wp, lp);
        }
        else
        {
            return super::WndProc(hwnd, msg, wp, lp);
        }
    }
};

//---------------------------------------------------------------------------//

class tapetums::TrackbarWnd : public tapetums::CtrlWnd
{
    using super = CtrlWnd;

public:
    struct RANGE { UINT min; UINT max; };

public:
    TrackbarWnd()  = default;
    ~TrackbarWnd() = default;

    TrackbarWnd(const TrackbarWnd&)             = delete;
    TrackbarWnd& operator =(const TrackbarWnd&) = delete;

    TrackbarWnd(TrackbarWnd&&)             noexcept = default;
    TrackbarWnd& operator =(TrackbarWnd&&) noexcept = default;

public:
    UINT WINAPI position() const noexcept
    {
        return (UINT)::SendMessage(m_hwnd, TBM_GETPOS, 0, 0);
    }

    void WINAPI position(UINT pos) noexcept
    {
        Post(TBM_SETPOS, TRUE, LPARAM(pos));
    }

    UINT WINAPI page_size() const noexcept
    {
        return (UINT)::SendMessage(m_hwnd, TBM_GETPAGESIZE, 0, 0);
    }

    void WINAPI page_size(UINT size) noexcept
    {
        Post(TBM_SETPAGESIZE, 0, LPARAM(size));
    }

    RANGE WINAPI range() const noexcept
    {
        const auto min = (UINT)::SendMessage(m_hwnd, TBM_GETRANGEMIN, 0, 0);
        const auto max = (UINT)::SendMessage(m_hwnd, TBM_GETRANGEMAX, 0, 0);
        return RANGE{ min, max };
    }

    void WINAPI range(UINT min, UINT max) noexcept
    {
        ::PostMessage(m_hwnd, TBM_SETTICFREQ, 1, 0);
        ::PostMessage(m_hwnd, TBM_SETRANGE, TRUE, MAKELPARAM(min, max));
    }

public:
    HWND WINAPI Create(DWORD style, DWORD styleEx, HWND hwndParent, INT16 id)
    {
        return super::Create(TRACKBAR_CLASS, style, styleEx, hwndParent, id);
    }

public:
    LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) override
    {
        if ( msg == WM_HSCROLL || msg == WM_VSCROLL )
        {
            return ::SendMessage(GetParent(), msg, wp, lp);
        }
        else if ( msg == WM_NOTIFY )
        {
            return OnNotify(hwnd, msg, wp, lp);
        }
        else
        {
            return super::WndProc(hwnd, msg, wp, lp);
        }
    }

protected:
    LRESULT CALLBACK OnNotify(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
    {
        const auto hdr = LPNMHDR(lp);
        if ( hdr->code != NM_CUSTOMDRAW )
        {
            return super::WndProc(hwnd, msg, wp, lp);
        }

        return super::WndProc(hwnd, msg, wp, lp);
    }
};

//---------------------------------------------------------------------------//

class tapetums::UpDownWnd : public tapetums::CtrlWnd
{
    using super = CtrlWnd;

public:
    struct RANGE { UINT min; UINT max; };

public:
    UpDownWnd()  = default;
    ~UpDownWnd() = default;

    UpDownWnd(const UpDownWnd&)             = delete;
    UpDownWnd& operator =(const UpDownWnd&) = delete;

    UpDownWnd(UpDownWnd&&)             noexcept = default;
    UpDownWnd& operator =(UpDownWnd&&) noexcept = default;

public:
    HWND WINAPI Create(DWORD style, DWORD styleEx, HWND hwndParent, INT16 id)
    {
        return super::Create(UPDOWN_CLASS, style, styleEx, hwndParent, id);
    }

    HWND buddy()
    {
        return (HWND)::SendMessage(m_hwnd, UDM_GETBUDDY, WPARAM(0), LPARAM(0));
    }

    HWND buddy(HWND hBuddy)
    {
        return (HWND)::SendMessage(m_hwnd, UDM_SETBUDDY, WPARAM(hBuddy), LPARAM(0));
    }

    INT32 position()
    {
        return (INT32)::SendMessage(m_hwnd, UDM_GETPOS32, WPARAM(0), LPARAM(0));
    }

    INT32 position(INT32 value)
    {
        return (INT32)::SendMessage(m_hwnd, UDM_SETPOS32, WPARAM(0), LPARAM(value));
    }

    RANGE range()
    {
        RANGE range;

        ::SendMessage(m_hwnd, UDM_GETRANGE32, WPARAM(&range.min), LPARAM(&range.max));

        return range;
    }

    void range(INT32 min, INT32 max)
    {
        ::SendMessage(m_hwnd, UDM_SETRANGE32, WPARAM(min), LPARAM(max));
    }
};

//---------------------------------------------------------------------------//

class tapetums::DateTimeWnd : public tapetums::CtrlWnd
{
    using super = CtrlWnd;

public:
    DateTimeWnd()  = default;
    ~DateTimeWnd() = default;

    DateTimeWnd(const DateTimeWnd&)             = delete;
    DateTimeWnd& operator =(const DateTimeWnd&) = delete;

    DateTimeWnd(DateTimeWnd&&)             noexcept = default;
    DateTimeWnd& operator =(DateTimeWnd&&) noexcept = default;

public:
    HWND WINAPI Create(DWORD style, DWORD styleEx, HWND hwndParent, INT16 id)
    {
        return super::Create(DATETIMEPICK_CLASS, style, styleEx, hwndParent, id);
    }

public:
    void WINAPI GetDateTime(SYSTEMTIME* p_st)
    {
        ::SendMessage(m_hwnd, DTM_GETSYSTEMTIME, 0, LPARAM(p_st));
    }

    void WINAPI SetDateTime(const SYSTEMTIME& st)
    {
        ::SendMessage(m_hwnd, DTM_SETSYSTEMTIME, 0, LPARAM(&st));
    }

    void WINAPI SetFormat(LPCTSTR format)
    {
        ::SendMessage(m_hwnd, DTM_SETFORMAT, 0, LPARAM(format));
    }
};

//---------------------------------------------------------------------------//

// CtrlWnd.hpp