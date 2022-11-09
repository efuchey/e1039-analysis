#include <iomanip>
#include <TFile.h>
#include <TTree.h>

#include <fun4all/Fun4AllReturnCodes.h>
#include <phool/getClass.h>
#include <interface_main/SQHitVector_v1.h>
#include <interface_main/SQTrackVector_v1.h>
#include <interface_main/SQDimuonVector_v1.h>
#include <interface_main/SQHit_v1.h>
#include <interface_main/SQEvent_v1.h>
#include <interface_main/SQEvent.h>
#include <interface_main/SQHardSpill.h>
#include <ktracker/SRecEvent.h>
#include <ktracker/FastTracklet.h>
#include <TCanvas.h>

#include "AnaModule.h"


using namespace std;

AnaModule::AnaModule(const std::string& name): SubsysReco(name), p_geomSvc(GeomSvc::instance())
{}

AnaModule::~AnaModule()
{}

int AnaModule::Init(PHCompositeNode* topNode)
{
   nEvent = 0;
   TString histname, title;
   for( int ip  = 1 ; ip <= 30 ; ip++ ){
      nHitsOnChambers[ip] = 0;
      nHitsOnChambersLR[ip][0] = 0;
      nHitsOnChambersLR[ip][1] = 0;
      histname  = "h1_elementID_" + std::to_string(ip);
      title =  p_geomSvc->getDetectorName(ip) + ";elementID;Count";
      Plane plane = p_geomSvc->getPlane(ip);
      h1_elementID[ip] = new TH1D(histname, title, plane.nElements, 0.5, plane.nElements+0.5);

      histname  = "h1_rate_elementID_" + std::to_string(ip);
      title =  p_geomSvc->getDetectorName(ip) + ";elementID;Rate (Hz)";
      plane = p_geomSvc->getPlane(ip);
      h1_rate_elementID[ip] = new TH1D(histname, title, plane.nElements, 0.5, plane.nElements+0.5);

      double width = plane.nElements * plane.cellWidth;
      histname  = "h1_pos_" + std::to_string(ip);
      title =  p_geomSvc->getDetectorName(ip) + ";Pos W (cm);Count";
      h1_pos      [ip] = new TH1D(histname, title, plane.nElements, -width/2., width/2.);

      histname  = "h1_tdcTime_" + std::to_string(ip);
      title = p_geomSvc->getDetectorName(ip) + ";TDC Time (ns);Counts";
      if( ip > 6 )
         h1_tdcTime[ip] = new TH1D(histname, title, 100,  700, 1300);
      else
         h1_tdcTime[ip] = new TH1D(histname, title, 100, 1000, 1600);

      histname  = "h2_eventID_elementID_" + std::to_string(ip);
      title =  p_geomSvc->getDetectorName(ip) + ";elementID;EventID";
      h2_eventID_elementID[ip] = new TH2D(histname, title, plane.nElements, 0.5, plane.nElements+0.5, 100, 0, 683939);


      histname  = "h2_tdcTime_elementID_" + std::to_string(ip);
      title =  p_geomSvc->getDetectorName(ip) + ";elementID;tdcTime";
      h2_tdcTime_elementID[ip] = new TH2D(histname, title, plane.nElements, 0.5, plane.nElements+0.5, 100, 1000, 1600);
   }

   for( int it = 0 ; it < 2 ; it++ )
      for( int in = 0 ; in < 6 ; in++ ){
         nHitsTrig[it][in] = 0;
         nEventsTrig[it][in] = 0;
      }
   
   return Fun4AllReturnCodes::EVENT_OK;
}

int AnaModule::InitRun(PHCompositeNode* topNode)
{
   int ret = GetNodes(topNode);
   if(ret != Fun4AllReturnCodes::EVENT_OK) return ret;

   return Fun4AllReturnCodes::EVENT_OK;
}

