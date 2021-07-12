#--------------------------------------------------------
# The CMakeLists.txt for:                 pRangeToPosition
# Author(s):                              Nicole Thumma
#--------------------------------------------------------

SET(SRC
  RangeToPosition.cpp
  RangeToPosition_Info.cpp
  main.cpp
)

ADD_EXECUTABLE(pRangeToPosition ${SRC})

TARGET_LINK_LIBRARIES(pRangeToPosition
   ${MOOS_LIBRARIES}
   apputil
   mbutil
   m
   pthread)

