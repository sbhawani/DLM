#include "DLM_Random.h"
#include <stdio.h>

using namespace std;

const double Pi(3.141592653589793);

DLM_Random::DLM_Random(const unsigned& seed):SEED(seed){
    std::random_device rd;  //Will be used to obtain a seed for the random number engine
    MT_RanGen = new std::mt19937_64(SEED?SEED:rd());
    RealDist = new std::uniform_real_distribution<> (0,1);
    ExpDist = new std::exponential_distribution<> (1);
    NormDist = new std::normal_distribution<> (0,1);
    CauchyDist = new std::cauchy_distribution<> (0,1);
    STAB_TEMP=NULL;
    SKEW_TEMP=NULL;
    SCAL_TEMP=NULL;
    LOCA_TEMP=NULL;
}
DLM_Random::~DLM_Random(){
    delete MT_RanGen;MT_RanGen=NULL;
    delete RealDist;RealDist=NULL;
    delete ExpDist;ExpDist=NULL;
}

double DLM_Random::Uniform(const double& from, const double& to){
    if(from>=to) return from;
    return (RealDist[0](*MT_RanGen))*(to-from)+from;
}
double DLM_Random::Gauss(const double& mean, const double& sigma){
    return (NormDist[0](*MT_RanGen))*sigma+mean;
}
double DLM_Random::Cauchy(const double& mean, const double& sigma){
    return (CauchyDist[0](*MT_RanGen))*sigma+mean;
}
double DLM_Random::Stable(const double& stability, const double& location, const double& scale, const double& skewness){
    if(stability<0||stability>2){
        printf("\033[1;33mWARNING:\033[0m Bad 'stability' parameter in DLM_Random::Stable\n");
        return 0;
    }
    if(skewness<-1||skewness>1){
        printf("\033[1;33mWARNING:\033[0m Bad 'skewness' parameter in DLM_Random::Stable\n");
        return 0;
    }

    if(stability==2){
        return Gauss(location,scale);
    }
    else if(stability==1&&skewness==0){
        return Cauchy(location,scale);
    }

    const double S = -skewness*tan(Pi*stability*0.5);
    const double U = Uniform(-0.5*Pi,0.5*Pi);
    const double W = ExpDist[0](*MT_RanGen);
    //make sure you use fabs(scale)
    if(stability==1){
        const double E = Pi*0.5;
        return ((Pi*0.5+skewness*U)*tan(U))/E-skewness*log((Pi*0.5*W*cos(U))/(Pi*0.5+skewness*U))*scale/sqrt(2)+location;
    }
    else{
        const double E = atan(-S)/stability;
        return pow(1.+S*S,0.5/stability)*sin(stability*(U+E))/pow(cos(U),1./stability)*pow(cos(U-stability*(U+E))/W,(1.-stability)/stability)*scale/sqrt(2)+location;

    }
}

