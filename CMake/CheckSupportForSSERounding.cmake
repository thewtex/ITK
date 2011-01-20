# check if the platform has sse functions
include(CheckCXXSourceCompiles)

check_cxx_source_compiles("
#include <emmintrin.h> // sse 2 intrinsics
int main()
{
float x = 0.5;
int64_t ix = _mm_cvtss_si64( _mm_set_ss(x) );
int64_t jx = _mm_cvtsd_si64( _mm_set_sd(x) );
return 0;
}
"
ITK_COMPILER_SUPPORTS_SSE2_64
)

check_cxx_source_compiles("
#include <emmintrin.h> // sse 2 intrinsics
int main()
{
float x = 0.5;
int32_t ix = _mm_cvtss_si32( _mm_set_ss(x) );
int32_t jx = _mm_cvtsd_si32( _mm_set_sd(x) );
return 0;
}
"
ITK_COMPILER_SUPPORTS_SSE2_32
)
