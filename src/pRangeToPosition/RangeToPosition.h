/************************************************************/
/*    NAME: Nicole Thumma                                   */
/*    ORGN: MIT, Cambridge MA                               */
/*    FILE: RangeToPosition.h                               */
/************************************************************/

#ifndef RangeToPosition_HEADER
#define RangeToPosition_HEADER

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
  double m_leader_range;
  double m_other_follower_range;

  double m_self_x;
  double m_self_y;
  double m_leader_x;
  double m_leader_y;

  double self_leader_angle;

  double prev_leader_range;
  double prev_self_leader_angle;
  double prev_self_x;
  double prev_self_y;

  double m_time;
  double prev_time;

  string m_gnd_node_report;
  string est_msg;
  string est_node_report;
};

#endif
