#ifndef _ANA_Module__H_
#define _ANA_Module__H_

#include <map>
#include <fun4all/SubsysReco.h>
#include <TString.h>
#include <TVector3.h>
#include <ktracker/SRecEvent.h>

class TH1D;
class TFile;
class TTree;
class SQReco;
class SQHitVector;
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

  void set_output_filename(const TString& n) { saveName = n; }
  void set_legacy_rec_container(bool b) { legacyContainer = b; }

  void drawMultiple(TH1D* h1[12], TString base_name, bool logY = false);//x0, y0, tx, ty, tracklets station 1, 2, 3, back partial tracks  

private:
  int GetNodes(PHCompositeNode* topNode);
  void MakeTree();

  bool legacyContainer;

  // Input
  SQHitVector* hitVector;
  SQTrackVector* trackVector;
  SQDimuonVector* dimuonVector;

  SRecEvent* recEvent;
  SQTrackVector*  recTrackVector;
  SQDimuonVector* recDimuonVector;

  // Output
  TString saveName;
  TFile* saveFile;
  int eventID;

  TTree* saveTree1;
  int charge;
  TVector3* pos1;
  TVector3* pos2;
  TVector3* pos3;
  TVector3* posvtx;
  TVector3* mom1;
  TVector3* mom2;
  TVector3* mom3;
  TVector3* momvtx;
  TVector3* rec_mom1;
  TVector3* rec_momvtx;
  TVector3* rec_posvtx;
  TVector3* rec_momtgt;
  TVector3* rec_postgt;

  TTree* saveTree2;
  TVector3* pmom;
  TVector3* nmom;
  TVector3* rec_pmom;
  TVector3* rec_nmom;
  TVector3* rec_ppos;
  TVector3* rec_npos;
  TVector3* rec_vtx;
  TVector3* vtx;
  double mass;
  double rec_mass;
  
  SQReco* recoTracker;
  
  TH1D* h_trackparameters[12];
};

#endif
