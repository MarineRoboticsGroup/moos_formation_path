##############################################################################
# FILE:        moos-ivp-extend/src/CMakeLists.txt
# DATE:        2010/09/07
#              2020/05/09 minor mods
# DESCRIPTION: CMakeLists.txt file for the moos-ivp-extend source directory
##############################################################################


#============================================================================
# Add the libraries in the current directory to the include path
#============================================================================
FILE(GLOB LOCAL_LIBRARY_DIRS ./lib_*)
INCLUDE_DIRECTORIES(${LOCAL_LIBRARY_DIRS})

#============================================================================
# List the subdirectories to build...
#============================================================================
ADD_SUBDIRECTORY(pRangeToPosition)
ADD_SUBDIRECTORY(pRigidityMetric)
ADD_SUBDIRECTORY(pTrajToWaypoints)
ADD_SUBDIRECTORY(pGenObstacle)

##############################################################################
#                           END of CMakeLists.txt
##############################################################################

