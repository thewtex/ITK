macro( itk_module_doxygen _name _description _outputfilename )

  # first let's make sure the file is empty at the beginning
  # most straightforward technique: deleting the file
  file( REMOVE ${_outputfilename} )
  file( APPEND ${_outputfilename} "/**\n" )

  # the second _name should be changed here!
  file( APPEND ${_outputfilename} "\\defgroup ${_name} ${_name} \n" )
  file( APPEND ${_outputfilename} "${_description} \n" )

endmacro()
