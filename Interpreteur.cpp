#include "Interpreteur.h"
#include <stdlib.h>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <typeinfo>
#include <chrono>
using namespace std;

Interpreteur::Interpreteur(ifstream & fichier) : m_lecteur(fichier), m_table(), m_arbre(nullptr), m_erreur(false) {
}

void Interpreteur::analyse() {
    m_arbre = programme(); // on lance l'analyse de la première règle
    if (m_erreur)
        throw *new InterpreteurException;
}

void Interpreteur::translate(std::ostream& out) {
    auto now = std::chrono::system_clock::now();
    auto duration = now.time_since_epoch();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
    out << "// Code generated from FRP Compiler (by Ivan ROGER)" << endl;
    out << "int main () {" << endl;
    out << "\t// Variables :" << endl;
    for (int i=0; i<m_table.getTaille(); i++) {
        if (m_table[i]=="<VARIABLE>")
            out << "\t" << "int " << m_table[i].getChaine() << ";" << endl;
    }
    out << endl << "\t// Code :" << endl;
    m_arbre->translate(out,1);
    out << "}" << endl;
    now = std::chrono::system_clock::now();
    duration = now.time_since_epoch();
    auto ms2 = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
    out << "// Done in " << ms2-ms << "ms." << endl;
}

void Interpreteur::tester(const string & symboleAttendu) const throw (SyntaxeException) {
    // Teste si le symbole courant est égal au symboleAttendu... Si non, lève une exception
    static char messageWhat[256];
    if (m_lecteur.getSymbole() != symboleAttendu) {
        cout << endl;
        sprintf(messageWhat,
                "Ligne %d, Colonne %d - Erreur de syntaxe - Symbole attendu : '%s' - Symbole trouvé : '%s'",
                m_lecteur.getLigne(), m_lecteur.getColonne(),
                symboleAttendu.c_str(), m_lecteur.getSymbole().getChaine().c_str());
        throw SyntaxeException(messageWhat);
    }
}

void Interpreteur::testerEtAvancer(const string & symboleAttendu) throw (SyntaxeException) {
    // Teste si le symbole courant est égal au symboleAttendu... Si oui, avance, Sinon, lève une exception
    tester(symboleAttendu);
    m_lecteur.avancer();
}

void Interpreteur::erreur(const string & message) const throw (SyntaxeException) {
    // Lève une exception contenant le message et le symbole courant trouvé
    // Utilisé lorsqu'il y a plusieurs symboles attendus possibles...
    cout << endl;
    static char messageWhat[256];
    sprintf(messageWhat,
            "Ligne %d, Colonne %d - Erreur de syntaxe - %s - Symbole trouvé : '%s'",
            m_lecteur.getLigne(), m_lecteur.getColonne(), message.c_str(), m_lecteur.getSymbole().getChaine().c_str());
    throw SyntaxeException(messageWhat);
}

////////////////////////////////////////////////////////////////////////////////

Noeud* Interpreteur::programme() {
    // <programme> ::= procedure principale() <seqInst> finproc FIN_FICHIER
    testerEtAvancer("procedure");
    testerEtAvancer("principale");
    testerEtAvancer("(");
    testerEtAvancer(")");
    Noeud* sequence = seqInst();
    testerEtAvancer("finproc");
    tester("<FINDEFICHIER>");
    return sequence;
}

Noeud* Interpreteur::seqInst() {
    // <seqInst> ::= <inst> { <inst> }
    NoeudSeqInst* sequence = new NoeudSeqInst();
    do {
        sequence->ajoute(inst());
    } while (m_lecteur.getSymbole() == "<VARIABLE>" || m_lecteur.getSymbole() == "si"
            || m_lecteur.getSymbole() == "tantque" || m_lecteur.getSymbole() == "repeter"
            || m_lecteur.getSymbole() == "pour" || m_lecteur.getSymbole() == "ecrire"
            || m_lecteur.getSymbole() == "lire");
    // Tant que le symbole courant est un début possible d'instruction...
    // Il faut compléter cette condition chaque fois qu'on rajoute une nouvelle instruction
    return sequence;
}

Noeud* Interpreteur::inst() {
    // <inst> ::= <affectation>  ; | <instSi>
    if (m_lecteur.getSymbole() == "<VARIABLE>") {
        Noeud *affect = affectation();
        testerEtAvancer(";");
        return affect;
    } else if (m_lecteur.getSymbole() == "si")
        return instSi();
    else if (m_lecteur.getSymbole() == "tantque")
        return instTantQue();
    else if (m_lecteur.getSymbole() == "repeter")
        return instRepeter();
    else if (m_lecteur.getSymbole() == "pour")
        return instPour();
    else if (m_lecteur.getSymbole() == "ecrire")
        return instEcrire();
    else if (m_lecteur.getSymbole() == "lire")
        return instLire();
    // Compléter les alternatives chaque fois qu'on rajoute une nouvelle instruction
    else erreur("Instruction incorrecte");
}

