//
// Created by Yasmine Tidane on 21/09/2018.
//

#include "Couverture.hpp"

Couverture::Couverture(AbstractPricer *pricer)
{
    pricer_ = pricer;
}

Couverture::~Couverture() {
    delete pricer_;
}

void Couverture::profits_and_losses(const PnlMat *market_trajectory, double &p_and_l, double &pl_sur_P0)
{

    // Nombre d'observations du marché
    int H = market_trajectory->m - 1;
    // Nombre de sous-jacents
    int nbAssets = market_trajectory->n;
    // Vecteur des prix initiaux
    PnlVect *spots = pnl_vect_create_from_scalar(nbAssets, 0);
    pnl_vect_clone(spots, pricer_->mod_->spot_);
    // Matrice représentant tout les prix passés
    PnlMat *past = pnl_mat_create_from_scalar(1, nbAssets, 0);
    // Vecteur des intervalles de confiance
    PnlVect *ics = pnl_vect_create_from_scalar(pricer_->opt_->size_, 0); //pour les deltas
    // Vecteur utilisés pour le calcul et sauvegarde des deltas 
    PnlVect *diff_delta = pnl_vect_create_from_scalar(nbAssets, 0);
    PnlVect *prev_delta = pnl_vect_create_from_scalar(nbAssets, 0);
    PnlVect *deltas = pnl_vect_create_from_scalar(nbAssets, 0);
    PnlVect *tmp_row = pnl_vect_create_from_scalar(nbAssets, 0);
    pnl_mat_get_row(tmp_row, market_trajectory, 0);
    pnl_mat_set_row(past, tmp_row, 0);
    // Prix + intervalle de confiance
    double prix = 0;
    double ic = 0;
    double v = 0;
    pricer_->price(prix, ic);
    double prix0 = prix;
    pricer_->delta(past, 0, deltas, ics);
    //calcul de V(0)
    v = prix - pnl_vect_scalar_prod(deltas, spots);
    // Le pas de rebalancement (calcul du delta)
    double step_for_delta = pricer_->opt_->T_ / H;  // T/H
    // Constante d'actualisation
    double actualisationFactor = exp(pricer_->mod_->r_ * step_for_delta);
    // Pas de constatation du flux
    int step_for_payoff = H/pricer_->opt_->nbTimeSteps_; // H/N
    for (int i = 1; i<=H; i++){
        //Mise à jour de la matrice past      
        pnl_mat_get_row(tmp_row, market_trajectory, i);
        pnl_mat_add_row(past, past->m, tmp_row);       
        // diff_delta = delta(i) - delta(i-1)
        pnl_vect_clone(prev_delta, deltas);
        pricer_->delta(past, i*step_for_delta, deltas, ics);
        pnl_vect_clone(diff_delta, deltas);
        pnl_vect_minus_vect(diff_delta, prev_delta);
        //spots = S(tho_i)
        pnl_mat_get_row(spots, market_trajectory, i);
        // v = V(i-1) * exp(rT/H) - (delta(i) - delta(i-1)) * S(tho_i)
        v = v * actualisationFactor - pnl_vect_scalar_prod(diff_delta, spots);
        if ((i%step_for_payoff != 0) && (i !=H) ){          
            pnl_mat_del_row(past, past->m - 1, tmp_row);
        }
    }   
    p_and_l = v + pnl_vect_scalar_prod(deltas, spots) - pricer_->opt_->payoff(past);
    pl_sur_P0 = p_and_l/prix0;   
    //Free
    pnl_vect_free(&spots);
    pnl_vect_free(&ics);
    pnl_vect_free(&diff_delta);
    pnl_vect_free(&prev_delta);
    pnl_vect_free(&deltas);
    pnl_vect_free(&tmp_row);  
    pnl_mat_free(&past);
}
