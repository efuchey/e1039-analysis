#include <TSystem.h>

R__LOAD_LIBRARY(libinterface_main)
R__LOAD_LIBRARY(libfun4all)
R__LOAD_LIBRARY(libg4detectors)
R__LOAD_LIBRARY(libg4eval)
R__LOAD_LIBRARY(libktracker)

/*
This macro takes severl external input files to run:
1. geom.root is the standard geometry dump from running the Fun4Sim macro;
2. e906_run7.opts is provided
3. digit_028692_009.root is E906 run6 data, can be found at /pnfs/e906/production/digit/R009/02/86

This is an example script intended to demonstrate how to run SQReco in a minimalistic fashion, it is NOT
suitable for production use and users should develop their own reconstruction macro for their own analysis.
*/

int RecoE906Data(const int nEvents = 1)
{
  const double FMAGSTR = -1.054;
  const double KMAGSTR = -0.951;

  recoConsts* rc = recoConsts::instance();
  rc->set_DoubleFlag("FMAGSTR", FMAGSTR);
  rc->set_DoubleFlag("KMAGSTR", KMAGSTR);
  rc->Print();

  Fun4AllServer* se = Fun4AllServer::instance();
  se->Verbosity(100);

  //JobOptsSvc* jobopt_svc = JobOptsSvc::instance();
  //jobopt_svc->init("support/e906_run7.opts");

  GeomSvc::UseDbSvc(true);  //set to true to run E1039 style data
  GeomSvc* geom_svc = GeomSvc::instance();

  SQReco* reco = new SQReco();
  reco->Verbosity(0);
  reco->set_geom_file_name("support/geom.root");
  reco->set_enable_KF(true); //Kalman filter not needed for the track finding, disabling KF saves a lot of initialization time
  reco->setInputTy(SQReco::E906);    //options are SQReco::E906 and SQReco::E1039
  reco->setFitterTy(SQReco::KFREF);  //not relavant for the track finding
  reco->set_evt_reducer_opt("aoc"); //if not provided, event reducer will be using JobOptsSvc to intialize; to turn off, set it to "none"
  reco->set_enable_eval(true);
  reco->set_eval_file_name("eval.root");
  se->registerSubsystem(reco);

  Fun4AllSRawEventInputManager* in = new Fun4AllSRawEventInputManager("SRawEventIM");
  in->Verbosity(0);
  in->set_tree_name("save");
  in->set_branch_name("rawEvent");
  in->fileopen("digit_028705_009.root");
  se->registerInputManager(in);

  Fun4AllDstOutputManager* out = new Fun4AllDstOutputManager("DSTOUT", "result.root");
  se->registerOutputManager(out);
  // out->AddNode("SRawEvent");
  // out->AddNode("SRecEvent");
  TVector3 ep1, ep2;
  for(int i =1; i<=54; i++){
    cout<<i<<"    "<<geom_svc->getPlanePosition(i)<<"    "<<geom_svc->getPlaneNElements(i)
	<<"    "<<geom_svc->getPlaneSpacing(i)<<"    "<<geom_svc->getPlane(i).xoffset
	<<"    "<<geom_svc->getPlaneScaleX(i)<<"    "<<geom_svc->getPlane(i).x0<<"    "<<geom_svc->getCostheta(i)
	<<"    "<<geom_svc->getPlaneScaleY(i)<<"    "<<geom_svc->getPlane(i).y0<<"    "<<geom_svc->getSintheta(i)
	<<"    "<<geom_svc->getPlaneResolution(i)<<"    ";
    geom_svc->getEndPoints(i, 1, ep1, ep2);
    double dp1x_ = -ep1.X();
    double dp1y_ = -ep1.Y();
    double dp1z_ = -ep1.Z();
    cout<< ep1.X() <<" "<< ep1.Y() <<" "<< ep1.Z() <<" ";//<< ep2.X()-ep1.X() <<" "<< ep2.Y()-ep1.Y() <<" "<< ep2.Z()-ep1.Z() <<" ";
    double deltapx_ = 0, deltapy_ = 0, deltapz_ = 0;
    for(int j = 1; j<geom_svc->getPlaneNElements(i); j++){
      geom_svc->getEndPoints(i, j, ep1, ep2);
      deltapx_+=(ep2.X()-ep1.X());
      deltapy_+=(ep2.Y()-ep1.Y());
      deltapz_+=(ep2.Z()-ep1.Z());
    }
    deltapx_/= (geom_svc->getPlaneNElements(i));
    deltapy_/= (geom_svc->getPlaneNElements(i));
    deltapz_/= (geom_svc->getPlaneNElements(i));
    geom_svc->getEndPoints(i, geom_svc->getPlaneNElements(i), ep1, ep2);
    dp1x_+=ep1.X();
    dp1y_+=ep1.Y();
    dp1z_+=ep1.Z();
    dp1x_/=(geom_svc->getPlaneNElements(i)-1);
    dp1y_/=(geom_svc->getPlaneNElements(i)-1);
    dp1z_/=(geom_svc->getPlaneNElements(i)-1);
    cout<< deltapx_ <<" "<< deltapy_ <<" "<< deltapz_<<" "<< dp1x_ <<" "<< dp1y_ <<" "<< dp1z_ <<"    ";
    if(i>46){
      for(int k =0; k<9; k++)cout<<geom_svc->getPlane(i).deltaW_module[k]<<" ";
      cout<<endl;
    }else{
      cout<<geom_svc->getPlane(i).deltaW<<endl;
    }
  }

  geom_svc->getEndPoints(16, 103, ep1, ep2);
  cout<< ep1.X() <<" "<< ep1.Y() <<" "<< ep1.Z() <<" "<< ep2.X()-ep1.X() <<" "<< ep2.Y()-ep1.Y() <<" "<< ep2.Z()-ep1.Z() <<" "<<endl;
  se->run(nEvents);
  se->End();

  delete se;
  gSystem->Exit(0);
  return 0;
}
