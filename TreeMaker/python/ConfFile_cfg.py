import FWCore.ParameterSet.Config as cms

process = cms.Process("OWNPARTICLES")

process.load("FWCore.MessageService.MessageLogger_cfi")

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(-1) )

process.source = cms.Source("PoolSource",
    # replace 'myfile.root' with the source file you want to use
    fileNames = cms.untracked.vstring(
        'file:/uscms_data/d3/bmahakud/DYJetsToLL_M-50_HT-600toInf_Tune4C_13TeV-madgraph-tauola_MINIAODSIM_PU20bx25_PHYS14_25_V1-v1.root'
    )
)

process.myProducerLabel = cms.EDProducer('TreeMaker',
TreeName          = cms.string('LeptoQuarkTree'),
## might help if something isn't working, will produce couts
debug = cms.bool(False),
#default: output RecoCands as vector<TLorentzVector>
#switches to vector<double> pt, eta, phi, energy if false
doLorentz = cms.bool(True),
#branches are sorted alphabetically by default
sortBranches = cms.bool(True),
# list of reco candidate objects: for each reco cand collection, the TLorentzVector will be stored in a vector.
VarsBool = cms.vstring(),
VarsInt = cms.vstring(),
VarsFloat = cms.vstring(),
VarsDouble =cms.vstring(),
VarsString = cms.vstring(),
VarsTLorentzVector = cms.vstring(),
VectorBool = cms.vstring(),
VectorInt = cms.vstring(),
VectorFloat = cms.vstring(),
VectorDouble = cms.vstring(),
VectorString = cms.vstring(),
VectorTLorentzVector = cms.vstring(),
VectorRecoCand = cms.vstring(),



)


process.TFileService = cms.Service("TFileService",
                                   fileName = cms.string('DY_Sample_mc.root')
                                   )


#process.out = cms.OutputModule("PoolOutputModule",
#    fileName = cms.untracked.string('Test.root')
#)

  
process.p = cms.Path(process.myProducerLabel)

#process.e = cms.EndPath(process.out)
