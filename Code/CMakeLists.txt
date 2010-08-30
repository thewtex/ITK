subdirs(Common Numerics BasicFilters Algorithms SpatialObject)

if( ITK_USE_PATENTED )
  subdirs( Patented )
endif( ITK_USE_PATENTED )

if( ITK_USE_REVIEW )
  subdirs( Review )
endif( ITK_USE_REVIEW )

## There should be no library dependances on IO, but IO may depend on other
## libraries, so build it last
subdirs(IO)
