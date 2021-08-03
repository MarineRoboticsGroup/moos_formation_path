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
  void setEnv();

private: // Configuration variables
  int env;
  // boundaries
  int int_min_x;
  int int_max_x;
  int int_min_y;
  int int_max_y;

private: // State variables
  vector<string> m_new_obstacle;
};

#endif
