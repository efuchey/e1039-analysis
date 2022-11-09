#include <TSystem.h>

R__LOAD_LIBRARY(libinterface_main)
R__LOAD_LIBRARY(libfun4all)
R__LOAD_LIBRARY(libg4detectors)
R__LOAD_LIBRARY(libg4eval)
R__LOAD_LIBRARY(libktracker)
R__LOAD_LIBRARY(libanamodule)

//int run(const int runNumber, const int threshold0 = 1000, const int threshold2 = 1600, const int threshold3p = 2000, const int threshold3m = 1450, const int nEvents = 0)
int run(const int runNumber = 3682, const int threshold2 = 1600, const int nEvents = 0)
{
   int threshold0  = 0;
   int threshold3p = 0;
   int threshold3m = 0;
   Fun4AllServer* se = Fun4AllServer::instance();
   se->Verbosity(0);

   GeomSvc::UseDbSvc(true);  
   GeomSvc* geom_svc = GeomSvc::instance();

   // string base = "results_nim3_2021_12_17";
   // string base = "results_noise_2022_02_15";
   string base = "noisy_finder";

   gSystem->mkdir(base.c_str());

   ostringstream oss; oss.str("");
   // oss << "results_D0XXp_wo71/noise_rate_" << runNumber << ".txt";
   // oss << "results_D0XXp_wo707172/noise_rate_" << runNumber << ".txt";
   oss << base << "/noise_rate_" << runNumber << ".txt";

   AnaModule* ana = new AnaModule();
   // ana->setName(oss.str().c_str());
   ana->setDir(base.c_str());
   ana->setRunNumber(runNumber);
//   ana->setThresholds(threshold0, threshold2, threshold3p, threshold3m);
   se->registerSubsystem(ana);

   Fun4AllInputManager* in = new Fun4AllDstInputManager("DSTIN");
   in->Verbosity(0);
   oss.str("");
   oss << "/data2/e1039/dst/run_" << setfill('0') << setw(6) << runNumber << "_spin.root";
   in->fileopen(oss.str().c_str());
   se->registerInputManager(in);

   se->run(nEvents);

   se->End();
   se->PrintTimer();
   std::cout << "All done" << std::endl;

   delete se;
   gSystem->Exit(0);
   return 0;
}
