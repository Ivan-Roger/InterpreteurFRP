#ifndef INTERPRETEUR_H
#define INTERPRETEUR_H

#include "Symbole.h"
#include "Lecteur.h"
#include "Exceptions.h"
#include "TableSymboles.h"
#include "ArbreAbstrait.h"

class Interpreteur {
public:
	Interpreteur(ifstream & fichier);   // Construit un interpréteur pour interpreter
	                                    //  le programme dans  fichier 
                                      
	void analyse();                     // Si le contenu du fichier est conforme à la grammaire,
	                                    //   cette méthode se termine normalement et affiche un message "Syntaxe correcte".
                                            //   la table des symboles (ts) et l'arbre abstrait (arbre) auront été construits
	                                    // Sinon, une exception sera levée
        void translate(std::ostream& out);  // Traduit tout le code en c (appelle la fonction translate de chaque feuille de l'arbre)

	inline const TableSymboles & getTable () const  { return m_table;    } // accesseur	
	inline Noeud* getArbre () const { return m_arbre; }                    // accesseur
	
private:
    Lecteur        m_lecteur;  // Le lecteur de symboles utilisé pour analyser le fichier
    TableSymboles  m_table;    // La table des symboles valués
    Noeud*         m_arbre;    // L'arbre abstrait
    bool           m_erreur;

    // Implémentation de la grammaire
    Noeud*  programme();        //   <programme> ::= { procedure <procedure>( { <variable> } ) <seqInst> finproc } procedure principale() <seqInst> finproc FIN_FICHIER
    Noeud*  seqInst();          //     <seqInst> ::= <inst> { <inst> }
    Noeud*  inst();	        //        <inst> ::= <affectation> ; | <instSi>
    Noeud*  affectation();      // <affectation> ::= [ <variable> [ = <expression> | ++ | -- ] ] | [ [ ++ | -- ] <variable> ]
    Noeud*  expression();       //  <expression> ::= <terme> { + <terme> | - <terme>}
    Noeud*  terme();            //       <terme> ::= <facteur> { * <facteur> | / <facteur> }
    Noeud*  facteur();          //     <facteur> ::= <entier> | <variable> | - <expBool> | non <expBool> | ( <expBool> )
    Noeud*  expBool();          //     <expBool> ::= <relationEt> { ou <relationEt> }
    Noeud*  relationEt();       //  <realtionEt> ::= <realtion> { et <relation> }
    Noeud*  relation();         //    <relation> ::= <expression> { <opRel> <expression> }
    //Noeud*  opRel();          //       <opRel> ::= == | != | < | <= | > | >=
    Noeud*  instSi();           //      <instSi> ::= si ( <expression> ) <seqInst> finsi
    Noeud*  instTantQue();      // <instTantQue> ::= tantque ( <expression> ) <seqInst> fintantque
    Noeud*  instRepeter();      // <instRepeter> ::= repeter <seqInst> jusqua( <expression> )
    Noeud*  instPour();         //    <instPour> ::= pour ( [ <affectation> ] ; <expression> ; [ <affectation> ] ) <seqInst> finpour
    Noeud*  instEcrire();       //  <instEcrire> ::= ecrire ( <expression> | <chaine> {, <expression> | <chaine> } )
    Noeud*  instLire();         //    <instLire> ::= lire ( <variable> { , <variable> } )
    //Noeud*  instSelon();      //   <instSelon> ::= selon ( <variable> ) { cas <entier> : <seqInst> } [ defaut : <seqInst> ] finselon
    
    // outils pour simplifier l'analyse syntaxique
    void tester (const std::string & symboleAttendu) const throw (SyntaxeException);   // Si symbole courant != symboleAttendu, on lève une exception
    void testerEtAvancer(const std::string & symboleAttendu) throw (SyntaxeException); // Si symbole courant != symboleAttendu, on lève une exception, sinon on avance
    void erreur (const std::string & mess) const throw (SyntaxeException);             // Lève une exception "contenant" le message mess
};

#endif /* INTERPRETEUR_H */
