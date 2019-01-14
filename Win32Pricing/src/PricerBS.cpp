#include "PricerBS.hpp"
#include "pnl/pnl_finance.h"
#include "pnl/pnl_vector.h"

PricerBS::PricerBS(char *fileName) : AbstractPricer(fileName)
{
}

PricerBS::PricerBS(BlackScholesModel *model, Option *option, RandomGenerator *rng, double fdStep, size_t nbSamples, int H) : AbstractPricer(model, option, rng, fdStep, nbSamples, H)
{
}

PricerBS::~PricerBS()
{
    delete mod_;
    delete opt_;
}

void PricerBS::price(double &prix, double &ic)
{
    double spot = pnl_vect_get(mod_->spot_, 0);
    double sigma = pnl_vect_get(mod_->sigma_, 0);
    prix = pnl_bs_call(spot, ((BasketOption*)opt_)->strike_, opt_->T_, mod_->r_, 0, sigma);
}

void PricerBS::price(const PnlMat *past, double t, double &prix, double &ic)
{
    int size_past = past->m;
    double S0 = pnl_mat_get(past, size_past-1, 0);
    double K = ((BasketOption*)opt_)->strike_;
    double r = mod_->r_;
    double sigma = pnl_vect_get(mod_->sigma_, 0);
    double T = opt_->T_;

    double d1 = 1.0/(sigma*sqrt(T-t)) * (log(S0/K) + (r + sigma*sigma/2)*(T-t));
    double d2 = d1 - (sigma*sqrt(T-t));

//    prix = S0 * pnl_cdfnor(d1) - K * exp(-r*(T-t)) * pnl_cdfnor(d2);


    prix = pnl_bs_call(S0, ((BasketOption*)opt_)->strike_, opt_->T_-t, mod_->r_, 0, sigma);
}

void PricerBS::delta(const PnlMat *past, double t, PnlVect *delta, PnlVect* icDelta)
{
    int size_past = past->m;
    double S0 = pnl_mat_get(past, size_past-1, 0);
    double K = ((BasketOption*)opt_)->strike_;
    double r = mod_->r_;
    double sigma = pnl_vect_get(mod_->sigma_, 0);
    double T = opt_->T_;

    double prix;
    double delta_double;

    pnl_cf_call_bs(S0, ((BasketOption*)opt_)->strike_, opt_->T_-t, mod_->r_, 0, sigma, &prix, &delta_double);

    pnl_vect_set(delta, 0, delta_double);
}