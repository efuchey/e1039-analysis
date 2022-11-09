#ifndef _ANA_Module__H_
#define _ANA_Module__H_

#include <map>
#include <set>
#include <fun4all/SubsysReco.h>
#include <TString.h>
#include <TVector3.h>
#include <TSystem.h>
#include <TH1.h>
#include <TH2.h>
#include <geom_svc/GeomSvc.h>
#include <fstream>

class TFile;
class TTree;
class SQHitVector;
class SQEvent;
class SQTrackVector;
class SQDimuonVector;

class AnaModule: public SubsysReco 
{
public:
   AnaModule(const std::string& name = "AnaModule");
   virtual ~AnaModule();

   int Init(PHCompositeNode* topNode);
   int InitRun(PHCompositeNode* topNode);
   int process_event(PHCompositeNode* topNode);
   int End(PHCompositeNode* topNode);

   // void setName( const char* v ){ name = v; };
   void setDir( const char* v ){ outDir = v; };
   void setRunNumber(const int v){ runNumber = v ; };
   /* void setThresholds(const int v0, const int v2, const int v3p, const int v3m){ */
   /*    threshold[0] = v0 ; */
   /*    threshold[1] = 0; */
   /*    threshold[2] = v2 ; */
   /*    threshold[3] = v3p; */
   /*    threshold[4] = v3m; */
   /* }; */
   void setThresholds(double thresholds[31]){ 
      for( int i = 1 ; i <= 30 ; i++ ) threshold[i] = thresholds[i];
   };
   void drawMultiple(TH1D* h1[31], TString base_name, bool logY = false);
   void drawMultiple(TH2D* h2[31], TString base_name, bool logY = false);

   void findNoisyWires();
private:
   int GetNodes(PHCompositeNode* topNode);

   int nEvent;
   int runNumber;

   // TString name;
   TString outDir;

   GeomSvc* p_geomSvc;

   // Input
   SQHitVector* hitVector;
   SQEvent*     sqEvent  ;

   // count
   int nHitsOnChambers[31];

   int nHitsOnChambersLR[31][2];


   // HIST
   TH1D* h1_elementID[31];
   TH1D* h1_rate_elementID[31];
   TH1D* h1_pos[31];
   TH1D* h1_tdcTime  [31];

   TH2D* h2_eventID_elementID[31];
   TH2D* h2_tdcTime_elementID[31];

   // N Hits from trigger
   int nHitsTrig[2][6];
   // N Evnets from trigger
   int nEventsTrig[2][6];

   // int
   int threshold[31];
};

#endif
