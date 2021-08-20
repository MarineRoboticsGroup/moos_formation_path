/************************************************************/
/*    NAME: Nicole Thumma                                              */
/*    ORGN: MIT, Cambridge MA                               */
/*    FILE: AvoidAgents.h                                          */
/*    DATE: December 29th, 1963                             */
/************************************************************/

#ifndef AvoidAgents_HEADER
#define AvoidAgents_HEADER

#include "MOOS/libMOOS/Thirdparty/AppCasting/AppCastingMOOSApp.h"

using namespace std;

class AvoidAgents : public AppCastingMOOSApp
{
public:
  AvoidAgents();
  ~AvoidAgents();

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
  string self_name;
  int num_agents;

private: // State variables
  vector<double> self_pos;
  map<string, vector<double>> all_est_poses;
};

#endif