int AnaModule::process_event(PHCompositeNode* topNode)
{

  if( !sqEvent->get_trigger(SQEvent::NIM3) ) return Fun4AllReturnCodes::EVENT_OK;
  //if( !sqEvent->get_trigger(SQEvent::NIM4) ) return Fun4AllReturnCodes::EVENT_OK;

   if( sqEvent->get_trigger(SQEvent::NIM1   ) ) nEventsTrig[0][1]++;
   if( sqEvent->get_trigger(SQEvent::NIM2   ) ) nEventsTrig[0][2]++;
   if( sqEvent->get_trigger(SQEvent::NIM3   ) ) nEventsTrig[0][3]++;
   if( sqEvent->get_trigger(SQEvent::NIM4   ) ) nEventsTrig[0][4]++;
   if( sqEvent->get_trigger(SQEvent::NIM5   ) ) nEventsTrig[0][5]++;
   if( sqEvent->get_trigger(SQEvent::MATRIX1) ) nEventsTrig[1][1]++;
   if( sqEvent->get_trigger(SQEvent::MATRIX2) ) nEventsTrig[1][2]++;
   if( sqEvent->get_trigger(SQEvent::MATRIX3) ) nEventsTrig[1][3]++;
   if( sqEvent->get_trigger(SQEvent::MATRIX4) ) nEventsTrig[1][4]++;
   if( sqEvent->get_trigger(SQEvent::MATRIX5) ) nEventsTrig[1][5]++;

   nEvent++;

   for( int ih = 0 ; ih < hitVector->size() ; ih++ ){
      SQHit* hit = hitVector->at(ih);
      int detectorID = hit->get_detector_id();
      if ( detectorID > nChamberPlanes || detectorID == 0 ) continue; 
      // if ( detectorID == 3 && fabs(hit->get_element_id()-71) <= 1 ) continue;
      nHitsOnChambers[detectorID]++;
      int lr = hit->get_element_id() < p_geomSvc->getPlane(detectorID).nElements / 2. ? 0 : 1;
      nHitsOnChambersLR[detectorID][lr]++;

      h1_elementID[detectorID]->Fill(hit->get_element_id());
      h1_rate_elementID[detectorID]->Fill(hit->get_element_id());
      h1_tdcTime  [detectorID]->Fill(hit->get_tdc_time  ());

      h1_pos[detectorID]->Fill(p_geomSvc->getMeasurement(detectorID, hit->get_element_id()));

      h2_eventID_elementID[detectorID]->Fill(hit->get_element_id(), nEvent);

      h2_tdcTime_elementID[detectorID]->Fill(hit->get_element_id(), hit->get_element_id());

      if( sqEvent->get_trigger(SQEvent::NIM1   ) ) nHitsTrig[0][1]++;
      if( sqEvent->get_trigger(SQEvent::NIM2   ) ) nHitsTrig[0][2]++;
      if( sqEvent->get_trigger(SQEvent::NIM3   ) ) nHitsTrig[0][3]++;
      if( sqEvent->get_trigger(SQEvent::NIM4   ) ) nHitsTrig[0][4]++;
      if( sqEvent->get_trigger(SQEvent::NIM5   ) ) nHitsTrig[0][5]++;
      if( sqEvent->get_trigger(SQEvent::MATRIX1) ) nHitsTrig[1][1]++;
      if( sqEvent->get_trigger(SQEvent::MATRIX2) ) nHitsTrig[1][2]++;
      if( sqEvent->get_trigger(SQEvent::MATRIX3) ) nHitsTrig[1][3]++;
      if( sqEvent->get_trigger(SQEvent::MATRIX4) ) nHitsTrig[1][4]++;
      if( sqEvent->get_trigger(SQEvent::MATRIX5) ) nHitsTrig[1][5]++;

//      if( nEvent < 100 ) cout << detectorID << "\t" << hit->get_element_id() << "\t" << hit->get_tdc_time  () << endl;
      // if( nEvent == 100 )exit(0);
   }

   return Fun4AllReturnCodes::EVENT_OK;
}

