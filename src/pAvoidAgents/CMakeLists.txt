#--------------------------------------------------------
# The CMakeLists.txt for:                       pAvoidAgents
# Author(s):                              Nicole Thumma
#--------------------------------------------------------

SET(SRC
  AvoidAgents.cpp
  AvoidAgents_Info.cpp
  main.cpp
)

ADD_EXECUTABLE(pAvoidAgents ${SRC})

TARGET_LINK_LIBRARIES(pAvoidAgents
   ${MOOS_LIBRARIES}
   apputil
   mbutil
   m
   pthread)

