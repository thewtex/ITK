set(ITK_BioCell_SRC
itkBioGenome.cxx
itkBioGene.cxx
itkBioGeneNetwork.cxx
itkBioCellBase.cxx
itkBioCellularAggregateBase.cxx
)

add_library(ITK_BioCell ${ITK_BioCell_SRC})
target_link_libraries(ITK_BioCell  ${ITK_Mesh_LIBRARIES})
itk_module_target(ITK_BioCell)
