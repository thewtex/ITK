macro( itk_module_doxygen _name _dependency _description _outputfilename )
  file( APPEND ${_outputfilename} "/**\n" )

  # the second _name should be changed here!
  file( APPEND ${_outputfilename} "\defgroup ${_name} ${_name}\n" )
  file( APPEND ${_outputfilename  "${_description}\n" )

  foreach( d ${_dependency} )
    file( APPEND ${_outputfilename} "\sa ${d}\n" )
  endforeach()
endmacro()
