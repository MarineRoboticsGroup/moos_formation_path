/************************************************************/
/*    NAME: Nicole Thumma                                   */
/*    ORGN: MIT, Cambridge MA                               */
/*    FILE: RangeToPosition.h                               */
/************************************************************/

#ifndef RangeToPosition_HEADER
#define RangeToPosition_HEADER

// #include <MOOS/libMOOSGeodesy>
#include "MOOS/libMOOSGeodesy/MOOSGeodesy.h"
#include "MOOS/libMOOS/Thirdparty/AppCasting/AppCastingMOOSApp.h"

using namespace std;

class RangeToPosition : public AppCastingMOOSApp
{
public:
  RangeToPosition();
  ~RangeToPosition();

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
  CMOOSGeodesy m_geodesy;
  double leader_speed;

  double m_leader_range;
  double m_other_follower_range;

  bool self_updated;
  double m_leader_x;
  double m_leader_y;

  double self_leader_angle;

  double m_self_x;
  double m_self_y;
  double prev_self_x;
  double prev_self_y;

  double leader_x_est;
  double leader_y_est;
  double prev_leader_x_est;
  double prev_leader_y_est;


  string m_gnd_node_report;
  string est_msg;
  string est_node_report;

  double m_leader_range_time;
  double m_bearing_time;
  double m_self_time;
  double prev_time;
};

#endif
