#include <TSystem.h>

R__LOAD_LIBRARY(libinterface_main)
R__LOAD_LIBRARY(libfun4all)
R__LOAD_LIBRARY(libg4detectors)
R__LOAD_LIBRARY(libg4eval)
R__LOAD_LIBRARY(libktracker)
R__LOAD_LIBRARY(libanamodule)

//int run(const int runNumber, const int threshold0 = 1000, const int threshold2 = 1600, const int threshold3p = 2000, const int threshold3m = 1450, const int nEvents = 0)
int run_with_list(string base, int runNumber, 
                  double thresholds1  = 0, double thrersholds2  = 0,
                  double thresholds3  = 0, double thrersholds4  = 0,
                  double thresholds5  = 0, double thrersholds6  = 0,

                  double thresholds7  = 0, double thrersholds8  = 0,
                  double thresholds9  = 0, double thrersholds10 = 0,
                  double thresholds11 = 0, double thrersholds12 = 0,

                  double thresholds13 = 0, double thrersholds14 = 0,
                  double thresholds15 = 0, double thrersholds16 = 0,
                  double thresholds17 = 0, double thrersholds18 = 0,

                  double thresholds19 = 0, double thrersholds20 = 0,
                  double thresholds21 = 0, double thrersholds22 = 0,
                  double thresholds23 = 0, double thrersholds24 = 0,

                  double thresholds25 = 0, double thrersholds26 = 0,
                  double thresholds27 = 0, double thrersholds28 = 0,
                  double thresholds29 = 0, double thrersholds30 = 0
   )
{

   // string base = "noiseRate_2022_april";
   gSystem->mkdir(base.c_str());

   double thresholds[31] = {0,
                            thresholds1 ,  thrersholds2 ,
                            thresholds3 ,  thrersholds4 ,
                            thresholds5 ,  thrersholds6 ,

                            thresholds7 ,  thrersholds8 ,
                            thresholds9 ,  thrersholds10,
                            thresholds11,  thrersholds12,

                            thresholds13,  thrersholds14,
                            thresholds15,  thrersholds16,
                            thresholds17,  thrersholds18,

                            thresholds19,  thrersholds20,
                            thresholds21,  thrersholds22,
                            thresholds23,  thrersholds24,

                            thresholds25,  thrersholds26,
                            thresholds27,  thrersholds28,
                            thresholds29,  thrersholds30
   };


   ostringstream oss; 

   cout << runNumber << "\t";
   for( int i = 1 ; i <= 30 ; i++ )
      cout << thresholds[i] << "\t";
   cout << endl;

   GeomSvc::UseDbSvc(true);  
   GeomSvc* geom_svc = GeomSvc::instance();
      
   Fun4AllServer* se = Fun4AllServer::instance();
   se->Verbosity(0);

   oss.str(""); oss << base << "/noise_" << runNumber;
   string baseDir = oss.str().c_str();

   AnaModule* ana = new AnaModule();
   // ana->setName(oss.str().c_str());
   ana->setDir(baseDir.c_str());
   ana->setRunNumber(runNumber);
   ana->setThresholds(thresholds);
   se->registerSubsystem(ana);

   Fun4AllInputManager* in = new Fun4AllDstInputManager("DSTIN");
   in->Verbosity(0);
   oss.str("");
   oss << "/data2/e1039/dst/run_" << setfill('0') << setw(6) << runNumber << "_spin.root";
   in->fileopen(oss.str().c_str());
   se->registerInputManager(in);

   // se->run(nEvents);
   se->run(0);

   se->End();
   se->PrintTimer();
   std::cout << "All done" << std::endl;

   delete se;

   return 0;
}
