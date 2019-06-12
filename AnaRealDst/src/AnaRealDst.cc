#include <iomanip>
#include <TFile.h>
#include <TNtuple.h>
#include <TH1D.h>
#include <TCanvas.h>
#include <interface_main/SQRun.h>
#include <interface_main/SQEvent.h>
#include <interface_main/SQHitVector.h>
#include <fun4all/Fun4AllReturnCodes.h>
#include <phool/PHNodeIterator.h>
#include <phool/PHIODataNode.h>
#include <phool/getClass.h>
#include "UtilSQHit.h"
#include "AnaRealDst.h"
using namespace std;

AnaRealDst::~AnaRealDst()
{
  ;
}

int AnaRealDst::Init(PHCompositeNode* topNode)
{
  return Fun4AllReturnCodes::EVENT_OK;
}

int AnaRealDst::InitRun(PHCompositeNode* topNode)
{
  //SQRun* run = findNode::getClass<SQRun>(topNode, "SQRun");
  //if (!run) return Fun4AllReturnCodes::ABORTEVENT;
  //int run_id = run->get_run_id();

  f_out = new TFile("output.root", "RECREATE");
  nt = new TNtuple("nt", "", "evt_id:n_h1t:n_h1b:n_h1l:n_h1r:n_h2t:n_h2b:n_h2l:n_h2r");
  h1_nhit_h1x = new TH1D("h1_nhit_h1x", ";N of hits on H1X;N of events", 10, -0.5, 9.5);
  h1_nhit_h1y = new TH1D("h1_nhit_h1y", ";N of hits on H1Y;N of events", 10, -0.5, 9.5);
  h1_nhit_h2x = new TH1D("h1_nhit_h2x", ";N of hits on H2X;N of events", 10, -0.5, 9.5);
  h1_nhit_h2y = new TH1D("h1_nhit_h2y", ";N of hits on H2Y;N of events", 10, -0.5, 9.5);

  return Fun4AllReturnCodes::EVENT_OK;
}

int AnaRealDst::process_event(PHCompositeNode* topNode)
{
  SQEvent* event       = findNode::getClass<SQEvent    >(topNode, "SQEvent");
  SQHitVector* hit_vec = findNode::getClass<SQHitVector>(topNode, "SQHitVector");
  if (!event || !hit_vec) return Fun4AllReturnCodes::ABORTEVENT;
  int spill_id = event->get_spill_id();
  int event_id = event->get_event_id();

  if (! event->get_trigger(SQEvent::NIM2)
      || event->get_data_quality() != 0) {
    return Fun4AllReturnCodes::EVENT_OK;
  }

  SQHitVector* hv_h1t = UtilSQHit::FindHits(hit_vec, "H1T");
  SQHitVector* hv_h1b = UtilSQHit::FindHits(hit_vec, "H1B");
  SQHitVector* hv_h1l = UtilSQHit::FindHits(hit_vec, "H1L");
  SQHitVector* hv_h1r = UtilSQHit::FindHits(hit_vec, "H1R");
  SQHitVector* hv_h2t = UtilSQHit::FindHits(hit_vec, "H2T");
  SQHitVector* hv_h2b = UtilSQHit::FindHits(hit_vec, "H2B");
  SQHitVector* hv_h2l = UtilSQHit::FindHits(hit_vec, "H2L");
  SQHitVector* hv_h2r = UtilSQHit::FindHits(hit_vec, "H2R");

  nt->Fill(
    hv_h1t->size(), hv_h1b->size(), hv_h1l->size(), hv_h1r->size(), 
    hv_h2t->size(), hv_h2b->size(), hv_h2l->size(), hv_h2r->size()
    );

  h1_nhit_h1x->Fill(hv_h1t->size() + hv_h1b->size());
  h1_nhit_h1y->Fill(hv_h1l->size() + hv_h1r->size());
  h1_nhit_h2x->Fill(hv_h2t->size() + hv_h2b->size());
  h1_nhit_h2y->Fill(hv_h2l->size() + hv_h2r->size());

  delete hv_h1t;
  delete hv_h1b;
  delete hv_h1l;
  delete hv_h1r;
  delete hv_h2t;
  delete hv_h2b;
  delete hv_h2l;
  delete hv_h2r;
  return Fun4AllReturnCodes::EVENT_OK;
}

int AnaRealDst::End(PHCompositeNode* topNode)
{
  TCanvas* c1 = new TCanvas("c1", "");
  c1->SetGrid();
  
  h1_nhit_h1x->Draw();  c1->SaveAs("h1_nhit_h1x.png");
  h1_nhit_h1y->Draw();  c1->SaveAs("h1_nhit_h1y.png");
  h1_nhit_h2x->Draw();  c1->SaveAs("h1_nhit_h2x.png");
  h1_nhit_h2y->Draw();  c1->SaveAs("h1_nhit_h2y.png");

  delete c1;

  f_out->cd();
  f_out->Write();
  f_out->Close();
  
  return Fun4AllReturnCodes::EVENT_OK;
}