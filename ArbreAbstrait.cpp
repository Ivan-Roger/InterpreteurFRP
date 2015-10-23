#include "ArbreAbstrait.h"

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

void NoeudSeqInst::translate(std::ostream& out, int offset) {
    string prefix;
    for(int i=0;i<offset;i++)
        prefix.append("\t");
    for (int i=0; i<m_instructions.size(); i++) {
        Noeud* r = m_instructions[i];
        out << prefix;
        r->translate(out,offset);
        out << ";";
        out << endl;
    }
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

void NoeudAffectation::translate(std::ostream& out, int offset) {
    out << ((SymboleValue*)m_variable)->getChaine() << " = (";
    m_expression->translate(out,offset);
    out << ")";
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
    else if (this->m_operateur == "++") {
        if (m_operandeGauche != nullptr) {
            valeur = og;
            ((SymboleValue*)m_operandeGauche)->setValeur(og+1);
        } else if (m_operandeDroit != nullptr) {
            ((SymboleValue*)m_operandeGauche)->setValeur(og+1);
            valeur = og;
        }
    } else if (this->m_operateur == "--") {
        if (m_operandeGauche != nullptr) {
            valeur = og;
            ((SymboleValue*)m_operandeGauche)->setValeur(og-1);
        } else if (m_operandeDroit != nullptr) {
            ((SymboleValue*)m_operandeGauche)->setValeur(og-1);
            valeur = og;
        }
    } else if (this->m_operateur == "/") {
        if (od == 0) throw DivParZeroException();
        valeur = og / od;
    }
    return valeur; // On retourne la valeur calculée
}

void NoeudOperateurBinaire::translate(std::ostream& out, int offset) {
    string op;
    if (m_operateur.getChaine() == "et") op = "&&";
    else if (m_operateur.getChaine() == "ou") op = "||";
    else if (m_operateur.getChaine() == "non") op = "!";
    else op = m_operateur.getChaine();
    if (m_operandeGauche!=nullptr)
        out << ((SymboleValue*)m_operandeGauche)->getChaine();
    out << op;
    if (m_operandeDroit!=nullptr)
        out << ((SymboleValue*)m_operandeDroit)->getChaine();
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

void NoeudInstSi::translate(std::ostream& out, int offset) {
    string prefix;
    for(int i=0;i<offset;i++)
        prefix.append("\t");
    out << "if (";
    m_condition->translate(out,offset);
    out << ") {" << endl;
    m_seqIf->translate(out,offset+1);
    out << prefix << "}";
    if (m_vpElseIf != nullptr) {
        for (int i=0; i<m_vpElseIf->size(); i++) {
            out << " else if (";
            (*m_vpElseIf)[i].first->translate(out,offset+1);
            out << ") {" << endl;
            (*m_vpElseIf)[i].second->translate(out,offset+1);
            out << prefix << "}";
        }
    }
    if (m_seqElse!=nullptr) {
        out << " else {" << endl;
        m_seqElse->translate(out,offset+1);
        out << prefix << "}";
    }
    out << "//";
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

void NoeudInstTantQue::translate(std::ostream& out, int offset) {
    string prefix;
    for(int i=0;i<offset;i++)
        prefix.append("\t");
    out << "while (";
    m_condition->translate(out,offset);
    out << ") {" << endl;
    m_sequence->translate(out,offset+1);
    out << prefix << "}";
    out << "//";
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

void NoeudInstRepeter::translate(std::ostream& out, int offset) {
    string prefix;
    for(int i=0;i<offset;i++)
        prefix.append("\t");
    out << "do {" << endl;
    m_sequence->translate(out,offset+1);
    out << prefix << "} while (";
    m_condition->translate(out,offset);
    out << ")";
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

void NoeudInstPour::translate(std::ostream& out, int offset) {
    string prefix;
    for(int i=0;i<offset;i++)
        prefix.append("\t");
    out << "for (";
    if (m_init!=nullptr)
        m_init->translate(out,offset);
    out << "; ";
    m_condition->translate(out,offset);
    out << "; ";
    if (m_inc!=nullptr)
        m_inc->translate(out,offset);
    out << ") {" << endl;
    m_sequence->translate(out, offset+1);
    out << prefix << "}";
    out << "//";
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

void NoeudInstEcrire::translate(std::ostream& out, int offset) {
    out << "printf(\"";
    vector<Noeud*> disp;
    for (int i = 0; i < m_expressions.size(); i++) {
        Noeud* p = m_expressions[i];
        if (typeid (*p) == typeid (SymboleValue)) {
            SymboleValue* r = ((SymboleValue*) p);
            if (*r == "<CHAINE>") {
                out << r->getChaine().substr(1, r->getChaine().size() - 2);
            } else {
                out << "%d";
                disp.push_back(((Noeud*)r));
            }
        } else {
            out << "%s";
            disp.push_back(p);
        }
    }
    out << "\"";
    for (int i=0; i<disp.size(); i++) {
        out << ", ";
        disp[i]->translate(out,offset);
    }
    out << ")";
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

void NoeudInstLire::translate(std::ostream& out, int offset) {
    out << "scanf(\"";
    for (int i=0; i<m_var.size(); i++) {
        out << "%d";
    }
    out << "\"";
    for (int i=0; i<m_var.size(); i++) {
        out << ", " << ((SymboleValue*)m_var[i])->getChaine();
    }
    out << ")";
}