#--------------------------------------------------------
# The CMakeLists.txt for:                       pGenObstacle
# Author(s):                              Nicole Thumma
#--------------------------------------------------------

SET(SRC
  GenObstacle.cpp
  GenObstacle_Info.cpp
  main.cpp
)

ADD_EXECUTABLE(pGenObstacle ${SRC})

TARGET_LINK_LIBRARIES(pGenObstacle
   ${MOOS_LIBRARIES}
   apputil
   mbutil
   m
   pthread)

