/*  ==================================================
    Module: vtkFluxDiffusion
    Author: Karl Krissian
    Email:  karl@bwh.harvard.edu

    This module implements a version of anisotropic diffusion published in 
    
    "Flux-Based Anisotropic Diffusion Applied to Enhancement of 3D Angiographiam"
    Karl Krissian
    IEEE Trans. Medical Imaging, 21(11), pp 1440-1442, nov 2002.
    
    It aims at restoring 2D and 3D images with the ability to preserve
    small and elongated structures.
    It comes with a Tcl/Tk interface for the '3D Slicer'.
    ==================================================
    Copyright (C) 2002  Karl Krissian

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
    ================================================== 
    The full GNU Lesser General Public License file is in vtkFluxDiffusion/LesserGPL_license.txt
*/

 /*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkAnisoGaussSeidel.cxx,v $
  Language:  C++
  Date:      $Date: 2004/02/11 23:35:24 $
  Version:   $Revision: 1.9 $

=========================================================================*/

#include "vtkAnisoGaussSeidel.h"
#include "vtkObjectFactory.h"
#include "vtkPointData.h"
#include "vtkStructuredPoints.h"
#include "vtkFloatArray.h"
#include "vtkStructuredPointsWriter.h"
#include "vtkImageGaussianSmooth.h"

//
//  Code by Karl Krissian
//  University of Las Palmas of Gran Canaria
//  &
//  vtk transfert at Brigham and Women's Hospital
//  Surgical Planning Lab
//



//BTX
#ifndef AMILAB 
#include "PrincipalCurvatures.cxx"
#else
#include "PrincipalCurvatures.h"
#endif
//ETX

// FOR
#define For(number, min, max)  for(number=min; number<=max; number++) {
#define EndFor                 }

#define Pour(number, min, max)  for(number=min; number<=max; number++) {
#define FinPour                 }

#define Si       if(
#define Alors    ) {
#define AlorsFait ) 
#define Sinon    } else {
#define FinSi    }
#define Local
#define Entier   int
#define Reel     float
#define Registre register
#define Retourne return
#define FAUX     0
#define VRAI     1
#define FALSE    0
#define TRUE     1
#define DebutFonction {
#define FinFonction   }
#define Rien          void
#define Non           !
#define Et            &&

#define macro_min(n1,n2) ((n1)<(n2)?(n1):(n2))
#define macro_max(n1,n2) ((n1)>(n2)?(n1):(n2))
#define macro_abs(n)     ((n)>0?(n):-(n))

//#define phi0(x)         exp(-((x)*(x)/k/k))
#define phi0(x)         exp(-0.5*((x)*(x)/k/k))
#define phi1(x)         TangCoeff

//#define phi3D_0(x) exp(-((x)*(x)/k/k))
#define phi3D_0(x) exp(-0.5*((x)*(x)/k/k))
#define phi3D_1(x) MaxcurvCoeff
#define phi3D_2(x) MincurvCoeff

#define macro_max_abs(n1,n2) (fabs(n1)>fabs(n2)?(n1):(n2))
#define macro_min_abs(n1,n2) (fabs(n1)<fabs(n2)?(n1):(n2))


static unsigned int static_tx, static_txy;


typedef struct { float x,y,z; } point3D;
typedef struct { float x,y,z; } vector3D;



//----------------------------------------------------------------------
static vtkImageData* CreateImageDataFloat( vtkImageData* im)
{

 vtkDataArray *TScalars;
 vtkImageData *Target;

 Target = vtkImageData::New();
 Target->SetDimensions(im->GetDimensions());
 Target->SetSpacing(   im->GetSpacing   ());
 Target->SetScalarType(VTK_FLOAT); 

 TScalars = vtkFloatArray::New();
 TScalars->SetNumberOfComponents(1);
 TScalars->SetNumberOfTuples( im->GetPointData()->GetScalars()->GetNumberOfTuples());
 // ts_pr=(unsigned short *) TScalars->GetVoidPointer(0);
 Target->GetPointData()->SetScalars(TScalars);
 TScalars->Delete();

 return Target;

}


//-----------------------------------------------------------------------
vtkAnisoGaussSeidel* vtkAnisoGaussSeidel::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkAnisoGaussSeidel");
  if(ret)
    {
    return (vtkAnisoGaussSeidel*)ret;
    }
  // If the factory was unable to create the object, then create it here.
  return new vtkAnisoGaussSeidel;

} // vtkAnisoGaussSeidel::New()


//----------------------------------------------------------------------
// Construct object to extract all of the input data.
//
vtkAnisoGaussSeidel::vtkAnisoGaussSeidel()
{

  im_tmp1               = NULL;
  im_tmp2               = NULL;

  //  use_filtre_rec     = FAUX;
  opt_mem            = FAUX;

  sigma              = 1;
  beta               = 0.05;
  k                  = 10;

  NumberOfIterations = 1;

  TangCoeff          = 1.0;
  MaxcurvCoeff       = 0.1;
  MincurvCoeff       = 1.0;

  epsilon            = 1E-2;

  SmoothedParam      = FAUX;

  IsoCoeff           = 0.2;

  TruncNegValues     = 0;

} // vtkAnisoGaussSeidel::vtkAnisoGaussSeidel()


//----------------------------------------------------------------------
// Construct object to extract all of the input data.
//
vtkAnisoGaussSeidel::~vtkAnisoGaussSeidel()
{


  DeleteCoefficients();

  /*
  Si image_lissee != NULL Alors
    image_lissee->Delete();
    image_lissee=NULL;
  FinSi
  */

  Si im_tmp1 != NULL Alors
    im_tmp1->Delete();
    im_tmp1 = NULL;
  FinSi

  Si im_tmp2 != NULL Alors
    im_tmp2->Delete();
    im_tmp2 = NULL;
  FinSi


} // vtkAnisoGaussSeidel::~vtkAnisoGaussSeidel()


//----------------------------------------------------------------------
void vtkAnisoGaussSeidel::Init( )
{

  Local
    int type;
  //  sigma       = p_sigma;
  //  beta        = p_beta;
  //  k           = p_k;

  image_entree = this->GetInput();

  if (image_entree == NULL) {
    vtkErrorMacro("Missing input");
    return;
  }
  else {

    //    Si opt_mem Alors
    //      use_filtre_rec = VRAI;
    //    FinSi
    
    // check the image is in float format, or convert
    type = GetInput()->GetScalarType();

    // Make a copy in float format in im_tmp1
    vtkDebugMacro(<<"making a copy of the input into float format");
    // Create a copy of the data
    printf("Create im_tmp1 \n");
    im_tmp1 = vtkImageData::New();
    im_tmp1->SetScalarType( VTK_FLOAT);
    im_tmp1->SetNumberOfScalarComponents(1);
    im_tmp1->SetDimensions( this->GetInput()->GetDimensions());
    im_tmp1->SetOrigin(     this->GetInput()->GetOrigin());
    im_tmp1->SetSpacing(    this->GetInput()->GetSpacing());
      
    im_tmp1->CopyAndCastFrom(this->GetInput(),
                      this->GetInput()->GetExtent());

    // Create second temporary image
    printf("Create im_tmp2 \n");
    im_tmp2 = CreateImageDataFloat(im_tmp1);
    im_tmp2->CopyAndCastFrom(this->GetInput(),
                    this->GetInput()->GetExtent());

    tx = this->im_tmp1->GetDimensions()[0];
    ty = this->im_tmp1->GetDimensions()[1];
    tz = this->im_tmp1->GetDimensions()[2];
    txy = tx*ty;
    
    Si tz>1 Alors mode=MODE_3D; Sinon mode=MODE_2D; 
    FinSi
                              
    //--- image_resultat
    image_resultat      = this->GetOutput();
    
    image_resultat->SetDimensions( this->GetInput()->GetDimensions() );
    image_resultat->SetSpacing(    this->GetInput()->GetSpacing() );
    image_resultat->SetOrigin(     this->GetInput()->GetOrigin());

    // Set the output to the same type as the input
    image_resultat->SetScalarType(type);
    image_resultat->SetNumberOfScalarComponents(1);
    image_resultat->AllocateScalars();

    // Smoothed image    
    image_lissee = NULL;
     
    InitCoefficients();
    
    iteration = 0;
  }
    

} // vtkAnisoGaussSeidel::InitParam()



