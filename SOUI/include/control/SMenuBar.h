#pragma once

#include <helper/SMenuEx.h>

namespace SOUI
{
	class SMenuItem;

	class SOUI_EXP SMenuBar : 
		public SWindow
	{
		SOUI_CLASS_NAME(SMenuBar, L"menubar")
		friend class SMenuItem;
	public:
		SMenuBar();
		~SMenuBar();

		BOOL Init(SHostWnd *pHostWnd);
		BOOL Insert(LPCTSTR pszTitle, LPCTSTR pszResName, int iPos = -1);
		BOOL Insert(pugi::xml_node xmlNode, int iPos = -1);

	protected:
		virtual BOOL CreateChildren(pugi::xml_node xmlNode);

		SArray<SMenuItem*> m_lstMenuItem;
		HWND m_hWnd;
		pugi::xml_document  m_xmlStyle;
		BOOL m_bIsShow;
		SMenuItem *m_pNowMenu;
	};

}