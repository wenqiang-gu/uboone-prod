void ReadFile(const char* filename1, const char* filename2=""){
  auto t = new TTree("t","microboone particle label");
  long nlines = t->ReadFile(filename1,"entry/I:run/I:subrun/I:event/I:weight/D:status/I:post_generic/I:numuCC/I:nueBDT/I:is_FC/I:Ereco/D:Evis/D:is_cosmic/I:is_numuCC/I:is_NC/I:is_nue/I",',');
  t->Show(0);

  // t->Draw("Ereco >> h1(80,0,4)","status==1 && post_generic==1 && numuCC==1");
  // auto h1 = (TH1F*)gROOT->FindObject("h1");


  // std::string filename2_str(filename2);
  // if(filename2_str!="") {
  //   auto t2 = new TTree("t","microboone particle label");
  //   nlines = t2->ReadFile(filename2,"entry/I:run/I:subrun/I:event/I:weight/D:status/I:post_generic/I:numuCC/I:nueBDT/I:Ereco/D:Evis/D",',');
  //   t2->Draw("Ereco >> h2(80,0,4)","status==1 && post_generic==1 && numuCC==1");
  //   auto h2 = (TH1F*)gROOT->FindObject("h2");
  //   h2->SetLineColor(2);
  //   h2->SetMarkerColor(2);


  //   h2->Add(h1,-1);
  //   h2->Divide(h1);
  //   h2->Draw();
  //   
  // }

}
