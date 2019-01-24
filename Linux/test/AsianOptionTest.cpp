#include <gtest/gtest.h>
#include "../src/AsianOption.hpp"
#include <iostream>
#include <string>
#include "jlparser/parser.hpp"
#include <gtest/gtest.h>
#include "../src/BlackScholesModel.hpp"
#include "../src/MonteCarlo.hpp"


using namespace std;

class AsianOptionTest : public ::testing::Test
{
  protected:
    virtual void SetUp()
    {
        // Code lauched before test starts.
    }

    virtual void TearDown()
    {
        // Code here will be called immediately after each test
        // (right before the destructor).
    }
};

TEST_F(AsianOptionTest, Payoff)
{
    cout << "\n\n\n";
    cout << "┌─────────────────────────────────────────────────────┐ \n";
    cout << "│   Test : Payoff d'une option Asiatique              │ \n";
    cout << "└─────────────────────────────────────────────────────┘ \n";

}

TEST_F(AsianOptionTest, CopyConstructor)
{
    cout << "\n\n\n";
    cout << "┌─────────────────────────────────────────────────────┐ \n";
    cout << "│   Test : Constructeur par copie                     │ \n";
    cout << "└─────────────────────────────────────────────────────┘ \n";


}

TEST_F(AsianOptionTest, Eq)
{
    cout << "\n\n\n";
    cout << "┌─────────────────────────────────────────────────────┐ \n";
    cout << "│   Test : operateur d'affectation =                  │ \n";
    cout << "└─────────────────────────────────────────────────────┘ \n";

    
}


int main(int argc, char **argv)
{
    cout << "\n\n\n";
    cout << "┌──────────────────────────────────────────────────────────────────────────────────────────────────────────┐ \n";
    cout << "│                                Tests :Option Asiatique                                                   │ \n";
    cout << "└──────────────────────────────────────────────────────────────────────────────────────────────────────────┘ \n";

    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
