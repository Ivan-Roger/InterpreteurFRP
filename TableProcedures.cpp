/* 
 * File:   TableProcedures.cpp
 * Author: rogeri
 * 
 * Created on 22 octobre 2015, 15:51
 */

#include <map>

#include "TableProcedures.h"

TableProcedures::TableProcedures() : m_table() {
}

NoeudProcedure * TableProcedures::chercheAjoute(const Symbole & s)
// si s est identique à une procedure déjà présente dans la table,
// on renvoie un pointeur sur cette procedure.
// Sinon, on insère une nouvelle procedure correspondant à s
// et on renvoie un pointeur sur la nouvelle procedure inséré.
{
    NoeudProcedure* res = m_table[s.getChaine()];
    if (res==nullptr) {
        m_table.insert(pair<string,NoeudProcedure*>(s.getChaine(),new NoeudProcedure()));
    }
}

ostream & operator<<(ostream & cout, const TableSymboles & ts)
// affiche ts sur cout
{
    cout << endl << "Contenu de la Table des Symboles Values :" << endl
            << "---------------------------------------" << endl << endl;
    for (unsigned int i = 0; i < ts.getTaille(); i++) {
        if (ts[i]=="<VARIABLE>")
            cout << "   " << i << ") " << ts[i] << endl;
    }
    cout << endl;
    return cout;
}
