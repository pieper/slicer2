/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkFSSurfaceReader.cxx,v $
  Language:  C++
  Date:      $Date: 2002/08/16 22:12:26 $
  Version:   $Revision: 1.1 $

=========================================================================*/
#include "vtkFSSurfaceReader.h"
#include "vtkObjectFactory.h"
#include "vtkByteSwap.h"
#include "vtkFloatArray.h"

//-------------------------------------------------------------------------
vtkFSSurfaceReader* vtkFSSurfaceReader::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkFSSurfaceReader");
  if(ret)
    {
    return (vtkFSSurfaceReader*)ret;
    }
  // If the factory was unable to create the object, then create it here.
  return new vtkFSSurfaceReader;
}

vtkFSSurfaceReader::vtkFSSurfaceReader()
{
  this->vtkSource::SetNthOutput(0, vtkPolyData::New());
  // Releasing data for pipeline parallism.
  // Filters will know it is empty. 
  this->Outputs[0]->ReleaseData();
  this->Outputs[0]->Delete();
  this->ExecutePiece = this->ExecuteNumberOfPieces = 0;
  this->ExecuteGhostLevel = 0;
}

vtkFSSurfaceReader::~vtkFSSurfaceReader()
{
}

//----------------------------------------------------------------------------
vtkPolyData *vtkFSSurfaceReader::GetOutput()
{
  if (this->NumberOfOutputs < 1)
    {
    return NULL;
    }
  
  return (vtkPolyData *)(this->Outputs[0]);
}

//----------------------------------------------------------------------------
void vtkFSSurfaceReader::SetOutput(vtkPolyData *output)
{
  this->vtkSource::SetNthOutput(0, output);
}


//----------------------------------------------------------------------------
void vtkFSSurfaceReader::ComputeInputUpdateExtents(vtkDataObject *data)
{
  int piece, numPieces, ghostLevel;
  vtkPolyData *output = (vtkPolyData *)data;
  int idx;

  output->GetUpdateExtent(piece, numPieces, ghostLevel);
  
  // make sure piece is valid
  if (piece < 0 || piece >= numPieces)
    {
    return;
    }
  
  if (ghostLevel < 0)
    {
    return;
    }
  
  // just copy the Update extent as default behavior.
  for (idx = 0; idx < this->NumberOfInputs; ++idx)
    {
    if (this->Inputs[idx])
      {
      this->Inputs[idx]->SetUpdateExtent(piece, numPieces, ghostLevel);
      }
    }
  
  // Save the piece so execute can use this information.
  this->ExecutePiece = piece;
  this->ExecuteNumberOfPieces = numPieces;
  
  this->ExecuteGhostLevel = ghostLevel;
}

int vtkFSSurfaceReader::ReadInt3 (FILE* iFile, int& oInt) {

  int i = 0;
  int result ;

  // Read three bytes. Swap if we need to. Stuff into a full sized int
  // and return.
  result = fread (&i, 3, 1, iFile);
  vtkByteSwap::Swap4BE (&i);
  oInt = ((i>>8) & 0xffffff);

  return result;
}

int vtkFSSurfaceReader::ReadInt2 (FILE* iFile, int& oInt) {

  int i = 0;
  int result ;

  // Read two bytes. Swap if we need to. Return the value
  result = fread (&i, 2, 1, iFile);
  vtkByteSwap::Swap4BE (&i);
  oInt = i;
  
  return result;
}

int vtkFSSurfaceReader::ReadFloat (FILE* iFile, float& oFloat) {

  float f = 0;
  int result ;

  // Read a float. Swap if we need to. Return the value
  result = fread (&f, 4, 1, iFile);
  vtkByteSwap::Swap4BE (&f);
  oFloat = f;
  
  return result;
}

