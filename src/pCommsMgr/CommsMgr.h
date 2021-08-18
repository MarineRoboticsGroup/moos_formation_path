/************************************************************/
/*    NAME: Nicole Thumma                                              */
/*    ORGN: MIT, Cambridge MA                               */
/*    FILE: CommsMgr.h                                          */
/*    DATE: December 29th, 1963                             */
/************************************************************/

#ifndef CommsMgr_HEADER
#define CommsMgr_HEADER

#include "MOOS/libMOOS/Thirdparty/AppCasting/AppCastingMOOSApp.h"

using namespace std;

class CommsMgr : public AppCastingMOOSApp
{
  public:
    CommsMgr();
    ~CommsMgr();

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
    int max_bits_per_min; //TODO: Incorporate
    int num_agents;
    string self_name;

  private: // State variables
    vector<double> self_gnd_pos; //{x, y, time}
    vector<double> self_est_pos; //{x, y, time}
    map<string, vector<double>> self_to_agent_range;
    map<string, vector<double>> name_to_pos; //agent4, {x, y, time}

    // Names are alphabetized
    map<vector<string>, vector<double>> agent_to_agent_range; //{"agent1", "agent2"}, {dist, time}
};

#endif
