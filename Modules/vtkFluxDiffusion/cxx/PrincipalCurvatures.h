

//
//  Code by Prof. Luis Alvarez Leon
//  University of Las Palmas of Gran Canaria
//



#ifndef CURVATURAS_PRINCIPALES_HPP
#define CURVATURAS_PRINCIPALES_HPP

#include <vtkFluxDiffusionConfigure.h>

//BTX
int CurvaturasPrincipales(float H[3][3],
              float p[3],
              float vmax[3],
              float vmin[3],
              float *lmax,
              float *lmin,
              float umbral);

// Without eigenvectors
int CurvaturasPrincipales(float H[3][3],
              float p[3],
              float *lmax,
              float *lmin,
              float umbral);

//ETX

#endif // CURVATURAS_PRINCIPALES_HPP
