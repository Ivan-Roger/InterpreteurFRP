#include <iostream>
using namespace std;
#include "Interpreteur.h"
#include "Exceptions.h"
#include <typeinfo>

int main(int argc, char* argv[]) {
    bool TABLE = true;
    bool CODE = true;
    bool EXEC = true;
    string nomFich("");
    
    // Gestion des arguments
    /*
    cout << "Arguments :";
    for (int i = 1; i < argc; i++) {
        if (argv[i] == "-t")
            TABLE = true;
        else if (argv[i] == "-c")
            CODE = true;
        else if (argv[i] == "-e")
            EXEC = true;
        else {
            nomFich = argv[i];
            cout << "\"" << argv[i] << "\",";
        }
    }
    cout << endl << "Table : " << TABLE << endl << "Code : " << CODE << endl << "Exec : " << EXEC << endl;
    

    if (nomFich=="") {*/
        cout << "Usage : " << argv[0] << " <nom_fichier_source> [-t] [-c] [-e]" << endl << endl;
        cout << "Entrez le nom du fichier que voulez-vous interpréter : ";
        getline(cin, nomFich);
    //}
    ifstream fichier(nomFich.c_str());
    ofstream code("out.c");
    try {
        Interpreteur interpreteur(fichier);
        cout << endl << "================ Analyse Syntaxe" << endl;
        interpreteur.analyse();
        // Si pas d'exception levée, l'analyse syntaxique a réussi
        cout << endl << "================ Syntaxe Correcte" << endl;
        if (TABLE) {
            // On affiche le contenu de la table des symboles avant d'exécuter le programme
            cout << endl << "================ Table des symboles avant exécution : " << endl << interpreteur.getTable();
        }
        if (EXEC) {
            cout << endl << "================ Execution de l'arbre" << endl << endl;
            // On exécute le programme si l'arbre n'est pas vide
            if (interpreteur.getArbre() != nullptr) interpreteur.getArbre()->executer();
        }
        if (TABLE) {
            // Et on vérifie qu'il a fonctionné en regardant comment il a modifié la table des symboles
            cout << endl << endl << "================ Table des symboles apres exécution : " << endl << interpreteur.getTable();
        }
        if (CODE) {
            // On affiche la traduction en C
            cout << endl << "================ Traduction en C" << endl;
            interpreteur.translate(code);
        }
    } catch (InterpreteurException & e) {
        cout << endl << endl << "================ Syntaxe Invalide" << endl;
        cout << endl << e.what() << endl;
    }
    return 0;
    
}
