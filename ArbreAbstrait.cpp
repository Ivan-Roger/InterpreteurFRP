#include <stdlib.h>
#include <typeinfo>
#include "ArbreAbstrait.h"
#include "Symbole.h"
#include "SymboleValue.h"
#include "Exceptions.h"

bool replace(std::string& str, const std::string& from, const std::string& to) {
    size_t start_pos = str.find(from);
    if (start_pos == std::string::npos)
        return false;
    str.replace(start_pos, from.length(), to);
    return true;
}

////////////////////////////////////////////////////////////////////////////////
// NoeudSeqInst
////////////////////////////////////////////////////////////////////////////////

NoeudSeqInst::NoeudSeqInst() : m_instructions() {
}

int NoeudSeqInst::executer() {
    for (unsigned int i = 0; i < m_instructions.size(); i++)
        m_instructions[i]->executer(); // on exécute chaque instruction de la séquence
    return 0; // La valeur renvoyée ne représente rien !
}

void NoeudSeqInst::ajoute(Noeud* instruction) {
    if (instruction != nullptr) m_instructions.push_back(instruction);
}

////////////////////////////////////////////////////////////////////////////////
// NoeudAffectation
////////////////////////////////////////////////////////////////////////////////

NoeudAffectation::NoeudAffectation(Noeud* variable, Noeud* expression)
: m_variable(variable), m_expression(expression) {
}

int NoeudAffectation::executer() {
    int valeur = m_expression->executer(); // On exécute (évalue) l'expression
    ((SymboleValue*) m_variable)->setValeur(valeur); // On affecte la variable
    return 0; // La valeur renvoyée ne représente rien !
}

////////////////////////////////////////////////////////////////////////////////
// NoeudOperateurBinaire
////////////////////////////////////////////////////////////////////////////////

NoeudOperateurBinaire::NoeudOperateurBinaire(Symbole operateur, Noeud* operandeGauche, Noeud* operandeDroit)
: m_operateur(operateur), m_operandeGauche(operandeGauche), m_operandeDroit(operandeDroit) {
}

int NoeudOperateurBinaire::executer() {
    int og, od, valeur;
    if (m_operandeGauche != nullptr) og = m_operandeGauche->executer(); // On évalue l'opérande gauche
    if (m_operandeDroit != nullptr) od = m_operandeDroit->executer(); // On évalue l'opérande droit
    // Et on combine les deux opérandes en fonctions de l'opérateur
    if (this->m_operateur == "+") valeur = (og + od);
    else if (this->m_operateur == "-") valeur = (og - od);
    else if (this->m_operateur == "*") valeur = (og * od);
    else if (this->m_operateur == "==") valeur = (og == od);
    else if (this->m_operateur == "!=") valeur = (og != od);
    else if (this->m_operateur == "<") valeur = (og < od);
    else if (this->m_operateur == ">") valeur = (og > od);
    else if (this->m_operateur == "<=") valeur = (og <= od);
    else if (this->m_operateur == ">=") valeur = (og >= od);
    else if (this->m_operateur == "et") valeur = (og && od);
    else if (this->m_operateur == "ou") valeur = (og || od);
    else if (this->m_operateur == "non") valeur = (!og);
    else if (this->m_operateur == "/") {
        if (od == 0) throw DivParZeroException();
        valeur = og / od;
    }
    return valeur; // On retourne la valeur calculée
}

////////////////////////////////////////////////////////////////////////////////
// NoeudInstSi
////////////////////////////////////////////////////////////////////////////////

NoeudInstSi::NoeudInstSi(Noeud* condition, Noeud* seqIf, std::vector<std::pair<Noeud*, Noeud*>>*vpElseIf, Noeud* seqElse)
: m_condition(condition), m_seqIf(seqIf), m_vpElseIf(vpElseIf), m_seqElse(seqElse) {
}

int NoeudInstSi::executer() {
    if (m_condition->executer()) m_seqIf->executer();
    else {
        bool done = false;
        if (m_vpElseIf != nullptr) {
            bool loop = true;
            int i = 0;
            while (loop) {
                if (i < m_vpElseIf->size()) {
                    if ((*m_vpElseIf)[i].first->executer()) {
                        (*m_vpElseIf)[i].second->executer();
                        done = true;
                        loop = false;
                    }
                } else loop = false;
                i++;
            }
        }
        if (m_seqElse != nullptr && !done) {
            m_seqElse->executer();
        }
    }
    return 0; // La valeur renvoyée ne représente rien !
}

////////////////////////////////////////////////////////////////////////////////
// NoeudInstTantQue
////////////////////////////////////////////////////////////////////////////////

NoeudInstTantQue::NoeudInstTantQue(Noeud* condition, Noeud* sequence)
: m_condition(condition), m_sequence(sequence) {
}

int NoeudInstTantQue::executer() {
    while (m_condition->executer()) {
        m_sequence->executer();
    }
    return 0; // La valeur renvoyée ne représente rien !
}

////////////////////////////////////////////////////////////////////////////////
// NoeudInstRepeter
////////////////////////////////////////////////////////////////////////////////

NoeudInstRepeter::NoeudInstRepeter(Noeud* condition, Noeud* sequence)
: m_condition(condition), m_sequence(sequence) {
}

int NoeudInstRepeter::executer() {
    do {
        m_sequence->executer();
    } while (m_condition->executer());
    return 0; // La valeur renvoyée ne représente rien !
}

////////////////////////////////////////////////////////////////////////////////
// NoeudInstPour
////////////////////////////////////////////////////////////////////////////////

NoeudInstPour::NoeudInstPour(Noeud* condition, Noeud* sequence, Noeud* init, Noeud* inc)
: m_condition(condition), m_sequence(sequence), m_init(init), m_inc(inc) {
}

int NoeudInstPour::executer() {
    if (m_init != nullptr) m_init->executer();
    while (m_condition->executer()) {
        m_sequence->executer();
        if (m_inc != nullptr) m_inc->executer();
    }
    return 0; // La valeur renvoyée ne représente rien !
}

////////////////////////////////////////////////////////////////////////////////
// NoeudInstEcrire
////////////////////////////////////////////////////////////////////////////////

NoeudInstEcrire::NoeudInstEcrire() {
}

int NoeudInstEcrire::executer() {
    for (int i = 0; i < m_expressions.size(); i++) {
        Noeud* p = m_expressions[i];
        if (typeid (*p) == typeid (SymboleValue)) {
            SymboleValue* r = ((SymboleValue*) p);
            if (*r == "<CHAINE>") {
                string str = r->getChaine().substr(1, r->getChaine().size() - 2);
                replace(str, string("\\n"), string("\n"));
                replace(str, string("\\r"), string("\r"));
                replace(str, string("\\t"), string("\t"));
                cout << str;
            } else cout << r->executer();
        } else {
            cout << p->executer();
        }
    }
    return 0; // La valeur renvoyée ne représente rien !
}

void NoeudInstEcrire::ajoute(Noeud* expression) {
    m_expressions.push_back(expression);
}

////////////////////////////////////////////////////////////////////////////////
// NoeudInstLire
////////////////////////////////////////////////////////////////////////////////

NoeudInstLire::NoeudInstLire(Noeud* var) {
    m_var.push_back(var);
}

int NoeudInstLire::executer() {
    for (int i = 0; i < m_var.size(); i++) {
        int tmp;
        cin >> tmp;
        ((SymboleValue*) m_var[i])->setValeur(tmp);
    }
    return 0; // La valeur renvoyée ne représente rien !
}

void NoeudInstLire::ajoute(Noeud* var) {
    m_var.push_back(var);
}