double DLM_Random::GaussR(const unsigned short& dim, const double& mean, const double& sigma){
    return StableR(dim,2,mean,sigma,0);
}
double DLM_Random::CauchyR(const unsigned short& dim, const double& mean, const double& sigma){
    return StableR(dim,1,mean,sigma,0);
}
double DLM_Random::StableR(const unsigned short& dim, const double& stability, const double& location, const double& scale, const double& skewness){
    if(!dim) return 0;
    double Result=0;
    for(unsigned short us=0; us<dim; us++){
        Result+=pow(Stable(stability,location,scale,skewness),2.);
    }
    return sqrt(Result);
}
double DLM_Random::GaussR(const unsigned short& dim, const double* mean, const double* sigma){
    return StableR(dim,2,mean,sigma,NULL);
}
double DLM_Random::CauchyR(const unsigned short& dim, const double* mean, const double* sigma){
    return StableR(dim,1,mean,sigma,NULL);
}
double DLM_Random::StableR(const unsigned short& dim, const double* stability, const double* location, const double* scale, const double* skewness){
    if(!dim) return 0;
    double Result=0;
    for(unsigned short us=0; us<dim; us++){
        Result+=pow(Stable(stability[us],location[us],scale[us],skewness[us]?skewness[us]:0),2.);
    }
    return sqrt(Result);
}
double DLM_Random::StableR(const unsigned short& dim, const double& stability, const double* location, const double* scale, const double* skewness){
    if(!dim) return 0;
    double Result=0;
    for(unsigned short us=0; us<dim; us++){
        Result+=pow(Stable(stability,location[us],scale[us],skewness[us]?skewness[us]:0),2.);
    }
    return sqrt(Result);
}
double DLM_Random::GaussDiffR(const unsigned short& dim, const double& mean, const double& sigma){
    return StableDiffR(dim,2,mean,sigma,0);
}
double DLM_Random::CauchyDiffR(const unsigned short& dim, const double& mean, const double& sigma){
    return StableDiffR(dim,1,mean,sigma,0);
}
double DLM_Random::StableDiffR(const unsigned short& dim,const double& stability, const double& location, const double& scale, const double& skewness){
    return StableDiffR(dim,stability,location,scale,skewness,stability,location,scale,skewness);
}
double DLM_Random::GaussDiffR(const unsigned short& dim,
                              const double& mean1, const double& sigma1,
                              const double& mean2, const double& sigma2){
    return StableDiffR(dim,2,mean1,sigma1,0,2,mean2,sigma2,0);
}
double DLM_Random::CauchyDiffR(const unsigned short& dim,
                               const double& mean1, const double& sigma1,
                               const double& mean2, const double& sigma2){
    return StableDiffR(dim,1,mean1,sigma1,0,1,mean2,sigma2,0);
}
double DLM_Random::StableDiffR( const unsigned short& dim,
                                const double& stability1, const double& location1, const double& scale1, const double& skewness1,
                                const double& stability2, const double& location2, const double& scale2, const double& skewness2){
    if(!dim) return 0;
    double Result=0;
    for(unsigned short us=0; us<dim; us++){
        Result+=pow(Stable(stability2,location2,scale2,skewness2)-Stable(stability1,location1,scale1,skewness1),2.);
    }
    return sqrt(Result);
}
double DLM_Random::GaussDiffR(const unsigned short& dim, const double* mean, const double* sigma){
    return StableDiffR(dim,2,mean,sigma,NULL);
}
double DLM_Random::CauchyDiffR(const unsigned short& dim, const double* mean, const double* sigma){
    return StableDiffR(dim,2,mean,sigma,NULL);
}
double DLM_Random::StableDiffR(const unsigned short& dim,const double* stability, const double* location, const double* scale, const double* skewness){
    return StableDiffR(dim,stability,location,scale,skewness,stability,location,scale,skewness);
}
double DLM_Random::StableDiffR(const unsigned short& dim,const double& stability, const double* location, const double* scale, const double* skewness){
    return StableDiffR(dim,stability,location,scale,skewness,stability,location,scale,skewness);
}
double DLM_Random::StableDiffR( const unsigned short& dim,
                                const double* stability1, const double* location1, const double* scale1, const double* skewness1,
                                const double* stability2, const double* location2, const double* scale2, const double* skewness2){
    if(!dim) return 0;
    double Result=0;
    for(unsigned short us=0; us<dim; us++){
        Result+=pow(Stable(stability2[us],location2[us],scale2[us],skewness2[us]?skewness2[us]:0)-Stable(stability1[us],location1[us],scale1[us],skewness1[us]?skewness1[us]:0),2.);
    }
    return sqrt(Result);
}
double DLM_Random::StableDiffR( const unsigned short& dim,
                                const double& stability1, const double* location1, const double* scale1, const double* skewness1,
                                const double& stability2, const double* location2, const double* scale2, const double* skewness2){
    if(!dim) return 0;
    double Result=0;
    for(unsigned short us=0; us<dim; us++){
        Result+=pow(Stable(stability2,location2[us],scale2[us],skewness2[us]?skewness2[us]:0)-Stable(stability1,location1[us],scale1[us],skewness1[us]?skewness1[us]:0),2.);
    }
    return sqrt(Result);
}