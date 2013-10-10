///////////////////////////////////////////////////////////////////////////////
// Iris Cross-Platform Library (ICL)
// icl_assert.h
///////////////////////////////////////////////////////////////////////////////

#ifndef _ICL_ASSERT_H_
#define _ICL_ASSERT_H_

namespace icl
{

///////////////////////////////////////////////////////////////////////////////

extern void InternalAssert(char *pCondition, char *pFileName, int nLineNumber);

#ifdef ICL_DEBUG
#define Assert(c)   ((c) ? (void)0 : InternalAssert(#c, __FILE__, __LINE__))
#else
#define Assert(c)   ((void)0)
#endif

///////////////////////////////////////////////////////////////////////////////

} // namespace icl

#endif // _ICL_ASSERT_H_
