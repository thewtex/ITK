add_example(DOMFindDemo)

add_executable(ParticleSwarmOptimizerReadWrite
  ParticleSwarmOptimizerReadWrite.cxx
  itkParticleSwarmOptimizerDOMReader.cxx
  itkParticleSwarmOptimizerDOMWriter.cxx
  itkParticleSwarmOptimizerSAXReader.cxx
  itkParticleSwarmOptimizerSAXWriter.cxx
  )
target_link_libraries(ParticleSwarmOptimizerReadWrite ${ITK_LIBRARIES})
