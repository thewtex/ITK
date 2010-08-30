subdirs(
  Iterators
  DataRepresentation
  Filtering
  IO
  Numerics
  Registration
  Segmentation
  SpatialObjects
)

#This code looks like it needs to be updated to the new statistics framework
#It was never updated as part of the refactoring to review statistics.
#TMP_RMV20100801 subdirs( Statistics )

if( ITK_USE_REVIEW )
  subdirs( Review )
endif( ITK_USE_REVIEW )

if( ITK_USE_PATENTED )
  subdirs( Patented )
endif( ITK_USE_PATENTED )

