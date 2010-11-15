// itkWin64.h

#ifndef _ITK_WIN64
#define _ITK_WIN64


#ifdef _MSC_VER
#ifdef _WIN64

#define ADDED_EXTRA_OVERLOADS_FOR_WIN64

inline unsigned __int64 abs(__int64 x)                                       { return x < 0 ? -x : x; }
inline unsigned __int64 abs(unsigned __int64 x)                              { return x; }

namespace std
{
inline unsigned __int64 abs(__int64 x)                                       { return x < 0 ? -x : x; }
inline unsigned __int64 abs(unsigned __int64 x)                              { return x; }
}

inline unsigned __int64 vnl_math_abs(__int64 x)          { return x < 0 ? -x : x; }
inline unsigned __int64 vnl_math_abs(unsigned __int64 x) { return x; }

inline bool vnl_math_isfinite(__int64)          { return true; }
inline bool vnl_math_isfinite(unsigned __int64) { return true; }

inline bool vnl_math_isnan(__int64)          { return false; }
inline bool vnl_math_isnan(unsigned __int64) { return false; }

inline __int64          vnl_math_max(__int64 x, __int64 y)                   { return (x > y) ? x : y; }
inline unsigned __int64 vnl_math_max(unsigned __int64 x, unsigned __int64 y) { return (x > y) ? x : y; }

inline unsigned __int64 vnl_math_squared_magnitude(__int64          x) { return x*x; }
inline unsigned __int64 vnl_math_squared_magnitude(unsigned __int64 x) { return x*x; }

#endif
#endif


#endif
