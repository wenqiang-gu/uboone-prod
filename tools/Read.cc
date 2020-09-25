#include "TFile.h"
#include "TTree.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TCanvas.h"
#include "TVector3.h"

#include <iostream>

bool inside_volume(const TVector3& point, float buff = 0)
{
	//cout<<"Buff: "<<buff<<endl;
	if(point.X()<0-buff || point.X()>254.8+buff || point.Y()<-115.53-buff || point.Y()>117.47+buff || point.Z()<0.1-buff || point.Z()>1036.9+buff)
	{
		return false;
	}	
	return true;
}

float muon_track_length(const TVector3& start, const TVector3& end) // track length in active volume
{
	//cout<<"Start: ("<<start.X()<<", "<<start.Y()<<", "<<start.Z()<<")"<<endl;
	//cout<<"End: ("<<end.X()<<", "<<end.Y()<<", "<<end.Z()<<")"<<endl;
	TVector3 v1(start); // default: inside
	TVector3 v2(end); // default: outside

	if(inside_volume(start) && inside_volume(end)){
		//cout<<"Fully contained!"<<endl;		
		return (end-start).Mag();
	}
	else if(!inside_volume(start) && !inside_volume(end)){
		cout<<"WARNING: invalid input to \"muon_track_length\"!"<<endl;
		return -1; // no visible energy in TPC active volume; invalid input
	}
	else{
		if(inside_volume(end)) {v1 = end; v2 = start;} // not default
		TVector3 full_vector = v2 - v1;
		TVector3 xproj_vector(full_vector.X(), 0, 0);
		TVector3 yproj_vector(0, full_vector.Y(), 0);
		TVector3 zproj_vector(0, 0, full_vector.Z());
		TVector3 xproj_point = v1 + xproj_vector;
		TVector3 yproj_point = v1 + yproj_vector;
		TVector3 zproj_point = v1 + zproj_vector;
		
		if(inside_volume(xproj_point) && inside_volume(yproj_point) && inside_volume(zproj_point))
		{
			cout<<"WARNING: no points found to be outside!"<<endl;
			return -1;
		}	
	
		float scale_xproj = 1.0;
		float scale_yproj = 1.0;
		float scale_zproj = 1.0;
			
		if(!inside_volume(xproj_point)) { 
			scale_xproj = xproj_vector.X()>0?(254.8-v1.X()):(v1.X()-0); 
			scale_xproj *= 1./xproj_vector.Mag();
		}
		if(!inside_volume(yproj_point)) { 
			scale_yproj = yproj_vector.Y()>0?(117.47-v1.Y()):(v1.Y()+115.53);  
			scale_yproj *= 1./yproj_vector.Mag();
		}
		if(!inside_volume(zproj_point)) {
			scale_zproj = zproj_vector.Z()>0?(1036.9-v1.Z()):(v1.Z()-0.1);
			scale_zproj *= 1./zproj_vector.Mag();
		}
	
		TVector3 test_xpoint = v1 + scale_xproj*full_vector;
		TVector3 test_ypoint = v1 + scale_yproj*full_vector;
		TVector3 test_zpoint = v1 + scale_zproj*full_vector;

		float track_length = full_vector.Mag()+1;
		float temp_track_length = 0;
		if(inside_volume(test_xpoint, 0.5) && !inside_volume(test_xpoint, -0.5)) {
			temp_track_length = scale_xproj*full_vector.Mag();
			if(temp_track_length<track_length) track_length = temp_track_length;
		}
		//cout<<"Track length: "<<track_length<<endl;
		if(inside_volume(test_ypoint, 0.5) && !inside_volume(test_ypoint, -0.5)) {
			temp_track_length = scale_yproj*full_vector.Mag();
			if(temp_track_length<track_length) track_length = temp_track_length;
		}
		//cout<<"Track length: "<<track_length<<endl;
		if(inside_volume(test_zpoint, 0.5) && !inside_volume(test_zpoint, -0.5)) {
			temp_track_length = scale_zproj*full_vector.Mag();
			if(temp_track_length<track_length) track_length = temp_track_length;
		}
		//cout<<"Track length: "<<track_length<<endl;
		//cout<<"Full length: "<<full_vector.Mag()<<endl;
		
		if(track_length>full_vector.Mag()) {
			cout<<"WARNING: no crossing point found!"<<endl;
			return -1;
		}

		TVector3 cross_point = v1 + track_length/full_vector.Mag()*full_vector;
		//cout<<"Crossing point: ("<<cross_point.X()<<", "<<cross_point.Y()<<", "<<cross_point.Z()<<")"<<endl;
		return track_length;
	} 	
}



