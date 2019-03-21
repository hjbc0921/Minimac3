#ifndef IMPUTATION_H_INCLUDED
#define IMPUTATION_H_INCLUDED
#include "MarkovParameters.h"

#include "HaplotypeSet.h"
#include "MemoryInfo.h"
#include "MemoryAllocators.h"
#include "MarkovModel.h"
#include "MarkovParameters.h"
#include "Unique.h"
#include <cstdio>
#include <cmath>



#ifdef _OPENMP
#include <omp.h>
#endif
using namespace std;


class Imputation
    {
        public:

            int targetCount,refCount,markerCount,noReducedStatesCurrent;
            String outFile;
            int MaxSample;
            bool full,makeOptmxOnly;
            bool em,vcfOutput,doseOutput,TypedOnly;
            bool phased;
            bool gzip,LowMemory;
            bool RsId;
            bool GT,DS,GP,unphasedOutput;
            vector<bool> format;
            bool includeGwas,updateModel;
            String errFile,recFile;
            int EstimationRounds,EstimationStates;

        Imputation(HaplotypeSet &rHap,String &out,bool gz)
        {
            refCount=rHap.numHaplotypes;
            markerCount=rHap.numMarkers;
            outFile=out;
            gzip=gz;
            updateModel=true;

        }

        Imputation                                                  (HaplotypeSet &tHap,HaplotypeSet &rHap,String &out,String &err,
                                                                    String &rec,bool Ph,bool gz,bool rsid, int rounds, int states,bool vcfoutput,
                                                                    bool doseoutput, bool onlyrefmarkers,vector<bool> &Format,bool lowMemory )
                                                                    {

                                                                        RsId=rsid;
                                                                        targetCount=tHap.numHaplotypes;
                                                                        refCount=rHap.numHaplotypes;
                                                                        markerCount=rHap.numMarkers;
                                                                        outFile=out;
                                                                        MaxSample=2000000;
                                                                        MaxSample=(MaxSample>(int)targetCount?(int)targetCount:MaxSample);
                                                                        errFile=err;
                                                                        recFile=rec;
                                                                        EstimationRounds=rounds;
                                                                        EstimationStates=states;
                                                                        em=true;
                                                                        phased=Ph;
                                                                        gzip=gz;
                                                                        vcfOutput=vcfoutput;
                                                                        doseOutput=doseoutput;
                                                                        TypedOnly=onlyrefmarkers;
                                                                        format=Format;
                                                                        GT=Format[0];
                                                                        DS=Format[1];
                                                                        GP=Format[2];
                                                                        updateModel=false;
                                                                        LowMemory=lowMemory;

                                                                    };

       Imputation                                                   (HaplotypeSet &tHap,HaplotypeSet &rHap,String &out,String &err,
                                                                    String &rec,bool Ph,bool gz,bool rsid, int rounds, int states,bool vcfoutput,
                                                                    bool doseoutput, bool onlyrefmarkers,vector<bool> &Format, bool updateMODEL,
                                                                    bool UnphasedOutput,bool lowMemory)
                                                                    {
                                                                        RsId=rsid;
                                                                        updateModel=updateMODEL;
                                                                        targetCount=tHap.numHaplotypes;
                                                                        refCount=rHap.numHaplotypes;
                                                                        markerCount=rHap.numMarkers;
                                                                        outFile=out;
                                                                        MaxSample=2000000;
                                                                        MaxSample=(MaxSample>(int)targetCount?(int)targetCount:MaxSample);
                                                                        errFile=err;
                                                                        recFile=rec;
                                                                        EstimationRounds=rounds;
                                                                        EstimationStates=states;
                                                                        em=true;
                                                                        phased=Ph;
                                                                        gzip=gz;
                                                                        vcfOutput=vcfoutput;
                                                                        doseOutput=doseoutput;
                                                                        TypedOnly=onlyrefmarkers;
                                                                        format=Format;
                                                                        GT=Format[0];
                                                                        DS=Format[1];
                                                                        GP=Format[2];
                                                                        unphasedOutput=UnphasedOutput;
                                                                        LowMemory=lowMemory;
                                                                    };

        MarkovParameters*               createEstimates             (HaplotypeSet &rHap,HaplotypeSet &tHap,vector<int> &optStructure,bool NoTargetEstimation);
        void                            splitFoldedProb             (vector<float> &SplitProb,vector<float> &totalProb, vector<float> &noRecomProb);
        void                            performImputation           (HaplotypeSet &tHap,HaplotypeSet &rHap, String Golden);
        void                            LooOptimalStructure         (int loo,HaplotypeSet &rHap, HaplotypeSet &HapLoo);
        void                            Condition                   (HaplotypeSet &rHap, int markerPos,vector<float> &Prob,
                                                                    vector<float> &noRecomProb,
                                                                    double e, double freq, bool observed, double backgroundError, int NoRedStates, ReducedHaplotypeInfo &Info);
        double                          CalculateLikelihood         (HaplotypeSet &rHap,MarkovModel &MM);
        void                            ImputeTraverse              (HaplotypeSet &rHap,HaplotypeSet &tHap,int hapID,
                                                                    MarkovModel &MM,int group, vector<float> &recomProb,
                                                                    vector<float> &PrevRightFoldedProb,
                                                                    vector<float> &CurrentRightProb,
                                                                    vector<float> &CurrentNoRecoRightProb,
                                                                    HaplotypeSet &rHapMain);
        void                            LeftTraverse                (HaplotypeSet &rHap,HaplotypeSet &tHap,int hapID,
                                                                    MarkovModel &MM,int group, vector<float> &recomProb,
                                                                    HaplotypeSet &rHapMain);
        void                            EMTraverse                  (HaplotypeSet &rHap,HaplotypeSet &tHap,int hapID,
                                                                    MarkovModel &MM,int group, vector<float> &recomProb,
                                                                    vector<float> &PrevRightFoldedProb,
                                                                    vector<float> &CurrentRightProb,
                                                                    vector<float> &CurrentNoRecoRightProb,
                                                                    HaplotypeSet &rHapMain);
        void                            ConditionJunctionProb       (HaplotypeSet &rHap, int markerPos,vector<float> &Prob,
                                                                    double e, double freq, bool observed, double backgroundError,
                                                                    ReducedHaplotypeInfo &Info);
        void                            FlushPartialVcf             (HaplotypeSet &rHap,HaplotypeSet &tHap,HaplotypeSet &PartialDosage, string &filename,int &Index);
        void                            MergeFinalVcf               (HaplotypeSet &rHap,HaplotypeSet &tHap,ImputationStatistics &stats,int MaxIndex);
        void                            MergeFinalVcfAllVariants    (HaplotypeSet &rHap,HaplotypeSet &tHap,ImputationStatistics &stats,int MaxIndex);
        void                            PrintDosageData             (HaplotypeSet &rHap,HaplotypeSet &tHap,
                                                                     IFILE dosages, vector<float> &ThisDosage,
                                                                     int ThisSampleId);
        void                            PrintHaplotypeData          (HaplotypeSet &rHap,HaplotypeSet &tHap,
                                                                     IFILE hapdose, IFILE haps,
                                                                     vector<float> &ThisimputedHap,vector<bool> ThisimputedAlleles,
                                                                     int ThisHapId, int ThisSampleId);
        void                            PrintInfoFile               (HaplotypeSet &rHap,HaplotypeSet &tHap, ImputationStatistics &stats);

};




#endif // IMPUTATION_H_INCLUDED
