#--------------------------------------------------------
# The CMakeLists.txt for:                       pTrajToWaypoints
# Author(s):                              Nicole Thumma
#--------------------------------------------------------

SET(SRC
  TrajToWaypoints.cpp
  TrajToWaypoints_Info.cpp
  main.cpp
)

ADD_EXECUTABLE(pTrajToWaypoints ${SRC})

TARGET_LINK_LIBRARIES(pTrajToWaypoints
   ${MOOS_LIBRARIES}
   geometry
   apputil
   mbutil
   m
   pthread)

