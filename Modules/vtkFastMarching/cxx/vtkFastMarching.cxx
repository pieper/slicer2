#include <stdio.h>
#include <math.h>
#include <algorithm>
#include <iostream.h>
#include "vtkObjectFactory.h"

#include "vtkFastMarching.h"

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////


JIT::JIT()
{
  initialized = false;
  assert( false );
}

JIT::JIT( int dimX, int dimY, int dimZ )
{
  this->dimX=dimX;
  this->dimY=dimY;
  this->dimZ=dimZ;
  this->dimXY=dimX*dimY;

  // allocate status and set everything to to 0
  status = new (unsigned char)[ dimX*dimY*dimZ ];
  assert( status!=NULL );
  memset( status, 0, dimX*dimY*dimZ*sizeof(unsigned char) );

  _avg = new float [ dimX*dimY*dimZ ];
  assert( _avg!=NULL );
  printf("allocation _avg = %d\n",dimX*dimY*dimZ*sizeof(float));

  _phi = new float [ dimX*dimY*dimZ ];
  assert( _phi!=NULL );
  printf("allocation _phi = %d\n",dimX*dimY*dimZ*sizeof(float));


  float sigma = 1.0;
  for(int i=-MASK_SIZE;i<=MASK_SIZE;i++)
    for(int j=-MASK_SIZE;j<=MASK_SIZE;j++)
      for(int k=-MASK_SIZE;k<=MASK_SIZE;k++)

    {
      gaussian[MASK_SIZE+i][MASK_SIZE+j][MASK_SIZE+k]
        =pow(2.0*M_PI*sigma*sigma,-3/2)
        *exp( -(i*i+j*j+k*k)/(2.0*sigma*sigma) );
    }

  initialized = true;
}

void JIT::setDepth(int depth)
{
  // max value of the volume
  this->depth=depth;

  printf("jit.depth=%d\n",this->depth);
}
void JIT::setSigma(float s)
{
  // max value of the volume
  this->sigma=s;

  printf("jit.sigma=%f\n",this->sigma);

  float sum=0.0;

  for(int i=-MASK_SIZE;i<=MASK_SIZE;i++)
    for(int j=-MASK_SIZE;j<=MASK_SIZE;j++)
      for(int k=-MASK_SIZE;k<=MASK_SIZE;k++)

    {
      gaussian[MASK_SIZE+i][MASK_SIZE+j][MASK_SIZE+k]
        =exp( -(i*i+j*j+k*k)/(2.0*sigma*sigma) );

      sum+=gaussian[MASK_SIZE+i][MASK_SIZE+j][MASK_SIZE+k];
    }

  
  for(int i=-MASK_SIZE;i<=MASK_SIZE;i++)
    for(int j=-MASK_SIZE;j<=MASK_SIZE;j++)
      for(int k=-MASK_SIZE;k<=MASK_SIZE;k++)
    gaussian[MASK_SIZE+i][MASK_SIZE+j][MASK_SIZE+k]/=sum;

  /*
    in the continuous case the integral on R3 of the pdf should be 1
    we force that on this window to avoid any energy problem
  */

}

void JIT::setStdev(int stdev)
{
  // max value of the volume
  this->stdev=stdev;

  printf("jit.stdev=%d\n",this->stdev);
}


void JIT::setInData( short* indata ) 
{
  this->data=indata;
}


JIT::~JIT()
{
  printf("JIT::~JIT()\n");
  assert( initialized );
  
  delete [] status;
  delete [] _avg;
  delete [] _phi;

  initialized=false;
}

inline int JIT::index( int x, int y, int z ) const
{
  return x+y*dimX+z*dimXY;
}

