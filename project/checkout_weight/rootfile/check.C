void check(const char* filename, const char* treename){

  // check entries
  auto file = TFile::Open(filename);
  auto t = (TTree*)file->Get(treename);
  int N = t->GetEntries();
  std::cout << "entries: " << N << std::endl;
}
