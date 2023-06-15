
# Configure example pipeline script for use from the build dir
# configure_file("SimRCExample.conf.in" "SimRCModuleExample.conf" @ONLY)

# Add a basic test of reading a brio file output by the standard pipeline
message( STATUS "============ LTTC testing ============")
 
set(_LttcTestConfFiles
  "simu.profile"
  "services.conf"
  "tracker_cell_status_service.conf"
  )

foreach(_file ${_LttcTestConfFiles})
  message( STATUS "==== configure file '${_file}'")
  configure_file("${CMAKE_CURRENT_SOURCE_DIR}/${_file}" "${CMAKE_CURRENT_BINARY_DIR}/${_file}" COPYONLY)
endforeach()

# - Truc
add_test(NAME testLTTC_truc
  COMMAND echo "Truc"
  )

# - Simulate 100 events
add_test(NAME testLTTC_simulate
  COMMAND flsimulate
  -c ${CMAKE_CURRENT_SOURCE_DIR}/simu.conf
  -o testLTTC-sd.brio
  -m testLTTC-simulate-meta.xml
  -N 1
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
  )
 
# - Apply RC then reconstruct
add_test(NAME testLTTC_reco_1
  COMMAND flreconstruct
  -p ${CMAKE_CURRENT_SOURCE_DIR}/reco_lttc.conf
  -i testLTTC-sd.brio
  -o testLTTC-rec-rc.brio
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
  ) 
set_tests_properties(testLTTC_reco_1 PROPERTIES DEPENDS testLTTC_simulate)

#############################################################
# 2022-06-15 FM :
# For developers only : uncomment the following tests to activate the visualization
add_test(NAME testLTTC_visu_1
  COMMAND flvisualize
  -i testLTTC-rec-rc.brio
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
  ) 
set_tests_properties(testLTTC_reco_1 PROPERTIES DEPENDS testLTTC_simulate) 

# end
