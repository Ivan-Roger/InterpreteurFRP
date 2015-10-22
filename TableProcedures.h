/* 
 * File:   TableProcedures.h
 * Author: rogeri
 *
 * Created on 22 octobre 2015, 15:51
 */

#ifndef TABLEPROCEDURES_H
#define	TABLEPROCEDURES_H

#include "SymboleValue.h"
#include "ArbreAbstrait.h"
#include <hash_map>
#include <iostream>
using namespace std;

class TableProcedures {
public:
    TableProcedures(); // Construit une table vide de pointeurs sur des symboles valués
    NoeudProcedure* chercheAjoute(const Symbole & symbole);
    // si symbole est identique à un symbole valué déjà présent dans la table,
    // on renvoie un pointeur sur ce symbole valué
    // Sinon on insère un nouveau symbole valué correspondant à symbole
    // et on renvoie un pointeur sur le nouveau symbole valué inséré

    inline unsigned int getTaille() const {
        return m_table.size();
    } // Taille de la table des symboles valués

    inline const NoeudProcedure & operator[](std::string i) const {
        return *m_table[i];
    } // accès au ième SymboleValue de la table
    friend ostream & operator<<(ostream & cout, const TableProcedures & ts); // affiche ts sur cout

private:
    std::map<std::string,NoeudProcedure*> m_table; // La table des procedures, triée sur la chaine
};

#endif	/* TABLEPROCEDURES_H */

