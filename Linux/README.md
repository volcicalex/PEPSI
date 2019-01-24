# PEPSL  
  
Compilation:  
  
mkdir build  
cd build  
cmake3 -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_CXX_FLAGS=-pg -DCMAKE_PREFIX_PATH=/matieres/5MMPMP6/pnl ..  
  
Execution:  
  
- Calcul du prix et des delta en zéro avec les intervalles de confiance correspondant et le temps de calcul : ./pricer [data_input]  
- Calcul du P& L : ./pricer -c [market_file] [data_input]  
  
Profiling:  
- Lancer l'éxecutable [nom_executable]. Cela crée un fichier gmon.out.  
- Lancer la commande gprof ./nom_executable gmon.out > profile.txt  
- Ouvrir le fichier profile.txt  

Memory test:  
valgrind --leak-check=full ./[nom_executable]  
