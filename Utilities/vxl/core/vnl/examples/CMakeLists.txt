# vxl/vnl/examples/CMakeLists.txt
set(EXECUTABLE_OUTPUT_PATH ${vxl_BINARY_DIR}/core/vnl/examples)

#link_libraries( vnl_algo vnl vul )
link_libraries( itkvnl_algo itkvnl )

#add_executable(time_fastops    time_fastops.cxx)
add_executable(vnl_amoeba      vnl_amoeba.cxx)
add_executable(vnl_complex_svd vnl_complex_svd.cxx)
add_executable(vnl_planefit    vnl_planefit.cxx)
add_executable(vnl_polyroots   vnl_polyroots.cxx)
add_executable(vnl_rosenbrock  vnl_rosenbrock.cxx)
add_executable(vnl_svd         vnl_svd.cxx)
#add_executable(vnl_svd_economy vnl_svd_economy.cxx)
add_executable(vnl_lsqr_test   vnl_lsqr_test.cxx)