Noeud* Interpreteur::affectation() {
    // <affectation> ::= <variable> = <expression> 
    tester("<VARIABLE>");
    Noeud* var = m_table.chercheAjoute(m_lecteur.getSymbole()); // La variable est ajoutée à la table et on la mémorise
    m_lecteur.avancer();
    testerEtAvancer("=");
    Noeud* exp = expression(); // On mémorise l'expression trouvée
    return new NoeudAffectation(var, exp); // On renvoie un noeud affectation
}

Noeud* Interpreteur::expression() {
    // <expression> ::= <facteur> { <opBinaire> <facteur> }
    //  <opBinaire> ::= + | - | *  | / | < | > | <= | >= | == | != | et | ou
    Noeud* fact = facteur();
    while (m_lecteur.getSymbole() == "+" || m_lecteur.getSymbole() == "-" ||
            m_lecteur.getSymbole() == "*" || m_lecteur.getSymbole() == "/" ||
            m_lecteur.getSymbole() == "<" || m_lecteur.getSymbole() == "<=" ||
            m_lecteur.getSymbole() == ">" || m_lecteur.getSymbole() == ">=" ||
            m_lecteur.getSymbole() == "==" || m_lecteur.getSymbole() == "!=" ||
            m_lecteur.getSymbole() == "et" || m_lecteur.getSymbole() == "ou") {
        Symbole operateur = m_lecteur.getSymbole(); // On mémorise le symbole de l'opérateur
        m_lecteur.avancer();
        Noeud* factDroit = facteur(); // On mémorise l'opérande droit
        fact = new NoeudOperateurBinaire(operateur, fact, factDroit); // Et on construuit un noeud opérateur binaire
    }
    return fact; // On renvoie fact qui pointe sur la racine de l'expression
}

Noeud* Interpreteur::facteur() {
    // <facteur> ::= <entier> | <variable> | - <facteur> | non <facteur> | ( <expression> )
    Noeud* fact = nullptr;
    if (m_lecteur.getSymbole() == "<VARIABLE>" || m_lecteur.getSymbole() == "<ENTIER>") {
        fact = m_table.chercheAjoute(m_lecteur.getSymbole()); // on ajoute la variable ou l'entier à la table
        m_lecteur.avancer();
    } else if (m_lecteur.getSymbole() == "-") { // - <facteur>
        m_lecteur.avancer();
        // on représente le moins unaire (- facteur) par une soustraction binaire (0 - facteur)
        fact = new NoeudOperateurBinaire(Symbole("-"), m_table.chercheAjoute(Symbole("0")), facteur());
    } else if (m_lecteur.getSymbole() == "non") { // non <facteur>
        m_lecteur.avancer();
        // on représente le moins unaire (- facteur) par une soustractin binaire (0 - facteur)
        fact = new NoeudOperateurBinaire(Symbole("non"), facteur(), nullptr);
    } else if (m_lecteur.getSymbole() == "(") { // expression parenthésée
        m_lecteur.avancer();
        fact = expression();
        testerEtAvancer(")");
    } else
        erreur("Facteur incorrect");
    return fact;
}

Noeud* Interpreteur::instSi() {
    // <instSi> ::= si ( <expression> ) <seqInst> finsi
    try {
        testerEtAvancer("si");
        testerEtAvancer("(");
        Noeud* condition = expression(); // On mémorise la condition
        testerEtAvancer(")");
        Noeud* seqIf = seqInst(); // On mémorise la séquence d'instruction
        vector<pair<Noeud*,Noeud*>>* vpElseIf = nullptr;
        while (m_lecteur.getSymbole()=="sinonsi") {
            vpElseIf = new vector<pair<Noeud*,Noeud*>>();
            pair<Noeud*,Noeud*> p;
            m_lecteur.avancer();
            testerEtAvancer("(");
            p.first = expression(); // On mémorise la condition
            testerEtAvancer(")");
            p.second = seqInst(); // On mémorise la séquence d'instruction
            vpElseIf->push_back(p);
        }
        Noeud* seqElse = nullptr;
        if (m_lecteur.getSymbole()=="sinon") {
            m_lecteur.avancer();
            seqElse = seqInst(); // On mémorise la séquence d'instruction
        }
        testerEtAvancer("finsi");
        return new NoeudInstSi(condition, seqIf, vpElseIf, seqElse); // Et on renvoie un noeud Instruction Si
    } catch(SyntaxeException e) {
        cout << e.what();
        while(m_lecteur.getSymbole()!="finsi") {
            m_lecteur.avancer();
        }
        m_lecteur.avancer();
        m_erreur=true;
        return nullptr;
    }
}

