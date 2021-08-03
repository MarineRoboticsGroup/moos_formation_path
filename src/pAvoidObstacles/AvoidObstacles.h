/************************************************************/
/*    NAME: Nicole Thumma                                              */
/*    ORGN: MIT, Cambridge MA                               */
/*    FILE: AvoidObstacles.h                                          */
/*    DATE: December 29th, 1963                             */
/************************************************************/

#ifndef AvoidObstacles_HEADER
#define AvoidObstacles_HEADER

#include "MOOS/libMOOS/Thirdparty/AppCasting/AppCastingMOOSApp.h"

using namespace std;

class AvoidObstacles : public AppCastingMOOSApp
{
  public:
    AvoidObstacles();
    ~AvoidObstacles();

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
    map<string, vector<vector<double>>> all_obstacles; //{"Obstacle_name", ((x_1, y_1), (x_2, y_2), ...)}

  private: // State variables
    vector<double> self_pos;
};

#endif
