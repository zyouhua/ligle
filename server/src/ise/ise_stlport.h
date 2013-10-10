///////////////////////////////////////////////////////////////////////////////
// Iris Server Engine (ISE)
//
// 文件名称: ise_stlport.h
// 功能描述: STL移植性处理
// 最后修改: 2005-12-30
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
// "非标准STL" 包含文件

#define USING_EXT_STL

#ifdef USING_EXT_STL
#include <ext/hash_map>
#include <ext/hash_set>
#endif

///////////////////////////////////////////////////////////////////////////////
// "非标准STL" 命名空间

#ifdef USING_EXT_STL
#define EXT_STL_NAMESPACE   __gnu_cxx
#endif

using namespace std;
using namespace EXT_STL_NAMESPACE;

///////////////////////////////////////////////////////////////////////////////
// string hasher 定义

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
