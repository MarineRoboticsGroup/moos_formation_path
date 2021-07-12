/************************************************************/
/*    NAME: Nicole Thumma                                              */
/*    ORGN: MIT, Cambridge MA                               */
/*    FILE: RRT.h                                          */
/*    DATE: December 29th, 1963                             */
/************************************************************/

#ifndef RRT_HEADER
#define RRT_HEADER

#include "MOOS/libMOOS/Thirdparty/AppCasting/AppCastingMOOSApp.h"

class RRT : public AppCastingMOOSApp
{
 public:
   RRT();
   ~RRT();

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
