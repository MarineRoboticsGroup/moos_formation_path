#--------------------------------------------------------
# The CMakeLists.txt for:                       pEOptimality
# Author(s):                              Nicole Thumma
#--------------------------------------------------------


# find_package(Eigen3 REQUIRED)

SET(SRC
  EOptimality.cpp
  EOptimality_Info.cpp
  main.cpp
)

ADD_EXECUTABLE(pEOptimality ${SRC})

TARGET_LINK_LIBRARIES(pEOptimality
   ${MOOS_LIBRARIES}
   apputil
   mbutil
   m
   pthread)

