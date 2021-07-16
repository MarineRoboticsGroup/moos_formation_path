/************************************************************/
/*    NAME: Nicole Thumma                                              */
/*    ORGN: MIT, Cambridge MA                               */
/*    FILE: GenObstacle.h                                          */
/*    DATE: December 29th, 1963                             */
/************************************************************/

#ifndef GenObstacle_HEADER
#define GenObstacle_HEADER

#include "MOOS/libMOOS/Thirdparty/AppCasting/AppCastingMOOSApp.h"

using namespace std;

class GenObstacle : public AppCastingMOOSApp
{
 public:
   GenObstacle();
   ~GenObstacle();

 protected: // Standard MOOSApp functions to overload  
   bool OnNewMail(MOOSMSG_LIST &NewMail);
   bool Iterate();
   bool OnConnectToServer();
   bool OnStartUp();

 protected: // Standard AppCastingMOOSApp function to overload 
   bool buildReport();

 protected:
   void registerVariables();

 private: // Configuration variables
 string name;
 double min_x;
 double max_x;
 double min_y;
 double max_y;
 int steps;

 private: // State variables
 bool all_posted;
};

#endif