//----------------------------------------------------------------------
// Allocate the coefficients alpha and gamma for each direction
void vtkAnisoGaussSeidel::InitCoefficients()
//                        -----------------
{

  Local
    Entier x;

  _alpha_y = new float[tx];
  _gamma_y = new float[tx];

  _alpha_x = _gamma_x = 0;
  Pour(x,0,tx-1)
    _alpha_y[x] = _gamma_y[x] = 0;
  FinPour

  Si mode == MODE_3D Alors
    Entier y;

    _alpha_z = new float*[tx];
    _gamma_z = new float*[tx];
    Pour(x,0,tx-1)
      _alpha_z[x] = new float[ty];
      _gamma_z[x] = new float[ty];


      Pour(y,0,ty-1)
    _alpha_z[x][y] = _gamma_z[x][y] = 0;
      FinPour

    FinPour

  FinSi

} // vtkAnisoGaussSeidel::InitCoefficients()


//----------------------------------------------------------------------
// Allocate the coefficients alpha and gamma for each direction
void vtkAnisoGaussSeidel::InitCoefficients(float& alpha_x, float*& alpha_y, float**& alpha_z,
//                        -----------------
                        float& gamma_x, float*& gamma_y, float**& gamma_z,
                        int sx, int sy)
{

  Local
    Entier x;


  alpha_y = new float[sx];
  gamma_y = new float[sx];

  alpha_x = gamma_x = 0;
  Pour(x,0,sx-1)
    alpha_y[x] = gamma_y[x] = 0;
  FinPour

  Si mode == MODE_3D Alors
    Entier y;

    alpha_z = new float*[sx];
    gamma_z = new float*[sx];
    Pour(x,0,sx-1)
      alpha_z[x] = new float[sy];
      gamma_z[x] = new float[sy];


      Pour(y,0,sy-1)
    alpha_z[x][y] = gamma_z[x][y] = 0;
      FinPour

    FinPour

  FinSi

} // vtkAnisoGaussSeidel::InitCoefficients()


//----------------------------------------------------------------------
// Reset the coefficients to 0
void vtkAnisoGaussSeidel::ResetCoefficients()
//                        -----------------
{

  Local
    Entier x;

  _alpha_x = _gamma_x = 0;

  Pour(x,0,tx-1)
    _alpha_y[x] = _gamma_y[x] = 0;
  FinPour

  Si mode == MODE_3D Alors
    Entier y;

    Pour(x,0,tx-1)
      Pour(y,0,ty-1)
    _alpha_z[x][y] = _gamma_z[x][y] = 0;
      FinPour
    FinPour

  FinSi

} // vtkAnisoGaussSeidel::ResetCoefficients()


//----------------------------------------------------------------------
// Reset the coefficients to 0
void vtkAnisoGaussSeidel::ResetCoefficients(float& alpha_x, float* alpha_y, float** alpha_z,
//                        -----------------
                        float& gamma_x, float* gamma_y, float** gamma_z,
                        int sx, int sy)
{

  Local
    Entier x;

  alpha_x = gamma_x = 0;

  Pour(x,0,sx-1)
    alpha_y[x] = gamma_y[x] = 0;
  FinPour

  Si mode == MODE_3D Alors
    Entier y;

    Pour(x,0,sx-1)
      Pour(y,0,sy-1)
    alpha_z[x][y] = gamma_z[x][y] = 0;
      FinPour
    FinPour

  FinSi

} // vtkAnisoGaussSeidel::ResetCoefficients(outExt)


//----------------------------------------------------------------------
// Free the memory allocated for the coefficients
void vtkAnisoGaussSeidel::DeleteCoefficients()
//                        ------------------
{

  Si _alpha_y != NULL Alors

    delete [] _alpha_y;
    _alpha_y = NULL;

    delete [] _gamma_y;

    Si mode == MODE_3D Alors
      Entier i;

      Pour(i,0,tx-1)
        delete [] _alpha_z[i];
        delete [] _gamma_z[i];
      FinPour
      delete [] _alpha_z;
      delete [] _gamma_z;

    FinSi
  FinSi

} // vtkAnisoGaussSeidel::DeleteCoefficients()


//----------------------------------------------------------------------
// Free the memory allocated for the coefficients
void vtkAnisoGaussSeidel::DeleteCoefficients( float*& alpha_y, float**& alpha_z,
//                        -----------------
                         float*& gamma_y, float**& gamma_z,
                         int sx)
{

  Si alpha_y != NULL Alors

    delete [] alpha_y;
    alpha_y = NULL;

    delete [] gamma_y;

    Si mode == MODE_3D Alors
      Entier i;

      Pour(i,0,sx-1)
        delete [] alpha_z[i];
        delete [] gamma_z[i];
      FinPour
      delete [] alpha_z;
      delete [] gamma_z;

    FinSi
  FinSi

} // vtkAnisoGaussSeidel::DeleteCoefficients()


//----------------------------------------------------------------------
float vtkAnisoGaussSeidel::Iterate()
{

  Local
    Reel       erreur;

  Si image_resultat==NULL Alors
    fprintf(stderr,"vtkAnisoGaussSeidel::Iterate() vtkAnisoGaussSeidel not initialized \n");
    return 0.0;
  FinSi

  iteration++;

  Si mode == MODE_2D Alors
    erreur = Iterate2D( );
  Sinon
    Si MaxcurvCoeff > 1E-5 Alors
    //      erreur = Itere3D_2(    image_resultat);
    //    Sinon
      erreur = Iterate3D( );
    FinSi
  FinSi
    
  printf(" iteration %d : %f \n", iteration,erreur);
  
  return erreur;

} // Iterate()