inline float JIT::avg( int idx )
{
  if( ! (status[idx] & AVG) )
    // if this value has not been computed, compute it
    {
      // gaussian filtering
      _avg[idx]=0.0;
      for(int i=-MASK_SIZE;i<=MASK_SIZE;i++)
    for(int j=-MASK_SIZE;j<=MASK_SIZE;j++)
      for(int k=-MASK_SIZE;k<=MASK_SIZE;k++)
        {
          _avg[idx]+=
        gaussian[MASK_SIZE+i][MASK_SIZE+j][MASK_SIZE+k]
        *1.0/(float)depth*(float)data[idx+index(i,j,k)];
        }
   

      // median filtering
      /*
    int tabindex[(2*MASK_SIZE+1)*(2*MASK_SIZE+1)*(2*MASK_SIZE+1)];

    for(int i=-MASK_SIZE;i<=MASK_SIZE;i++)
    for(int j=-MASK_SIZE;j<=MASK_SIZE;j++)
    for(int k=-MASK_SIZE;k<=MASK_SIZE;k++)
    {
        tabindex[ (i+MASK_SIZE)+(j+MASK_SIZE)*(2*MASK_SIZE+1) 
        + (k+MASK_SIZE)*(2*MASK_SIZE+1)*(2*MASK_SIZE+1)]
    = data[ idx+index(i,j,k) ];
    }

    qsort((int *) tabindex, 
    (2*MASK_SIZE+1)*(2*MASK_SIZE+1)*(2*MASK_SIZE+1), 
    sizeof(int), JIT::medianCompare);
     
    _avg[idx]=tabindex[ ((2*MASK_SIZE+1)*(2*MASK_SIZE+1)*(2*MASK_SIZE+1))/2  ];
      */
       
      // now we have it
      status[idx] |= AVG;
    }

  return _avg[idx];
} 

inline float JIT::phi( int idx )
{
  if( ! (status[idx] & PHI) )
    // if this value has not been computed, compute it
    {
      float avgXY = avg(idx);
      float gradX = avgXY - avg(idx-1);
      float gradY = avgXY - avg(idx-dimX);
      float gradZ = avgXY - avg(idx-dimXY);

      float normGrad = sqrt( gradX*gradX 
                 + gradY*gradY 
                 + gradZ*gradZ );

      _phi[idx]=1.0/(1.0+pow(float(depth)/float(stdev)*normGrad,4));

      // now we have it
      status[idx] |= PHI;
    }

  return _phi[idx];
} 

float JIT::value( int idx )
{

  // we might want to use something more elaborate than that...

  float phiXY = phi(idx);
  return phiXY;
} 


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

static void vtkFastMarchingExecute(vtkFastMarching *self,
                   vtkImageData *inData, short *inPtr,
                   vtkImageData *outData, short *outPtr, 
                   int outExt[6])
{
  printf("static void vtkFastMarchingExecute\n");

  double sumT=0.0;
  int n=0;

#define ITERMAX 30000

  self->setInData( (short *)inPtr );
  self->setOutData( (short *)outPtr );

  self->setSeed();
  
  for(n=0;n<ITERMAX;n++)
    {
      double T=self->step();

      if( T==INF )
    break;
    }

  cout << "TOTAL: " << n << " iterations" << endl;

}

void vtkFastMarching::setSeed(void)
{
  // empty everything
  while(tree.size()>0) 
    tree.pop_back();

  this->label=label;

  int index=xSeed+ySeed*dimY+zSeed*dimXY;
  node[index].T=0.0;
  node[index].status=KNOWN;

  for(int nei=1;nei<=nNeighbors;nei++)
    {
      FMleaf f;
      f.nodeIndex=index+shiftNeighbor(nei);

      node[f.nodeIndex].status=TRIAL;
      node[f.nodeIndex].T=dx/jit->value(f.nodeIndex);    
      
      insert( f );
      printf("init tree %f \n",node[f.nodeIndex].T);
    }

  // init this here because we are going to starts a new
  // segmentation
  estN=0;
  estM1=0.0;
  estM2=0.0;
}


//----------------------------------------------------------------------------
// Description:
// This method is passed a input and output data, and executes the filter
// algorithm to fill the output from the input.
// It just executes a switch statement to call the correct function for
// the datas data types.
void vtkFastMarching::ExecuteData(vtkDataObject *)
{
  printf("vtkFastMarching::Execute\n");

  vtkImageData *inData = this->GetInput();
  vtkImageData *outData = this->GetOutput();

  outData->SetExtent(this->GetOutput()->GetWholeExtent());
  outData->AllocateScalars();

  int outExt[6], id=0, s;
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

  VecInt v;
  knownPoints.push_back(v);

  vtkFastMarchingExecute(this, inData, (short *)inPtr, 
             outData, (short *)(outPtr), outExt);

  while(!emptyTree())
    {
      FMleaf leaf=tree[ tree.size()-1 ];
      tree.pop_back();

      knownPoints[nEvolutions].push_back(leaf.nodeIndex);
    }

  show();
  
}