Noeud* Interpreteur::instTantQue() {
    // <instTantQue> ::= tantque ( <expression> ) <seqInst> fintantque
    try {    
        testerEtAvancer("tantque");
        testerEtAvancer("(");
        Noeud* exp = expression(); // recup resultat
        testerEtAvancer(")");
        Noeud* seq = seqInst(); // recup resultat
        testerEtAvancer("fintantque");
        Noeud* res = new NoeudInstTantQue(exp,seq);
        return res;
    } catch(SyntaxeException e) {
        cout << e.what();
        while(m_lecteur.getSymbole()!="fintantque") {
            m_lecteur.avancer();
        }
        m_lecteur.avancer();
        m_erreur=true;
        return nullptr;
    }
}

Noeud* Interpreteur::instRepeter() {
    // <instRepeter> ::= repeter <seqInst> jusqua( <expression> )
    try {
        testerEtAvancer("repeter");
        Noeud* seq = seqInst(); // recup resultat
        testerEtAvancer("jusqua");
        testerEtAvancer("(");
        Noeud* exp = expression(); // recup resultat
        testerEtAvancer(")");
        //testerEtAvancer(";");
        Noeud* res = new NoeudInstRepeter(exp,seq);
        return res;
    } catch(SyntaxeException e) {
        cout << e.what();
        while(m_lecteur.getSymbole()!="jusqua") {
            m_lecteur.avancer();
        }
        m_lecteur.avancer();
        if (m_lecteur.getSymbole()=="(")
            m_lecteur.avancer();
        while (m_lecteur.getSymbole()!=")")
            m_lecteur.avancer();
        m_lecteur.avancer();
        m_erreur=true;
        return nullptr;
    }
}

Noeud* Interpreteur::instPour() {
    //    <instPour> ::= pour ( [ <affectation> ] ; <expression> ; [ <affectation> ] ) <seqInst> finpour
    try {
        testerEtAvancer("pour");
        testerEtAvancer("(");
        Noeud* init = nullptr;
        if (m_lecteur.getSymbole()=="<VARIABLE>") {
            init = affectation(); // recup resultat
        }
        testerEtAvancer(";");
        Noeud* exp = expression(); // recup resultat
        testerEtAvancer(";");
        Noeud* inc = nullptr;
        if (m_lecteur.getSymbole()=="<VARIABLE>") {
            inc = affectation(); // recup resultat
        }
        testerEtAvancer(")");
        Noeud* seq = seqInst(); // recup resultat
        testerEtAvancer("finpour");
        Noeud* res = new NoeudInstPour(exp,seq,init,inc);
        return res;
    } catch(SyntaxeException e) {
        cout << e.what();
        while(m_lecteur.getSymbole()!="finpour") {
            m_lecteur.avancer();
        }
        m_lecteur.avancer();
        m_erreur=true;
        return nullptr;
    }
}

Noeud* Interpreteur::instEcrire() {
    //  <instEcrire> ::= ecrire ( <expression> | <chaine> {, <expression> | <chaine> } )
    try {
        Noeud* res = new NoeudInstEcrire();
        testerEtAvancer("ecrire");
        tester("(");
        int i=0;
        do {
            m_lecteur.avancer();
            if (m_lecteur.getSymbole() == "<VARIABLE>" || m_lecteur.getSymbole() == "<ENTIER>") {
                res->ajoute(expression());
            } else if (m_lecteur.getSymbole() == "<CHAINE>") {
                res->ajoute(m_table.chercheAjoute(m_lecteur.getSymbole()));
                m_lecteur.avancer();
            } else {
                stringstream ss;
                ss << "Paramètre " << i+1 << " incorrect";
                erreur(ss.str().c_str());
            }
            i++;
        } while(m_lecteur.getSymbole() == ",");
        testerEtAvancer(")");
        testerEtAvancer(";");
        return res;
    } catch(SyntaxeException e) {
        cout << e.what();
        while(m_lecteur.getSymbole()!=";") {
            m_lecteur.avancer();
        }
        m_lecteur.avancer();
        m_erreur=true;
        return nullptr;
    }
}

Noeud* Interpreteur::instLire() {
    //    <instLire> ::= lire ( <variable> { , <variable> } )
    try {
        testerEtAvancer("lire");
        testerEtAvancer("(");
        tester("<VARIABLE>");
        Noeud* res = new NoeudInstLire(m_table.chercheAjoute(m_lecteur.getSymbole()));
        m_lecteur.avancer();
        while (m_lecteur.getSymbole()==",") {
            m_lecteur.avancer();
            tester("<VARIABLE>");
            res->ajoute(m_table.chercheAjoute(m_lecteur.getSymbole()));
            m_lecteur.avancer();
        }
        testerEtAvancer(")");
        testerEtAvancer(";");
        return res;
    } catch(SyntaxeException e) {
        cout << e.what();
        while(m_lecteur.getSymbole()!=";") {
            m_lecteur.avancer();
        }
        m_lecteur.avancer();
        m_erreur=true;
        return nullptr;
    }
}

