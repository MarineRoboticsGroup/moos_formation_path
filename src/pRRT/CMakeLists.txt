#--------------------------------------------------------
# The CMakeLists.txt for:                       pRRT
# Author(s):                              Nicole Thumma
#--------------------------------------------------------

SET(SRC
  RRT.cpp
  RRT_Info.cpp
  main.cpp
)

ADD_EXECUTABLE(pRRT ${SRC})

TARGET_LINK_LIBRARIES(pRRT
   ${MOOS_LIBRARIES}
   apputil
   mbutil
   m
   pthread)

