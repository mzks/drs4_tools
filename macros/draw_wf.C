
int draw_wf(TString filename = "sub0000.0000", ULong64_t event_id=0){

    auto file = TFile::Open(filename + ".root");
    auto tree = dynamic_cast<TTree*>(file->Get("tree"));

    ULong64_t       eventid;
    ULong64_t       timestamp;
    UInt_t          timestamp_usec;
    Float_t         wf[4][1024];
    Float_t         tbin[4][1024];
    UInt_t          scaler[6];

    TBranch        *b_eventid;   //!
    TBranch        *b_timestamp;   //!
    TBranch        *b_timestamp_usec;   //!
    TBranch        *b_wf;   //!
    TBranch        *b_tbin;   //!
    TBranch        *b_scaler;   //!

    tree->SetBranchAddress("eventid", &eventid, &b_eventid);
    tree->SetBranchAddress("timestamp", &timestamp, &b_timestamp);
    tree->SetBranchAddress("timestamp_usec", &timestamp_usec, &b_timestamp_usec);
    tree->SetBranchAddress("wf", wf, &b_wf);
    tree->SetBranchAddress("tbin", tbin, &b_tbin);
    tree->SetBranchAddress("scaler", scaler, &b_scaler);

    tree->GetEntry(event_id);

    TGraph* tg1 = new TGraph(1024, tbin[0], wf[0]);
    TGraph* tg2 = new TGraph(1024, tbin[1], wf[1]);
    TGraph* tg3 = new TGraph(1024, tbin[2], wf[2]);
    TGraph* tg4 = new TGraph(1024, tbin[3], wf[3]);

    tg1->SetLineColor(kBlue);
    tg2->SetLineColor(kOrange);
    tg3->SetLineColor(kGreen+2);
    tg4->SetLineColor(kRed);

    tg1->Draw("AL");
    tg2->Draw("L");
    tg3->Draw("L");
    tg4->Draw("L");


    return 0;
}