//----------------------------------------------------------------------
float vtkAnisoGaussSeidel::Iterate2D()
{

  Local 
    Entier x,y,z; //,n,i;
    Reel   val0,val1;
    Reel   val1div;
    Reel   u_e0;
    Reel   u_e1;
    Reel   alpha1_x, gamma1_x;
    Reel   alpha1_y, gamma1_y;
    Reel*  in;
    Registre Reel    *Iconv; 
    Reel gradient[2];
    Reel hessien[2][2];

    point3D e0;
    point3D e1;
    Reel     norm;

    vector3D grad;

    Reel   erreur; //,norm_grad;
    Entier nb_points_instables;
    Entier erreur_x,erreur_y,erreur_z;

    Reel   phi0_param;

    //  fprintf(stderr,"vtkAnisoGaussSeidel.cxx begin \n");

  ResetCoefficients();

  erreur = 0;

  nb_points_instables = 0;

  in    = (float*) this->image_resultat->GetScalarPointer(0,0,0);
  Iconv = (float*) this->image_lissee  ->GetScalarPointer(0,0,0);


  Pour(z,0,tz-1)
  Pour(y,0,ty-1)
  Pour(x,0,tx-1)
  

    val0 = *in;

    //----- Calcul de alpha1_x, gamma1_x

    // Gradient en (x+1/2,y)
    // et Calcul de e0, e1
    Si (x<tx-1) Et (y > 0)Et(y < ty-1) Alors
        grad.y = (*(in   +tx)-*(in   -tx)+*(in   +tx+1)-*(in   -tx+1))/4.0;
        e0.y   = (*(Iconv+tx)-*(Iconv-tx)+*(Iconv+tx+1)-*(Iconv-tx+1))/4.0;
    Sinon
        grad.y = 0;
        e0.y   = 0;
    FinSi
 
    Si (x>0) Et (x < tx-1)  Alors
        grad.x = *(in+1)  - *in;
        e0.x   = *(Iconv+1) - *(Iconv);
    Sinon
        grad.x = 0;
        e0.x   = 0;
    FinSi

    norm = sqrt(e0.x*e0.x+e0.y*e0.y);
    Si norm > 1E-5 Alors
        e0.x /= norm;
        e0.y /= norm;
    Sinon
        e0.x = 1.0;
        e0.y = 0;
    FinSi
    e1.x = -e0.y;
    e1.y = e0.x;

    // Derivees directionnelles

    u_e0 = grad.x*e0.x + grad.y*e0.y;
    u_e1 = grad.x*e1.x + grad.y*e1.y;

    Si SmoothedParam Alors
      phi0_param = norm;
    Sinon
      phi0_param = u_e0;
    FinSi

    alpha1_x = phi0(phi0_param)*e0.x*e0.x + 
               phi1(u_e1)*e1.x*e1.x;

    gamma1_x  = grad.y*( e0.y*phi0(phi0_param)*e0.x + 
                         e1.y*phi1(u_e1)*e1.x );

    //----- Calcul de alpha1_y, gamma1_y 
     
    // Gradient en (x,y+1/2)
    Si  (y>0) Et (y < ty -1)  Alors
      //        grad.y = (*(in  +tx+tx) - *(in) + (*in+tx) - (*in-tx))/4.0;
      grad.y = *(in   +tx) - *(in);
      e0.y   = *(Iconv+tx) - *(Iconv);
    Sinon
      grad.y  = 0.0;
        e0.y    = 0.0;
    FinSi
 
    //  gradient en X
    Si (y < ty -1) Et (x > 0) Et (x< tx-1)  Alors
      grad.x = (*(in   +1)-*(in   -1)+*(in   +1+tx)-*(in   -1+tx))/4.0;
      e0.x   = (*(Iconv+1)-*(Iconv-1)+*(Iconv+1+tx)-*(Iconv-1+tx))/4.0;
    Sinon
      grad.x = 0;
      e0.x = 0.0;
    FinSi


    // Calcul de e0, e1

    norm = sqrt(e0.x*e0.x+e0.y*e0.y); 
    Si norm > 1E-5 Alors
      e0.x /= norm;
      e0.y /= norm;
    Sinon
      e0.x = 1.0;
      e0.y = 0.0;
    FinSi

    e1.x = -e0.y;
    e1.y = e0.x;

    // Derivees directionnelles
    u_e0 = grad.x*e0.x + grad.y*e0.y;
    u_e1 = grad.x*e1.x + grad.y*e1.y;

    Si SmoothedParam Alors
      phi0_param = norm;
    Sinon
      phi0_param = u_e0;
    FinSi

    alpha1_y = phi0(phi0_param)*e0.y*e0.y + 
               phi1(u_e1)*e1.y*e1.y;

    gamma1_y = grad.x*e0.x*phi0(phi0_param)*e0.y + 
               grad.x*e1.x*phi1(u_e1)*e1.y;


    //----- Mise a jour de l'image

    val1    =  beta* (*((float*)(this->im_tmp1->GetScalarPointer(x,y,z))));
    val1div = beta;
  
    Si (x>0)Et(x<tx-1) Alors
      val1 += alpha1_x  * (*(in+1 )) + 
              _alpha_x   * (*(in-1 )) +
              gamma1_x  - _gamma_x;

      val1div += alpha1_x + _alpha_x;
    FinSi

    Si (y>0)Et(y<ty-1) Alors
      val1 += alpha1_y  * (*(in+tx )) + 
              _alpha_y [x]  * (*(in-tx )) +
              gamma1_y  - _gamma_y[x];

      val1div += alpha1_y + _alpha_y[x];
    FinSi

    Si fabs(val1div)<1E-5 Alors
      val1 = *((float*)(this->im_tmp1->GetScalarPointer(x,y,z)));
    Sinon
      val1/= val1div;
    FinSi

    _alpha_y[x] = alpha1_y;
    _alpha_x    = alpha1_x;

    _gamma_y[x] = gamma1_y;
    _gamma_x    = gamma1_x;

 
    Si fabs(val1-val0) > epsilon Alors
      nb_points_instables++;
    FinSi

    Si fabs(val1-val0) > erreur Alors
      erreur = fabs(val1-val0);
      erreur_x = x;
      erreur_y = y;
      erreur_z = z;
    FinSi

    *((float*)(im_tmp2->GetScalarPointer(x,y,z))) = val1;


    in++;
    Iconv++;

  FinPour
  FinPour
  FinPour


  im_tmp1->CopyAndCastFrom(this->im_tmp2,
               this->im_tmp2->GetExtent());

  //  cout << endl;
  //  cout << " Erreur = " << erreur << endl;
  //  cout << "( " << erreur_x << ", " 
  //              << erreur_y << ", " 
  //               << erreur_z << " )" << endl; 
  //  cout << "nb de points variables " << nb_points_instables << endl;


  Retourne erreur;

} // Iterate2D()


