#ifdef _WIN32
#define _USE_MATH_DEFINES
#endif

#include <stdio.h>
#include <stdlib.h>

#include <math.h>
#include "vtkObjectFactory.h"

#include "vtkFastMarching.h"

#include "FMpdf.cxx"

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////

vtkFastMarching* vtkFastMarching::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkFastMarching");
  if(ret)
    {
      return (vtkFastMarching*)ret;
    }

  // If the factory was unable to create the object, then create it here.
  return new vtkFastMarching;
}

void vtkFastMarching::collectInfoSeed( int index )
{
  for(int ip=-1;ip<=1;ip++)
    for(int jp=-1;jp<=1;jp++)
      for(int kp=-1;kp<=1;kp++)
        {
          int neiIndex = index + ip + jp*dimY + kp*dimXY;

      int med, inh;
      getMedianInhomo(neiIndex, med, inh);

          pdfIntensityIn->addRealization( med );
          pdfInhomoIn->addRealization( inh );
        }
}

// speed at index
float vtkFastMarching::speed( int index )
{
  int I;
  int H;

  getMedianInhomo( index, I, H );

  if( (I<0) || (I>depth) || (H<0) || (H>depth) )
    {
      cerr << "Error in vtkFastMarching::speed(index)!" << endl
       << "index=" << index << " I=" << I << " H=" << H 
       << " depth=" << depth << endl;
    }

  return min(pdfIntensityIn->value(I)/pdfIntensityAll->value(I),
         pdfInhomoIn->value(H)/pdfInhomoAll->value(H));
}

void vtkFastMarching::setSeed( int index )
{
  assert( (index>=(1+dimX+dimXY)) && (index<(dimXYZ-1-dimX-dimXY)) );

  if( node[index].status!=fmsFAR )
    // this seed has already been planted
    return;

  // by definition, T=0, and that voxel is known
  node[index].T=0.0;
  node[index].status=fmsKNOWN;

  knownPoints[nEvolutions].push_back(index);

  // add all FAR neighbors to trial band
  for(int nei=1;nei<=nNeighbors;nei++)
    {
      FMleaf f;
      f.nodeIndex=index+shiftNeighbor(nei);

      if( node[f.nodeIndex].status == fmsFAR )
    {
      node[f.nodeIndex].status=fmsTRIAL;
      node[f.nodeIndex].T=computeT(f.nodeIndex);
      insert( f ); // insert in minheap
    }
    }
}

int compareInt(const void *a, const void *b)
{
  return  (*(int*)a) >= (*(int*)b);
}

inline void vtkFastMarching::getMedianInhomo( int index, int &med, int &inh )
{
  // assert( (index>=(1+dimX+dimXY)) && (index<(dimXYZ-1-dimX-dimXY)) );

  inh = inhomo[index];
  if( inh != (-1) )
    // then the values have already been computed
    {
      med = median[index]; 
      return;
    }

  // otherwise, just do it
  for(int k=0;k<27;k++)
    tmpNeighborhood[k] = (int)indata[index + arrayShiftNeighbor[k]];

  qsort( (void*)tmpNeighborhood, 27, sizeof(int), &compareInt );
      
  inh = inhomo[ index ] = (tmpNeighborhood[21] - tmpNeighborhood[5]);
  med = median[ index ] = tmpNeighborhood[13];

  return;
}

