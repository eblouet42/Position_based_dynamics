# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles\\Feur_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\Feur_autogen.dir\\ParseCache.txt"
  "Feur_autogen"
  )
endif()