//----------------------------------------------------------------------
float vtkAnisoGaussSeidel::Iterate3D()
{

  Local 
    Entier x,y,z; //,n,i;
    Reel   val0,val1;
    Reel   val1div;
    Reel   u_e0;
    Reel   u_e1;
    Reel   u_e2;
    Reel   alpha1_x, gamma1_x;
    Reel   alpha1_y, gamma1_y;
    Reel   alpha1_z, gamma1_z;
    Reel*  in;
    Registre Reel    *Iconv; 
    Reel gradient[3];
    Reel hessien[3][3];
    float vmax[3];
    float vmin[3];
    float lmin;
    float lmax;

    point3D e0;
    point3D e1;
    point3D e2;

    vector3D grad;

    Reel   erreur,norm_grad;
    double diff;
    Entier nb_points_instables;
    Entier erreur_x,erreur_y,erreur_z;

    unsigned long nbpts_isotropic    = 0;
    unsigned long nbpts_anisotropic  = 0;

    unsigned char isotropic;

  ResetCoefficients();

  erreur              = 0;
  nb_points_instables = 0;

  in    = (float*) this->im_tmp1->GetScalarPointer(0,0,0);
  Iconv = (float*) this->image_lissee  ->GetScalarPointer(0,0,0);

  diff = 0;

  Pour(z,0,tz-1)

    /*
    Si z==0 Alors
      printf("z = %3d",z);
      fflush(stdout);
    Sinon 
      printf("\b\b\b");
      printf("%3d",z);
      fflush(stdout);
    FinSi
    */

  Pour(y,0,ty-1)
  Pour(x,0,tx-1)
  

    val1 = val0 = *in;

    Si x>0 Et x<tx-1 Et y>0 Et y<ty-1 Et z>0 Et z<tz-1 Alors

      // Gradient en (x+1/2,y,z)
      //----- Calcul de alpha1_x, gamma1_x
      grad.x = *(in+1) - *(in);
      grad.y = ( *(in + tx) - *(in - tx) + *(in+tx+1) - *(in-tx+1) )/ 4.0;
      grad.z = (*(in+txy)   - *(in-txy  )+ *(in+txy+1)- *(in-txy+1))/ 4.0;

      // Calcul du gradient, du hessien en (x+1/2,y,z)
      gradient[0] = *(Iconv+1) - *Iconv;
      gradient[1] = ((*(Iconv+tx )-*(Iconv-tx ))+(*(Iconv+tx+1 )-*(Iconv-tx+1 )))/4.0;
      gradient[2] = ((*(Iconv+txy)-*(Iconv-txy))+(*(Iconv+txy+1)-*(Iconv-txy+1)))/4.0;


      // Courbures principales
      norm_grad =  sqrt(gradient[0]*gradient[0] + 
                        gradient[1]*gradient[1] +
                        gradient[2]*gradient[2]);

      isotropic = TRUE;

      Si norm_grad > k*IsoCoeff Alors

        // Compute the Hessian Matrix in (x+1/2,y,z)
        Si x<tx-2 Alors
          hessien[0][0] = ((*(Iconv+2)-*Iconv)-(*(Iconv+1)-*(Iconv-1)))/2.0;
        Sinon
          hessien[0][0] = ((*(Iconv+1)-*Iconv)-(*(Iconv+1)-*(Iconv-1)))/2.0;
        FinSi

        hessien[1][0] = 
        hessien[0][1] = ((*(Iconv+1+tx)-*(Iconv+tx))-(*(Iconv+1-tx)-*(Iconv-tx)))/2.0;
        hessien[2][0] = 
        hessien[0][2] = ((*(Iconv+1+txy)-*(Iconv+txy))-(*(Iconv+1-txy)-*(Iconv-txy)))/2.0;

        hessien[1][1] = ((*(Iconv+tx)   - 2* *Iconv     + *(Iconv-tx  )) +
                         (*(Iconv+tx+1) - 2* *(Iconv+1) + *(Iconv-tx+1))
                )/2.0;
        hessien[2][1] = 

        hessien[1][2] = ( (*(Iconv+tx+txy)-*(Iconv-tx+txy)) -
                          (*(Iconv+tx-txy)-*(Iconv-tx-txy)) +
                          (*(Iconv+tx+txy+1)-*(Iconv-tx+txy+1)) -
                          (*(Iconv+tx-txy+1)-*(Iconv-tx-txy+1)) 
                )/8.0;

        hessien[2][2] = ( *(Iconv+txy  ) - 2*(*Iconv  ) + *(Iconv-txy  ) +
              *(Iconv+txy+1) - 2*(*Iconv+1) + *(Iconv-txy+1)
                )/2.0;

        // Compute the basis (e0, e1, e2):
        Si ::FluxDiffusion::CurvaturasPrincipales(  hessien, 
                 (float*) gradient, 
                 (float*) vmax, 
                 (float*) vmin, 
                 &lmax, &lmin,
                 1E-2) != -1 Alors
          e0.x = gradient[0]/norm_grad;
          e0.y = gradient[1]/norm_grad;
          e0.z = gradient[2]/norm_grad;
          e1.x = vmax[0]; e1.y = vmax[1]; e1.z = vmax[2];
          e2.x = vmin[0]; e2.y = vmin[1]; e2.z = vmin[2];
      isotropic = FALSE;
        Sinon
          fprintf(stderr,"CurvaturasPrincipales failed \n");
          fprintf(stderr,"  %d %d %d \n", x,y,z);
          fprintf(stderr," grad = %f %f %f \n", gradient[0], gradient[1], gradient[2]);
        FinSi

      FinSi

      Si Non(isotropic) Alors
      nbpts_anisotropic++;
      Sinon
      nbpts_isotropic++;
      FinSi

      // Directional Derivatives
      Si Non(isotropic) Alors
        u_e0 = grad.x*e0.x + grad.y*e0.y + grad.z*e0.z;
        u_e1 = grad.x*e1.x + grad.y*e1.y + grad.z*e1.z;
        u_e2 = grad.x*e2.x + grad.y*e2.y + grad.z*e2.z;

        alpha1_x = phi3D_0(u_e0)*e0.x*e0.x + 
                   phi3D_1(u_e1)*e1.x*e1.x +
                   phi3D_2(u_e2)*e2.x*e2.x;

        gamma1_x = (grad.y*e0.y + grad.z*e0.z)*phi3D_0(u_e0)*e0.x + 
                   (grad.y*e1.y + grad.z*e1.z)*phi3D_1(u_e1)*e1.x +
                   (grad.y*e2.y + grad.z*e2.z)*phi3D_2(u_e2)*e2.x;
      Sinon
        alpha1_x = phi3D_0(grad.x);
        gamma1_x = 0;
      FinSi


      //----- Calcul de alpha1_y, gamma1_y 
   
      // Gradient en (x,y+1/2)
      grad.x = (*(in   +1)-*(in   -1)+*(in   +1+tx)-*(in   -1+tx))/4.0;
      grad.y = *(in   +tx) - *(in);
      grad.z = (*(in   +txy)-*(in   -txy)+*(in   +txy+tx)-*(in   -txy+tx))/4.0;

      // Calcul du gradient, du hessien en (x,y+1/2,z)
      gradient[0] = ((*(Iconv+1)-*(Iconv-1))+(*(Iconv+1+tx)-*(Iconv-1+tx)))/4.0;
      gradient[1] = (*(Iconv+tx) - *Iconv      );
      gradient[2] = ((*(Iconv+txy)-*(Iconv-txy))+(*(Iconv+txy+tx)-*(Iconv-txy+tx)))/4.0;

      // Courbures principales
      norm_grad =  sqrt(gradient[0]*gradient[0] + 
                        gradient[1]*gradient[1] +
                        gradient[2]*gradient[2]);

      isotropic = TRUE;

      Si norm_grad > k*IsoCoeff Alors
    // Computing the Hessian matrix in (x,y+1/2,z)
        hessien[0][0] = (
                         *(Iconv+1   )-2*(*Iconv   )+*(Iconv-1   ) +
                 *(Iconv+1+tx)-2*(*Iconv+tx)+*(Iconv-1+tx) 
                )/2.0;

        hessien[1][0] = 
        hessien[0][1] = ((*(Iconv+1+tx)-*(Iconv-1+tx))-(*(Iconv+1)-*(Iconv-1)))/2.0;

        hessien[2][0] = 
        hessien[0][2] = (
                    (*(Iconv+1+txy)-*(Iconv-1+txy))-
                          (*(Iconv+1-txy)-*(Iconv-1-txy)) +
                  (*(Iconv+1+txy+tx)-*(Iconv-1+txy+tx))-
                          (*(Iconv+1-txy+tx)-*(Iconv-1-txy+tx)) 
                )/8.0;

        Si y<ty-2 Alors
          hessien[1][1] = ((*(Iconv+2*tx) - (*Iconv)) - (*(Iconv+tx)-*(Iconv-tx)))/2.0;
        Sinon
          hessien[1][1] = ((*(Iconv+tx) - (*Iconv)) - (*(Iconv+tx)-*(Iconv-tx)))/2.0;
        FinSi

        hessien[2][1] = 
        hessien[1][2] = ( (*(Iconv+tx+txy)-*(Iconv+txy)) -
                          (*(Iconv+tx-txy)-*(Iconv-txy))  )/2.0;

        hessien[2][2] = (
                   *(Iconv+txy)    - 2*(*Iconv   ) + *(Iconv-txy) +
                 *(Iconv+txy+tx) - 2*(*Iconv+tx) + *(Iconv-txy+tx)
                )/2.0;


        // Calcul de la base (e0, e1, e2):
        Si ::FluxDiffusion::CurvaturasPrincipales(  hessien, 
                 (float*) gradient, 
                 (float*) vmax, 
                 (float*) vmin, 
                 &lmax, &lmin,
                 1E-2) != -1 Alors
          e0.x = gradient[0]/norm_grad;
          e0.y = gradient[1]/norm_grad;
          e0.z = gradient[2]/norm_grad;
          e1.x = vmax[0]; e1.y = vmax[1]; e1.z = vmax[2];
          e2.x = vmin[0]; e2.y = vmin[1]; e2.z = vmin[2];
      isotropic = FALSE;
        Sinon
          fprintf(stderr,"CurvaturasPrincipales failed \n");
          fprintf(stderr,"  %d %d %d \n", x,y,z);
          fprintf(stderr," grad = %f %f %f \n", gradient[0], gradient[1], gradient[2]);
        FinSi
      FinSi

      Si Non(isotropic) Alors
      nbpts_anisotropic++;
      Sinon
      nbpts_isotropic++;
      FinSi

      // Directional Derivatives
      Si Non(isotropic) Alors
        u_e0 = grad.x*e0.x + grad.y*e0.y + grad.z*e0.z;
        u_e1 = grad.x*e1.x + grad.y*e1.y + grad.z*e1.z;
        u_e2 = grad.x*e2.x + grad.y*e2.y + grad.z*e2.z;

        alpha1_y = phi3D_0(u_e0)*e0.y*e0.y + 
                   phi3D_1(u_e1)*e1.y*e1.y +
                   phi3D_2(u_e2)*e2.y*e2.y;

        gamma1_y = (grad.x*e0.x + grad.z*e0.z)*phi3D_0(u_e0)*e0.y + 
                   (grad.x*e1.x + grad.z*e1.z)*phi3D_1(u_e1)*e1.y +
                   (grad.x*e2.x + grad.z*e2.z)*phi3D_2(u_e2)*e2.y;
      Sinon
        alpha1_y = phi3D_1(grad.y);
        gamma1_y = 0;
      FinSi


      //----- Calcul de alpha1_z, gamma1_z 
      grad.x = (*(in+1 ) - *(in-1 ) + *(in+1 +txy) - *(in-1 +txy))/4.0;
      grad.y = (*(in+tx) - *(in-tx) + *(in+tx+txy) - *(in-tx+txy))/4.0; 
      grad.z = *(in+txy)-*in;

      // Calcul du gradient, du hessien en (x,y,z+1/2)
      gradient[0] = ((*(Iconv+1)-*(Iconv-1))+(*(Iconv+1+txy)-*(Iconv-1+txy)))/4.0;
      gradient[1] = ((*(Iconv+tx)-*(Iconv-tx))+(*(Iconv+tx+txy)-*(Iconv-tx+txy)))/4.0;
      gradient[2] = (*(Iconv+txy)- *(Iconv)    );


      // Courbures principales
      norm_grad =  sqrt(gradient[0]*gradient[0] + 
                        gradient[1]*gradient[1] +
                        gradient[2]*gradient[2]);

      isotropic = TRUE;

      Si norm_grad > k*IsoCoeff Alors

    // Computing the Hessian matrix in (x,y,z+1/2)
        hessien[0][0] = ( *(Iconv+1)    -2*(*Iconv    )+*(Iconv-1    ) +
               *(Iconv+1+txy)-2*(*Iconv+txy)+*(Iconv-1+txy)
                )/2.0;

        hessien[1][0] = 
        hessien[0][1] = (
                 (*(Iconv+1+tx    )-*(Iconv-1+tx    ))-
                 (*(Iconv+1-tx    )-*(Iconv-1-tx    )) +
                 (*(Iconv+1+tx+txy)-*(Iconv-1+tx+txy))-
                 (*(Iconv+1-tx+txy)-*(Iconv-1-tx+txy)) 
                )/8.0;

 
        hessien[2][0] = 
        hessien[0][2] = ((*(Iconv+1+txy)-*(Iconv-1+txy))-
                           (*(Iconv+1    )-*(Iconv-1    )) )/2.0;

        hessien[1][1] = ( 
                   *(Iconv+tx) - 2*(*Iconv) + *(Iconv-tx) +
                 *(Iconv+tx+txy) - 2*(*Iconv+txy) + *(Iconv-tx+txy)
                 )/2.0;

        hessien[2][1] = 
        hessien[1][2] = ( (*(Iconv+tx+txy)-*(Iconv-tx+txy)) -
                          (*(Iconv+tx    )-*(Iconv-tx    ))  )/2.0;

  
        Si z<tz-2 Alors
          hessien[2][2] = ((*(Iconv+2*txy)-*(Iconv    )) - 
                           (*(Iconv+  txy)-*(Iconv-txy))  )/2.0;
        Sinon
          hessien[2][2] = ((*(Iconv+txy)-*(Iconv    )) - 
                         (*(Iconv+txy)-*(Iconv-txy))  )/2.0;
        FinSi

        // Calcul de la base (e0, e1, e2):
        Si ::FluxDiffusion::CurvaturasPrincipales(  hessien, 
                 (float*) gradient, 
                 (float*) vmax, 
                 (float*) vmin, 
                 &lmax, &lmin,
                 1E-2) != -1 Alors
          e0.x = gradient[0]/norm_grad;
          e0.y = gradient[1]/norm_grad;
          e0.z = gradient[2]/norm_grad;
          e1.x = vmax[0]; e1.y = vmax[1]; e1.z = vmax[2];
          e2.x = vmin[0]; e2.y = vmin[1]; e2.z = vmin[2];
      isotropic = FALSE;
        Sinon
          fprintf(stderr,"CurvaturasPrincipales failed \n");
          fprintf(stderr,"  %d %d %d \n", x,y,z);
          fprintf(stderr," grad = %f %f %f \n", gradient[0], gradient[1], gradient[2]);
        FinSi
      FinSi

      // Derivees directionnelles
      Si Non(isotropic) Alors
      nbpts_anisotropic++;
      Sinon
      nbpts_isotropic++;
      FinSi

      // Directional Derivatives
      Si Non(isotropic) Alors
        u_e0 = grad.x*e0.x + grad.y*e0.y + grad.z*e0.z;
        u_e1 = grad.x*e1.x + grad.y*e1.y + grad.z*e1.z;
        u_e2 = grad.x*e2.x + grad.y*e2.y + grad.z*e2.z;

        alpha1_z = phi3D_0(u_e0)*e0.z*e0.z + 
                   phi3D_1(u_e1)*e1.z*e1.z +
                   phi3D_2(u_e2)*e2.z*e2.z;

        gamma1_z = (grad.x*e0.x + grad.y*e0.y)*phi3D_0(u_e0)*e0.z + 
                   (grad.x*e1.x + grad.y*e1.y)*phi3D_1(u_e1)*e1.z +
                   (grad.x*e2.x + grad.y*e2.y)*phi3D_2(u_e2)*e2.z;
      Sinon
    alpha1_z = phi3D_2(grad.z);
        gamma1_z = 0;
      FinSi


      //----- Mise a jour de l'image

      val1    =  beta**((float*)(this->im_tmp1->GetScalarPointer(x,y,z)));
      val1div =  beta;

      val1    += alpha1_x    * (*(in+1  )) +
                 _alpha_x     * (*(in-1  )) +
                 gamma1_x  - _gamma_x;

      val1div += alpha1_x + _alpha_x;

      val1    += alpha1_y     * (*(in+tx )) +
                 _alpha_y[x]   * (*(in-tx )) +
                 gamma1_y  - _gamma_y[x];

      val1div += alpha1_y + _alpha_y[x];

      val1    += alpha1_z      * (*(in+txy  )) +
                 _alpha_z[x][y] * (*(in-txy  )) +
                 gamma1_z  - _gamma_z[x][y];

      val1div += alpha1_z + _alpha_z[x][y];

      Si fabs(val1div)>1E-4 Alors
        val1 /= val1div;
      Sinon
        fprintf(stderr,"AnisoGaussSeidel.c:Itere3D() \t fabs(val1div)<1E-4 ");
        fprintf(stderr,"%d %d %d \n",x,y,z);
        val1 = *in;
      FinSi

      _alpha_z[x][y] = alpha1_z;
      _alpha_y[x]    = alpha1_y;
      _alpha_x       = alpha1_x;

      _gamma_z[x][y] = gamma1_z;
      _gamma_y[x]    = gamma1_y;
      _gamma_x       = gamma1_x;

    Sinon

      val1 = *in;
    
      _alpha_z[x][y] = 
      _alpha_y[x]    = 
      _alpha_x       = 
      _gamma_z[x][y] = 
      _gamma_y[x]    = 
      _gamma_x       = 0;

    FinSi


    Si fabs(val1-val0) > epsilon Alors
      nb_points_instables++;
    FinSi

    diff += (val1-val0)*(val1-val0);
    Si fabs(val1-val0) > erreur Alors
      erreur = fabs(val1-val0);
      erreur_x = x;
      erreur_y = y;
      erreur_z = z;
    FinSi

    *((float*)(im_tmp2->GetScalarPointer(x,y,z))) = val1;

    in++;
    Iconv++;

  FinPour
  FinPour
  FinPour

    /*
  image_resultat->CopyAndCastFrom(this->im_tmp,
                  this->im_tmp->GetExtent());
    */

    /*
  fprintf(stderr," Pourcentage of isotropic points = %2.5f \n",
      nbpts_isotropic/(1.0*(nbpts_isotropic+nbpts_anisotropic))*100.0);
  cout << endl;
  cout << " Erreur = " << erreur << endl;
  cout << "( " << erreur_x << ", " 
               << erreur_y << ", " 
               << erreur_z << " )" << endl; 
  cout << "nb de points variables " << nb_points_instables << endl;

  diff /= txy*tz;

  cout << "diff =" << sqrt(diff) << endl;
    */

  Retourne erreur;


} // Iterate3D()


