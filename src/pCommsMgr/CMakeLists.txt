#--------------------------------------------------------
# The CMakeLists.txt for:                       pCommsMgr
# Author(s):                              Nicole Thumma
#--------------------------------------------------------

SET(SRC
  CommsMgr.cpp
  CommsMgr_Info.cpp
  main.cpp
)

ADD_EXECUTABLE(pCommsMgr ${SRC})

TARGET_LINK_LIBRARIES(pCommsMgr
   ${MOOS_LIBRARIES}
   apputil
   mbutil
   m
   pthread)