void Read(const char* filename)
{
	gROOT->ProcessLine(".x DrawOption.cc");

	bool _LIST_=true; //run-subrun-list for POT
	bool _ANA_=true; //passing rates
	bool _POT_=true; //overlay POT counting
	bool _MC_=true;
	bool _tagger_eval_=true;

	TFile* file = new TFile(filename);
	if(_POT_){
	TTree* T_pot = (TTree*)file->Get("wcpselection/T_pot");
	Double_t pot_tor875;
	Double_t POT_value=0;

	T_pot->SetBranchStatus("*", 0);
	T_pot->SetBranchStatus("pot_tor875", 1);
	T_pot->SetBranchAddress("pot_tor875", &pot_tor875);
	
	for(int i=0; i<T_pot->GetEntries(); i++)
        {
                T_pot->GetEntry(i);
		POT_value += pot_tor875;
        }
	std::cout<<"POT counting: "<<POT_value<<std::endl;	

	}
	
	if(_LIST_){
	TTree* T_pot = (TTree*)file->Get("wcpselection/T_pot");

	Int_t subRun;
	Int_t run;
	
	T_pot->SetBranchStatus("*", 0);
	T_pot->SetBranchStatus("runNo", 1);
	T_pot->SetBranchStatus("subRunNo", 1);
	T_pot->SetBranchAddress("runNo", &run);
	T_pot->SetBranchAddress("subRunNo", &subRun);
 		

	ofstream runsubrunlist("runsubrun.list", ios::out|ios::trunc);
	for(int i=0; i<T_pot->GetEntries(); i++)
	{
		T_pot->GetEntry(i);
		runsubrunlist << run << " " << subRun << std::endl;
	}

	runsubrunlist.close();
	}

	if(_ANA_){
	Int_t           f_run;
	Int_t           f_subRun;
	Int_t           f_event;
	Bool_t          f_flash_found;
	Float_t         f_flash_time;
	Float_t         f_flash_measPe;
	Float_t         f_flash_predPe;
	Bool_t          f_match_found;
	UInt_t          f_match_type;
	Bool_t          f_match_isFC;
	Bool_t          f_match_isTgm;
	Bool_t          f_match_notFC_FV;
	Bool_t          f_match_notFC_SP;
	Bool_t          f_match_notFC_DC;
	Float_t         f_match_charge; // main flag collection plane charge
	Float_t         f_match_energy;
	Float_t         f_match_chargeU;
	Float_t         f_match_chargeV;
	Float_t         f_match_chargeY;
	Float_t         f_match_energyY;
	Bool_t          f_lightmismatch;

	Float_t         f_truth_nuEnergy;
	Float_t         f_truth_energyInside;
	Float_t         f_truth_electronInside;
	Int_t           f_truth_nuPdg;
	Bool_t          f_truth_isCC;
	Bool_t          f_truth_isEligible;
	Bool_t          f_truth_isFC;
	Bool_t          f_truth_vtxInside;
	Float_t         f_truth_vtxX;
	Float_t         f_truth_vtxY;
	Float_t         f_truth_vtxZ;
	Float_t         f_truth_nuTime;
	Float_t         f_match_completeness;
	Float_t         f_match_completeness_energy;
	Float_t         f_match_purity;
	Float_t         f_match_purity_xz;
	Float_t         f_match_purity_xy;
        // GENIE weights
	Float_t         f_weight_spline;
	Float_t         f_weight_cv;

	Int_t 		f_stm_eventtype;
	Int_t 		f_stm_lowenergy;
	Int_t		f_stm_LM;
	Int_t		f_stm_TGM;
	Int_t		f_stm_STM;
	Int_t		f_stm_FullDead;
	Float_t		f_stm_clusterlength;

	TTree* T_eval = (TTree*)file->Get("wcpselection/T_eval");
	T_eval->SetBranchAddress("run",                      &f_run);
	T_eval->SetBranchAddress("subrun",                   &f_subRun);
	T_eval->SetBranchAddress("event",                    &f_event);
	T_eval->SetBranchAddress("flash_found",              &f_flash_found);
	T_eval->SetBranchAddress("flash_time",               &f_flash_time);
	T_eval->SetBranchAddress("flash_measPe",             &f_flash_measPe);
	T_eval->SetBranchAddress("flash_predPe",             &f_flash_predPe);
	T_eval->SetBranchAddress("match_found",              &f_match_found);
	T_eval->SetBranchAddress("match_type",               &f_match_type);
	T_eval->SetBranchAddress("match_isFC",               &f_match_isFC);
	T_eval->SetBranchAddress("match_isTgm",              &f_match_isTgm);
	T_eval->SetBranchAddress("match_notFC_FV",           &f_match_notFC_FV);
	T_eval->SetBranchAddress("match_notFC_SP",           &f_match_notFC_SP);
	T_eval->SetBranchAddress("match_notFC_DC",           &f_match_notFC_DC);
	T_eval->SetBranchAddress("match_chargeU",            &f_match_chargeU);
	T_eval->SetBranchAddress("match_chargeV",            &f_match_chargeV);
	T_eval->SetBranchAddress("match_chargeY",            &f_match_chargeY);
	T_eval->SetBranchAddress("match_energyY",            &f_match_energyY);
	T_eval->SetBranchAddress("light_mismatch",           &f_lightmismatch);	
	T_eval->SetBranchAddress("match_charge",             &f_match_charge);
	T_eval->SetBranchAddress("match_energy",             &f_match_energy);
	T_eval->SetBranchAddress("stm_eventtype",            &f_stm_eventtype);
	T_eval->SetBranchAddress("stm_lowenergy",            &f_stm_lowenergy);
	T_eval->SetBranchAddress("stm_LM",            	     &f_stm_LM);
	T_eval->SetBranchAddress("stm_TGM",            	     &f_stm_TGM);
	T_eval->SetBranchAddress("stm_STM",            	     &f_stm_STM);
	T_eval->SetBranchAddress("stm_FullDead",             &f_stm_FullDead);
	T_eval->SetBranchAddress("stm_clusterlength",        &f_stm_clusterlength);
	if(_MC_){ //Overlay or MC
	T_eval->SetBranchAddress("truth_nuEnergy",           &f_truth_nuEnergy);
	T_eval->SetBranchAddress("truth_energyInside",       &f_truth_energyInside);
	T_eval->SetBranchAddress("truth_electronInside",     &f_truth_electronInside);
	T_eval->SetBranchAddress("truth_nuPdg",              &f_truth_nuPdg);
	T_eval->SetBranchAddress("truth_isCC",               &f_truth_isCC);
	T_eval->SetBranchAddress("truth_isEligible",         &f_truth_isEligible);
	T_eval->SetBranchAddress("truth_isFC",               &f_truth_isFC);
	T_eval->SetBranchAddress("truth_vtxInside",          &f_truth_vtxInside);
	T_eval->SetBranchAddress("truth_vtxX",               &f_truth_vtxX);
	T_eval->SetBranchAddress("truth_vtxY",               &f_truth_vtxY);
	T_eval->SetBranchAddress("truth_vtxZ",               &f_truth_vtxZ);
	T_eval->SetBranchAddress("truth_nuTime",             &f_truth_nuTime);
	T_eval->SetBranchAddress("match_completeness",       &f_match_completeness);
	T_eval->SetBranchAddress("match_completeness_energy",&f_match_completeness_energy);
	T_eval->SetBranchAddress("match_purity",             &f_match_purity);
	T_eval->SetBranchAddress("match_purity_xz",          &f_match_purity_xz);
	T_eval->SetBranchAddress("match_purity_xy",          &f_match_purity_xy);
	T_eval->SetBranchAddress("weight_spline",            &f_weight_spline);
	T_eval->SetBranchAddress("weight_cv",                &f_weight_cv);
	}

	Float_t         reco_nuvtxX;
	Float_t         reco_nuvtxY;
	Float_t         reco_nuvtxZ;
	Float_t         reco_showervtxX;
	Float_t         reco_showervtxY;
	Float_t         reco_showervtxZ;
	Float_t         reco_muonvtxX;
	Float_t         reco_muonvtxY;
	Float_t         reco_muonvtxZ;
	Float_t         reco_muonMomentum[4];
	Float_t         truth_corr_nuvtxX;
	Float_t         truth_corr_nuvtxY;
	Float_t         truth_corr_nuvtxZ;
	Float_t         truth_corr_showervtxX;
	Float_t         truth_corr_showervtxY;
	Float_t         truth_corr_showervtxZ;	
	Float_t         truth_corr_muonvtxX;
	Float_t         truth_corr_muonvtxY;
	Float_t         truth_corr_muonvtxZ;	
	Float_t         truth_muonvtxX;
	Float_t         truth_muonvtxY;
	Float_t         truth_muonvtxZ;	
	Float_t         truth_muonendX;
	Float_t         truth_muonendY;
	Float_t         truth_muonendZ;	
	Float_t         truth_muonMomentum[4];
	Int_t		neutrino_type;

	TTree* T_PFeval = (TTree*)file->Get("wcpselection/T_PFeval");
	T_PFeval->SetBranchAddress("reco_nuvtxX",		&reco_nuvtxX);
	T_PFeval->SetBranchAddress("reco_nuvtxY",		&reco_nuvtxY);
	T_PFeval->SetBranchAddress("reco_nuvtxZ",		&reco_nuvtxZ);
	T_PFeval->SetBranchAddress("reco_showervtxX",		&reco_showervtxX);
	T_PFeval->SetBranchAddress("reco_showervtxY",		&reco_showervtxY);
	T_PFeval->SetBranchAddress("reco_showervtxZ",		&reco_showervtxZ);
	T_PFeval->SetBranchAddress("reco_muonvtxX",		&reco_muonvtxX);
	T_PFeval->SetBranchAddress("reco_muonvtxY",		&reco_muonvtxY);
	T_PFeval->SetBranchAddress("reco_muonvtxZ",		&reco_muonvtxZ);
	T_PFeval->SetBranchAddress("reco_muonMomentum",		&reco_muonMomentum);
	T_PFeval->SetBranchAddress("neutrino_type",		&neutrino_type);
	if(_MC_){
	T_PFeval->SetBranchAddress("truth_corr_nuvtxX",		&truth_corr_nuvtxX);
	T_PFeval->SetBranchAddress("truth_corr_nuvtxY",		&truth_corr_nuvtxY);
	T_PFeval->SetBranchAddress("truth_corr_nuvtxZ",		&truth_corr_nuvtxZ);
	T_PFeval->SetBranchAddress("truth_corr_showervtxX",	&truth_corr_showervtxX);
	T_PFeval->SetBranchAddress("truth_corr_showervtxY",	&truth_corr_showervtxY);
	T_PFeval->SetBranchAddress("truth_corr_showervtxZ",	&truth_corr_showervtxZ);
	T_PFeval->SetBranchAddress("truth_corr_muonvtxX",	&truth_corr_muonvtxX);
	T_PFeval->SetBranchAddress("truth_corr_muonvtxY",	&truth_corr_muonvtxY);
	T_PFeval->SetBranchAddress("truth_corr_muonvtxZ",	&truth_corr_muonvtxZ);
	T_PFeval->SetBranchAddress("truth_muonvtxX",		&truth_muonvtxX);
	T_PFeval->SetBranchAddress("truth_muonvtxY",		&truth_muonvtxY);
	T_PFeval->SetBranchAddress("truth_muonvtxZ",		&truth_muonvtxZ);
	T_PFeval->SetBranchAddress("truth_muonendX",		&truth_muonendX);
	T_PFeval->SetBranchAddress("truth_muonendY",		&truth_muonendY);
	T_PFeval->SetBranchAddress("truth_muonendZ",		&truth_muonendZ);
	T_PFeval->SetBranchAddress("truth_muonMomentum",	&truth_muonMomentum);
	}

	Float_t		f_numu_score;

	TTree* T_BDTvars = (TTree*)file->Get("wcpselection/T_BDTvars");
	T_BDTvars->SetBranchAddress("numu_score",                      &f_numu_score);

	TH1F* h_eligible = new TH1F("h_eligible","Eligible events",40,0,4000);
	TH1F* h_precuts = new TH1F("h_precuts","Pre-select events",40,0,4000);
	TH1F* h_tag = new TH1F("h_tag","Tagged events",40,0,4000);
	TH1F* h_tag_NC = new TH1F("h_tag_NC","Mis-tagged events",40,0,4000);
	TH1F* h_tag_cosmic = new TH1F("h_tag_cosmic","Mis-tagged events",40,0,4000);
	
	TH1F* h_eligible_tlen = new TH1F("h_eligible_tlen","Eligible events",200,0,1000); // track length from 0 to 1500 cm
	TH1F* h_precuts_tlen = new TH1F("h_precuts_tlen","Pre-select events",200,0,1000);
	TH1F* h_tag_tlen = new TH1F("h_tag_tlen","Tagged events",200,0,1000);
	TH1F* h_tag_tlen_NC = new TH1F("h_tag_tlen_NC","Mis-tagged events",200,0,1000);
	TH1F* h_tag_tlen_cosmic = new TH1F("h_tag_tlen_cosmic","Mis-tagged events",200,0,1000);

	TH2F* htrue_muonKE_tlen = new TH2F("htrue_muonKE_tlen","Muon KE vs track length", 100,0,1000,200,0,2000);
	TH2F* hreco_muonKE_tlen = new TH2F("hreco_muonKE_tlen","Muon KE vs track length", 100,0,1000,200,0,2000);

	TH1F* h_bnb = new TH1F("h_bnb","Tagged bnb events",40, 0, 4000);
	TH1F* h_bnb1 = new TH1F("h_bnb1","Tagged bnb events",80, 0, 4000);
	TH1F* h_extbnb = new TH1F("h_extbnb","Tagged extbnb events",40, 0, 4000);
	TH1F* h_overlay = new TH1F("h_overlay","Tagged overlay events",40, 0, 4000);
	TH1F* h_overlay1 = new TH1F("h_overlay1","Tagged overlay events",40, 0, 4000);
	TH1F* h_numuCC_FV = new TH1F("h_numuCC_FV","Tagged numuCC FV events",40, 0, 4000);
	TH1F* h_NC_FV = new TH1F("h_NC_FV","Tagged NC FV events",40, 0, 4000);
	TH1F* h_cosmic = new TH1F("h_cosmic","Tagged cosmic FV events",40, 0, 4000);
	
	TH1F* h_overlay_w2 = new TH1F("h_overlay_w2","weight square for Tagged overlay events",40, 0, 4000);
        TH2F* h_nuEnergy_visEnergy = new TH2F("h_nuEnergy_visEnergy","match_energy vs. truth_energyInside",40,0,4000,40,0,4000);
        TH1F* h_weight = new TH1F("h_weight", "GENIE Total Weight", 1000,-2,8);
	
	TH1F* h_nuvtxXDiff = new TH1F("h_nuvtxXDiff","Neutrino vertex reco/true difference in X direction", 200, -10, 10);
	TH1F* h_nuvtxYDiff = new TH1F("h_nuvtxYDiff","Neutrino vertex reco/true difference in Y direction", 200, -10, 10);
	TH1F* h_nuvtxZDiff = new TH1F("h_nuvtxZDiff","Neutrino vertex reco/true difference in Z direction", 200, -10, 10);
	TH1F* h_nuvtxDiff = new TH1F("h_nuvtxDiff","Neutrino vertex reco/true difference", 1010, -1, 100);
	TH1F* h_nushowervtxDiff = new TH1F("h_nushowervtxDiff","Neutrino-shower reco vertex difference", 200, 0, 20);
	TH1F* h_numuonvtxDiff = new TH1F("h_numuonvtxDiff","Neutrino-muon reco vertex difference", 200, 0, 20);

	TH2F* h_nuvtx_nushowervtx = new TH2F("h_nuvtx_nushowervtx","reco/true neutrino vertex diff vs. reco shower/nu vertex diff", 2000, 0, 200, 2000, 0, 200);
	TH2F* h_showerfailure = new TH2F("h_showerfailure","No shower reconstructed", 2000, 0, 200, 2000, 0, 200);
	TH2F* h_showerfailure2 = new TH2F("h_showerfailure2","No shower reconstructed", 200, 0, 200, 200, 0, 200);
	TH2F* h_showersuccess = new TH2F("h_showersuccess", "Shower vertex correct", 2000, 0, 200, 2000, 0, 200);
	TH2F* h_collinear = new TH2F("h_collinear","Reco shower, reco nu, true nu/shower collinear", 2000, 0, 200, 2000, 0, 200);

	TH2F* h_nuvtx_numuonvtx = new TH2F("h_nuvtx_numuonvtx","reco/true neutrino vertex diff vs. reco muon/nu vertex diff", 2000, 0, 200, 2000, 0, 200);
	TH2F* h_goodvtx_dirdiff_KEdiff = new TH2F("h_goodvtx_dirdiff_KEdiff","both neutrino and muon vertices correct: reco/true muon direction diff vs. KE diff", 180, 0, 180, 40, -2, 2);
	TH2F* h_muonfailure = new TH2F("h_muonfailure","No muon reconstructed", 2000, 0, 200, 2000, 0, 200);
	TH2F* h_muonfailure2 = new TH2F("h_muonfailure2","No muon reconstructed", 200, 0, 200, 200, 0, 200);
	TH2F* h_muonsuccess = new TH2F("h_muonsuccess", "Shower vertex correct", 2000, 0, 200, 2000, 0, 200);
	
	bool eligible_numuCC = false;
	int jobfailure = 0;
	int showerfailure = 0;
	int showersuccess = 0;
	int muonfailure = 0;
	int muonsuccess = 0;

	//tagger evaluation
	int cosmic_truth = 0;
	int numuCC_truth = 0;
	int NC_truth = 0;
	int nueCC_truth = 0;
	int other_truth = 0;

	int cosmic_misid_numuCC = 0;
	int cosmic_misid_NC = 0;
	int numuCC_misid_cosmic = 0;
	int numuCC_misid_NC = 0;
	int NC_misid_numuCC = 0;
	int NC_misid_cosmic = 0;

	int total_data = 0;
	int cosmic_tag = 0;
	int numuCC_tag = 0;
	int NC_tag = 0;
	int nueCC_tag = 0;
	int nothing_tag = 0;

	int cosmic_misid_nueCC = 0;
	int NC_misid_nueCC = 0;
	int numuCC_misid_nueCC = 0;

	ofstream cosmic2numuCClist("cosmic2numuCC.list", ios::out|ios::trunc);
	ofstream NC2numuCClist("NC2numuCC.list", ios::out|ios::trunc);
	ofstream goodnumuCClist("goodnumuCC.list", ios::out|ios::trunc);
	ofstream numuCC2cosmiclist("numuCC2cosmic.list", ios::out|ios::trunc);
	ofstream numuCC2NClist("numuCC2NC.list", ios::out|ios::trunc);
	ofstream badnueCClist("badnueCC.list", ios::out|ios::trunc);
	ofstream goodnueCClist("goodnueCC.list", ios::out|ios::trunc);
	ofstream truthnueCClist("truthnueCC.list", ios::out|ios::trunc);
	ofstream misidnueCClist("misidnueCC.list", ios::out|ios::trunc);
	ofstream extlist("ext.list", ios::out|ios::trunc);
	//end
	
        int entries = T_eval->GetEntries();
        int genericEntries = 0;
        int eligibleEntries = 0;
        int precutsEntries = 0;
	for(int i=0; i<T_eval->GetEntries(); i++)
	{
		T_eval->GetEntry(i);
		T_PFeval->GetEntry(i);
		T_BDTvars->GetEntry(i);

		if(_tagger_eval_){
		// tagger evaluation
		if(f_flash_found && f_match_found && !f_match_isTgm && !f_stm_TGM && !f_stm_STM && !f_stm_lowenergy && !f_stm_LM && !f_stm_FullDead && f_stm_clusterlength>15 && _MC_){
		if(neutrino_type<0) {cout<<"Entry: "<<i<<" Neutrino vertex: "<<reco_nuvtxX<<endl; continue;} 

		bool cosmic = (neutrino_type >> 1) & 1U;	
		bool numuCC = (neutrino_type >> 2) & 1U;	
		bool NC = (neutrino_type >> 3) & 1U;	
		bool nueCC = (neutrino_type >> 5) & 1U;

                // anti-aliasing in neutrino tagging
                numuCC = !cosmic && numuCC;
                NC = !cosmic && NC;
                nueCC = !cosmic && nueCC;
                
                // numuCC = (f_numu_score>0);
                // cosmic = cosmic && !numuCC;
                // NC = !cosmic && !numuCC;
                // nueCC = !cosmic && nueCC;

                if(numuCC && !f_match_isFC) genericEntries ++;

	
		bool cosmic_t = false;	
		bool numuCC_t = false;	
		bool NC_t = false;	
		bool nueCC_t = false;
					
                if (f_weight_spline>1000 or f_weight_spline<0) f_weight_spline = 1.0;
                if (f_weight_cv>1000 or f_weight_cv<0) f_weight_cv = 1.0;
                double total_weight = f_weight_spline * f_weight_cv;
                // enforce equal weight
                // double total_weight = 1.0;

		if(numuCC) {
                  h_overlay->Fill(f_match_energy+0.01, total_weight);
                  h_overlay1->Fill(f_match_energy+0.01, total_weight);
                  h_overlay_w2->Fill(f_match_energy+0.01, total_weight * total_weight);
                  // h_overlay->Fill(f_match_energy+0.01); 
                  // h_weight->Fill(total_weight);
                }

		if( !f_truth_vtxInside || f_match_completeness_energy/f_truth_energyInside<0.1){
			cosmic_t = true;
			cosmic_truth++;
			if(cosmic) cosmic_tag++;
			if(!cosmic) {
				if(numuCC) cosmic_misid_numuCC++;
				if(NC) cosmic_misid_NC++;
				if(numuCC) cosmic2numuCClist<<f_run<<"_"<<f_subRun<<"_"<<f_event<<endl;
				if(numuCC) h_cosmic->Fill(f_match_energy+0.01, total_weight); 
			}
			if(nueCC) {
				cosmic_misid_nueCC++;
				if(neutrino_type<0) cout<<"Neutrino type: "<<neutrino_type<<endl;
				misidnueCClist<<f_run<<" "<<f_subRun<<" "<<f_event<<endl;
			}
		}
		else if( f_truth_vtxInside && f_match_completeness_energy/f_truth_energyInside>0.1 && f_truth_isCC && abs(f_truth_nuPdg)==14){

                        h_nuEnergy_visEnergy->Fill(f_truth_energyInside, f_match_energy);

			numuCC_t = true;
			numuCC_truth++;
			if(numuCC) numuCC_tag++;
			if(numuCC) goodnumuCClist<<f_run<<"_"<<f_subRun<<"_"<<f_event<<endl;
			if(numuCC) h_numuCC_FV->Fill(f_match_energy+0.01, total_weight); 
		
			TVector3 muonstart(truth_muonvtxX, truth_muonvtxY, truth_muonvtxZ);
			TVector3 muonend(truth_muonendX, truth_muonendY, truth_muonendZ);
			float muon_tlen = muon_track_length(muonstart, muonend); // visible track length in active volume
	
			if(!numuCC) {
				if(cosmic){ 
					numuCC_misid_cosmic++;
					numuCC2cosmiclist<<f_run<<"_"<<f_subRun<<"_"<<f_event<<endl;
				}
				if(NC){
					numuCC_misid_NC++;
					if(muon_tlen>10 && muon_tlen<50){
					numuCC2NClist<<f_run<<"_"<<f_subRun<<"_"<<f_event<<endl;
					}
				}
			}
			if(nueCC) {
				numuCC_misid_nueCC++;
				if(neutrino_type<0) cout<<"Neutrino type: "<<neutrino_type<<endl;
				misidnueCClist<<f_run<<" "<<f_subRun<<" "<<f_event<<endl;
			}
		}
		else if( f_truth_vtxInside && f_match_completeness_energy/f_truth_energyInside>0.1 && !f_truth_isCC){
			NC_t = true;
			NC_truth++;
			if(NC) NC_tag++;
			if(!NC) {
				if(cosmic) NC_misid_cosmic++;
				if(numuCC) NC_misid_numuCC++;
				if(numuCC) NC2numuCClist<<f_run<<"_"<<f_subRun<<"_"<<f_event<<endl;
				if(numuCC) h_NC_FV->Fill(f_match_energy+0.01, total_weight); 
			}
			if(nueCC) {
				NC_misid_nueCC++;
				if(neutrino_type<0) cout<<"Neutrino type: "<<neutrino_type<<endl;
				misidnueCClist<<f_run<<" "<<f_subRun<<" "<<f_event<<endl;
			}
			
		}
		else { 
			other_truth++; 
			nothing_tag++; 
		}
		
		//override other taggers
		if( f_truth_vtxInside && f_match_completeness_energy/f_truth_energyInside>0.1 && f_truth_isCC && abs(f_truth_nuPdg)==12){
 			nueCC_t = true;
			truthnueCClist<<f_run<<" "<<f_subRun<<" "<<f_event<<endl;
			nueCC_truth++;
			if(nueCC) nueCC_tag++;
			if(!nueCC) {
				badnueCClist<<f_run<<" "<<f_subRun<<" "<<f_event<<endl;
			}
		}
	
		}// MC end
		if(f_flash_found && f_match_found && !f_match_isTgm && !f_stm_TGM && !f_stm_STM && !f_stm_lowenergy && !f_stm_LM && !f_stm_FullDead && f_stm_clusterlength>15 && !_MC_){
		if(neutrino_type<=1) {
			cout<<"Entry: "<<i<<endl;
			continue;
		}
		total_data++; 
		bool cosmic = (neutrino_type >> 1) & 1U;	
		bool numuCC = (neutrino_type >> 2) & 1U;	
		bool NC = (neutrino_type >> 3) & 1U;	
		bool nueCC = (neutrino_type >> 5) & 1U;

                // anti-aliasing in neutrino tagging
                numuCC = !cosmic && numuCC;
                NC = !cosmic && NC;
	
		if(cosmic) cosmic_tag++;
		if(NC) NC_tag++;
		if(numuCC) numuCC_tag++;
		if(numuCC) extlist<<f_run<<" "<<f_subRun<<" "<<f_event<<endl;
		if(nueCC) nueCC_tag++;

		// if(numuCC) h_extbnb->Fill(f_match_energy+0.01);
		if(numuCC) h_bnb->Fill(f_match_energy+0.01);
		if(numuCC) h_bnb1->Fill(f_match_energy+0.01);

		}// data end
		
		}
			
		// track length
		TVector3 muonstart(truth_muonvtxX, truth_muonvtxY, truth_muonvtxZ);
		TVector3 muonend(truth_muonendX, truth_muonendY, truth_muonendZ);
		float muon_tlen = 0; //(muonstart - muonend).Mag();

		if( f_truth_isCC && f_truth_nuPdg==14 && f_truth_vtxInside){
			eligible_numuCC = true;
			muon_tlen = muon_track_length(muonstart, muonend); // visible track length in active volume
			h_eligible->Fill(f_match_energy+0.001);
			h_eligible_tlen->Fill(muon_tlen);

                        eligibleEntries ++;
		}
		else eligible_numuCC = false;

		if(f_flash_found && f_match_found && !f_match_isTgm && !f_stm_TGM && !f_stm_STM && !f_stm_lowenergy && !f_stm_LM && !f_stm_FullDead && f_stm_clusterlength>15 && eligible_numuCC && f_match_completeness_energy/f_truth_energyInside>0.1){
			if( reco_nuvtxX<0 ) { jobfailure++; continue; } // should be very rare: job failure
			h_precuts->Fill(f_match_energy+0.01);

                        precutsEntries ++;

			h_precuts_tlen->Fill(muon_tlen);
			bool numuCCtag = false;
			bool NCtag = false;
			bool cosmictag = false;
			if( neutrino_type>0) {
				cosmictag = (neutrino_type >> 1) & 1U;	
				numuCCtag = (neutrino_type >> 2) & 1U;
				NCtag = (neutrino_type >> 3) & 1U;
			}	
			if( NCtag ) {
				h_tag_NC->Fill(f_match_energy+0.01); 
				h_tag_tlen_NC->Fill(muon_tlen);
				continue;}
			if( cosmictag ) {
				h_tag_cosmic->Fill(f_match_energy+0.01);
				h_tag_tlen_cosmic->Fill(muon_tlen);
				continue;}
			h_tag->Fill(f_match_energy+0.01);
			h_tag_tlen->Fill(muon_tlen);
					
	
			h_nuvtxXDiff->Fill(reco_nuvtxX - truth_corr_nuvtxX);
			h_nuvtxYDiff->Fill(reco_nuvtxY - truth_corr_nuvtxY);	
			h_nuvtxZDiff->Fill(reco_nuvtxZ - truth_corr_nuvtxZ);
			TVector3 vr(reco_nuvtxX, reco_nuvtxY, reco_nuvtxZ);
			TVector3 vt(truth_corr_nuvtxX, truth_corr_nuvtxY, truth_corr_nuvtxZ);
			TVector3 vrmuon(reco_muonvtxX, reco_muonvtxY, reco_muonvtxZ);
			
			h_nuvtxDiff->Fill((vr-vt).Mag());
			h_numuonvtxDiff->Fill((vrmuon-vr).Mag());
			h_nuvtx_numuonvtx->Fill((vr-vt).Mag(), (vrmuon-vr).Mag());	
		
			TVector3 recoMom(reco_muonMomentum[0], reco_muonMomentum[1], reco_muonMomentum[2]);
			TVector3 trueMom(truth_muonMomentum[0], truth_muonMomentum[1], truth_muonMomentum[2]);
			Double_t dirDiff = 180*TMath::ACos(recoMom.Dot(trueMom)/recoMom.Mag()/trueMom.Mag())/TMath::Pi();
			float muon_mass = 0.105658; //GeV
			float reco_muonKE = reco_muonMomentum[3] - muon_mass;
			float true_muonKE = truth_muonMomentum[3] - muon_mass;
			//cout<<"Reco mass: "<<TMath::Sqrt(reco_muonMomentum[3]*reco_muonMomentum[3]-recoMom.Mag2())<<endl;
			//cout<<"True mass: "<<TMath::Sqrt(truth_muonMomentum[3]*truth_muonMomentum[3]-trueMom.Mag2())<<endl;
			if((vrmuon-vr).Mag()<1) h_goodvtx_dirdiff_KEdiff->Fill(dirDiff, reco_muonKE/true_muonKE-1);

			htrue_muonKE_tlen->Fill(muon_tlen, true_muonKE*1000.);
			hreco_muonKE_tlen->Fill(muon_tlen, reco_muonKE*1000.);
	
			if(reco_muonvtxX<0) { 
				muonfailure++; 
				float minX = f_truth_vtxX<(255-f_truth_vtxX)?f_truth_vtxX:(255-f_truth_vtxX);
				float minY = (f_truth_vtxY+116)<(116-f_truth_vtxY)?(f_truth_vtxY+116):(116-f_truth_vtxY);
				float minZ = f_truth_vtxZ<(1036-f_truth_vtxZ)?f_truth_vtxZ:(1036-f_truth_vtxZ);
				float min = minX;
				if(min>minY) min=minY;
				if(min>minZ) min=minZ;
				h_muonfailure->Fill((vr-vt).Mag(), (vrmuon-vr).Mag());
				h_muonfailure2->Fill((vr-vt).Mag(), min);
			}
			if((vrmuon-vt).Mag()<1) { muonsuccess++; h_muonsuccess->Fill((vr-vt).Mag(), (vrmuon-vr).Mag());}

		}

	}
        std::cout << "[gu] entries: " << entries << " generic passed: " << genericEntries << std::endl;
        std::cout << "[gu] eligibleEntries: " << eligibleEntries << " passed: " << precutsEntries << std::endl;

	if(_tagger_eval_ && _MC_){
	cout<<"Tagger efficiency: "<<endl;
	cout<<"Cosmic (incoming dirt): "<<1.0*cosmic_tag/cosmic_truth<<endl;	
	cout<<"numuCC: "<<1.0*numuCC_tag/numuCC_truth<<endl;	
	cout<<"NC: "<<1.0*NC_tag/NC_truth<<endl;
	cout<<"Not tagged: "<<1.0*nothing_tag/(other_truth+cosmic_truth+numuCC_truth+NC_truth)<<endl;
	cout<<"Other truth: "<<1.0*other_truth/(other_truth+cosmic_truth+numuCC_truth+NC_truth)<<endl;
	cout<<"nueCC: "<<1.0*nueCC_tag/nueCC_truth<<endl;	
	cout<<"Confusion Matrix:"<<endl;
	cout<<cosmic_truth<<" "<<1.0*cosmic_tag/cosmic_truth<<" "<<1.0*cosmic_misid_NC/cosmic_truth<<" "<<1.0*cosmic_misid_numuCC/cosmic_truth<<" "<<(cosmic_tag+cosmic_misid_NC+cosmic_misid_numuCC)/cosmic_truth*1.0<<endl;
	cout<<NC_truth<<" "<<1.0*NC_misid_cosmic/NC_truth<<" "<<1.0*NC_tag/NC_truth<<" "<<1.0*NC_misid_numuCC/NC_truth<<" "<<(NC_misid_cosmic+NC_tag+NC_misid_numuCC)/NC_truth*1.0<<endl;
	cout<<numuCC_truth<<" "<<1.0*numuCC_misid_cosmic/numuCC_truth<<" "<<1.0*numuCC_misid_NC/numuCC_truth<<" "<<1.0*numuCC_tag/numuCC_truth<<" "<<(numuCC_misid_cosmic+numuCC_misid_NC+numuCC_tag)/numuCC_truth*1.0<<endl;
	cout<<"Purity: "<<1.0*cosmic_tag/(cosmic_tag+NC_misid_cosmic+numuCC_misid_cosmic)<<" "<<1.0*NC_tag/(NC_tag+cosmic_misid_NC+numuCC_misid_NC)<<" "<<1.0*numuCC_tag/(numuCC_tag+cosmic_misid_numuCC+NC_misid_numuCC)<<endl;

	cout<<"Cosmic misid nueCC: "<<1.0*cosmic_misid_nueCC/cosmic_truth<<endl;
	cout<<"NC misid nueCC: "<<1.0*NC_misid_nueCC/NC_truth<<endl;
	cout<<"numuCC misid nueCC: "<<1.0*numuCC_misid_nueCC/numuCC_truth<<endl;
	cout<<"Tagged nueCC: "<<nueCC_tag<<" "<<1.0*nueCC_tag/nueCC_truth<<endl;
	cout<<"Purity: "<<1.0*nueCC_tag/(NC_misid_nueCC+numuCC_misid_nueCC+cosmic_misid_nueCC+nueCC_tag)<<endl;
	int EXT_truth = NC_truth; //after generic neutrino selection, no GENIE weights
	cout<<"Purity [+EXT]: "<<1.0*nueCC_tag/(1.0*cosmic_misid_nueCC/cosmic_truth*EXT_truth+cosmic_misid_nueCC+NC_misid_nueCC+numuCC_misid_nueCC+nueCC_tag)<<endl;

	cout<<endl;
	cout<<endl;
	cout<<endl;

	TFile* output = new TFile("overlay_breakdown_dummy.root", "RECREATE");	
	h_overlay->Write();
	h_numuCC_FV->Write();
	h_NC_FV->Write();
	h_cosmic->Write();

        // set error
        for (int ibin=0; ibin<h_overlay1->GetNbinsX(); ibin++) {
          h_overlay1->SetBinError(ibin+1, std::sqrt(h_overlay_w2->GetBinContent(ibin+1)) );
        }
	h_overlay1->Write();

	h_nuEnergy_visEnergy->Write();
	output->Close();
	}

	if(!_MC_){
	cout<<"Total Entries: "<<T_eval->GetEntries()<<endl;
	cout<<"Total passing: "<<total_data<<" "<<1.0*total_data/T_eval->GetEntries()<<endl;
	cout<<"Tagged Cosmic: "<<cosmic_tag<<" "<<1.0*cosmic_tag/total_data<<endl;
	cout<<"Tagged NC: "<<NC_tag<<" "<<1.0*NC_tag/total_data<<endl;
	cout<<"Tagged numuCC: "<<numuCC_tag<<" "<<1.0*numuCC_tag/total_data<<endl;
	cout<<"Tagged nueCC: "<<nueCC_tag<<" "<<1.0*nueCC_tag/total_data<<endl;

	// h_extbnb->SaveAs("extbnb_selected_dummy.root");
	h_bnb->SaveAs("bnb_selected_dummy.root");
	h_bnb1->SaveAs("bnb_selected_dummy_50MeV.root");

	}

	if(_MC_){	

        // h_weight->SaveAs("weight_dummy.root");


	cout<<"Total eligible: "<<h_eligible->Integral(1,-1)<<endl;
	cout<<"Job failure: "<<jobfailure<<" ("<<jobfailure/(jobfailure+h_precuts->Integral(1,-1))<<")"<<endl;
	cout<<"Precuts: "<<h_precuts->Integral(1,-1)<<" ("<<h_precuts->Integral(1,-1)/h_eligible->Integral(1,-1)<<")"<<endl;
	cout<<"Tag: "<<h_tag->Integral(1,-1)<<" ("<<h_tag->Integral(1,-1)/h_precuts->Integral(1,-1)<<")"<<endl;
	cout<<"Muon failure (no muon): "<<muonfailure<<" ("<<muonfailure/h_precuts->Integral(1,-1)<<")"<<endl;
	cout<<"Muon failure nu vertex diff <1 cm: "<<h_muonfailure2->Integral(1,1,1,-1)/h_precuts->Integral(1,-1)<<endl;
	cout<<"Muon failure <10 cm to boundary & nu vertex diff <1 cm: "<<h_muonfailure2->Integral(1,1,1,10)/h_precuts->Integral(1,-1)<<endl;
	cout<<"Muon success (<1 cm): "<<muonsuccess<<" ("<<muonsuccess/h_precuts->Integral(1,-1)<<")"<<endl;	
	cout<<"Muon success >1 cm muon - nu vertex (reco) & >1 cm nu vertex diff: "<<h_muonsuccess->Integral(11,-1,11,-1)/h_precuts->Integral(1,-1)<<endl;
	cout<<"<1 cm nu vertex diff: "<<h_nuvtxDiff->Integral(11,21)/h_nuvtxDiff->Integral(11,1010)<<endl;		
	cout<<"<1 cm reco muon-nu vertex diff: "<<h_numuonvtxDiff->Integral(1,10)/h_numuonvtxDiff->Integral(1,-1)<<endl;		
	cout<<"<1 cm muon - nu vertex (reco): "<<h_nuvtx_numuonvtx->Integral(1,-1,1,10)/h_nuvtx_numuonvtx->Integral(1,-1,1,-1)<<endl;
	cout<<"<1 cm muon - nu vertex (reco) & <1 cm nu vertex diff: "<<h_nuvtx_numuonvtx->Integral(1,10,1,10)/h_nuvtx_numuonvtx->Integral(1,-1,1,-1)<<endl;
	cout<<">1 cm muon - nu vertex (reco) & >1 cm nu vertex diff: "<<h_nuvtx_numuonvtx->Integral(11,-1,11,-1)/h_nuvtx_numuonvtx->Integral(1,-1,1,-1)<<endl;
	cout<<">1 cm muon - nu vertex (reco) & <1 cm nu vertex diff: "<<h_nuvtx_numuonvtx->Integral(1,10,11,-1)/h_nuvtx_numuonvtx->Integral(1,-1,1,-1)<<endl;

	cout<<"Good neutrino and muon vertices: <10 degree: "<<h_goodvtx_dirdiff_KEdiff->Integral(1,10,1,-1)/h_goodvtx_dirdiff_KEdiff->Integral(1,-1,1,-1)<<endl;
	cout<<"Good neutrino and muon vertices: <10 degree && <0.2 KE diff: "<<h_goodvtx_dirdiff_KEdiff->Integral(1,10,18,22)/h_goodvtx_dirdiff_KEdiff->Integral(1,-1,1,-1)<<endl;
	
	TCanvas* c0 = new TCanvas("c0","c0", 600, 800);
	c0->Divide(2,2);
	c0->cd(1);
	h_nuvtxXDiff->Draw("hist");	
	c0->cd(2);
	h_nuvtxYDiff->Draw("hist");	
	c0->cd(3);
	h_nuvtxZDiff->Draw("hist");	
	c0->cd(4);
	gPad->SetLogy();
	h_nuvtxDiff->Draw("hist");	
/*
	TCanvas* c1 = new TCanvas("c1", "c1", 900, 650);
	gPad->SetLogz();
	h_nuvtx_numuonvtx->Draw("colz");
	h_nuvtx_numuonvtx->GetXaxis()->SetRangeUser(-1,20);
	h_nuvtx_numuonvtx->GetYaxis()->SetRangeUser(-1,20);
	h_muonfailure->Draw("scat same");
	h_muonfailure->SetMarkerColor(kRed);
	h_muonfailure->SetMarkerStyle(24);
*/	//h_muonsuccess->Draw("scat same");
	//h_muonsuccess->SetMarkerColor(kBlack);
	//h_muonsuccess->SetMarkerStyle(24);

	TCanvas* c5 = new TCanvas("c5", "c5", 900, 650);
	h_goodvtx_dirdiff_KEdiff->Draw("colz"); 

	TCanvas* c3 = new TCanvas("c3", "c3", 900, 650);
	gPad->SetLogy();
	gPad->SetLogx();
	h_numuonvtxDiff->Draw("hist");

/*	TCanvas* c4 = new TCanvas("c4", "c4", 900, 650);
	h_muonfailure2->Draw("colz");	
	h_muonfailure2->GetXaxis()->SetRangeUser(-1,20);
	h_muonfailure2->GetYaxis()->SetRangeUser(-1,150);
*/
	TCanvas* c2 = new TCanvas("c2", "c2", 1200, 650);
	c2->Divide(2,1);
	c2->cd(1);
	gPad->SetLogy();
	h_eligible->Draw("histE");
	h_eligible->GetXaxis()->SetRangeUser(0,2000);
	h_precuts->Draw("histE same");
	h_precuts->SetLineColor(kBlue);
 	h_tag->Draw("histE same");
	h_tag->SetLineColor(kRed);
 	h_tag_NC->Draw("histE same");
	h_tag_NC->SetLineColor(kMagenta);
 	h_tag_cosmic->Draw("histE same");
	h_tag_cosmic->SetLineColor(kOrange);
	c2->cd(2);

	TH1F* heff_tag = new TH1F("heff_tag","Tagged events",40,0,4000);
	heff_tag->Add(h_tag);
	heff_tag->Divide(h_precuts);
	heff_tag->Draw("hist");	
	heff_tag->SetLineColor(kRed);

	heff_tag->GetXaxis()->SetRangeUser(0,2000);
	heff_tag->GetYaxis()->SetRangeUser(0,1.1);

	TH1F* heff_tag_NC = new TH1F("heff_tag_NC","Mis-tagged events",40,0,4000);
	heff_tag_NC->Add(h_tag_NC);
	heff_tag_NC->Divide(h_precuts);
	heff_tag_NC->Draw("hist same");	
	heff_tag_NC->SetLineColor(kMagenta);
	TH1F* heff_tag_cosmic = new TH1F("heff_tag_cosmic","Mis-tagged events",40,0,4000);
	heff_tag_cosmic->Add(h_tag_cosmic);
	heff_tag_cosmic->Divide(h_precuts);
	heff_tag_cosmic->Draw("hist same");	
	heff_tag_cosmic->SetLineColor(kOrange);
	TH1F* heff_precuts = new TH1F("heff_precuts","Pre-select events",40,0,4000);
	heff_precuts->Add(h_precuts);
	heff_precuts->Divide(h_eligible);
	heff_precuts->Draw("hist same");	
	heff_precuts->SetLineColor(kBlue);

	TCanvas* c6 = new TCanvas("c6", "c6", 1200, 650);
	c6->Divide(2,1);
	c6->cd(1);
	h_eligible_tlen->Draw("histE");
	h_precuts_tlen->Draw("histE same");
	h_precuts_tlen->SetLineColor(kBlue);
 	h_tag_tlen->Draw("histE same");
	h_tag_tlen->SetLineColor(kRed);
 	h_tag_tlen_NC->Draw("histE same");
	h_tag_tlen_NC->SetLineColor(kMagenta);
 	h_tag_tlen_cosmic->Draw("histE same");
	h_tag_tlen_cosmic->SetLineColor(kOrange);
	c6->cd(2);
	
	TH1F* heff_tag_tlen = new TH1F("heff_tag_tlen","Tagged events",200,0,1000);
	heff_tag_tlen->Add(h_tag_tlen);
	heff_tag_tlen->Divide(h_precuts_tlen);
	heff_tag_tlen->Draw("hist");	
	heff_tag_tlen->SetLineColor(kRed);

	TH1F* heff_tag_tlen_NC = new TH1F("heff_tag_tlen_NC","Mis-tagged events",200,0,1000);
	heff_tag_tlen_NC->Add(h_tag_tlen_NC);
	heff_tag_tlen_NC->Divide(h_precuts_tlen);
	heff_tag_tlen_NC->Draw("hist same");	
	heff_tag_tlen_NC->SetLineColor(kMagenta);
	TH1F* heff_tag_tlen_cosmic = new TH1F("heff_tag_tlen_cosmic","Mis-tagged events",200,0,1000);
	heff_tag_tlen_cosmic->Add(h_tag_tlen_cosmic);
	heff_tag_tlen_cosmic->Divide(h_precuts_tlen);
	heff_tag_tlen_cosmic->Draw("hist same");	
	heff_tag_tlen_cosmic->SetLineColor(kOrange);


	TH1F* heff_precuts_tlen = new TH1F("heff_precuts_tlen","Pre-select events",200,0,1000);
	heff_precuts_tlen->Add(h_precuts_tlen);
	heff_precuts_tlen->Divide(h_eligible_tlen);
	heff_precuts_tlen->Draw("hist same");	
	heff_precuts_tlen->SetLineColor(kBlue);



	//file->Close();

	TCanvas* c7 = new TCanvas("c7", "c7", 900, 650);
	c7->Divide(2,1);
	c7->cd(1);
	htrue_muonKE_tlen->Draw("colz");
	c7->cd(2);
	hreco_muonKE_tlen->Draw("colz");	

	}	

	}//block switch

}
