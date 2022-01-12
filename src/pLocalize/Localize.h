/************************************************************/
/*    NAME: Nicole Thumma                                   */
/*    ORGN: MIT, Cambridge MA                               */
/*    FILE: Localize.h                               */
/************************************************************/

#ifndef Localize_HEADER
#define Localize_HEADER

#include "MOOS/libMOOS/Thirdparty/AppCasting/AppCastingMOOSApp.h"

using namespace std;

class Localize : public AppCastingMOOSApp
{
  public:
    Localize();
    ~Localize();

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
    int self_id;
    int num_agents;
    int num_anchors = 3;

  private: // State variables
    vector<double> self_est_pos;
    vector<double> self_gnd_pos;
    map<string, vector<double>> neighbor_ranges;
    map<string, vector<double>> all_est_poses;

    double last_iteration;
};

#endif
