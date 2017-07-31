#include "souistd.h"
#include "control/SMenuBar.h"

#define TIMER_POP	10

namespace SOUI
{
	static const TCHAR XmlBtnStyle[] = _T("btnStyle");
	static const TCHAR XmlMenus[] = _T("menus");

	class SMenuItem : 
		public SButton
	{
		SOUI_CLASS_NAME(SMenuItem, L"menuItem")
		friend class SMenuBar;
	public:
		SMenuItem(SMenuBar *pHostMenu);
		~SMenuItem();

		void SetData(ULONG_PTR data) { m_data = data; }
		ULONG_PTR GetData() { return m_data; }

		bool IsMenuLoaded() const;
	protected:
		UINT PopMenu();
		HRESULT OnAttrSrc(const SStringW & strValue, BOOL bLoading);

		bool OnCmd(EventArgs *e);

		virtual CSize GetDesiredSize(LPCRECT pRcContainer);
		virtual void OnFinalRelease() { delete this; }
		void OnMouseMove(UINT nFlags, CPoint pt);
		void OnLButtonUp(UINT nFlags, CPoint pt);
		void OnLButtonDown(UINT nFlags, CPoint pt);

		void OnTimer(UINT_PTR timerID);

		SOUI_MSG_MAP_BEGIN()
			MSG_WM_MOUSEMOVE(OnMouseMove)
			MSG_WM_TIMER(OnTimer)
		SOUI_MSG_MAP_END()

		SOUI_ATTRS_BEGIN()
			ATTR_CUSTOM(_T("src"), OnAttrSrc)
		SOUI_ATTRS_END()

		ULONG_PTR m_data;
		SMenuBar* m_pHostMenu;
		SMenuEx m_Menu;
	};

	SMenuItem::SMenuItem(SMenuBar *pHostMenu):
		m_pHostMenu(pHostMenu)
	{
		m_bDrawFocusRect = FALSE;
		GetEventSet()->subscribeEvent(EventCmd::EventID,Subscriber(&SMenuItem::OnCmd,this));
	}

	SMenuItem::~SMenuItem()
	{
	}

	UINT SMenuItem::PopMenu()
	{
		m_pHostMenu->m_bIsShow = TRUE;
		m_pHostMenu->m_pNowMenu = this;

		SetCheck(TRUE);

		CRect rcHost;
		::GetWindowRect(m_pHostMenu->m_hWnd, rcHost);
		CRect rcMenu = GetClientRect();
		int iRet = 0;
		iRet = m_Menu.TrackPopupMenu(TPM_RETURNCMD,
			rcHost.left + rcMenu.left, rcHost.top + rcMenu.bottom + 2, m_pHostMenu->m_hWnd);

		SetCheck(FALSE);
		m_pHostMenu->m_bIsShow = FALSE;
		if (m_pHostMenu->m_pNowMenu != this || iRet == 0)
		{
			m_pHostMenu->m_pNowMenu = NULL;
			return iRet;
		}
		m_pHostMenu->m_pNowMenu = NULL;

		// 把选择事件发送过去
		EventSelectMenu evt(m_pHostMenu);
		evt.m_id = iRet;
		evt.m_pMenu = &m_Menu;
		FireEvent(evt);

		return iRet;
	}

	void SMenuItem::OnMouseMove(UINT nFlags, CPoint pt)
	{
		CRect rcWnd = GetWindowRect();
		if (m_pHostMenu->m_bIsShow && m_pHostMenu->m_pNowMenu != this)
		{
			m_pHostMenu->m_pNowMenu = this;
			SMenuEx::ExitPopupMenu();
			SetTimer(TIMER_POP, 10);
		}
	}

	void SMenuItem::OnTimer(UINT_PTR timerID)
	{
		if (timerID == TIMER_POP)
		{
			if (!m_pHostMenu->m_bIsShow)
			{
				PopMenu();
				KillTimer(timerID);
			}
		}
	}