void vtkFastMarching::PrintSelf(ostream& os, vtkIndent indent)
{
  vtkImageToImageFilter::PrintSelf(os,indent);

  os << indent << "dimX: " << this->dimX << "\n";
  os << indent << "dimY: " << this->dimY << "\n";
  os << indent << "dimZ: " << this->dimZ << "\n";
  os << indent << "dimXY: " << this->dimXY << "\n";
  os << indent << "label: " << this->label << "\n";
  os << indent << "estN: " << this->estN << "\n";
  os << indent << "estM1: " << this->estM1 << "\n";
  os << indent << "estM2: " << this->estM2 << "\n";
}

bool vtkFastMarching::emptyTree(void)
{
  return (tree.size()==0);
}

void vtkFastMarching::insert(const FMleaf leaf) {

  // insert element at the back
  tree.push_back( leaf );
  node[ leaf.nodeIndex ].leafIndex=tree.size()-1;

  // trickle the element up until everything 
  // is sorted again
  upTree( tree.size()-1 );
}

void vtkFastMarching::downTree(int index) {
  /*
   * This routine sweeps downward from leaf 'index',
   * swapping child and parent if the value of the child
   * is smaller than that of the parent. Note that this only
   * guarantees the heap property if the value at the
   * starting index is greater than all its parents.
   */

  unsigned int LeftChild = 2 * index + 1;
  unsigned int RightChild = 2 * index + 2;
  
  while (LeftChild < tree.size())
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
      if (RightChild < tree.size()) {
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
    index = tree.size();
     
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

void vtkFastMarching::init(int dimX, int dimY, int dimZ)
{
  printf("vtkFastMarching::init(%d,%d,%d) %xd\n",dimX,dimY,dimZ,this);

  nNeighbors=6;
  dx=1.0;

  nEvolutions=0;

  this->dimX=dimX;
  this->dimY=dimY;
  this->dimZ=dimZ;
  this->dimXY=dimX*dimY;

  jit = new JIT(dimX, dimY, dimZ);
  assert( jit!=NULL );

  node = new FMnode[ dimX*dimY*dimZ ];
  assert( node!=NULL );
  printf("allocation: node=%d\n",dimX*dimY*dimZ*sizeof(FMnode) );


  int index=0;
  for(int k=0;k<dimZ;k++)
    for(int j=0;j<dimY;j++)
      for(int i=0;i<dimX;i++)
    {
      node[index].T=INF;
      node[index].status=FAR;
    
      if( (i<BAND_OUT) || (j<BAND_OUT) ||  (k<BAND_OUT) ||
          (i>=dimX-BAND_OUT) || (j>=dimY-BAND_OUT) || (k>=dimZ-BAND_OUT) )
        node[index].status=OUT;

      index++;
    }

  initialized=true;
}

void vtkFastMarching::setDepth(int depth) 
{
  printf("vtkFastMarching::setDepth(%d)\n",depth);
  jit->setDepth(depth);
}

void vtkFastMarching::setSigma(float s) 
{
  printf("vtkFastMarching::setSigma(%f)\n",s);
  jit->setSigma(s);
}

void vtkFastMarching::setStdev(int stdev) 
{
  printf("vtkFastMarching::setStdev(%d)\n",stdev);
  jit->setStdev(stdev);
}

void vtkFastMarching::setInData(short* data)
{
  printf("vtkFastMarching::setInData\n");
  jit->setInData(data);
}

void vtkFastMarching::setOutData(short* data)
{
  outdata=data;
}

void vtkFastMarching::setSeedAndLabel(int xSeed, int ySeed, int zSeed,
                      int label)
{
  printf("vtkFastMarching::setSeedAndLabel(%d,%d,%d,%d)\n",xSeed,ySeed,zSeed,label);

  this->xSeed=xSeed;
  this->ySeed=ySeed;
  this->zSeed=zSeed;
  this->label=label;
}



void vtkFastMarching::show( void )
{
  printf("vtkFastMarching::show\n");
  assert( initialized );

  /*  

  float max=-INF;
  float min=INF;

  int index, i, j, k;
  float d=30;

  index=0;
  for(k=0;k<dimZ;k++)
  for(j=0;j<dimY;j++)
  for(i=0;i<dimX;i++)
  {
  if( (i-xSeed)*(i-xSeed)
  + (j-ySeed)*(j-ySeed)
  + (k-zSeed)*(k-zSeed)
  < d*d )
  {
        
  node[index].T=jit->value(index);

  //cout << node[index].T << endl;

  if( node[index].T>max )
  max=node[index].T;

  if( node[index].T<min )
  min=node[index].T;
  }
  index++;
  }

  cout << "min=" << min << endl;
  cout << "max=" << max << endl;


  index=0;
  for(k=0;k<dimZ;k++)
  for(j=0;j<dimY;j++)
  for(i=0;i<dimX;i++)
  {
  if( (i-xSeed)*(i-xSeed)
  + (j-ySeed)*(j-ySeed)
  + (k-zSeed)*(k-zSeed)
  < d*d )
  {
  outdata[index]=255*(node[index].T-min)/(max-min);
  }
  index++;
  }

  */
  
  //   for(int index=0;index<dimX*dimY*dimZ;index++)
  //     {
  //       if( node[index].status==KNOWN )
  //     outdata[ index ]=label;
  //     }


  int n, index;

  VecInt points=knownPoints[nEvolutions];

  printf("%d points to show\n",points.size());

  
  for(n=0;n<points.size();n++)
    {
      index=points[n];

      outdata[ index ]=label;
      node[ index ].status=DONE;      
    }
  
  /*

  float min=INF;
  float max=-INF;
  for(n=0;n<points.size();n++)
  {
  index=points[n];

  if( node[ index ].T<min )
  min=node[ index ].T;

  if( node[ index ].T>max )
  max=node[ index ].T;
  }
  
  for(n=0;n<points.size();n++)
  {
  index=points[n];

  outdata[ index ]=1+float(1000.0)*(node[ index ].T-min)/(max-min);
  node[ index ].status=DONE;      
  }

  */


  nEvolutions++;
}

/*
  void vtkFastMarching::showImg( short* outdata, int label )
  {
  assert( initialized );

  float max=-INF;
  float min=INF;

  int index, i, j, k;
  float d=30;

  index=0;
  for(k=0;k<dimZ;k++)
  for(j=0;j<dimY;j++)
  for(i=0;i<dimX;i++)
  {
  if( (i-xSeed)*(i-xSeed)
  + (j-ySeed)*(j-ySeed)
  + (k-zSeed)*(k-zSeed)
  < d*d )
  {
        
  node[index].T=jit->value(index);

  //cout << node[index].T << endl;

  if( node[index].T>max )
  max=node[index].T;

  if( node[index].T<min )
  min=node[index].T;
  }
  index++;
  }

  cout << "min=" << min << endl;
  cout << "max=" << max << endl;


  index=0;
  for(k=0;k<dimZ;k++)
  for(j=0;j<dimY;j++)
  for(i=0;i<dimX;i++)
  {
  if( (i-xSeed)*(i-xSeed)
  + (j-ySeed)*(j-ySeed)
  + (k-zSeed)*(k-zSeed)
  < d*d )
  {
  outdata[index]=(short int)(255*(node[index].T-min)/(max-min));
  }
  index++;
  } 
  }

  void vtkFastMarching::showT( short* outdata, int label )
  {
  assert( initialized );

  float max=-INF;
  float min=INF;

  int index, i, j, k;

  index=0;
  for(k=0;k<dimZ;k++)
  for(j=0;j<dimY;j++)
  for(i=0;i<dimX;i++)
  {
  if( node[index].status==KNOWN )
  {
        
  if( node[index].T>max )
  max=node[index].T;

  if( node[index].T<min )
  min=node[index].T;
  }
  index++;
  }

  cout << "min=" << min << endl;
  cout << "max=" << max << endl;


  index=0;
  for(k=0;k<dimZ;k++)
  for(j=0;j<dimY;j++)
  for(i=0;i<dimX;i++)
  {
  if( node[index].status==KNOWN )
  {
  outdata[index]=(short int)(jit->depth-jit->depth*(node[index].T-min)/(max-min));
  }
  index++;
  } 
  }
*/

vtkFastMarching::~vtkFastMarching()
{
  // assert( initialized );
  printf("vtkFastMarching::~vtkFastMarching() %xd\n",this);


  /* all the delete below are done by unInit() */

  //   delete [] node;

  //   while(tree.size()>0)
  //     tree.pop_back();

  //   while(knownPoints.size()>0)
  //     knownPoints.pop_back();  

  //   while(maskPoints.size()>0)
  //     maskPoints.pop_back();

  //   delete jit;
}

inline int vtkFastMarching::shiftNeighbor(int n)
{
  assert(initialized);
  assert(n>=0 && n<=nNeighbors);

  /*
    1
    4 0 2 on the XY plane, 5 is below 0 and 6 is above 0
    3
  */

  switch(n)
    {
    case 0: return 0; // neighbor 0 is the node itself
    case 1: return -dimX;
    case 2: return +1;
    case 3: return dimX;
    case 4: return -1;
    case 5: return -dimXY;
    case 6: return dimXY;
    }

  // we should never be there...
  assert( false );
  return 0;
}

bool vtkFastMarching::updateEstim(int index)
{
  assert( node[index].T<INF );

  double TminNeighbor=INF;

  for(int n=1;n<=nNeighbors;n++)
    {
      if (node[index+shiftNeighbor(n)].T<TminNeighbor)
    TminNeighbor=node[index+shiftNeighbor(n)].T;
    }

  // there must be at least one not INF !
  assert( TminNeighbor!=INF );

  // and it must smaller than local T
  assert( TminNeighbor<node[index].T );

  float dt=node[index].T-TminNeighbor;

  if(estN>100)
    {
      float moy=estM1/(double)estN;
      float std=sqrt( estM2/(double)estN-moy*moy );
      assert( estM2/(double)estN-moy*moy>0.0 );

      printf("\nmoy=%f\tstd=%f ",moy,std);

      if( (dt-moy) > 4.0*std )
    {
      //printf("\n%d rejected: dt=%f, moy=%f, std=%f\n",index,dt,moy,std);
      return false;
    }
      // dt is definitely too strange
    }

  estN++;
  estM1+=dt;
  estM2+=dt*dt;

  return true;
}

double vtkFastMarching::step( void )
{
  //  printf("vtkFastMarching::step()\n");

  int indexN;
  int n;

  FMleaf min;

  /* find point in TRIAL with smallest T, remove it from TRIAL and put
     it in KNOWN */

  if( emptyTree() )
    {
      cout << "emptyTree() is true here !" << endl;
      return INF;
    }

  min=removeSmallest();

  node[min.nodeIndex].status=KNOWN;
  knownPoints[nEvolutions].push_back(min.nodeIndex);

  while(  updateEstim(min.nodeIndex)==false )
    {
      if( emptyTree() )
    {
      cout << "emptyTree() is true !" << endl;
      return INF;
    }

      min=removeSmallest(); 

      node[min.nodeIndex].status=KNOWN;
      knownPoints[nEvolutions].push_back(min.nodeIndex);
    }


  /* then we consider all the neighbors */
  for(n=1;n<=nNeighbors;n++)
    {
      /* 'indexN' is the index of the nth neighbor 
     of node of index 'index' */
      indexN=min.nodeIndex+shiftNeighbor(n);      
      
      /*
       * Check the status of the neighbors. If
       * they are TRIAL, recompute their crossing time values and
       * adjust their position in the tree with an UpHeap (Note that 
       * recomputed value must be less than or equal to the original). 
       * If they are FAR, recompute their crossing times, and move 
       * them into TRIAL.
       */
      if( node[indexN].status==FAR )
    {
      FMleaf f;
      node[indexN].T=computeT(indexN);
      f.nodeIndex=indexN;

      insert( f );
      node[indexN].status=TRIAL;
    }
      else if( node[indexN].status==TRIAL )
    {
      node[indexN].T=computeT(indexN);
      upTree( node[indexN].leafIndex );
    }
    }

  return node[min.nodeIndex].T; 
}

double vtkFastMarching::computeT(int index)
{
  double A, B, C, Discr;

  A = 0.0;
  B = 0.0;
  float speed=jit->value(index);
  C = -dx*dx/( speed*speed ); 

  double Tij, Txm, Txp, Tym, Typ, Tzm, Tzp, TijNew;

  Tij = node[index].T;

  /* we know that all neighbors are defined
     because this node is not OUT */
  Txm = node[index+shiftNeighbor(4)].T;
  Txp = node[index+shiftNeighbor(2)].T;
  Tym = node[index+shiftNeighbor(1)].T;
  Typ = node[index+shiftNeighbor(3)].T;
  Tzm = node[index+shiftNeighbor(5)].T;
  Tzp = node[index+shiftNeighbor(6)].T;
  
  double Dxm, Dxp, Dym, Dyp, Dzm, Dzp;

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
    printf("A==0 \n");
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
    return minT+dx/speed;

    

    return -B/(2*A); // fixme

    return Tij;
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
  printf("vtkFastMarching::setRAStoIJKmatrix\n");

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



void vtkFastMarching::addMask( float centerX, float centerY, float centerZ,
                   float R, float theta, float phi )
{
  printf("vtkFastMarching::addMask(%f,%f,%f,%f,%f,%f)\n",centerX,centerY,centerZ,R,theta,phi);

  theta=M_PI*(theta+90)/180.0;
  phi=M_PI*phi/180.0;

  float u1, u2, u3;
  // normal vector if the disc
  u1=cos(theta)*cos(phi);
  u2=sin(theta)*cos(phi);
  u3=sin(phi);

  // then compute 2 normal vectors
  float v1, v2, v3;
  float w1, w2, w3;
  float norm;

  // first pick one
  v1=-u2;
  v2=u1;
  v3=0.0;
  norm=sqrt(v1*v1+v2*v2+v3*v3);

  if(norm==0.0)
    {
      // we were unlucky
      // let's pick another one... it will be ok
      
      v1=0.0;
      v2=-u3;
      v3=u2;
      norm=sqrt(v1*v1+v2*v2+v3*v3);     
    }

  v1/=norm;
  v2/=norm;
  v3/=norm;

  // then we can define the remaining vector of our
  // orthonormal basis by w=u x v

  w1=u2*v3-v2*u3;
  w2=u3*v1-v3*u1;
  w3=u1*v2-v1*u2;

  int I, J, K;
  float cR, cA, cS;

  for(int ir=1; ir<R; ir++)
    {
      float r=(float)ir;
      float deltaAlpha=atan(0.1/r);
      for(float alpha=0.0;alpha<2*M_PI;alpha+=deltaAlpha)
    {
      cR=r*cos(alpha)*v1+r*sin(alpha)*w1+centerX;
      cA=r*cos(alpha)*v2+r*sin(alpha)*w2+centerY;
      cS=r*cos(alpha)*v3+r*sin(alpha)*w3+centerZ;

      I = (int) ( m11*cR + m12*cA + m13*cS + m14*1 );
      J = (int) ( m21*cR + m22*cA + m23*cS + m24*1 );
      K = (int) ( m31*cR + m32*cA + m33*cS + m34*1 );

      if( (I>=0) && (I<dimX)
          &&  (J>=0) && (J<dimY)
          &&  (K>=0) && (K<dimZ) )
        {
          maskPoints.push_back( I+J*dimX+K*dimXY );
          node[I+J*dimX+K*dimXY].status = (FMstatus)
        ( (int)node[I+J*dimX+K*dimXY].status & MASK_BIT );
        }
    }
    }
}

void vtkFastMarching::back1Step( void )
{
  if(nEvolutions>0)
    {
      VecInt points=knownPoints[knownPoints.size()-1];
      knownPoints.pop_back();

      while(points.size()>0)
    {
      int index=points[points.size()-1];
      points.pop_back();

      outdata[ index ]=0;
      node[ index ].status=FAR;
      node[index].T=INF; 
    }
      
      nEvolutions--;
    }
}


void vtkFastMarching::unInit( void )
{
  assert( initialized );
  printf("vtkFastMarching::unInit() %xd\n",this);

  delete [] node;

  while(tree.size()>0)
    tree.pop_back();

  while(knownPoints.size()>0)
    {
      while(knownPoints[knownPoints.size()-1].size()>0)
    knownPoints[knownPoints.size()-1].pop_back();  

      knownPoints.pop_back();
    }

  while(maskPoints.size()>0)
    maskPoints.pop_back();

  delete jit;

  initialized = false;
}

