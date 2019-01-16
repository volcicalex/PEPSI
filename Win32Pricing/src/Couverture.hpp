#include "MonteCarlo.hpp"

class Couverture {
public:

    MonteCarlo *pricer_;
    int H_;

    Couverture(MonteCarlo *pricer_);

    ~Couverture();

    /**
     * Calcule le P&L a partir d'une trajectoire de marché donnée
     *
     * @param[in] market_trajectory contient la trajectoire du sous-jacent
     * sur une grille de temps 0 à T
     * @param[out] p_and_l  contient l'erreur de couverture
     */
    void profits_and_losses(const PnlMat *market_trajectory, double &p_and_l, double &pl_sur_P0);

};
