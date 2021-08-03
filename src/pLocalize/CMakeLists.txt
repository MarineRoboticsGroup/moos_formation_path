#--------------------------------------------------------
# The CMakeLists.txt for:                       pLocalize
# Author(s):                              Nicole Thumma
#--------------------------------------------------------

SET(SRC
  Localize.cpp
  Localize_Info.cpp
  main.cpp
)

ADD_EXECUTABLE(pLocalize ${SRC})

TARGET_LINK_LIBRARIES(pLocalize
   ${MOOS_LIBRARIES}
   apputil
   mbutil
   m
   pthread)