//----------------------------------------------------------------------
float vtkAnisoGaussSeidel::Iterate3D( vtkImageData *inData,  int inExt[6],
                      vtkImageData *outData, int outExt[6])
{

  Local 
    register Entier x,y,z; 
  //    Entier x1,y1,z1;
    Reel   val0,val1;
    Reel   val1div;
    Reel   u_e0;
    Reel   u_e1;
    Reel   u_e2;
    Reel   alpha1_x, gamma1_x;
    Reel   alpha1_y, gamma1_y;
    Reel   alpha1_z, gamma1_z;

    float           alpha_x;
    float           gamma_x;
    float*          alpha_y;
    float*          gamma_y;
    float**         alpha_z;
    float**         gamma_z;

    Reel*  in;
    Registre Reel    *Iconv; 
    Reel gradient[3];
    Reel hessien[3][3];
    float vmax[3];
    float vmin[3];
    float lmin;
    float lmax;

    point3D e0;
    point3D e1;
    point3D e2;

    vector3D grad;

    Reel   erreur;
    Reel   norm_grad;
    double diff;
    Entier nb_points_instables;
    Entier erreur_x,erreur_y,erreur_z;

    unsigned long nbpts_isotropic    = 0;
    unsigned long nbpts_anisotropic  = 0;

    unsigned char isotropic;
    int           *wholeExtent;
    int           sx,sy,sz;
    register int           x1,y1,z1;
    register int           xp,xm;
    register int           yp,ym;
    register int           zp,zm;


    wholeExtent = this->GetInput()->GetWholeExtent();

    /*
    printf("Iterate3D outext: %d %d %d %d %d %d \n",
       outExt[0],outExt[1],outExt[2],outExt[3],outExt[4],outExt[5]
       );
    */


  InitCoefficients(alpha_x,alpha_y,alpha_z,
           gamma_x,gamma_y,gamma_z,
           inExt[1]-inExt[0]+1, 
           inExt[3]-inExt[2]+1);

  //  ResetCoefficients(outExt);

  erreur = 0.0;

  nb_points_instables = 0;

  diff = 0;

  sx = inExt[1]-inExt[0]+1;
  sy = inExt[3]-inExt[2]+1;
  sz = inExt[5]-inExt[4]+1;

  Pour(z,0,sz-1)

    z1 = z+inExt[4];
    zm = -txy;
    if (z1==0)    zm = 0;
    zp =  txy;
    if (z1==tz-1) zp = 0;


    partial_progress += 1.0*(outExt[1]-outExt[0]+1)*(outExt[3]-outExt[2]+1)*(outExt[5]-outExt[4]+1)
                        /(inExt[5]-inExt[4]+1.0);
    if ((partial_progress > target)&&(!update_busy)) {
      update_busy = 1;
      while (partial_progress > target)
    {     
      partial_progress -= target;
      progress         += target;
    }
      //      printf("progress %f \n", progress/total);
      this->UpdateProgress( progress / total);
      update_busy = 0;
    }

  Pour(y,0,sy-1)

  y1 = y+inExt[2];
  ym = -tx;
  if (y1==0)    ym = 0;
  yp =  tx;
  if (y1==ty-1) yp = 0;

  in    = (float*) inData             ->GetScalarPointer(inExt[0],y1,z1);
  Iconv = (float*) this->image_lissee ->GetScalarPointer(inExt[0],y1,z1);

  Pour(x,0,sx-1)

    x1 = x+inExt[0];
    xm = -1;
    if (x1==0)     xm = 0;
    xp =  1;
    if (x1==tx-1)  xp = 0;
  
    val1 = val0 = *in;

  //    printf("%d %d %d; 1 \n",x,y,z);

  // I should compute the 27 neighborhood for all cases ...

    //    Si x1>wholeExtent[0] Et x1<wholeExtent[1] Et 
    //       y1>wholeExtent[2] Et y1<wholeExtent[3] Et 
    //       z1>wholeExtent[4] Et z1<wholeExtent[5]
    //    Alors

      //      printf("%d %d %d; 1 \n",x,y,z);
      // Gradient en (x+1/2,y,z)
      //----- Calcul de alpha1_x, gamma1_x
      grad.x = *(in+xp) - *(in);
      grad.y = ( *(in+yp) - *(in+ym) + *(in+yp+xp) - *(in+ym+xp) )/ 4.0;
      grad.z = (*(in+zp)   - *(in+zm  )+ *(in+zp+xp)- *(in+zm+xp))/ 4.0;

      // Calcul du gradient, du hessien en (x+1/2,y,z)
      gradient[0] = *(Iconv+xp) - *Iconv;
      gradient[1] = ((*(Iconv+yp)-*(Iconv+ym ))+(*(Iconv+yp+xp )-*(Iconv+ym+xp )))/4.0;
      gradient[2] = ((*(Iconv+zp)-*(Iconv+zm))+(*(Iconv+zp+xp)-*(Iconv+zm+xp)))/4.0;


      // Courbures principales
      norm_grad =  sqrt(gradient[0]*gradient[0] + 
                        gradient[1]*gradient[1] +
                        gradient[2]*gradient[2]);

      isotropic = TRUE;

      Si norm_grad > k*IsoCoeff Alors

        // Compute the Hessian Matrix in (x+1/2,y,z)
        Si x1<tx-2 Alors
          hessien[0][0] = ((*(Iconv+2)-*Iconv)-(*(Iconv+xp)-*(Iconv+xm)))/2.0;
        Sinon
          hessien[0][0] = ((*(Iconv+xp)-*Iconv)-(*(Iconv+xp)-*(Iconv+xm)))/2.0;
        FinSi

        hessien[1][0] = 
        hessien[0][1] = ((*(Iconv+xp+yp)-*(Iconv+yp))-(*(Iconv+xp+ym)-*(Iconv+ym)))/2.0;
        hessien[2][0] = 
        hessien[0][2] = ((*(Iconv+xp+zp)-*(Iconv+zp))-(*(Iconv+xp+zm)-*(Iconv+zm)))/2.0;

        hessien[1][1] = ((*(Iconv+yp)   - 2* *Iconv     + *(Iconv+ym  )) +
                         (*(Iconv+yp+xp) - 2* *(Iconv+xp) + *(Iconv+ym+xp))
                )/2.0;
        hessien[2][1] = 

        hessien[1][2] = ( (*(Iconv+yp+zp)-*(Iconv+ym+zp)) -
                          (*(Iconv+yp+zm)-*(Iconv+ym+zm)) +
                          (*(Iconv+yp+zp+xp)-*(Iconv+ym+zp+xp)) -
                          (*(Iconv+yp+zm+xp)-*(Iconv+ym+zm+xp)) 
                )/8.0;

        hessien[2][2] = ( *(Iconv+zp  ) - 2*(*Iconv  ) + *(Iconv+zm  ) +
              *(Iconv+zp+xp) - 2*(*Iconv+xp) + *(Iconv+zm+xp)
                )/2.0;

        // Compute the basis (e0, e1, e2):
        Si ::FluxDiffusion::CurvaturasPrincipales(  hessien, 
                 (float*) gradient, 
                 (float*) vmax, 
                 (float*) vmin, 
                 &lmax, &lmin,
                 1E-2) != -1 Alors
          e0.x = gradient[0]/norm_grad;
          e0.y = gradient[1]/norm_grad;
          e0.z = gradient[2]/norm_grad;
          e1.x = vmax[0]; e1.y = vmax[1]; e1.z = vmax[2];
          e2.x = vmin[0]; e2.y = vmin[1]; e2.z = vmin[2];
      isotropic = FALSE;
        Sinon
          fprintf(stderr,"CurvaturasPrincipales failed \n");
          fprintf(stderr,"  %d %d %d \n", x,y,z);
          fprintf(stderr," grad = %f %f %f \n", gradient[0], gradient[1], gradient[2]);
        FinSi

      FinSi

      Si Non(isotropic) Alors
      nbpts_anisotropic++;
      Sinon
      nbpts_isotropic++;
      FinSi

      // Directional Derivatives
      Si Non(isotropic) Alors
        u_e0 = grad.x*e0.x + grad.y*e0.y + grad.z*e0.z;
        u_e1 = grad.x*e1.x + grad.y*e1.y + grad.z*e1.z;
        u_e2 = grad.x*e2.x + grad.y*e2.y + grad.z*e2.z;

        alpha1_x = phi3D_0(u_e0)*e0.x*e0.x + 
                   phi3D_1(u_e1)*e1.x*e1.x +
                   phi3D_2(u_e2)*e2.x*e2.x;

        gamma1_x = (grad.y*e0.y + grad.z*e0.z)*phi3D_0(u_e0)*e0.x + 
                   (grad.y*e1.y + grad.z*e1.z)*phi3D_1(u_e1)*e1.x +
                   (grad.y*e2.y + grad.z*e2.z)*phi3D_2(u_e2)*e2.x;
      Sinon
        alpha1_x = phi3D_0(grad.x);
        gamma1_x = 0;
      FinSi


      //----- Calcul de alpha1_y, gamma1_y 
   
    //      printf("%d %d %d; 2 \n",x,y,z);
      // Gradient en (x,y+1/2)
      grad.x = (*(in+xp)-*(in+xm)+*(in+xp+yp)-*(in+xm+yp))/4.0;
      grad.y = *(in+yp) - *(in);
      grad.z = (*(in+zp)-*(in+zm)+*(in+zp+yp)-*(in+zm+yp))/4.0;

      // Calcul du gradient, du hessien en (x,y+1/2,z)
      gradient[0] = ((*(Iconv+xp)-*(Iconv+xm))+(*(Iconv+xp+yp)-*(Iconv+xm+yp)))/4.0;
      gradient[1] = (*(Iconv+yp) - *Iconv      );
      gradient[2] = ((*(Iconv+zp)-*(Iconv+zm))+(*(Iconv+zp+yp)-*(Iconv+zm+yp)))/4.0;

      // Courbures principales
      norm_grad =  sqrt(gradient[0]*gradient[0] + 
                        gradient[1]*gradient[1] +
                        gradient[2]*gradient[2]);

      isotropic = TRUE;

      Si norm_grad > k*IsoCoeff Alors
    // Computing the Hessian matrix in (x,y+1/2,z)
        hessien[0][0] = (
                         *(Iconv+xp   )-2*(*Iconv   )+*(Iconv+xm   ) +
                 *(Iconv+xp+yp)-2*(*Iconv+yp)+*(Iconv+xm+yp) 
                )/2.0;

        hessien[1][0] = 
        hessien[0][1] = ((*(Iconv+xp+yp)-*(Iconv+xm+yp))-(*(Iconv+xp)-*(Iconv+xm)))/2.0;


        hessien[2][0] = 
        hessien[0][2] = (
                    (*(Iconv+xp+zp)-*(Iconv+xm+zp))-
                          (*(Iconv+xp+zm)-*(Iconv+xm+zm)) +
                  (*(Iconv+xp+zp+yp)-*(Iconv+xm+zp+yp))-
                          (*(Iconv+xp+zm+yp)-*(Iconv+xm+zm+yp)) 
                )/8.0;

        Si y1<ty-2 Alors
          hessien[1][1] = ((*(Iconv+2*tx) - (*Iconv)) - (*(Iconv+yp)-*(Iconv+ym)))/2.0;
        Sinon
          hessien[1][1] = ((*(Iconv+yp) - (*Iconv)) - (*(Iconv+yp)-*(Iconv+ym)))/2.0;
        FinSi

        hessien[2][1] = 
        hessien[1][2] = ( (*(Iconv+yp+zp)-*(Iconv+zp)) -
                          (*(Iconv+yp+zm)-*(Iconv+zm))  )/2.0;

        hessien[2][2] = (
                   *(Iconv+zp)    - 2*(*Iconv   ) + *(Iconv+zm) +
                 *(Iconv+zp+yp) - 2*(*Iconv+yp) + *(Iconv+zm+yp)
                )/2.0;


        // Calcul de la base (e0, e1, e2):
        Si ::FluxDiffusion::CurvaturasPrincipales(  hessien, 
                 (float*) gradient, 
                 (float*) vmax, 
                 (float*) vmin, 
                 &lmax, &lmin,
                 1E-2) != -1 Alors
          e0.x = gradient[0]/norm_grad;
          e0.y = gradient[1]/norm_grad;
          e0.z = gradient[2]/norm_grad;
          e1.x = vmax[0]; e1.y = vmax[1]; e1.z = vmax[2];
          e2.x = vmin[0]; e2.y = vmin[1]; e2.z = vmin[2];
      isotropic = FALSE;
        Sinon
          fprintf(stderr,"CurvaturasPrincipales failed \n");
          fprintf(stderr,"  %d %d %d \n", x,y,z);
          fprintf(stderr," grad = %f %f %f \n", gradient[0], gradient[1], gradient[2]);
        FinSi
      FinSi

      Si Non(isotropic) Alors
      nbpts_anisotropic++;
      Sinon
      nbpts_isotropic++;
      FinSi

      // Directional Derivatives
      Si Non(isotropic) Alors
        u_e0 = grad.x*e0.x + grad.y*e0.y + grad.z*e0.z;
        u_e1 = grad.x*e1.x + grad.y*e1.y + grad.z*e1.z;
        u_e2 = grad.x*e2.x + grad.y*e2.y + grad.z*e2.z;

        alpha1_y = phi3D_0(u_e0)*e0.y*e0.y + 
                   phi3D_1(u_e1)*e1.y*e1.y +
                   phi3D_2(u_e2)*e2.y*e2.y;

        gamma1_y = (grad.x*e0.x + grad.z*e0.z)*phi3D_0(u_e0)*e0.y + 
                   (grad.x*e1.x + grad.z*e1.z)*phi3D_1(u_e1)*e1.y +
                   (grad.x*e2.x + grad.z*e2.z)*phi3D_2(u_e2)*e2.y;
      Sinon
        alpha1_y = phi3D_1(grad.y);
        gamma1_y = 0;
      FinSi


    //      printf("%d %d %d; 3 \n",x,y,z);
      //----- Calcul de alpha1_z, gamma1_z 
      grad.x = (*(in+xp) - *(in+xm) + *(in+xp+zp) - *(in+xm+zp))/4.0;
      grad.y = (*(in+yp) - *(in+ym) + *(in+yp+zp) - *(in+ym+zp))/4.0; 
      grad.z = *(in+zp)-*in;

      // Calcul du gradient, du hessien en (x,y,z+1/2)
      gradient[0] = ((*(Iconv+xp)-*(Iconv+xm))+(*(Iconv+xp+zp)-*(Iconv+xm+zp)))/4.0;
      gradient[1] = ((*(Iconv+yp)-*(Iconv+ym))+(*(Iconv+yp+zp)-*(Iconv+ym+zp)))/4.0;
      gradient[2] = (*(Iconv+zp)- *(Iconv)    );


      // Courbures principales
      norm_grad =  sqrt(gradient[0]*gradient[0] + 
                        gradient[1]*gradient[1] +
                        gradient[2]*gradient[2]);

      isotropic = TRUE;

      Si norm_grad > k*IsoCoeff Alors

    // Computing the Hessian matrix in (x,y,z+1/2)
        hessien[0][0] = ( *(Iconv+xp)    -2*(*Iconv    )+*(Iconv+xm    ) +
               *(Iconv+xp+zp)-2*(*Iconv+zp)+*(Iconv+xm+zp)
                )/2.0;

        hessien[1][0] = 
        hessien[0][1] = (
                 (*(Iconv+xp+yp   )-*(Iconv+xm+yp   ))-
                 (*(Iconv+xp+ym   )-*(Iconv+xm+ym   )) +
                 (*(Iconv+xp+yp+zp)-*(Iconv+xm+yp+zp))-
                 (*(Iconv+xp+ym+zp)-*(Iconv+xm+ym+zp)) 
                )/8.0;

 
        hessien[2][0] = 
        hessien[0][2] = ((*(Iconv+xp+zp)-*(Iconv+xm+zp))-
                         (*(Iconv+xp   )-*(Iconv+xm    )) )/2.0;

        hessien[1][1] = ( 
                   *(Iconv+yp) - 2*(*Iconv) + *(Iconv+ym) +
                 *(Iconv+yp+zp) - 2*(*Iconv+zp) + *(Iconv+ym+zp)
                 )/2.0;

        hessien[2][1] = 
        hessien[1][2] = ( (*(Iconv+yp+zp)-*(Iconv+ym+zp)) -
                          (*(Iconv+yp   )-*(Iconv+ym    ))  )/2.0;

  
        Si z1<tz-2 Alors
          hessien[2][2] = ((*(Iconv+2*txy)-*(Iconv    )) - 
                           (*(Iconv+  txy)-*(Iconv+zm))  )/2.0;
        Sinon
          hessien[2][2] = ((*(Iconv+zp)-*(Iconv    )) - 
                         (*(Iconv+zp)-*(Iconv+zm))  )/2.0;
        FinSi

        // Calcul de la base (e0, e1, e2):
        Si ::FluxDiffusion::CurvaturasPrincipales(  hessien, 
                 (float*) gradient, 
                 (float*) vmax, 
                 (float*) vmin, 
                 &lmax, &lmin,
                 1E-2) != -1 Alors
          e0.x = gradient[0]/norm_grad;
          e0.y = gradient[1]/norm_grad;
          e0.z = gradient[2]/norm_grad;
          e1.x = vmax[0]; e1.y = vmax[1]; e1.z = vmax[2];
          e2.x = vmin[0]; e2.y = vmin[1]; e2.z = vmin[2];
      isotropic = FALSE;
        Sinon
          fprintf(stderr,"CurvaturasPrincipales failed \n");
          fprintf(stderr,"  %d %d %d \n", x,y,z);
          fprintf(stderr," grad = %f %f %f \n", gradient[0], gradient[1], gradient[2]);
        FinSi
      FinSi

      // Derivees directionnelles
      Si Non(isotropic) Alors
      nbpts_anisotropic++;
      Sinon
      nbpts_isotropic++;
      FinSi

      // Directional Derivatives
      Si Non(isotropic) Alors
        u_e0 = grad.x*e0.x + grad.y*e0.y + grad.z*e0.z;
        u_e1 = grad.x*e1.x + grad.y*e1.y + grad.z*e1.z;
        u_e2 = grad.x*e2.x + grad.y*e2.y + grad.z*e2.z;

        alpha1_z = phi3D_0(u_e0)*e0.z*e0.z + 
                   phi3D_1(u_e1)*e1.z*e1.z +
                   phi3D_2(u_e2)*e2.z*e2.z;

        gamma1_z = (grad.x*e0.x + grad.y*e0.y)*phi3D_0(u_e0)*e0.z + 
                   (grad.x*e1.x + grad.y*e1.y)*phi3D_1(u_e1)*e1.z +
                   (grad.x*e2.x + grad.y*e2.y)*phi3D_2(u_e2)*e2.z;
      Sinon
    alpha1_z = phi3D_2(grad.z);
        gamma1_z = 0;
      FinSi


      //----- Mise a jour de l'image

      Si x==0 Alors
    gamma_x = 0;
        alpha_x = 1;
      FinSi
      Si y == 0 Alors
    gamma_y[x] = 0;
        alpha_y[x] = 1;
      FinSi
      Si z == 0 Alors
    gamma_z[x][y] = 0;
        alpha_z[x][y] = 1;
      FinSi

    //      Si x>0 Et y>0 Et z>0  Alors

        val1    =  beta**((float*)(this->im_tmp1->GetScalarPointer(x1,y1,z1)));
        val1div =  beta;

        val1    += alpha1_x    * (*(in+xp  )) +
                   alpha_x     * (*(in+xm  )) +
                   gamma1_x  - gamma_x;

        val1div += alpha1_x + alpha_x;
  
        val1    += alpha1_y     * (*(in+yp )) +
                   alpha_y[x]   * (*(in+ym )) +
                   gamma1_y  - gamma_y[x];

        val1div += alpha1_y + alpha_y[x];

        val1    += alpha1_z      * (*(in+zp  )) +
                   alpha_z[x][y] * (*(in+zm  )) +
                   gamma1_z  - gamma_z[x][y];

        val1div += alpha1_z + alpha_z[x][y];
    
        Si fabs(val1div)>1E-4 Alors
          val1 /= val1div;
        Sinon
          fprintf(stderr,"AnisoGaussSeidel.c:Itere3D() \t fabs(val1div)<1E-4 ");
          fprintf(stderr,"%d %d %d \n",x1,y1,z1);
          val1 = *in;
        FinSi

      //      Sinon

      //         val1 = *in;
    
      //      FinSi

      alpha_z[x][y] = alpha1_z;
      alpha_y[x]    = alpha1_y;
      alpha_x       = alpha1_x;

      gamma_z[x][y] = gamma1_z;
      gamma_y[x]    = gamma1_y;
      gamma_x       = gamma1_x;

      //    Sinon

      //      alpha_z[x][y] = 
      //      alpha_y[x]    = 
      //      alpha_x       = phi3D_1(0);

      //      gamma_z[x][y] = 
      //      gamma_y[x]    = 
      //      gamma_x       = 0;

      //    FinSi


    Si fabs(val1-val0) > epsilon Alors
      nb_points_instables++;
    FinSi

    diff += (val1-val0)*(val1-val0);
    Si fabs(val1-val0) > erreur Alors
      erreur = fabs(val1-val0);
      erreur_x = x;
      erreur_y = y;
      erreur_z = z;
    FinSi

    *((float*)(outData->GetScalarPointer(x1,y1,z1))) = val1;

    in++;
    Iconv++;

  FinPour
  FinPour
  FinPour


    /*    
  fprintf(stderr," Pourcentage of isotropic points = %2.5f \n",
      nbpts_isotropic/(1.0*(nbpts_isotropic+nbpts_anisotropic))*100.0);
  cout << endl;
  cout << " Erreur = " << erreur << endl;
  cout << "( " << erreur_x << ", " 
               << erreur_y << ", " 
               << erreur_z << " )" << endl; 
  cout << "nb de points variables " << nb_points_instables << endl;

  diff /= txy*tz;

  cout << "diff =" << sqrt(diff) << endl;
    */

  DeleteCoefficients(alpha_y,alpha_z,
             gamma_y,gamma_z,
             inExt[1]-inExt[0]+1);

  Retourne erreur;


} // Iterate3D(in,out,outExt)


