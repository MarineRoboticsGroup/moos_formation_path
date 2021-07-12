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

  double m_self_heading;
  double m_leader_heading;

  string est_msg;
};

#endif
