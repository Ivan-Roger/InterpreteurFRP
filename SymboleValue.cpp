#include "SymboleValue.h"
#include "Exceptions.h"
#include <stdlib.h>

SymboleValue::SymboleValue(const Symbole & s) :
Symbole(s.getChaine()) {
  m_defini = true;
  if (s == "<ENTIER>") {
    m_valeur = atoi(s.getChaine().c_str()); // c_str convertit une string en char*
  } else if (s == "<CHAINE>") {
    m_valeur = atoi(s.getChaine().c_str()); // c_str convertit une string en char*
  } else  {
    m_defini = false;
  }
}

int SymboleValue::executer() {
  if (!m_defini) throw IndefiniException(); // on lève une exception si valeur non définie
  return m_valeur;
}

ostream & operator<<(ostream & cout, const SymboleValue & symbole) {
  cout << (Symbole) symbole << "\t\t - Valeur=";
  if (symbole.m_defini) cout << symbole.m_valeur << " ";
  else cout << "indefinie ";
  return cout;
}

void SymboleValue::translate(std::ostream& out, int offset) {
    if (*this == "<ENTIER>")
        out << m_valeur;
    if (*this == "<CHAINE>" || *this == "<VARIABLE>")
        out << this->getChaine();
}
