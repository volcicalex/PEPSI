//
// Created by PEPSI on 9/18/18.
//

#include "Option.hpp"

/// \brief Option Asiatique hérite de la classe abstraite option
class FCPMementis : public Option
{
public:
	/* Valeur liquidative d'origine */
	double VLO_;

	/* Point d'entrée */
	double PE_;

	/* Vecteur de performances */
	PnlVect *performances_;

	/* Vecteur de dividendes */
	PnlVect *dividendes_;

	/**
	* Constructeur de la classe
	* @param[in]  T : maturité
	* @param[in]  nbTimeSteps : nombre de pas de temps de discrétisation
	* @param[in]  size : dimension du modèle
	* @param[in] weights : poids des actifs
	* @param[in]  strike : prix d'exercice de l'option
	*/
	FCPMementis();

	/**
	* Remplit le vecteur de performances
	*
	* @return phi(trajectoire)
	*/
	void fill_performances(const PnlMat *path);

	/**
	 * Calcule le point d'entrée la trajectoire avec le vecteur de performances
	 *
	 * @return phi(trajectoire)
	 */
	void PE();

	/**
	 * Remplit le vecteur de dividendes pour chaque année
	 *
	 * @return phi(trajectoire)
	 */
	void fill_dividendes(const PnlMat *path);

	/**
	 * Calcule la valeur du remboursement sur la trajectoire
	 *
	 * @param[in] path est une matrice de taille (N+1) x d
	 * contenant une trajectoire du modèle telle que créée
	 * par la fonction asset.
	 * @return phi(trajectoire)
	 */
	double payoff(const PnlMat *path) override;


};