	CSize SMenuItem::GetDesiredSize(LPCRECT pRcContainer)
	{
		return SWindow::GetDesiredSize(pRcContainer);
	}

	HRESULT SMenuItem::OnAttrSrc(const SStringW & strValue, BOOL bLoading)
	{
		return m_Menu.LoadMenu(strValue)?S_OK:E_INVALIDARG;		
	}

	bool SMenuItem::IsMenuLoaded() const
	{
		return !m_Menu.IsEmpty();
	}

	bool SMenuItem::OnCmd(EventArgs *e)
	{
		PopMenu();
		e->bubbleUp = false;
		return true;
	}

	SMenuBar::SMenuBar() :
		m_bIsShow(FALSE),
		m_hWnd(NULL),
		m_pNowMenu(NULL)
	{
		m_evtSet.addEvent(EVENTID(EventSelectMenu));
	}

	SMenuBar::~SMenuBar()
	{
	}
	BOOL SMenuBar::Init(SHostWnd * pHostWnd)
	{
		if (::IsWindow(pHostWnd->m_hWnd))
		{
			m_hWnd = pHostWnd->m_hWnd;
			return TRUE;
		}
		return FALSE;
	}

	BOOL SMenuBar::Insert(LPCTSTR pszTitle, LPCTSTR pszResName, int iPos)
	{
		if (!pszResName)
			return FALSE;

		SMenuItem *pNewMenu = new SMenuItem(this);
		SASSERT(pNewMenu);
		InsertChild(pNewMenu);

		pugi::xml_node xmlBtnStyle = m_xmlStyle.child(XmlBtnStyle);
		if (xmlBtnStyle)
			pNewMenu->InitFromXml(xmlBtnStyle);

		if(pszTitle)
			pNewMenu->SetWindowText(pszTitle);

		pNewMenu->SetAttribute(L"src",S_CT2W(pszResName));
		pNewMenu->SetWindowText(pszTitle);

		if(!pNewMenu->IsMenuLoaded())
		{
			DestroyChild(pNewMenu);
			return FALSE;
		}
		if (iPos < 0) iPos = m_lstMenuItem.GetCount();
		m_lstMenuItem.InsertAt(iPos, pNewMenu);

		return TRUE;
	}

	BOOL SMenuBar::Insert(pugi::xml_node xmlNode, int iPos /*= -1*/)
	{
		SMenuItem *pNewMenu = new SMenuItem(this);
		SASSERT(pNewMenu);
		InsertChild(pNewMenu);

		pugi::xml_node xmlBtnStyle = m_xmlStyle.child(XmlBtnStyle);
		if (xmlBtnStyle)
			pNewMenu->InitFromXml(xmlBtnStyle);
		pNewMenu->InitFromXml(xmlNode);

		if(!pNewMenu->IsMenuLoaded())
		{
			DestroyChild(pNewMenu);
			return FALSE;
		}
		if (iPos < 0) iPos = m_lstMenuItem.GetCount();
		m_lstMenuItem.InsertAt(iPos, pNewMenu);		
		return TRUE;
	}

	BOOL SMenuBar::CreateChildren(pugi::xml_node xmlNode)
	{
		pugi::xml_node xmlBtnStyle = xmlNode.child(XmlBtnStyle);
		if (xmlBtnStyle)
		{
			m_xmlStyle.append_copy(xmlBtnStyle);
		}
		pugi::xml_node xmlTMenus = xmlNode.child(XmlMenus);
		if (xmlTMenus)
		{
			for (pugi::xml_node xmlChild = xmlTMenus.first_child(); xmlChild; xmlChild = xmlChild.next_sibling())
			{
				if (_tcsicmp(xmlChild.name(), SMenuItem::GetClassName()) != 0)
					continue;
				Insert(xmlChild);
			}
		}

		return TRUE;
	}

}