void vtkFastMarchingExecute(vtkFastMarching *self,
                vtkImageData *inData, short *inPtr,
                vtkImageData *outData, short *outPtr, 
                int outExt[6])
{
  int n=0;

  self->setInData( (short *)inPtr );
  self->setOutData( (short *)outPtr );

  if( !self->initialized )
    {
      self->initialized = true;

      // make sure that there is no 0 in these pdf
      // because we will divide by these 
      for(int r=0;r<=self->depth;r++)
    {
      self->pdfIntensityAll->addRealization( r );
      self->pdfInhomoAll->addRealization( r );
    }

      int index=0;
      int lastPercentageProgressBarUpdated=-1;

      int I, H;
      int sparseSampling=100;

      for(int k=0;k<self->dimZ;k++)
    for(int j=0;j<self->dimY;j++)
      for(int i=0;i<self->dimX;i++)
        {
          self->node[index].T=INF;
          self->node[index].status=fmsFAR;
      
          self->inhomo[index]=-1; // meaning inhomo and median have not been computed there

          if( (i<BAND_OUT) || (j<BAND_OUT) ||  (k<BAND_OUT) ||
          (i>=(self->dimX-BAND_OUT)) || (j>=(self->dimY-BAND_OUT)) || (k>=(self->dimZ-BAND_OUT)) )
        {

          // update progress bar
          int currentPercentage = GRANULARITY_PROGRESS*index 
            / self->dimXYZ;
          
          if( currentPercentage > lastPercentageProgressBarUpdated )
            {
              lastPercentageProgressBarUpdated = currentPercentage;
              self->UpdateProgress(float(currentPercentage)/float(GRANULARITY_PROGRESS));
            }

          self->node[index].status=fmsOUT;

          // we should never have to look at these values anyway !
          self->inhomo[ index ] = self->depth;
          self->median[ index ] = 0;
        }
          else
        {
          // we're not on one of the border of the volume
      
          // pick some values to estimate the properties of the grayscale values
          if( (index % sparseSampling) == 0)
            {
              self->getMedianInhomo( index, I, H );
              self->pdfIntensityAll->addRealization( I );
              self->pdfInhomoAll->addRealization( H );
            }
        }

          index++;
        }
      
      return;
    }

  if( self->firstCall )
    {
      self->firstCall=false;


      assert(self->seedPoints.size()>0);
  
      for(int k=0;k<(int)self->seedPoints.size();k++)
    self->collectInfoSeed( self->seedPoints[k] );

    }

  // reinitialize the points that were removed by the user
  if( self->nEvolutions>=0 )
    if( (self->knownPoints[self->nEvolutions].size()>1) && 
    ((signed)self->knownPoints[self->nEvolutions].size()-1>self->nPointsBeforeLeakEvolution) )
      {
    // reinitialize all the points
    for(int k=self->nPointsBeforeLeakEvolution;k<(int)self->knownPoints[self->nEvolutions].size();k++)
      {
        int index = self->knownPoints[self->nEvolutions][k];
        self->node[ index ].status = fmsFAR;
        self->node[ index ].T = INF;
       
        /* we also want to remove the neighbors of these points that would be in TRIAL
           as it is not trivial to remove points from the minheap, we will just set their T
           to infinity to make sure they appear in the back of the heap
        */

        for(int n=1;n<=self->nNeighbors;n++)
          {
        int indexN=index+self->shiftNeighbor(n);
        if( self->node[indexN].status==fmsTRIAL )
          {
            self->node[indexN].T=INF;
            self->downTree( self->node[indexN].leafIndex );
          }
          }
      }

    // if the points still have a KNOWN neighbor, put them back in TRIAL
    for(int k=self->nPointsBeforeLeakEvolution;k<(int)self->knownPoints[self->nEvolutions].size();k++)
      {
        int index = self->knownPoints[self->nEvolutions][k];
        int indexN;

        bool hasKnownNeighbor =  false;
        for(int n=1;n<=self->nNeighbors;n++)
          {
        indexN=index+self->shiftNeighbor(n);
        if( self->node[indexN].status==fmsKNOWN )
          hasKnownNeighbor=true;
          }

        if( (hasKnownNeighbor) && (self->node[index].status!=fmsOUT) )
          {
        FMleaf f;

        self->node[index].T=self->computeT(index);
        self->node[index].status=fmsTRIAL;        
        f.nodeIndex=index;

        self->insert( f );
          }
      }

    // remove all the points from the displayed knownPoints
    // ok since (self->knownPoints[self->nEvolutions].size()-1>self->nPointsBeforeLeakEvolution)
    // is true
    while((int)self->knownPoints[self->nEvolutions].size()>self->nPointsBeforeLeakEvolution)
      self->knownPoints[self->nEvolutions].pop_back();
      }

  // start a new evolution
  VecInt v;
  self->knownPoints.push_back(v);
  self->nEvolutions++;
  self->nPointsBeforeLeakEvolution=-1;

  // use the seeds
  // note: we have to make sure knownPoints[nEvolution] is ready to receive
  // indexes, ok
  while(self->seedPoints.size()>0)
    {
      int index=self->seedPoints[self->seedPoints.size()-1];
      self->seedPoints.pop_back();

      self->setSeed( index );
    }
  
  for(n=0;n<self->nPointsEvolution;n++)
    {
      if( n % (self->nPointsEvolution/GRANULARITY_PROGRESS) == 0 )
    self->UpdateProgress(float(n)/float(self->nPointsEvolution));

      float T=self->step();

      if( T==INF )
    {
      std::cout << "FastMarching: nowhere else to go. End of evolution." << endl;
      break;
    }
    }
}