//----------------------------------------------------------------------
void vtkAnisoGaussSeidel::ExecuteData(vtkDataObject *out)
{
  
  Local
    float error;
    int   i;
    char resname[100];
    float min,max;
    float* tmp2_ptr;
    char progresstext[100];

    //    fprintf(stderr,"vtkAnisoGaussSeidel::Execute() \n");

    printf("vtkAnisoGaussSeidel::Execute() \n");

    
  Init();

  // This should be put as an option ...

  if (NumberOfIterations < 1) NumberOfIterations = 1; 

  filter = (vtkImageGaussianSmooth*) vtkImageGaussianSmooth::New();

  total = (image_entree->GetExtent()[1] - image_entree->GetExtent()[0] + 1) 
    * (image_entree->GetExtent()[3] - image_entree->GetExtent()[2] + 1) 
    * (image_entree->GetExtent()[5] - image_entree->GetExtent()[4] + 1);
  
  total *= NumberOfIterations;

  target           = total/100.0;
  partial_progress = 0;
  progress         = 0;
  update_busy      = 0;

  for(i=1; i<=NumberOfIterations; i++) {

    sprintf(progresstext," Flux Diffusion %3d ",i);
    this->SetProgressText(progresstext);
    im_tmp1->Modified();
    filter->SetInput(im_tmp1);
    // 1. compute the smoothed image
    switch (mode) {
    case MODE_2D:
      filter->SetDimensionality(2);
      filter->SetStandardDeviations(sigma,sigma,0);
      filter->SetRadiusFactors(3.01,3.01,0);
      break;
    case MODE_3D:
      filter->SetDimensionality(3);
      //      printf("sigma = %f \n",sigma);
      filter->SetStandardDeviations(sigma,sigma,sigma);
      filter->SetRadiusFactors(4.01,4.01,4.01);
      break;
    }
    
    filter->SetNumberOfThreads(this->GetNumberOfThreads());
    filter->Update();
    image_lissee = filter->GetOutput();
    
    
    // 2. run the threaded iteration
    
    //   Iterate3D();
    vtkImageToImageFilter::MultiThread(im_tmp2,im_tmp1);
    
    im_tmp2->CopyAndCastFrom(im_tmp1,
                 im_tmp1->GetExtent());
    
    
  } // end for
  
  filter->Delete();

  // Check the limits
  tmp2_ptr = (float*) im_tmp2->GetScalarPointer(0,0,0);
  min = image_resultat->GetScalarTypeMin();
  if ((TruncNegValues)&&(min<0))  min = 0;
  max = image_resultat->GetScalarTypeMax();

  for(i=0;i<txy*tz;i++) {
    if (*tmp2_ptr<min) *tmp2_ptr = min;
    if (*tmp2_ptr>max) *tmp2_ptr = max;
    tmp2_ptr++;
  }

  image_resultat->CopyAndCastFrom(im_tmp2,
                  im_tmp2->GetExtent());

    

} // vtkAnisoGaussSeidel::Execute()


