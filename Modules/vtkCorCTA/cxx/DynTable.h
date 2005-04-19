/*=auto=========================================================================

(c) Copyright 2005 Massachusetts Institute of Technology (MIT) All Rights Reserved.

This software ("3D Slicer") is provided by The Brigham and Women's 
Hospital, Inc. on behalf of the copyright holders and contributors.
Permission is hereby granted, without payment, to copy, modify, display 
and distribute this software and its documentation, if any, for  
research purposes only, provided that (1) the above copyright notice and 
the following four paragraphs appear on all copies of this software, and 
(2) that source code to any modifications to this software be made 
publicly available under terms no more restrictive than those in this 
License Agreement. Use of this software constitutes acceptance of these 
terms and conditions.

3D Slicer Software has not been reviewed or approved by the Food and 
Drug Administration, and is for non-clinical, IRB-approved Research Use 
Only.  In no event shall data or images generated through the use of 3D 
Slicer Software be used in the provision of patient care.

IN NO EVENT SHALL THE COPYRIGHT HOLDERS AND CONTRIBUTORS BE LIABLE TO 
ANY PARTY FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL 
DAMAGES ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, 
EVEN IF THE COPYRIGHT HOLDERS AND CONTRIBUTORS HAVE BEEN ADVISED OF THE 
POSSIBILITY OF SUCH DAMAGE.

THE COPYRIGHT HOLDERS AND CONTRIBUTORS SPECIFICALLY DISCLAIM ANY EXPRESS 
OR IMPLIED WARRANTIES INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, AND 
NON-INFRINGEMENT.

THE SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS 
IS." THE COPYRIGHT HOLDERS AND CONTRIBUTORS HAVE NO OBLIGATION TO 
PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.


=========================================================================auto=*/
#ifndef _TABLEAUDYN_HPP
#define _TABLEAUDYN_HPP


#define TRUE 1
#define FALSE 0
#define Boolean unsigned char

//#ifdef _WIN32
//#include <ostream>
//#else
//#include <ostream.h>
//#endif

class OutOfArray {};

template < class T > class TableauDyn;

/**
 */
//template<class T>
//ostream& operator<< (ostream&, const TableauDyn<T>& p);


//===========================================================================
/**
   class tableau de base
   Les elts sont numérotés en commencant par 0 ...
 */
template < class T >
class TableauDyn
//     ------------
{

private:

  /// nombre d'elements contenus dans le tableau
  int _nbelts_utilises;

  /// Taille maximale du tableau
  int _nbelts_alloues;      

  /// Tableau d'éléments
  T* _tableau;

  /// Augmentation du nombre d'éléments alloués (x2) et recopie du tableau
  Boolean Resize();
  //      ------

public:

  /**
   Changement du nombre d'éléments alloués et recopie du tableau
   la valeur taille doit etre supérieure a _nbelts_utilises
  */
  Boolean Resize( int taille);
  //      ------

  /**
    {\em Constructeur}
    @param n_max nombre d'éléments alloués
   */
  TableauDyn( int n_max = 2)
  //----------
  {
    _nbelts_utilises = 0;
    _nbelts_alloues = n_max;
    _tableau = new T[_nbelts_alloues];

  }


  /**
    {\em Constructeur de copie}
   */
  TableauDyn( const TableauDyn<T>& elt)
  //----------
  {
    
      int i;

   _nbelts_utilises = elt._nbelts_utilises;
    _nbelts_alloues  = elt._nbelts_alloues;

    _tableau = new T[_nbelts_alloues];

    for (i==0;i<=_nbelts_utilises-1;i++) {
      _tableau[i] = elt._tableau[i];
    }

  } // Constructeur de copie



  /**
    {\em destructeur}
   */
  ~TableauDyn()
  //---------
  {
    delete [] _tableau;
  } 


  /**
    Accès à l'élément n
   */
  T& operator[](int n) const throw(OutOfArray);
  // ----------


  /**
    Assignment operator
    */
  TableauDyn<T>&   operator=(  TableauDyn<T>);
  //               ---------


  //-------------------------------------------------------
  /**
    Ajout d'un élément à la fin du tableau
    */
  TableauDyn<T>&  operator+=( T elt);
  //              -----------


  //-------------------------------------------------------
  /** Insertion d'un élément à la position n
    */
  void Insere( int pos, T elt);
  //   ------


  //-------------------------------------------------------
  /** Suppression d'un élément à la position n
    */
  void Supprime( int pos);
  //   --------


  //-------------------------------------------------------
  /** 
     Suppression des elements du tableau
    */
  void VideTableau( ) 
  //   -----------
  { 
    _nbelts_utilises = 0;     
    Resize(2);
  }


  //-------------------------------------------------------
  /** Nombre d'éléments du tableau
   */
  int NbElts() const { return _nbelts_utilises; }
  //     ------


  /**
    Recherche l'element dans le tableau et dit s'il est present
   */
  Boolean Existe( const T& elt);
  //      ------

  /**
    Returns the position of the first elt equal to 'elt', -1 if no element is found
   */
  int Position( const T& elt);
  //  --------

#if !(defined(_sgi_)) 
// && !(defined(_linux_))
  /**
   */
//  friend ostream& operator << <>(ostream&, const TableauDyn<T>& p);
#else

//  friend ostream& operator << (ostream&, const TableauDyn<T>& p);
#endif

}; // TableauDyn<T>