void vtkFastMarching::show(float r)
{
  assert( (r>=0) && (r<=1.0) );

  if( nEvolutions<0 )
    return;

  if( knownPoints[nEvolutions].size()<1 )
    return;

  int oldIndex = nPointsBeforeLeakEvolution;
  int newIndex = (int)((knownPoints[nEvolutions].size()-1)*r);

  if( newIndex > oldIndex )
    for(int index=(oldIndex+1);index<=newIndex;index++)
      {
    if( node[ knownPoints[nEvolutions][index] ].status==fmsKNOWN )
      outdata[ knownPoints[nEvolutions][index] ]=label;
      }
  else if( newIndex < oldIndex )
    for(int index=oldIndex;index>newIndex;index--)
      {
    if(node[  knownPoints[nEvolutions][index] ].status==fmsKNOWN )
      outdata[ knownPoints[nEvolutions][index] ]=0;
      }

  nPointsBeforeLeakEvolution=newIndex;
}

void vtkFastMarching::setActiveLabel(int label)
{
  this->label=label;
}

//----------------------------------------------------------------------------
// Description:
// This method is passed a input and output data, and executes the filter
// algorithm to fill the output from the input.
// It just executes a switch statement to call the correct function for
// the datas data types.
void vtkFastMarching::ExecuteData(vtkDataObject *)
{
  vtkImageData *inData = this->GetInput();
  vtkImageData *outData = this->GetOutput();

  outData->SetExtent(this->GetOutput()->GetWholeExtent());
  outData->AllocateScalars();

  int outExt[6], s;
  outData->GetWholeExtent(outExt);
  void *inPtr = inData->GetScalarPointerForExtent(outExt);
  void *outPtr = outData->GetScalarPointerForExtent(outExt);

  int x1;

  x1 = GetInput()->GetNumberOfScalarComponents();
  if (x1 != 1) 
    {
      vtkErrorMacro(<<"Input has "<<x1<<" instead of 1 scalar component.");
      return;
    }

  /* Need short data */
  s = inData->GetScalarType();
  if (s != VTK_SHORT) 
    {
      vtkErrorMacro("Input scalars are type "<<s 
            << " instead of "<<VTK_SHORT);
      return;
    }

  vtkFastMarchingExecute(this, inData, (short *)inPtr, 
             outData, (short *)(outPtr), outExt);

}

void vtkFastMarching::setNPointsEvolution( int n )
{
  nPointsEvolution=n;
}

void vtkFastMarching::PrintSelf(ostream& os, vtkIndent indent)
{
  vtkImageToImageFilter::PrintSelf(os,indent);

  os << indent << "dimX: " << this->dimX << "\n";
  os << indent << "dimY: " << this->dimY << "\n";
  os << indent << "dimZ: " << this->dimZ << "\n";
  os << indent << "dimXY: " << this->dimXY << "\n";
  os << indent << "label: " << this->label << "\n";
}

bool vtkFastMarching::emptyTree(void)
{
  return (tree.size()==0);
}

void vtkFastMarching::insert(const FMleaf leaf) {

  // insert element at the back
  tree.push_back( leaf );
  node[ leaf.nodeIndex ].leafIndex=(int)(tree.size()-1);

  // trickle the element up until everything 
  // is sorted again
  upTree( (int)(tree.size()-1) );
}

