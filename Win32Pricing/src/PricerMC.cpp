#include "PricerMC.hpp"

PricerMC::PricerMC(char *fileName) : AbstractPricer(fileName)
{
    mc_ = new MonteCarlo(mod_, opt_, rng_, fdStep_, nbSamples_);
}

PricerMC::PricerMC(BlackScholesModel *model, Option *option, RandomGenerator *rng, double fdStep, size_t nbSamples, int H) : AbstractPricer(model, option, rng, fdStep, nbSamples, H)
{
    mc_ = new MonteCarlo(mod_, opt_, rng_, fdStep_, nbSamples_);
}

PricerMC::PricerMC():AbstractPricer()
{
}

PricerMC::~PricerMC()
{
    delete mc_;
}

void PricerMC::price(double &prix, double &ic)
{
    mc_->price(prix, ic);
}

void PricerMC::price(const PnlMat *past, double t, double &prix, double &ic)
{
    mc_->price(past, t, prix, ic);
}

void PricerMC::delta(const PnlMat *past, double t, PnlVect *delta, PnlVect *icDelta)
{
    mc_->delta(past, t, delta, icDelta);
}