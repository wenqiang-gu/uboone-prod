#include <string>
#include <fstream>

void split_tree2(const char* filelist="", const char* outName="output.root", const char* SaveGenieFluxKnob="UBGenieFluxSmallUni") {
  auto T_wgt = new TChain("wcpweights/T_wgt");

  std::ifstream in(filelist);
  std::string line;
  while (std::getline(in, line)) {
    T_wgt->Add(line.c_str());
  }

  // cout << T_wgt->GetEntries() << endl;

  Int_t run, subrun, event;   
  std::string* file_type = new std::string;
  Float_t weight_cv, weight_spline, weight_lee;
  std::map<std::string, std::vector<float>>* mcweight = new std::map<std::string, std::vector<float>>;
  bool mcweight_filled;

  T_wgt->SetBranchAddress("run", &run);
  T_wgt->SetBranchAddress("subrun", &subrun);
  T_wgt->SetBranchAddress("event", &event);
  T_wgt->SetBranchAddress("file_type", &file_type);
  T_wgt->SetBranchAddress("weight_cv", &weight_cv);
  T_wgt->SetBranchAddress("weight_spline", &weight_spline);
  T_wgt->SetBranchAddress("weight_lee", &weight_lee);
  T_wgt->SetBranchAddress("mcweight", &mcweight);
  T_wgt->SetBranchAddress("mcweight_filled", &mcweight_filled);
  // T_wgt->Print();


  std::vector<std::string> UBGenieFluxSmallUni = {
    "All_UBGenie"
    , "AxFFCCQEshape_UBGenie"
    , "DecayAngMEC_UBGenie"
    , "NormCCCOH_UBGenie"
    , "NormNCCOH_UBGenie"
    , "RPA_CCQE_Reduced_UBGenie"
    , "RPA_CCQE_UBGenie"
    , "RootinoFix_UBGenie"
    , "ThetaDelta2NRad_UBGenie"
    , "Theta_Delta2Npi_UBGenie"
    , "TunedCentralValue_UBGenie"
    , "VecFFCCQEshape_UBGenie"
    , "XSecShape_CCMEC_UBGenie"
    , "splines_general_Spline"
    , "xsr_scc_Fa3_SCC"
    , "xsr_scc_Fv3_SCC"
  };

  std::vector<std::string> UBGenieFluxBigUni = {
    "expskin_FluxUnisim"
    , "horncurrent_FluxUnisim"
    , "kminus_PrimaryHadronNormalization"
    , "kplus_PrimaryHadronFeynmanScaling"
    , "kzero_PrimaryHadronSanfordWang"
    , "nucleoninexsec_FluxUnisim"
    , "nucleonqexsec_FluxUnisim"
    , "nucleontotxsec_FluxUnisim"
    , "piminus_PrimaryHadronSWCentralSplineVariation"
    , "pioninexsec_FluxUnisim"
    , "pionqexsec_FluxUnisim"
    , "piontotxsec_FluxUnisim"
    , "piplus_PrimaryHadronSWCentralSplineVariation"
  };

  auto ofile = TFile::Open(outName, "recreate");
  // ofile->mkdir("wcpselection")->cd();

  // set branch
  TTree* UBTree = nullptr;
  std::map<std::string, std::vector<float> > weight_f1;
  std::vector<float> weight_f2;

  if (std::string(SaveGenieFluxKnob) == "UBGenieFluxSmallUni") {
    UBTree = new TTree("UBGenieFluxSmallUni","UBGenieFluxSmallUni");
    UBTree->Branch("run", &run, "run/I");
    UBTree->Branch("subrun", &subrun, "subrun/I");
    UBTree->Branch("event", &event, "event/I");
    UBTree->Branch("file_type", &file_type);
    UBTree->Branch("weight_cv", &weight_cv, "weight_cv/F");
    UBTree->Branch("weight_spline", &weight_spline, "weight_spline/F");
    UBTree->Branch("weight_lee", &weight_lee, "weight_lee/F");
    UBTree->Branch("mcweight_filled", &mcweight_filled);

    for (auto const& knob : UBGenieFluxSmallUni) {
      weight_f1.emplace(knob, std::vector<float>());
      UBTree->Branch(knob.c_str(), &(weight_f1.at(knob)) );
    }

  }
  else {
    int N = std::count(UBGenieFluxBigUni.begin(), UBGenieFluxBigUni.end(), std::string(SaveGenieFluxKnob));
    if (N!=1) {
      std::cerr << "Warning, cannot find the knob: " << SaveGenieFluxKnob << std::endl;
      return 0;
    }

    UBTree = new TTree(SaveGenieFluxKnob, SaveGenieFluxKnob);
    UBTree->Branch("run", &run, "run/I");
    UBTree->Branch("subrun", &subrun, "subrun/I");
    UBTree->Branch("event", &event, "event/I");
    UBTree->Branch("file_type", &file_type);
    UBTree->Branch("weight_cv", &weight_cv, "weight_cv/F");
    UBTree->Branch("weight_spline", &weight_spline, "weight_spline/F");
    UBTree->Branch("weight_lee", &weight_lee, "weight_lee/F");
    UBTree->Branch("mcweight_filled", &mcweight_filled);
    UBTree->Branch(SaveGenieFluxKnob, &weight_f2);
  }


  // loop through events
  for (size_t i=0; i<T_wgt->GetEntries(); i++) {
    T_wgt->GetEntry(i);
    // for (auto const& ele: *mcweight) {
    //   cout << ele.first << " " << ele.second.size() << endl;
    // }

    for (auto x: weight_f1) { x.second.clear(); }
    weight_f2.clear();
 
    if (mcweight_filled>0) {
      // cout << mcweight_filled << endl;
      if (std::string(SaveGenieFluxKnob)=="UBGenieFluxSmallUni") {
        for(auto const& knob: UBGenieFluxSmallUni) {
          // weight_f1->push_back(mcweight->at(knob));
          weight_f1.at(knob) = mcweight->at(knob);
        }
      }
      else {
         weight_f2 = mcweight->at(std::string(SaveGenieFluxKnob));
      }
    }
 
    UBTree->Fill();
  }

  UBTree->Write();
  ofile->Close();

}
