option(ITK_USE_SYSTEM_DCMTK "Use an outside build of DCMTK." OFF)

# iconv library seems to be present unpredictably, never
# on linux, sometimes on OS X and probably never on Windows
option(DCMTK_USE_LIBICONV "Use IConv Library in DCMTK" OFF)
