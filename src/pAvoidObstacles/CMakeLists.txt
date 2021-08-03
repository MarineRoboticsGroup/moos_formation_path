#--------------------------------------------------------
# The CMakeLists.txt for:                       pAvoidObstacles
# Author(s):                              Nicole Thumma
#--------------------------------------------------------

SET(SRC
  AvoidObstacles.cpp
  AvoidObstacles_Info.cpp
  main.cpp
)

ADD_EXECUTABLE(pAvoidObstacles ${SRC})

TARGET_LINK_LIBRARIES(pAvoidObstacles
   ${MOOS_LIBRARIES}
   apputil
   mbutil
   m
   pthread)