void vtkFastMarching::downTree(int index) {
  /*
   * This routine sweeps downward from leaf 'index',
   * swapping child and parent if the value of the child
   * is smaller than that of the parent. Note that this only
   * guarantees the heap property if the value at the
   * starting index is greater than all its parents.
   */

  int LeftChild = 2 * index + 1;
  int RightChild = 2 * index + 2;
  
  while (LeftChild < (int)tree.size())
    {
      /*
       * Terminate the process when the current leaf has no
       * children. If no swap occurs at a higher leaf, this
       * condition is forced.
       */

      /* 
       * Find the child with the smallest value. The node has at least
       * one child, and so has at least a left child.
       */
      int MinChild = LeftChild;

      /*
       * If the node has a right child, and if the right child
       * has smaller crossing time than the left child, then the 
       * right child is the MinChild.
       */
      if (RightChild < (int)tree.size()) {
    if (node[tree[LeftChild].nodeIndex].T>
        node[tree[RightChild].nodeIndex].T) 
      MinChild = RightChild;
      }
    
      /*
       * If the MinChild has smaller T than the current leaf,
       * swap them, and move the current leaf to the MinChild.
       */
      if (node[tree[MinChild].nodeIndex].T<
      node[tree[index].nodeIndex].T)
    {
      FMleaf tmp=tree[index];
      tree[index]=tree[MinChild];
      tree[MinChild]=tmp;

      // make sure pointers remain correct
      node[ tree[MinChild].nodeIndex ].leafIndex = MinChild;
      node[ tree[index].nodeIndex ].leafIndex = index;

      index = MinChild;
    }
      else
    /*
     * If the current leaf has a lower value than its
     * MinChild, the job is done, force a stop.
     */
    index = (int) tree.size();
     
      LeftChild = 2 * index + 1;
      RightChild = 2 * index + 2;
    } 
}

void vtkFastMarching::upTree(int index) {
  /*
   * This routine sweeps upward from leaf 'index',
   * swapping child and parent if the value of the child
   * is less than that of the parent. Note that this only
   * guarantees the heap property if the value at the
   * starting leaf is less than all its children.
   */

  while( index>0 )
    {
      int upIndex = (int) (index-1)/2;
      if( node[tree[index].nodeIndex].T < 
      node[tree[upIndex].nodeIndex].T )
    {
      // then swap the 2 nodes

      FMleaf tmp=tree[index];
      tree[index]=tree[upIndex];
      tree[upIndex]=tmp;

      // make sure pointers remain correct
      node[ tree[upIndex].nodeIndex ].leafIndex = upIndex;
      node[ tree[index].nodeIndex ].leafIndex = index;
    
      index = upIndex;
    }
      else
    // then there is nothing left to do
    // force stop
    index=0;
    }
}

FMleaf vtkFastMarching::removeSmallest( void ) {

  FMleaf f;
  f=*(tree.begin());

  /*
   * Now move the bottom, rightmost, leaf to the root.
   */
  tree[0]=tree[ tree.size()-1 ];
  tree.pop_back();

  // trickle the element down until everything 
  // is sorted again
  downTree( 0 );

  return f;
}

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////

vtkFastMarching::vtkFastMarching() 
{ initialized=false; }

void vtkFastMarching::init(int dimX, int dimY, int dimZ, int depth)
{
  nNeighbors=6; // 6 or 26
  dx=1.0;

  nEvolutions=-1;

  this->dimX=dimX;
  this->dimY=dimY;
  this->dimZ=dimZ;
  this->dimXY=dimX*dimY;
  this->dimXYZ=dimX*dimY*dimZ;

  arrayShiftNeighbor[0] = 0; // neighbor 0 is the node itself
  arrayShiftNeighbor[1] = -dimX;
  arrayShiftNeighbor[2] = +1;
  arrayShiftNeighbor[3] = dimX;
  arrayShiftNeighbor[4] = -1;
  arrayShiftNeighbor[5] = -dimXY;
  arrayShiftNeighbor[6] = dimXY;
  arrayShiftNeighbor[7] =  -dimX+dimXY;
  arrayShiftNeighbor[8] =  -dimX+dimXY;
  arrayShiftNeighbor[9] =   dimX+dimXY;
  arrayShiftNeighbor[10] =  dimX-dimXY;
  arrayShiftNeighbor[11] = -1+dimXY;
  arrayShiftNeighbor[12] = -1-dimXY;
  arrayShiftNeighbor[13] = +1+dimXY;
  arrayShiftNeighbor[14] = +1-dimXY;
  arrayShiftNeighbor[15] = +1-dimX;
  arrayShiftNeighbor[16] = +1-dimX-dimXY;
  arrayShiftNeighbor[17] = +1-dimX+dimXY;
  arrayShiftNeighbor[18] = 1+dimX;
  arrayShiftNeighbor[19] = 1+dimX-dimXY;
  arrayShiftNeighbor[20] = 1+dimX+dimXY;
  arrayShiftNeighbor[21] = -1+dimX;
  arrayShiftNeighbor[22] = -1+dimX-dimXY;
  arrayShiftNeighbor[23] = -1+dimX+dimXY;
  arrayShiftNeighbor[24] = -1-dimX;
  arrayShiftNeighbor[25] = -1-dimX-dimXY;
  arrayShiftNeighbor[26] = -1-dimX+dimXY;

  this->depth=depth;

  node = new FMnode[ dimX*dimY*dimZ ];
  assert( node!=NULL );
  // else there was not enough memory

  inhomo = new int[ dimX*dimY*dimZ ];
  assert( inhomo!=NULL );
  // else there was not enough memory
  
  median = new int[ dimX*dimY*dimZ ];
  assert( median!=NULL );
  // else there was not enough memory
  
  pdfIntensityIn = new FMpdf( depth );
  pdfIntensityAll = new FMpdf( depth );
  pdfInhomoIn = new FMpdf( depth );
  pdfInhomoAll = new FMpdf( depth );

  initialized=false; // we will need one pass in the execute
  // function before we are properly initialized

  firstCall = true;
}

