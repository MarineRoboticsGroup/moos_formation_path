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


private: // State variables

};

#endif