int AnaModule::End(PHCompositeNode* topNode)
{
   gSystem->mkdir(outDir, true);
   gSystem->cd(outDir);

   ofstream ofs;
   ofs.open("results.txt");
   for( int ip = 1 ; ip < 31 ; ip++ ){
      ofs << ip << "\t" << nHitsOnChambers[ip] << "\t" << nEvent << "\t" << (double)nHitsOnChambers[ip]/nEvent 
          << "\t" << (double)nHitsOnChambers[ip]/nEvent * 1e-3 / (400e-9)
          << "\t" << threshold[ip]
          << endl;
      h1_rate_elementID[ip]->Scale(1./(nEvent*400.e-9) );
   }
   ofs.close();

   ofs.open("LR_results.txt");
   for( int ip = 1 ; ip < 31 ; ip++ )
      ofs << ip 
          << "\t" << nHitsOnChambersLR[ip][0] << "\t" << nEvent << "\t" << (double)nHitsOnChambersLR[ip][0]/nEvent 
          << "\t" << nHitsOnChambersLR[ip][1] << "\t" << nEvent << "\t" << (double)nHitsOnChambersLR[ip][1]/nEvent 
          << "\t" << threshold[ip]
          << endl;
   ofs.close();

   cout << nEvent << endl;

   TString name = "h1_tdcTime_" + std::to_string(runNumber) + ".pdf";
   drawMultiple(h1_tdcTime  , name);
   name = "h1_elementID_" + std::to_string(runNumber) + ".pdf";
   drawMultiple(h1_elementID, name, true);
   name = "h1_rate_elementID_" + std::to_string(runNumber) + ".pdf";
   drawMultiple(h1_rate_elementID, name, true);
   name = "h2_eventID_elementID_" + std::to_string(runNumber) + ".pdf";
   drawMultiple(h2_eventID_elementID, name);

   name = "h2_tdcTime_elementID_" + std::to_string(runNumber) + ".pdf";
   drawMultiple(h2_tdcTime_elementID, name);

   name = "h1_pos_" + std::to_string(runNumber) + ".pdf";
   drawMultiple(h1_pos, name);

   // vector<int> noise[6];
   // for( int i =   1 ; i <=  32 ; i++ ) noise[0].push_back(i);
   // for( int i =   1 ; i <=  32 ; i++ ) noise[1].push_back(i);
   // for( int i =  55 ; i <=  64 ; i++ ) noise[2].push_back(i);
   // for( int i =  55 ; i <=  64 ; i++ ) noise[3].push_back(i);
   // for( int i = 124 ; i <= 134 ; i++ ) noise[4].push_back(i);
   // for( int i = 124 ; i <= 134 ; i++ ) noise[5].push_back(i);

   // for( int id = 19 ; id <= 24 ; id++ ){
   //    int index = id - 19;
   //    cout << endl;
   //    for( int iw = 0 ; iw < noise[index].size() ; iw++ ){
   //       double x_min, x_max, y_min, y_max;
   //       p_geomSvc->getWireEndPoints(id, noise[index][iw], x_min, x_max, y_min, y_max);
   //       cout << p_geomSvc->getDetectorName(id) << "\t" << noise[index][iw] << "\t" << x_min << "\t" << x_max << endl;
   //    }
   // }

   cout << " ---- # of triggered events ---- " << endl;
   cout << "   NIM1: " << nEventsTrig[0][1] << endl;
   cout << "   NIM2: " << nEventsTrig[0][2] << endl;
   cout << "   NIM3: " << nEventsTrig[0][3] << endl;
   cout << "   NIM4: " << nEventsTrig[0][4] << endl;
   cout << "   NIM5: " << nEventsTrig[0][5] << endl;
   cout << "MATRIX1: " << nEventsTrig[1][1] << endl;
   cout << "MATRIX2: " << nEventsTrig[1][2] << endl;
   cout << "MATRIX3: " << nEventsTrig[1][3] << endl;
   cout << "MATRIX4: " << nEventsTrig[1][4] << endl;
   cout << "MATRIX5: " << nEventsTrig[1][5] << endl;

   cout << "\n ---- # of triggered hits ---- " << endl;
   cout << "   NIM1: " << nHitsTrig[0][1] << endl;
   cout << "   NIM2: " << nHitsTrig[0][2] << endl;
   cout << "   NIM3: " << nHitsTrig[0][3] << endl;
   cout << "   NIM4: " << nHitsTrig[0][4] << endl;
   cout << "   NIM5: " << nHitsTrig[0][5] << endl;
   cout << "MATRIX1: " << nHitsTrig[1][1] << endl;
   cout << "MATRIX2: " << nHitsTrig[1][2] << endl;
   cout << "MATRIX3: " << nHitsTrig[1][3] << endl;
   cout << "MATRIX4: " << nHitsTrig[1][4] << endl;
   cout << "MATRIX5: " << nHitsTrig[1][5] << endl;


   findNoisyWires();

   cout << outDir << endl;

   return Fun4AllReturnCodes::EVENT_OK;
}

