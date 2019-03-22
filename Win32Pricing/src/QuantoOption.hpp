#include "Option.hpp"

/// \brief Option Quanto h�rite de la classe abstraite option
class QuantoOption : public Option
{
public:

	/*! Prix d'exercice de l'option Quanto */
	double strike_;

	/**
	* Constructeur de la classe
	* @param[in]  T : maturit�
	* @param[in]  nbTimeSteps : nombre de pas de temps de discr�tisation
	* @param[in]  size : dimension du mod�le
	* @param[in] weights : poids des actifs
	* @param[in]  strike : prix d'exercice de l'option
	*/
	QuantoOption(double T, int nbTimeSteps, int size, PnlVect* weights, double strike);


	/**
	 * Calcule la valeur du payoff sur la trajectoire
	 *
	 * @param[in] path est une matrice de taille (N+1) x d
	 * contenant une trajectoire du mod�le telle que cr��e
	 * par la fonction asset.
	 * @return phi(trajectoire)
	 */
	double payoff(const PnlMat *path) override;

	/**
	*Calcul du prix exacte par la formule ferm�e
	*@param[in] Le modele de black-scholes
	*/
	double prixExact0(BlackScholesModel *bs, double rho);
};
