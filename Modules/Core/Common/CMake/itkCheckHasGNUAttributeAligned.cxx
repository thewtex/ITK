//
// Check if the compoler support the GNU attribute extension for
// alignment, and does not contain a bug which causes internal
// compiler segfault.
//

struct A
{
  char a;
};

struct B
{
  char b;
} __attribute__ ((aligned (64)));

// fail for gcc 4.1 and 4.2.1
#if __GNUC__ == 4 && ( __GNUC_MINOR__ == 1 || \
  ( __GNUC_MINOR__ == 2 && __GNUC_PATCHLEVEL__ == 1 && __APPLE__ ) )
#error This version of GCC is known to have a internal compilation error with this feature in ITK.
#endif

// BUG DETECTION: This following usage may generate a segfault during
// compilation.
//
// The following block of code causes an internal compiler error with
// Apple's (GCC) 4.2.1 (Apple Inc. build5666) (dot 3) compiler.
//
// See https://itk.icts.uiowa.edu/jira/browse/ITK-3172
template <typename T>
class foo
{
  // NOTE: implicit constructor is required for this test

  struct A
  {
    char a;
  };
  typedef A AlignedA __attribute__ ((aligned(64)));
  AlignedA *AlignedElementsOfA;
};


// This structure will generate a compiler error if the template
// argument is false
template<bool t> struct OnlyTrue;
template<> struct OnlyTrue<true> { static const bool Result = true; };


int main()
{
  foo<int> f;

  typedef A AlignedA __attribute__ ((aligned(64)));

  return OnlyTrue<__alignof__( AlignedA ) == 64>::Result
    && OnlyTrue<__alignof__( B ) == 64>::Result;


  return 0;
}
