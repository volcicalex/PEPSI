 #include <iostream>
#include <string>
#include <stdio.h>
#include <ctime>
#include <string.h>
#include "jlparser/parser.hpp"
#include "pnl/pnl_random.h"
#include "pnl/pnl_vector.h"
#include "Model.hpp"
#include "MonteCarlo.hpp"
#include "BlackScholesModel.hpp"
#include "Option.hpp"
#include "BasketOption.hpp"
#include "PerformanceOption.hpp"
#include "AsianOption.hpp"
#include "CallOption.hpp"




using namespace std;

int main(int argc, char **argv)
{
    const char * call_ = "call";
    const char * perf_ = "perf";
    const char * basket_ = "basket";
    const char * asian_ = "asian";

    double T, r, strike;
    PnlVect *spot, *sigma, *divid, *payoffCoeff;
    double correlation;
    string type;
    int size;
    int timestep;
    size_t n_samples;
    double price = 0 ;
    double ic = 0;

    char *data_input;
    char *market_file_input;

    MonteCarlo *monteCarlo;

  if (argc==2)
   {

      char *data_input = argv[1];
      Param *P = new Parser( data_input);

      P->extract("option type", type);
      P->extract("correlation", correlation);
      P->extract("maturity", T);
      P->extract("timestep number", timestep);
      P->extract("option size", size);
      P->extract("spot", spot, size);
      P->extract("volatility", sigma, size);
      P->extract("payoff coefficients", payoffCoeff, size);
      P->extract("interest rate", r);

    if (P->extract("dividend rate", divid, size, true) == false)
    {
        divid = pnl_vect_create_from_zero(size);
    }
    P->extract("sample number", n_samples);

    PnlMat *rho_vect = pnl_mat_create_from_scalar(size, size, correlation);
  	pnl_mat_set_diag(rho_vect, 1, 0);

    Model *bs = new BlackScholesModel(size, r, rho_vect, sigma, spot, spot);

    PnlRng *rng = pnl_rng_create(PNL_RNG_KNUTH);
    pnl_rng_sseed(rng, time(NULL));

    PnlMat *past = pnl_mat_create_from_scalar(1, size, pnl_vect_get(spot, 0));
    PnlVect *delta = pnl_vect_create(size);
    PnlVect *ic_vect = pnl_vect_create(size);

 //OPTION BASKET

    clock_t start = clock();

     if (strstr(data_input, basket_ )){

       P->extract("strike", strike);

       Option *basket = new BasketOption(T, timestep, size, payoffCoeff, strike);
       monteCarlo = new MonteCarlo(bs, basket, rng, 0.1, n_samples);

       monteCarlo->price( price, ic);
       monteCarlo->delta(past, 0, delta, ic_vect);

       cout<<"prix: " << price <<endl;
       cout<<"intervalle_confinace: " << ic <<endl;
       cout << "delta: \n";
       pnl_vect_print_asrow(delta);

       std::cout << "Time: " << (std::clock() - start) / (double)(CLOCKS_PER_SEC / 1000) << " ms" << std::endl;


     }else if (strstr(data_input, perf_ )) {
       Option *perf = new PerformanceOption(T, timestep, size, payoffCoeff);
        monteCarlo = new MonteCarlo(bs, perf, rng, 0.1, n_samples);
       monteCarlo->price( price, ic);
       monteCarlo->delta(past, 0, delta, ic_vect);

       cout<<"prix: " << price <<endl;
      cout<<"intervalle_confinace: " << ic <<endl;

       cout << "delta: \n";
       pnl_vect_print_asrow(delta);
       cout << " IC_vecteur : \n";
       pnl_vect_print_asrow(ic_vect);
       std::cout << "Time: " << (std::clock() - start) / (double)(CLOCKS_PER_SEC / 1000) << " ms" << std::endl;

     }else if (strstr(data_input, call_ )) {


      P->extract("strike", strike);

       Option *call = new CallOption(T, timestep, size, payoffCoeff,strike);
       monteCarlo = new MonteCarlo(bs, call, rng, 0.1, n_samples);

       monteCarlo->price( price, ic);
       monteCarlo->delta(past, 0, delta, ic_vect);

       cout<<"prix: " << price <<endl;
       cout<<"intervalle_confinace: " << ic <<endl;

       cout << "delta: \n";
       pnl_vect_print_asrow(delta);
       cout << " IC_vecteur : \n";
       pnl_vect_print_asrow(ic_vect);
       std::cout << "Time: " << (std::clock() - start) / (double)(CLOCKS_PER_SEC / 1000) << " ms" << std::endl;

     }else if (strstr(data_input, asian_ )) {


        P->extract("strike", strike);
        Option *asian = new AsianOption(T, timestep, size, payoffCoeff,strike);
         monteCarlo = new MonteCarlo(bs, asian, rng, 0.1, n_samples);
        monteCarlo->price( price, ic);
        monteCarlo->delta(past, 0, delta, ic_vect);



        cout<<"prix: " << price <<endl;
        cout<<"intervalle_confinace: " << ic <<endl;

        cout << "delta: \n";
        pnl_vect_print_asrow(delta);
        cout << " IC_vecteur : \n";
        pnl_vect_print_asrow(ic_vect);
        std::cout << "Time: " << (std::clock() - start) / (double)(CLOCKS_PER_SEC / 1000) << " ms" << std::endl;

     }

     //pnl_rng_free(&rng);
     pnl_mat_free(&past);
     pnl_vect_free(&delta);
     pnl_vect_free(&ic_vect);
     pnl_vect_free(&divid);
     //pnl_vect_free(&spot);
     //pnl_vect_free(&sigma);
     //pnl_vect_free(&payoffCoeff);

     delete P;


   } else if(argc==4){
      if (!strcmp(argv[1],"-c")) {
          char *market_file_input = argv[2];
          char *data_input=argv[3];



           Param *P = new Parser(data_input);
           PnlMat *history = pnl_mat_create_from_file(market_file_input);

           P->extract("option type", type);
           P->extract("correlation", correlation);
           P->extract("maturity", T);
           P->extract("timestep number", timestep);
           P->extract("option size", size);
           P->extract("spot", spot, size);
           P->extract("volatility", sigma, size);
           P->extract("payoff coefficients", payoffCoeff, size);
           P->extract("interest rate", r);

         if (P->extract("dividend rate", divid, size, true) == false)
         {
             divid = pnl_vect_create_from_zero(size);
         }
         P->extract("sample number", n_samples);

         PnlMat *rho_vect = pnl_mat_create_from_scalar(size, size, correlation);
         pnl_mat_set_diag(rho_vect, 1, 0);

         Model *bs = new BlackScholesModel(size, r, rho_vect, sigma, spot, spot);

         PnlRng *rng = pnl_rng_create(PNL_RNG_KNUTH);
         pnl_rng_sseed(rng, time(NULL));


        PnlVect *delta = pnl_vect_new();
        PnlVect *ic_vect = pnl_vect_new();
        clock_t start = clock();


          if (strstr(data_input, call_ ) ){

            P->extract("strike", strike);

            Option *call = new CallOption(T, timestep, size, payoffCoeff,strike);
             monteCarlo = new MonteCarlo(bs, call, rng, 0.1, n_samples);

             double d;
             monteCarlo->pnl(d,history);
             cout << "P&L =============== "  << d << endl;

             std::cout << "Time: " << (std::clock() - start) / (double)(CLOCKS_PER_SEC / 1000) << " ms" << std::endl;

          }else if (strstr(data_input, perf_ )) {


            Option *perf = new PerformanceOption(T, timestep, size, payoffCoeff);
             monteCarlo = new MonteCarlo(bs, perf, rng, 0.1, n_samples);

            double d;
            monteCarlo->pnl(d,history);
            cout << "P&L =============== "  << d << endl;

            std::cout << "Time: " << (std::clock() - start) / (double)(CLOCKS_PER_SEC / 1000) << " ms" << std::endl;

          }else if (strstr(data_input, basket_ )) {

            P->extract("strike", strike);

            Option *basket = new BasketOption(T, timestep, size, payoffCoeff, strike);
             monteCarlo = new MonteCarlo(bs, basket, rng, 0.1, n_samples);


            double d;
            monteCarlo->pnl(d,history);
            cout << "P&L =============== "  << d << endl;

            std::cout << "Time: " << (std::clock() - start) / (double)(CLOCKS_PER_SEC / 1000) << " ms" << std::endl;

          }else if (strstr(data_input, asian_ )) {

            P->extract("strike", strike);

            Option *asian = new AsianOption(T, timestep, size, payoffCoeff,strike);
             monteCarlo = new MonteCarlo(bs, asian, rng, 0.1, n_samples);


             double d;
             monteCarlo->pnl(d,history);
             cout << "P&L =============== "  << d << endl;

            std::cout << "Time: " << (std::clock() - start) / (double)(CLOCKS_PER_SEC / 1000) << " ms" << std::endl;

          }

      } else {
        cout<<"Veuillez entrer deux arguments "<<endl;
      }


   } else {
     cout<<" veuillez entrer un fichier "<<endl;
   }

    delete monteCarlo;
    exit(0);
}
