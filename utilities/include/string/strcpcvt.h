#pragma  once

#ifndef CP_ACP
#define CP_ACP 0
#endif//CP_ACP

#include "..\utilities-def.h"
#include "tstring.h"

namespace SOUI
{

    class UTILITIES_API CDuiStrCpCvt
    {
    public:
        static SStringA CvtW2A(const SStringW & str,unsigned int cp=CP_ACP);

        static SStringW CvtA2W(const SStringA & str,unsigned int cp=CP_ACP,unsigned int cp2=0);

        static SStringA CvtA2A(const SStringA & str,unsigned int cpFrom=CP_UTF8,unsigned int cpTo=CP_ACP);

        static SStringW CvtW2W(const SStringW &str,unsigned int cp=CP_ACP);

    };


}//end of namespace SOUI

#define DUI_CA2W CDuiStrCpCvt::CvtA2W
#define DUI_CW2A CDuiStrCpCvt::CvtW2A
#define DUI_CA2A CDuiStrCpCvt::CvtA2A
#define DUI_CW2W CDuiStrCpCvt::CvtW2W

#ifdef UNICODE
#define DUI_CA2T DUI_CA2W
#define DUI_CT2A DUI_CW2A
#define DUI_CW2T DUI_CW2W
#define DUI_CT2W DUI_CW2W
#else
#define DUI_CA2T DUI_CA2A
#define DUI_CT2A DUI_CA2A
#define DUI_CW2T DUI_CW2A
#define DUI_CT2W DUI_CA2W
#endif // UNICODE