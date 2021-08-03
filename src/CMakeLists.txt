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

#===============================================================================
# FINDING MOOSGeodesy' HEADERS AND LIBRARIES...  
#===============================================================================
# moos - http://www.robots.ox.ac.uk/~mobile/MOOS/wiki/pmwiki.php
# find_package(MOOSGeodesy)
# INCLUDE_DIRECTORIES(${MOOSGeodesy_INCLUDE_DIRS})

# message("+++++++++++++++++++++++++++++++++++++++++")
# message("MOOSGeodesy_INCLUDE_DIRS:" ${MOOSGeodesy_INCLUDE_DIRS})
# message("MOOSGeodesy_LIBRARY_PATH:" ${MOOSGeodesy_LIBRARY_PATH})
# message("+++++++++++++++++++++++++++++++++++++++++")

#============================================================================
# List the subdirectories to build...
#============================================================================
ADD_SUBDIRECTORY(pAvoidAgents)
ADD_SUBDIRECTORY(pAvoidObstacles)
ADD_SUBDIRECTORY(pCommsMgr)
ADD_SUBDIRECTORY(pEOptimality)
ADD_SUBDIRECTORY(pGenObstacle)
ADD_SUBDIRECTORY(pLocalize)
ADD_SUBDIRECTORY(pTrajToWaypoints)

##############################################################################
#                           END of CMakeLists.txt
##############################################################################

