#include "ImputationStatistics.h"

#include <math.h>
#include <iostream>

using namespace std;

ImputationStatistics::ImputationStatistics(int markers)
   {
   sum.resize(markers, 0.0);
   sumSq.resize(markers, 0.0);
   sumCall.resize(markers, 0.0);
   looSum.resize(markers, 0.0);
   looSumSq.resize(markers, 0.0);
   looProduct.resize(markers, 0.0);
   looObserved.resize(markers, 0.0);
    looObservedSq.resize(markers, 0.0);

   count.resize(markers,0);
   looCount.resize(markers,0);
   }

ImputationStatistics::~ImputationStatistics()
   { }

void ImputationStatistics::Update(vector<float> &doses, vector<float> &loo
                                  ,vector<bool> &observed,vector<bool> &Miss, vector<bool> &major)
{
    for (int i = 0; i < (int)doses.size(); i++)
    {
        sum[i] += doses[i];
        sumSq[i] += doses[i] * doses[i];
        sumCall[i] += doses[i] > 0.5 ? doses[i] : 1.0 - doses[i];
        count[i] ++;

    }

    for (int i = 0; i < (int)loo.size(); i++)
        if (!Miss[i])
        {
            looSum[i] += loo[i];
            looSumSq[i] += loo[i] * loo[i];
            looProduct[i] += (observed[i]) ? loo[i] : 0.0;
            looObserved[i] += (observed[i]) ? 1.0 : 0.0;
            looCount[i]++;
        }
}

double ImputationStatistics::Rsq(int marker)
   {
   if (count[marker] < 2)
      return 0.0;

   double f = sum[marker] / (count[marker] + 1e-30);
   double evar = f * (1.0 - f);
   double ovar = 0.0;

   if((sumSq[marker] - sum[marker] * sum[marker] / (count[marker] + 1e-30))>0)
        ovar=(sumSq[marker] - sum[marker] * sum[marker] / (count[marker] + 1e-30)) / (count[marker] + 1e-30);

   return ovar / (evar + 1e-30);
   }


double ImputationStatistics::LooRsq(int marker)
   {
    if (looCount[marker] < 2)
        return 0.0;

    double f = looSum[marker] / (looCount[marker] + 1e-30);
    double evar = f * (1.0 - f);
    double ovar = 0.0;

    if(((looSumSq[marker] - looSum[marker] * looSum[marker] / (looCount[marker] + 1e-30)))>0)
        ovar = (looSumSq[marker] - looSum[marker] * looSum[marker] / (looCount[marker] + 1e-30)) / (looCount[marker] + 1e-30);

    return ovar / (evar + 1e-30);
   }

double ImputationStatistics::AlleleFrequency(int marker)
   {
   if (count[marker] < 2)
      return 0.0;

   return sum[marker] / (count[marker] + 1e-30);
   }

double ImputationStatistics::EmpiricalR(int marker)
   {
   if (looCount[marker] < 2)
      return 0.0;

   // n * Sum xy - Sum x * Sum y
    double p = looCount[marker] * looProduct[marker] - looSum[marker] * looObserved[marker];
    double qx=0.0,qy=0.0;
   // sqrt(n*Sum xx - Sum x * Sum x)


    if((looCount[marker] * looSumSq[marker] - looSum[marker] * looSum[marker])>0)
        qx = sqrt((looCount[marker] * looSumSq[marker] - looSum[marker] * looSum[marker]));

    if((looCount[marker] * looObserved[marker] - looObserved[marker] * looObserved[marker])>0)
        qy = sqrt((looCount[marker] * looObserved[marker] - looObserved[marker] * looObserved[marker]));

   if (qx / (qy + 1e-30) < 1e-3)
      return 0.0;

   if (qy / (qx + 1e-30) < 1e-3)
      return 0.0;

   double r = p / (qx * qy + 1e-30);

   return r;
   }

double ImputationStatistics::EmpiricalRsq(int marker)
   {
   double r = EmpiricalR(marker);

   return r * r;
   }




double ImputationStatistics::LooMajorDose(int marker)
   {
   return looProduct[marker] / (looObserved[marker] + 1e-30);
   }

double ImputationStatistics::LooMinorDose(int marker)
   {
   return (looSum[marker] - looProduct[marker]) / (looCount[marker] - looObserved[marker] + 1e-30);
   }

double ImputationStatistics::AverageCallScore(int marker)
   {
   return sumCall[marker] / (count[marker] + 1e-30);
   }

