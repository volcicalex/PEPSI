#include <iostream>
#include <jlparser/parser.hpp>
#include "AbstractPricer.hpp"
#include "PnlRandom.hpp"

AbstractPricer::~AbstractPricer() {}

AbstractPricer::AbstractPricer(BlackScholesModel *mod, Option *opt, RandomGenerator *rng, double fdStep, size_t nbSamples, int H) {
    mod_ = mod;
    opt_ = opt;
    rng_ = rng;
    fdStep_ = fdStep;
    nbSamples_ = nbSamples;
    H_ = H;
}

AbstractPricer::AbstractPricer(char *fileName) {
    Param *P = new Parser(fileName);

    int size;
    double r;
    double rho;
    PnlVect *sigma, *spot;

    bool fdStepAvailable = P->extract("fd step", fdStep_, true);
    if (!fdStepAvailable)
    {
        fdStep_ = 0.1;
    }

    bool H_Available = P->extract("hedging dates number", H_, true);
    if (!H_Available)
    {
        H_ = 150;
    }


    P->extract("sample number", this->nbSamples_);
    P->extract("option size", size);
    P->extract("interest rate", r);
    P->extract("correlation", rho);
    P->extract("volatility", sigma, size);
    P->extract("spot", spot, size);

    PnlVect *trend;
    bool trendAvailable = P->extract("trend", trend, size, true);

    if (!trendAvailable)
    {
        this->mod_ = new BlackScholesModel(size, r, rho, sigma, spot);
    }
    else
    {
        this->mod_ = new BlackScholesModel(size, r, rho, sigma, spot, trend);
    }


    double T;
    int nbTimeSteps;
    PnlVect *weights;
    std::string optionType;
    P->extract("option type", optionType);

    P->extract("maturity", T);
    P->extract("TimeStep number", nbTimeSteps);

    P->extract("payoff coefficients", weights, size);

    rng_ = new PnlRandom();

    if (optionType.compare("performance") == 0 )
    {
        this->opt_ = new PerformanceOption(T, nbTimeSteps, size, weights);
    }
    else {
        double strike;
        P->extract("strike", strike);
        if (optionType.compare("basket") == 0 )
        {
            this->opt_ = new BasketOption(T, nbTimeSteps, size, strike, weights);
        }
        else if (optionType.compare("asian") == 0 )
        {
            this->opt_ = new AsianOption(T, nbTimeSteps, size, strike, weights);
        }
        else {
            std::cerr << "Unkonwn option type" << std::endl;
        }
    }
    pnl_vect_free(&weights);
    pnl_vect_free(&sigma);
    pnl_vect_free(&spot);
    delete P;
}

void AbstractPricer::delta(const PnlMat *past, double t, PnlVect *delta, PnlVect *icDelta) {}

void AbstractPricer::price(double &prix, double &ic) {}

void AbstractPricer::price(const PnlMat *past, double t, double &prix, double &ic) {}

AbstractPricer::AbstractPricer() {

}