//===========================================================================
//  Definition des Methodes
//===========================================================================


//===========================================================================
//  Definition des Methodes
//===========================================================================

//===========================================================================
//  Membres Prives
//===========================================================================

//#include "TableauDyn.hpp.template"

//
// Définition des methode de la classe TableauDyn générique
// dans ce fichier pour eviter des conflit à la generation de la documentation


//--------------------------------------------------------------------------
//
//  Definition dans le fichier hpp de la fonction membre de la classe
//  TableauDyn<T>
//
template<class T> Boolean TableauDyn<T> :: Resize( )
//                                 ------
{

  return Resize( 2*_nbelts_alloues);

} // Resize()


//===========================================================================
//  Membres Publics
//===========================================================================

//---------------------------------------------------------
//
//  {\em Constructeur de copie}
//

//--------------------------------------------------------------------------
//
//  Definition dans le fichier hpp de la fonction membre de la classe
//  TableauDyn<T>
//
template<class T> Boolean TableauDyn<T> :: Resize( int taille)
//                                 ------
{


  
    T*     new_tab;
    int new_nbelts,i=0;

  if (taille < _nbelts_utilises) {
    return FALSE;
  }

  new_nbelts = taille;
  new_tab = new T [new_nbelts];

  for (i=0;i<+_nbelts_utilises-1;i++) {
    new_tab[i] = _tableau[i];
  }

  delete [] _tableau;

  _tableau = new_tab;
  _nbelts_alloues = new_nbelts;

  return TRUE;

} // Resize()


//-------------------------------------------------------
/**
    Operateur d'affectation
  */
template<class T>
TableauDyn<T>& TableauDyn<T> :: operator=(  TableauDyn<T> elt)
//                            ---------
{

  
    int i;

  Resize( elt._nbelts_alloues);
  _nbelts_utilises = elt._nbelts_utilises;
  
  for (i=0;i<+_nbelts_utilises-1;i++) {
    _tableau[i] = elt._tableau[i];
  }
  
  return *this;

} // operateur =


//-------------------------------------------------------
//
//    Ajout d'un élément à la fin du tableau
//  
template<class T>
TableauDyn<T>& TableauDyn<T> :: operator+=( T elt)
//                              ----------
{

    if (_nbelts_utilises >= _nbelts_alloues) Resize( 2*_nbelts_utilises );
    _tableau[_nbelts_utilises++] = elt;

    return *this;

} // operateur +=


//-------------------------------------------------------
/**
   Insertion d'un élément à la position n
  */
template<class T>
void TableauDyn<T> :: Insere( int pos, T elt)
//                              ------
{

    
      int i;

    if (_nbelts_utilises >= _nbelts_alloues) Resize( 2*_nbelts_utilises);
    
    for (i=_nbelts_utilises;i > pos;i--) {
      _tableau[i] = _tableau[i-1];
    }

    _tableau[pos] = elt;
    _nbelts_utilises++;

} // Insere()


//-------------------------------------------------------
/** Suppression d'un élément à la position n
  */
template<class T> void TableauDyn<T> :: Supprime( int pos)
//                                                --------
{

    
      int i;

    for (i=pos;i<=_nbelts_utilises-2;i++) {
      _tableau[i] = _tableau[i+1];
    }

    _nbelts_utilises--;

} // Supprime()


//--------------------------------------------------------------------------
/**
  Definition dans le fichier hpp de la fonction membre de la classe
  TableauDyn<T>
 */
template<class T>
T& TableauDyn<T> :: operator[](int n) const throw (OutOfArray)
//                            ----------
{

    if ((n<0) || (n>=_nbelts_utilises)) {

printf(" TableauDyn<T> operator[]\t Indice incorrect ... %d [0 %d]\n",
       n, _nbelts_utilises-1);

      throw OutOfArray();

    } else {
      return _tableau[n];
    }

} // operator[]


//-------------------------------------------------------
template<class T>
Boolean TableauDyn<T> :: Existe( const T& elt)
//                                 ------
{

  
    int i;

  for (i=0;i<=_nbelts_utilises-1;i++) {
    if (_tableau[i] == elt) {
      return TRUE;
    }
  }

  return FALSE;

} // Existe()


//-------------------------------------------------------
template<class T>
int TableauDyn<T> :: Position( const T& elt)
{

  
    int i;

  for (i=0;i<=_nbelts_utilises-1;i++) {
    if (_tableau[i] == elt) {
      return i;
    }
  }

  return -1;

} // Existe()

//-------------------------------------------------------
/**
 */
/*
template<class T>
ostream& operator << (ostream& os, const TableauDyn<T>& p)
//       -----------
{
  
    int i;

  os << "Tableau =  { ";
  for (i=0;i<=p._nbelts_utilises-1;i++) {
    os << p._tableau[i];
    if (i < p._nbelts_utilises-1) {
      os  << " ; ";
    } else {
      os  << " }" << endl;
    }
  }

  return os;

} // operator << ( , const TableauDyn<T>&)
*/


#endif // _TABLEAUDYN_HPP