void vtkFSSurfaceReader::Execute()
{
  FILE* surfaceFile;
  int magicNumber;
  int numVertices = 0;
  int numFaces = 0;
  int vIndex, fIndex;
  int tmpX, tmpY, tmpZ, tmpfIndex;
  float locations[3];
  int fvIndex;
  vtkIdType faceIndices[FS_NUM_SIDES_IN_FACE];
  vtkPoints *outputVertices;
  vtkCellArray *outputFaces;
  vtkPolyData *output = this->GetOutput();
#if FS_CALC_NORMALS
  vtkFloatArray *outputNormals;
  FSVertex* vertices;
  FSFace* faces;
  FSVertex* v;
  FSFace* f;
  FSVertex* fv0;
  FSVertex* fv1;
  FSVertex* fv2;
  int fvIndex1, fvIndex2, fvIndex0;
  float faceVector0[3], faceVector1[3];
  float faceNormal[3];
  float length;
#endif

  vtkDebugMacro(<<"Reading vtk polygonal data...");

  // Try to open the file.
  surfaceFile = fopen(this->FileName, "rb") ;
  if (!surfaceFile) {
    vtkErrorMacro (<< "Could not open file " << this->FileName);
    return;
  }  

  // Get the three byte magic number. We support two file types.
  ReadInt3 (surfaceFile, magicNumber);
  if (magicNumber != FS_QUAD_FILE_MAGIC_NUMBER &&
      magicNumber != FS_NEW_QUAD_FILE_MAGIC_NUMBER) {
    vtkErrorMacro (<< "Wrong file type");
    return;
  }

  // Read the number of vertices and faces, three bytes each.
  ReadInt3 (surfaceFile, numVertices);
  ReadInt3 (surfaceFile, numFaces);
  
  // Allocate our VTK arrays.
  outputVertices = vtkPoints::New();
  outputVertices->Allocate (numVertices);
  outputFaces = vtkCellArray::New();
  outputFaces->Allocate (outputFaces->EstimateSize(numFaces,
                           FS_NUM_SIDES_IN_FACE));
#if FS_CALC_NORMALS
  outputNormals = vtkFloatArray::New();
  outputNormals->Allocate (numVertices);
  outputNormals->SetNumberOfComponents (3);
  outputNormals->SetName ("Normals");
  
  // Allocate our vertex and face connectivity arrays for calculating
  // the normals. If we can't, no big deal.
  vertices = (FSVertex*) calloc (numVertices, sizeof(FSVertex));
  faces = (FSFace*) calloc (numFaces, sizeof(FSFace));
  if (NULL == vertices || NULL == faces) {
    vtkErrorMacro (<< "Couldn't allocate vertex or face connectivity "
           "structuroes, unable to calc normals");
  }
#endif

  // For each vertex...
  for (vIndex = 0; vIndex < numVertices; vIndex++) {

    // Depending on the file type, read in three two bytes ints and
    // convert them from meters to millimeters or read in three floats
    // in millimeters. Insert them into the vertices array.
    if (FS_QUAD_FILE_MAGIC_NUMBER == magicNumber) {
      ReadInt2 (surfaceFile, tmpX);
      ReadInt2 (surfaceFile, tmpY);
      ReadInt2 (surfaceFile, tmpZ);
      locations[0] = (float)tmpX / 100.0;
      locations[1] = (float)tmpY / 100.0;
      locations[2] = (float)tmpZ / 100.0;
    } else {
      ReadFloat (surfaceFile, locations[0]);
      ReadFloat (surfaceFile, locations[1]);
      ReadFloat (surfaceFile, locations[2]);
    }
    
    outputVertices->InsertNextPoint (locations);

#if FS_CALC_NORMALS
    // If we have connectivity info, fill out the location and
    // initialize the normals to 0 for this vertex.
    if (NULL != vertices && NULL != faces) {
      v = &vertices[vIndex];
      v->x = locations[0];
      v->y = locations[1];
      v->z = locations[2];
      v->nx = 0;
      v->ny = 0;
      v->nz = 0;
      v->numFaces = 0;
    }
#endif
  }

  // For each face...
  for (fIndex = 0; fIndex < numFaces; fIndex += 2) {

    // For each vertex in the face...
    for (fvIndex = 0; fvIndex < FS_NUM_SIDES_IN_FACE; fvIndex++) {

      // Read in a three byte vertex index.
      ReadInt3 (surfaceFile, tmpfIndex);
      faceIndices[fvIndex] = tmpfIndex;

#if FS_CALC_NORMALS
      // Fill out connectivity info for this face. Get the vertex from
      // the vertex index and add this face index to its list of
      // faces. Then add this vertex index to the list of indicies in
      // the face.
      if (NULL != vertices && NULL != faces) {
    v = &vertices[tmpfIndex];
    v->faces[v->numFaces] = fIndex;
    v->indicesInFace[v->numFaces] = fvIndex;
    v->numFaces++;
    
    f = &faces[fIndex];
    f->vertices[fvIndex] = tmpfIndex;
      }
#endif
    }

    // Add the face to the list.
    outputFaces->InsertNextCell (FS_NUM_SIDES_IN_FACE, faceIndices);
  }
  
  // Close the surface file.
  fclose (surfaceFile);

#if FS_CALC_NORMALS

  // If we allocated the space for our vertex and face connectivity,
  // calculate the normals.
  if (NULL != vertices && NULL != faces) {

    // For each vertex...
    for (vIndex = 0; vIndex < numVertices; vIndex++) {
      
      // For each face it is a part of...
      fv1 = &vertices[vIndex];
      for (fIndex = 0; fIndex < fv1->numFaces; fIndex++) {

    // Get this face. fv1->indicesInFace tells us which index,
    // 0 - FS_NUM_SIDES_IN_FACE-1, it is in the face. Get the two
    // indicies surrounding it so we can get the vertices
    // surrounding it. Now we have fv0, fv1, and fv2 which are all
    // adjacent in the face, with fv1 being the orignal vertex.
    f = &faces[fv1->faces[fIndex]];
    fvIndex1 = fv1->indicesInFace[fIndex];
    if (0 == fvIndex1) 
      fvIndex0 = FS_NUM_SIDES_IN_FACE - 1;
    else
      fvIndex0 = fvIndex1 - 1;
    if (FS_NUM_SIDES_IN_FACE == fvIndex1) 
      fvIndex2 = 0;
    else
      fvIndex2 = fvIndex1 + 1;
    
    fv0 = &vertices[f->vertices[fvIndex0]];
    fv2 = &vertices[f->vertices[fvIndex2]];
    
    // Get two vectors from these points and normalize them. Then
    // just cross them to get the perpendicular vector. This is
    // the normal for the face. Add this to the normal for the
    // vertex. 
    faceVector0[0] = fv1->x - fv0->x;
    faceVector0[1] = fv1->y - fv0->y;
    faceVector0[2] = fv1->z - fv0->z;
    faceVector1[0] = fv2->x - fv1->x;
    faceVector1[1] = fv2->y - fv1->y;
    faceVector1[2] = fv2->z - fv1->z;
    length = sqrt( faceVector0[0]*faceVector0[0] +
               faceVector0[1]*faceVector0[1] +
               faceVector0[2]*faceVector0[2] );
    if (length > 0) {
      faceVector0[0] /= length;
      faceVector0[1] /= length;
      faceVector0[2] /= length;
    }
    length = sqrt( faceVector1[0]*faceVector1[0] +
               faceVector1[1]*faceVector1[1] +
               faceVector1[2]*faceVector1[2] );
    if (length > 0) {
      faceVector1[0] /= length;
      faceVector1[1] /= length;
      faceVector1[2] /= length;
    }

    // get the cross product
    faceNormal[0] = -faceVector1[1]*faceVector0[2] + 
      faceVector0[1]*faceVector1[2];
    faceNormal[1] = faceVector1[0]*faceVector0[2] - 
      faceVector0[0]*faceVector1[2];
    faceNormal[2] = -faceVector1[0]*faceVector0[1] + 
      faceVector0[0]*faceVector1[1];
    
    // add it to the normal vector at this vertex.
    fv1->nx += faceNormal[0];
    fv1->ny += faceNormal[1];
    fv1->nz += faceNormal[2];
      }
      
      // When all the faces for this vertex have been processed, the
      // normal at the vertex is the sum of all the normals for the
      // adjacent faces. Normalize it and we're done.
      faceNormal[0] = fv1->nx;
      faceNormal[1] = fv1->ny;
      faceNormal[2] = fv1->nz;
      length = sqrt( faceNormal[0]*faceNormal[0] +
             faceNormal[1]*faceNormal[1] +
             faceNormal[2]*faceNormal[2] );
      if (length > 0) {
    faceNormal[0] /= length;
    faceNormal[1] /= length;
    faceNormal[2] /= length;
      }
      
      // Add the final normal to the array.
      outputNormals->InsertNextTuple(faceNormal);
    }
    
    free (vertices);
    free (faces);
  }
#endif

  // Set all the arrays in the output.
  output->SetPoints (outputVertices);
  outputVertices->Delete();

#if FS_CALC_NORMALS
  output->GetPointData()->SetNormals (outputNormals);
  outputNormals->Delete();
#endif

  outputFaces->Squeeze(); 
  output->SetPolys(outputFaces);
  outputFaces->Delete();
}

void vtkFSSurfaceReader::PrintSelf(ostream& os, vtkIndent indent)
{
  vtkDataReader::PrintSelf(os,indent);
}
