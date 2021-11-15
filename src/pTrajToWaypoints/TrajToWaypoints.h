/************************************************************/
/*    NAME: Nicole Thumma                                              */
/*    ORGN: MIT, Cambridge MA                               */
/*    FILE: TrajToWaypoints.h                                          */
/*    DATE: December 29th, 1963                             */
/************************************************************/

#ifndef TrajToWaypoints_HEADER
#define TrajToWaypoints_HEADER

#include "MOOS/libMOOS/Thirdparty/AppCasting/AppCastingMOOSApp.h"

using namespace std;

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
  vector<vector<double>> waypoints;
  double visit_radius;

private: // State variables
  bool no_posts;
  int current_index;
  int synced_waypoint;
  bool go_to_next;

  double nav_x;
  double nav_y;

  double dist_to_waypoint;
  string update_pt;
};

#endif
