// --------------------------------- //
// convert.C -- root macro
// Usage : $ root -l -b -q convert.C
// For ./drscmd output files
// Author: Keita Mizukoshi (Kobe)
// Nov. 12 2020
// --------------------------------- //
int convert(TString filename = "sub0000.0000", TString dirname = "/Users/mzks/data/testdir/"){

	ifstream ifs(dirname+filename+".dat");
	if(!ifs.is_open()) return -1;

	ULong64_t eventid;
	ULong64_t timestamp;
	UInt_t timestamp_usec;
	Float_t t1, u1, t2, u2, t3, u3, t4, u4;
    UInt_t s0, s1, s2, s3, s4, s5;
	Float_t wf[4][1024];
	Float_t tbin[4][1024];
    UInt_t scaler[6];

	auto out_file = TFile::Open("./"+filename+".root", "RECREATE");
	auto tree = new TTree("tree", "tree");
	tree->Branch("eventid", &eventid, "eventid/l");
	tree->Branch("timestamp", &timestamp, "timestamp/l");
	tree->Branch("timestamp_usec", &timestamp_usec, "timestamp_usec/i");
	tree->Branch("wf", wf, "wf[4][1024]/F");
	tree->Branch("tbin", tbin, "tbin[4][1024]/F");
	tree->Branch("scaler", scaler, "scaler[6]/i");


	while(ifs >> eventid >> timestamp >> timestamp_usec){
            ifs >> s0 >> s1 >> s2 >> s3 >> s4 >> s5;
            scaler[0] = s0;
            scaler[1] = s1;
            scaler[2] = s2;
            scaler[3] = s3;
            scaler[4] = s4;
            scaler[5] = s5;
		for(int ibin=0;ibin<1024;++ibin){
			ifs >> t1 >> u1 >> t2 >> u2 >> t3 >> u3 >> t4 >> u4;
			tbin[0][ibin] = t1;
			tbin[1][ibin] = t2;
			tbin[2][ibin] = t3;
			tbin[3][ibin] = t4;
			wf[0][ibin] = u1;
			wf[1][ibin] = u2;
			wf[2][ibin] = u3;
			wf[3][ibin] = u4;
		}
		tree->Fill();
	}
	tree->Write();
	out_file->Close();

	return 0;
}
