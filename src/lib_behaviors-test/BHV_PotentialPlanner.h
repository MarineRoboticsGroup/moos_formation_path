/************************************************************/
/*    NAME: Nicole Thumma                                              */
/*    ORGN: MIT                                             */
/*    FILE: BHV_PotentialPlanner.h                                      */
/*    DATE:                                                 */
/************************************************************/

#ifndef PotentialPlanner_HEADER
#define PotentialPlanner_HEADER

#include <string>
#include "IvPBehavior.h"

class BHV_PotentialPlanner : public IvPBehavior {
public:
  BHV_PotentialPlanner(IvPDomain);
  ~BHV_PotentialPlanner() {};
  
  bool         setParam(std::string, std::string);
  void         onSetParamComplete();
  void         onCompleteState();
  void         onIdleState();
  void         onHelmStart();
  void         postConfigStatus();
  void         onRunToIdleState();
  void         onIdleToRunState();
  IvPFunction* onRunState();
  IvPFunction* buildFunctionWithZAIC();


protected: // Local Utility functions

protected: // Configuration parameters
  vector<string> force_names;

protected: // State variables
  map<string, vector<double>> forces;
  map<string, double> headings;
  map<string, double> weights;
};

#define IVP_EXPORT_FUNCTION

extern "C" {
  IVP_EXPORT_FUNCTION IvPBehavior * createBehavior(std::string name, IvPDomain domain) 
  {return new BHV_PotentialPlanner(domain);}
}
#endif
