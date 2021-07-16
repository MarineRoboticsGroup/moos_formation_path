/************************************************************/
/*    NAME: Nicole Thumma                                              */
/*    ORGN: MIT, Cambridge MA                               */
/*    FILE: TrajToWaypoints.h                                          */
/*    DATE: December 29th, 1963                             */
/************************************************************/

#ifndef TrajToWaypoints_HEADER
#define TrajToWaypoints_HEADER

#include "MOOS/libMOOS/Thirdparty/AppCasting/AppCastingMOOSApp.h"

class TrajToWaypoints : public AppCastingMOOSApp
{
 public:
   TrajToWaypoints();
   ~TrajToWaypoints();

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

 private: // State variables
};

#endif 
