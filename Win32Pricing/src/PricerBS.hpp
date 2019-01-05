//
// Created by Yasmine Tidane on 21/09/2018.
//

#ifndef PROJETPRICERGIT_PRICERBS_HPP
#define PROJETPRICERGIT_PRICERBS_HPP


#include "AbstractPricer.hpp"

class PricerBS : public AbstractPricer
{
public:


    ~PricerBS();
    PricerBS(char *fileName);
    PricerBS(BlackScholesModel *model, Option *option, RandomGenerator *rng, double fdStep, size_t nbSamples, int H);

    void price(double &prix, double &ic);

    void price(const PnlMat *past, double t, double &prix, double &ic);

    void delta(const PnlMat *past, double t, PnlVect *delta, PnlVect *icDeltas);

};


#endif //PROJETPRICERGIT_PRICERBS_HPP
