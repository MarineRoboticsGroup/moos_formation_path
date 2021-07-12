#--------------------------------------------------------
# The CMakeLists.txt for:                       pRigidityMetric
# Author(s):                              Nicole Thumma
#--------------------------------------------------------

SET(SRC
  RigidityMetric.cpp
  RigidityMetric_Info.cpp
  main.cpp
)

ADD_EXECUTABLE(pRigidityMetric ${SRC})

TARGET_LINK_LIBRARIES(pRigidityMetric
   ${MOOS_LIBRARIES}
   apputil
   mbutil
   m
   pthread)

