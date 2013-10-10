///////////////////////////////////////////////////////////////////////////////
// Iris Server Engine (ISE)
//
// �ļ�����: ise_stlport.h
// ��������: STL��ֲ�Դ���
// ����޸�: 2005-12-30
///////////////////////////////////////////////////////////////////////////////

#ifndef _ICL_STLPORT_H_
#define _ICL_STLPORT_H_

#include <string>
#include <vector>
#include <list>
#include <set>
#include <map>
#include <deque>

///////////////////////////////////////////////////////////////////////////////
// "�Ǳ�׼STL" �����ļ�

#define USING_EXT_STL

#ifdef USING_EXT_STL
#include <ext/hash_map>
#include <ext/hash_set>
#endif

///////////////////////////////////////////////////////////////////////////////
// "�Ǳ�׼STL" �����ռ�

#ifdef USING_EXT_STL
#define EXT_STL_NAMESPACE   __gnu_cxx
#endif

using namespace std;
using namespace EXT_STL_NAMESPACE;

///////////////////////////////////////////////////////////////////////////////
// string hasher ����

#ifdef USING_EXT_STL
namespace EXT_STL_NAMESPACE
{
    template <class _CharT, class _Traits, class _Alloc>
    inline size_t stl_string_hash(const basic_string<_CharT,_Traits,_Alloc>& s)
    {
        unsigned long h = 0;
        size_t len = s.size();
        const _CharT* data = s.data();
        for (size_t i = 0; i < len; ++i)
            h = 5 * h + data[i];
        return size_t(h);
    }

    template <> struct hash <string> {
        size_t operator()(const string& s) const { return stl_string_hash(s); }
    };

    template <> struct hash <wstring> {
        size_t operator()(const wstring& s) const { return stl_string_hash(s); }
    };
}
#endif

///////////////////////////////////////////////////////////////////////////////

#endif // _ICL_STLPORT_H_ 