//----------------------------------------------------------------------------
void vtkAnisoGaussSeidel::ComputeInputUpdateExtent(int inExt[6], 
                           int outExt[6])
{
  int *wholeExtent;
  int idx, border;

  border = 2;

  // copy
  memcpy((void *)inExt, (void *)outExt, 6 * sizeof(int));
  // Expand filtered axes
  wholeExtent = this->GetInput()->GetWholeExtent();
  for (idx = 0; idx < 3; ++idx)
    {
      //    border = (int)(this->StandardDeviations[idx] * this->BorderFactors[idx]);
    inExt[idx*2] -= border;
    if (inExt[idx*2] < wholeExtent[idx*2])
      {
      inExt[idx*2] = wholeExtent[idx*2];
      }

    inExt[idx*2+1] += border;
    if (inExt[idx*2+1] > wholeExtent[idx*2+1])
      {
      inExt[idx*2+1] = wholeExtent[idx*2+1];
      }
    }
}

//----------------------------------------------------------------------
void vtkAnisoGaussSeidel::ThreadedExecute(vtkImageData *inData, 
             vtkImageData *outData, int outExt[6], int id)
{
  int inExt[6];
  float error;

  //  printf("begin %d ",id);fflush(stdout);

  this->ComputeInputUpdateExtent(inExt, outExt);


  switch (mode) {
  case MODE_2D:
    //    error = Iterate2D(inData, inExt, outData, outExt, 
    //              &cycle, target, &count, total);
    break;
  case MODE_3D:
    error = Iterate3D(inData, inExt, outData, outExt);
    break;
  }

  //  printf("End  %d ",id);fflush(stdout);

} // ThreadedExecute()


//----------------------------------------------------------------------
void vtkAnisoGaussSeidel::PrintSelf(ostream& os, vtkIndent indent)
{
  // Nothing for the moment ...
}

