#ifndef PROJETPRICERGIT_ABSTRACTPRICER_H
#define PROJETPRICERGIT_ABSTRACTPRICER_H

#include <iostream>
#include <string>
#include "jlparser/parser.hpp"
#include "BasketOption.hpp"
#include "AsianOption.hpp"
#include "PerformanceOption.hpp"

#include "BlackScholesModel.hpp"
#include "Option.hpp"
#include "pnl/pnl_finance.h"
#include "pnl/pnl_vector.h"

class AbstractPricer {

public:

    AbstractPricer();

    BlackScholesModel *mod_; /*! pointeur vers le modèle */
    Option *opt_; /*! pointeur sur l'option */
    RandomGenerator *rng_; /*! pointeur sur le générateur */
    double fdStep_; /*! pas de différence finie */
    size_t nbSamples_; /*! nombre de tirages Monte Carlo */

    int H_;

    AbstractPricer(char *fileName);
    AbstractPricer(BlackScholesModel *model, Option *option, RandomGenerator *rng, double d, size_t i, int H);

    virtual void price(double &prix, double &ic);

    /**
     * Calcule le prix de l'option à la date t
     *
     * @param[in]  past contient la trajectoire du sous-jacent
     * jusqu'à l'instant t
     * @param[in] t date à laquelle le calcul est fait
     * @param[out] prix contient le prix
     * @param[out] ic contient la largeur de l'intervalle
     * de confiance sur le calcul du prix
     */
    virtual void price(const PnlMat *past, double t, double &prix, double &ic);

    /**
     * Calcule le delta de l'option à la date t
     *
     * @param[in] past contient la trajectoire du sous-jacent
     * jusqu'à l'instant t
     * @param[in] t date à laquelle le calcul est fait
     * @param[out] delta contient le vecteur de delta
     * de confiance sur le calcul du delta
     */
    virtual void delta(const PnlMat *past, double t, PnlVect *delta, PnlVect *icDelta);

    virtual ~AbstractPricer();
};


#endif //PROJETPRICERGIT_ABSTRACTPRICER_H