void vtkFastMarching::setInData(short* data)
{
  indata=data;
}

void vtkFastMarching::setOutData(short* data)
{
  outdata=data;
}

vtkFastMarching::~vtkFastMarching()
{
  /* all the delete below are done by unInit() */
}

inline int vtkFastMarching::shiftNeighbor(int n)
{
  //assert(initialized);
  //assert(n>=0 && n<=nNeighbors);

  return arrayShiftNeighbor[n];
}

float vtkFastMarching::step( void )
{
  int indexN;
  int n;

  FMleaf min;

  /* find point in fmsTRIAL with smallest T, remove it from fmsTRIAL and put
     it in fmsKNOWN */

  if( emptyTree() )
    return INF;

  min=removeSmallest();

  if( node[min.nodeIndex].T==INF )
    // this would happen if the only points left were artificially put back
    // by the user paying with the slider
    // we do not want to consider those before the expansion has naturally 
    // reachjed them.
    return INF;

  float EPS=1e-1;

  while( speed(min.nodeIndex)<EPS )
    {
      if( emptyTree() )
    {
      return INF;
    }

      min=removeSmallest(); 
            
      node[min.nodeIndex].status=fmsKNOWN;

      knownPoints[nEvolutions].push_back(min.nodeIndex);
    }

  int I, H;
  getMedianInhomo( min.nodeIndex, I, H );

  pdfIntensityIn->addRealization( I );
  pdfInhomoIn->addRealization( H );

  node[min.nodeIndex].status=fmsKNOWN;
  knownPoints[nEvolutions].push_back(min.nodeIndex);
      
  /* then we consider all the neighbors */
  for(n=1;n<=nNeighbors;n++)
    {
      /* 'indexN' is the index of the nth neighbor 
     of node of index 'index' */
      indexN=min.nodeIndex+shiftNeighbor(n);      
      
      /*
       * Check the status of the neighbors. If
       * they are fmsTRIAL, recompute their crossing time values and
       * adjust their position in the tree with an UpHeap (Note that 
       * recomputed value must be less than or equal to the original). 
       * If they are fmsFAR, recompute their crossing times, and move 
       * them into fmsTRIAL.
       */
      if( node[indexN].status==fmsFAR )
    {
      FMleaf f;
      node[indexN].T=computeT(indexN);
      f.nodeIndex=indexN;

      insert( f );
      node[indexN].status=fmsTRIAL;
    }
      else if( node[indexN].status==fmsTRIAL )
    {
      node[indexN].T=computeT(indexN);
      upTree( node[indexN].leafIndex );
    }
    }

  return node[min.nodeIndex].T; 
}

