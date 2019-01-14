#include "Option.hpp"

/// \brief Option Mementis hérite de la classe abstraite option
class FCPMementis : public Option
{
public:
	/* Valeur liquidative d'origine */
	double VLO_;

	/* Point d'entrée */
	float PE_;

	/* Vecteur de performances */
	PnlVect *performances_;

	/* Vecteur de performances plafonnees */
	PnlVect *performances_plaf_;

	/* Vecteur de dividendes */
	PnlVect *dividendes_;

	/**
	* Constructeur de la classe
	*/
	FCPMementis(int nbTimeSteps);

	/**
	* Remplit le vecteur de performances
	*/
	void fill_performances(const PnlMat *path);

	/**
	* Remplit le vecteur de performances plafonnees
	*/
	void fill_performances_plaf(const PnlMat *path);

	/**
	 * Calcule le point d'entrée la trajectoire avec le vecteur de performances
	 */
	void PE();

	/**
	 * Remplit le vecteur de dividendes pour chaque année
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
