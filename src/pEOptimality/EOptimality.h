/************************************************************/
/*    NAME: Nicole Thumma                                              */
/*    ORGN: MIT, Cambridge MA                               */
/*    FILE: EOptimality.h                                          */
/*    DATE: December 29th, 1963                             */
/************************************************************/

#ifndef EOptimality_HEADER
#define EOptimality_HEADER

#include "MOOS/libMOOS/Thirdparty/AppCasting/AppCastingMOOSApp.h"

using namespace std;

class EOptimality : public AppCastingMOOSApp
{
 public:
   EOptimality();
   ~EOptimality();

 protected: // Standard MOOSApp functions to overload  
   bool OnNewMail(MOOSMSG_LIST &NewMail);
   bool Iterate();
   bool OnConnectToServer();
   bool OnStartUp();
   vector<vector<double>> buildFischerMatrix();
   vector<vector<double>> getMinFIMEigenpair();
   vector<double> getGradientOfFIMEigenpair(double lambda, vector<double> v);
   vector<vector<double>> getPartialDerivOfMatrix(int index);

 protected: // Standard AppCastingMOOSApp function to overload 
   bool buildReport();

 protected:
   void registerVariables();

private: // Configuration variables
  string self_name;
  int num_agents;
  int num_anchors = 3;

private: // State variables
  vector<double> self_pos;
  map<string, vector<double>> all_est_poses;
  vector<vector<double>> fim;
};

#endif 
