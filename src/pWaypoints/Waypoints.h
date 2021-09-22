/************************************************************/
/*    NAME: Nicole Thumma                                              */
/*    ORGN: MIT, Cambridge MA                               */
/*    FILE: Waypoints.h                                          */
/*    DATE: December 29th, 1963                             */
/************************************************************/

#ifndef Waypoints_HEADER
#define Waypoints_HEADER

#include "MOOS/libMOOS/Thirdparty/AppCasting/AppCastingMOOSApp.h"

class Waypoints : public AppCastingMOOSApp
{
public:
  Waypoints();
  ~Waypoints();

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
  double nav_x;
  double nav_y;
  double waypoint_x;
  double waypoint_y;
};

#endif