float vtkFastMarching::computeT(int index )
{
  float A, B, C, Discr;

  A = 0.0;
  B = 0.0;


  float s=speed(index);

  if( s==0.0 )
    s=1.0/INF;

  C = -dx*dx/( s*s ); 

  float Tij, Txm, Txp, Tym, Typ, Tzm, Tzp, TijNew;

  Tij = node[index].T;

  /* we know that all neighbors are defined
     because this node is not fmsOUT */
  Txm = node[index+shiftNeighbor(4)].T;
  Txp = node[index+shiftNeighbor(2)].T;
  Tym = node[index+shiftNeighbor(1)].T;
  Typ = node[index+shiftNeighbor(3)].T;
  Tzm = node[index+shiftNeighbor(5)].T;
  Tzp = node[index+shiftNeighbor(6)].T;
  
  float Dxm, Dxp, Dym, Dyp, Dzm, Dzp;

  Dxm = Tij - Txm;
  Dxp = Txp - Tij;
  Dym = Tij - Tym;
  Dyp = Typ - Tij;
  Dzm = Tij - Tzm;
  Dzp = Tzp - Tij;

  /*
   * Set up the quadratic equation for TijNew.
   */
  if ((Dxm>0.0) || (Dxp<0.0)) {
    if (Dxm > -Dxp) {
      A++;
      B -= 2.0 * Txm;
      C += Txm * Txm;
    }
    else {
      A++;
      B -= 2.0 * Txp;
      C += Txp * Txp;
    }
  }
  if ((Dym>0.0) || (Dyp<0.0)) {
    if (Dym > -Dyp) {
      A++;
      B -= 2.0 * Tym;
      C += Tym * Tym;
    }
    else {
      A++;
      B -= 2.0 * Typ;
      C += Typ * Typ;
    }
  }
  if ((Dzm>0.0) || (Dzp<0.0)) {
    if (Dzm > -Dzp) {
      A++;
      B -= 2.0 * Tzm;
      C += Tzm * Tzm;
    }
    else {
      A++;
      B -= 2.0 * Tzp;
      C += Tzp * Tzp;
    }
  }

  if (A==0) {
    //printf("A==0 \n");
    /*
      printf("A=0, index=%d\n",index);
      printf("Txm=%f, Tym=%f, Txp=%f, Typ=%f\n",Txm, Tym, Txp, Typ);
    */

    return Tij;
  }

  /*
   * Negative discriminant? Complex crossing times?
   */
  Discr = B*B - 4.0*A*C;
  if (Discr < 0.0) {
    //printf("Discr<0 ");
    /*
      printf("Error from FMTijNew at index=%d: Discriminant = %f!\n", index, Discr);
      printf("A, B, C = %f, %f, %f\n", A, B, C);
      printf("Txm=%f, Tym=%f, Txp=%f, Typ=%f\n",Txm, Tym, Txp, Typ);
      printf("Returning Tij %f\n",Tij);
    */

    // this is what Tony Yezzi suggested
    float minT=INF;
    for(int n=1;n<=nNeighbors;n++)
      minT=min(minT, node[index+shiftNeighbor(n)].T);

    return minT+dx/s ;
    
  }

  /*
   * Solve the quadratic equation. Note that the new crossing
   * must be GREATER than the average of the active neighbors,
   * since only EARLIER elements are active. Therefore the plus
   * sign is appropriate.
   */
  TijNew = (-B + sqrt(Discr))/(2.0*A);

  return TijNew; 
}

void vtkFastMarching::setRAStoIJKmatrix
(float m11, float m12, float m13, float m14,
 float m21, float m22, float m23, float m24,
 float m31, float m32, float m33, float m34,
 float m41, float m42, float m43, float m44)
{
  this->m11=m11;
  this->m12=m12;
  this->m13=m13;
  this->m14=m14;

  this->m21=m21;
  this->m22=m22;
  this->m23=m23;
  this->m24=m24;

  this->m31=m31;
  this->m32=m32;
  this->m33=m33;
  this->m34=m34;

  this->m41=m41;
  this->m42=m42;
  this->m43=m43;
  this->m44=m44;
}

int vtkFastMarching::addSeed( float r, float a, float s )
{
  int I, J, K;

  I = (int) ( m11*r + m12*a + m13*s + m14*1 );
  J = (int) ( m21*r + m22*a + m23*s + m24*1 );
  K = (int) ( m31*r + m32*a + m33*s + m34*1 );

  if ( (I>=0) && (I<dimX)
       &&  (J>=0) && (J<dimY)
       &&  (K>=0) && (K<dimZ) )
    {
      seedPoints.push_back( I+J*dimX+K*dimXY );
      return 1;
    }

  return 0; // we're trying to put a seed outside of the volume
}

void vtkFastMarching::unInit( void )
{
  assert( initialized );

  delete [] node;
  delete [] inhomo;
  delete [] median;

  delete pdfIntensityIn;
  delete pdfIntensityAll;
  
  delete pdfInhomoIn;
  delete pdfInhomoAll;

  while(tree.size()>0)
    tree.pop_back();

  while(knownPoints.size()>0)
    {
      while(knownPoints[knownPoints.size()-1].size()>0)
    knownPoints[knownPoints.size()-1].pop_back();  

      knownPoints.pop_back();
    }

  initialized = false;
}










