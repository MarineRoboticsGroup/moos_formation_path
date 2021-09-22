#--------------------------------------------------------
# The CMakeLists.txt for:                       pWaypoints
# Author(s):                              Nicole Thumma
#--------------------------------------------------------

SET(SRC
  Waypoints.cpp
  Waypoints_Info.cpp
  main.cpp
)

ADD_EXECUTABLE(pWaypoints ${SRC})

TARGET_LINK_LIBRARIES(pWaypoints
   ${MOOS_LIBRARIES}
   apputil
   mbutil
   m
   pthread)

