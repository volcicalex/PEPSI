#include "Option.hpp"

/// \brief Option Mementis hérite de la classe abstraite option
class FCPMementis : public Option
{
public:
	/* Valeur liquidative d'origine */
	double VLO_;

	/*! nombre de marchés */
	int nbMarkets_;

	/*! nombre d'actifs par marché */
	PnlVectInt *nbAssetsPerMarket_;

	/*! vecteur des tendances */
	PnlVect *trend_;

	/* Point d'entrée */
	float PE_;

	/* Taux de capitalisation des dividendes */
	double taux_capitalisation_;

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
	* Remplit le vecteur de performances plafonnees sachant que path prend en compte les taux de change.
	*/
	void fill_performances_plaf_CR(const PnlMat *path);

	/**
	 * Calcule le point d'entrée la trajectoire avec le vecteur de performances
	 */
	void PE();

	/**
	 * Remplit le vecteur de dividendes pour chaque année
	 */
	void fill_dividendes(const PnlMat *path);
	
	/**
	* Calcule la valeur de la plus value sur la trajectoire
	*
	* @param[in] path est une matrice de taille (N+1) x d
	* contenant une trajectoire du modèle telle que créée
	* par la fonction asset.
	* @return plus value
	*/
	double calcul_plus_value(const PnlMat *path);

	/**
	* Calcule la valeur du gain total de l'investisseur sur la trajectoire
	*
	* @param[in] path est une matrice de taille (N+1) x d
	* contenant une trajectoire du modèle telle que créée
	* par la fonction asset.
	* @return remboursement comprenant dividendes
	*/
	double payoff(const PnlMat *path) override;

	/**
	 * Calcule la valeur du remboursement sur la trajectoire
	 *
	 * @param[in] path est une matrice de taille (N+1) x d
	 * contenant une trajectoire du modèle telle que créée
	 * par la fonction asset.
	 * @return phi(trajectoire)
	 */
	double remboursement_echeance(const PnlMat *path);

	/*Change Rate Functions*/
	double payoff_CR(const PnlMat *path);
	double remboursement_echeance_CR(const PnlMat *path);
	double calcul_plus_value_CR(const PnlMat *path);
	void fill_dividendes_CR(const PnlMat *path);
	void fill_performances_CR(const PnlMat *path);
};