int AnaModule::GetNodes(PHCompositeNode* topNode)
{
   hitVector = findNode::getClass<SQHitVector>(topNode, "SQHitVector");
   sqEvent   = findNode::getClass<SQEvent    >(topNode, "SQEvent"    );
   if( !hitVector || !sqEvent )
   {
      return Fun4AllReturnCodes::ABORTEVENT;
   }
   return Fun4AllReturnCodes::EVENT_OK;
}

void AnaModule::drawMultiple(TH1D* h1[31], TString base_name, bool logY){
   gROOT->SetStyle("Plain");

   TCanvas* c1 = new TCanvas("c1", "", 1200, 1800);
   c1->Divide(2, 3);
   for( int ip = 1 ; ip <= 30 ; ip++ ){
      if( (ip-1) / 6 == 1 ) continue;
      c1->cd((ip-1)%6+1)->SetGrid();
      if( logY ) c1->cd((ip-1)%6+1)->SetLogy();
      else       h1[ip]->GetYaxis()->SetRangeUser(0, h1[ip]->GetMaximum() * 1.05);
      // else       h1[ip]->GetYaxis()->SetRangeUser(0, 150);
      // else       h1[ip]->GetYaxis()->SetRangeUser(0, 300);
      // h1[ip]->GetXaxis()->SetRangeUser(85, 95);
      h1[ip]->Draw("HIST");
      if( ip % 6 == 0 ){
         TString name = base_name;
         if( ip == 6  ) name += "(";
         if( ip == 30 ) name += ")";
         c1->SaveAs(name);
      }
   }
   delete c1;
}

void AnaModule::drawMultiple(TH2D* h2[31], TString base_name, bool logY){
   gROOT->SetStyle("Plain");

   TCanvas* c1 = new TCanvas("c1", "", 800, 600);
   c1->SetGrid();
   for( int ip = 1 ; ip <= 30 ; ip++ ){
      // c1->cd((ip-1)%6+1)->SetGrid();
      // if( logY ) c1->cd((ip-1)%6+1)->SetLogy();
      // else       h1[ip]->GetYaxis()->SetRangeUser(0, h1[ip]->GetMaximum() * 1.05);
      h2[ip]->Draw("COLZ");
      TString name = base_name;
      if( ip == 1  ) name += "(";
      if( ip == 30 ) name += ")";
      c1->SaveAs(name);
   }
   delete c1;
}

void AnaModule::findNoisyWires(){

   for( int id = 1 ; id <= 30 ; id++ ){
      cout << " %%%%%%%%% " << setw(2) << id << " %%%%%%%%%% " << endl;

      double average = 0, sigma = 0;
      int nEle = 0;
      int prev_noisy = -1;
      vector<int> noisy_ele;
      while( prev_noisy != (int)noisy_ele.size() ){
         prev_noisy = noisy_ele.size();
         average = 0;
         nEle = 0;
         sigma = 0;
         for( int ib = 1 ; ib <= h1_elementID[id]->GetNbinsX() ; ib++ )
            if( std::find(noisy_ele.begin(), noisy_ele.end(), ib) == noisy_ele.end() ){
               average += h1_elementID[id]->GetBinContent(ib);
               nEle++;
            }
         average /= nEle;
         for( int ib = 1 ; ib <= h1_elementID[id]->GetNbinsX() ; ib++ )
            if( std::find(noisy_ele.begin(), noisy_ele.end(), ib) == noisy_ele.end() )
               sigma += ( h1_elementID[id]->GetBinContent(ib) - average ) * ( h1_elementID[id]->GetBinContent(ib) - average );
         
         sigma = sqrt( sigma / nEle );
         
         for( int ib = 1 ; ib <= h1_elementID[id]->GetNbinsX() ; ib++ )
            if( std::find(noisy_ele.begin(), noisy_ele.end(), ib) == noisy_ele.end() && fabs(h1_elementID[id]->GetBinContent(ib) - average) > 3 * sigma )
               noisy_ele.push_back(ib);

      }

      sort(noisy_ele.begin(), noisy_ele.end());      

      cout << "nEle: " << noisy_ele.size() << endl;
      for( int in = 0 ; in < noisy_ele.size() ; in++ )
         cout << noisy_ele[in] << ", " << flush;
      cout << endl;
      cout << " %%%%%%%%%%" << "%%" << "%%%%%%%%%%% " << endl;
   }

}
