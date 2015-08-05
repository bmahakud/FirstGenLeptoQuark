// -*- C++ -*-
//
// Package:    FirstGenLeptoQuark/TreeMaker
// Class:      TreeMaker
// 
/**\class TreeMaker TreeMaker.cc FirstGenLeptoQuark/TreeMaker/plugins/TreeMaker.cc

 Description: [one line class summary]

 Implementation:
     [Notes on implementation]
*/
//
// Original Author:  Bibhuprasad Mahakud
//         Created:  Wed, 05 Aug 2015 16:01:42 GMT
//
//

#include "FirstGenLeptoQuark/TreeMaker/interface/TreeMaker.h"
// system include files


using namespace std;
using namespace edm;
using namespace reco;
using namespace pat;



//
// static data member definitions
//

//
// constructors and destructor
//
TreeMaker::TreeMaker(const edm::ParameterSet& iConfig)
: tree(0),
  VarTypeNames{"VarsBool","VarsInt","VarsDouble","VarsString","VarsTLorentzVector","VectorBool","VectorInt","VectorDouble","VectorString","VectorTLorentzVector","VectorRecoCand"},
  VarTypes{t_bool,t_int,t_double,t_string,t_lorentz,t_vbool,t_vint,t_vdouble,t_vstring,t_vlorentz,t_recocand}
{
   //register your products
/* Examples
   produces<ExampleData2>();

   //if do put with a label
   produces<ExampleData2>("label");
 
   //if you want to put into the Run
   produces<ExampleData2,InRun>();
*/
   //now do what ever other initialization is needed
  

        treeName = iConfig.getParameter<string>("TreeName");
        debug = iConfig.getParameter<bool>("debug");
        doLorentz = iConfig.getParameter<bool>("doLorentz");
        sortBranches = iConfig.getParameter<bool>("sortBranches");
        //loop over all var type names
         VarNames.reserve(VarTypeNames.size());
         for(unsigned v = 0; v < VarTypeNames.size(); ++v){
          VarNames.push_back(iConfig.getParameter< vector<string> >(VarTypeNames.at(v)));
          }
        //






}


TreeMaker::~TreeMaker()
{
 
   // do anything here that needs to be done at desctruction time
   // (e.g. close files, deallocate resources etc.)

}


//
// member functions
//

// ------------ method called to produce the data  ------------
void
TreeMaker::produce(edm::Event& iEvent, const edm::EventSetup& iSetup)
{
   using namespace edm;
/* This is an event example
   //Read 'ExampleData' from the Event
   Handle<ExampleData> pIn;
   iEvent.getByLabel("example",pIn);

   //Use the ExampleData to create an ExampleData2 which 
   // is put into the Event
   std::unique_ptr<ExampleData2> pOut(new ExampleData2(*pIn));
   iEvent.put(std::move(pOut));
*/

/* this is an EventSetup example
   //Read SetupData from the SetupRecord in the EventSetup
   ESHandle<SetupData> pSetup;
   iSetup.get<SetupRecord>().get(pSetup);
*/

        runNum = 0;
        lumiBlockNum = 0;
        evtNum = 0;


     edm::EventAuxiliary aux = iEvent.eventAuxiliary();
        runNum       = aux.run();
        lumiBlockNum = aux.luminosityBlock();
        evtNum       = aux.event();



     for(unsigned t = 0; t < variables.size(); ++t){
                variables[t]->FillTree(iEvent);
        }

        tree->Fill();
        if(debug) cout << "Done filling tree" << endl;




 
}

// ------------ method called once each job just before starting event loop  ------------
void 
TreeMaker::beginJob()
{
edm::Service<TFileService> fs;
	if( !fs ) {
		throw edm::Exception(edm::errors::Configuration, "TFile Service is not registered in cfg file");
	}
	tree = fs->make<TTree>(treeName.c_str(),treeName.c_str());  
	tree->SetAutoSave(10000000000);
	tree->SetAutoFlush(1000000);
	tree->Branch("RunNum",&runNum,"RunNum/i");
	tree->Branch("LumiBlockNum",&lumiBlockNum,"LumiBlockNum/i");
	tree->Branch("EvtNum",&evtNum,"EvtNum/i");

	//initialize TreeObjects
	for(unsigned v = 0; v < VarNames.size(); ++v){
		cout << VarTypeNames[v] << ":" << endl;
		for(unsigned t = 0; t < VarNames[v].size(); ++t){
			//check for the right type
			TreeObjectBase* tmp = NULL;
			switch(VarTypes[v]){
				case TreeTypes::t_bool     : tmp = new TreeObject<bool>(VarNames[v][t],tree); break;
				case TreeTypes::t_int      : tmp = new TreeObject<int>(VarNames[v][t],tree); break;
				case TreeTypes::t_double   : tmp = new TreeObject<double>(VarNames[v][t],tree); break;
				case TreeTypes::t_string   : tmp = new TreeObject<string>(VarNames[v][t],tree); break;
				case TreeTypes::t_lorentz  : tmp = new TreeObject<TLorentzVector>(VarNames[v][t],tree); break;
				case TreeTypes::t_vbool    : tmp = new TreeObject<vector<bool> >(VarNames[v][t],tree); break;
				case TreeTypes::t_vint     : tmp = new TreeObject<vector<int> >(VarNames[v][t],tree); break;
				case TreeTypes::t_vdouble  : tmp = new TreeObject<vector<double> >(VarNames[v][t],tree); break;
				case TreeTypes::t_vstring  : tmp = new TreeObject<vector<string> >(VarNames[v][t],tree); break;
				case TreeTypes::t_vlorentz : tmp = new TreeObject<vector<TLorentzVector> >(VarNames[v][t],tree); break;
				case TreeTypes::t_recocand : tmp = new TreeRecoCand(VarNames[v][t],tree,doLorentz); break;
			}
			//if a known type was found, initialize and store the object
			if(tmp) {
				tmp->Initialize(nameCache);
				variables.push_back(tmp);
			}
		}
	}
	
	//sort TreeObjects (if desired)
	if(sortBranches) sort(variables.begin(),variables.end(),TreeObjectComp());
	
	//add branches to tree
	for(unsigned t = 0; t < variables.size(); ++t){
		variables[t]->AddBranch();
	}


}

// ------------ method called once each job just after ending the event loop  ------------
void 
TreeMaker::endJob() {

//memory management
	for(unsigned t = 0; t < variables.size(); ++t){
		delete (variables[t]);
	}
	variables.clear();



}

// ------------ method called when starting to processes a run  ------------
/*
void
TreeMaker::beginRun(edm::Run const&, edm::EventSetup const&)
{
}
*/
 
// ------------ method called when ending the processing of a run  ------------
/*
void
TreeMaker::endRun(edm::Run const&, edm::EventSetup const&)
{
}
*/
 
// ------------ method called when starting to processes a luminosity block  ------------
/*
void
TreeMaker::beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
{
}
*/
 
// ------------ method called when ending the processing of a luminosity block  ------------
/*
void
TreeMaker::endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
{
}
*/
 
// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void
TreeMaker::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);
}

//define this as a plug-in
DEFINE_FWK_MODULE(TreeMaker);
