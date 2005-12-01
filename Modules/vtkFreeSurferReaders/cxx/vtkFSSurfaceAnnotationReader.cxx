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
/*=========================================================================
  
Program:   Visualization Toolkit
Module:    $RCSfile: vtkFSSurfaceAnnotationReader.cxx,v $
Language:  C++
Date:      $Date: 2005/12/01 21:12:09 $
Version:   $Revision: 1.8 $

=========================================================================*/
#include <stdio.h>
#include "vtkFSSurfaceAnnotationReader.h"
#include "vtkObjectFactory.h"
#include "vtkFSIO.h"
#include "vtkByteSwap.h"
#include "vtkFloatArray.h"

//-------------------------------------------------------------------------
vtkFSSurfaceAnnotationReader* vtkFSSurfaceAnnotationReader::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkFSSurfaceAnnotationReader");
  if(ret)
  {
      return (vtkFSSurfaceAnnotationReader*)ret;
  }
  // If the factory was unable to create the object, then create it here.
  return new vtkFSSurfaceAnnotationReader;
}

vtkFSSurfaceAnnotationReader::vtkFSSurfaceAnnotationReader()
{
  this->Labels = NULL;
  this->Colors = NULL;
  this->NamesList = NULL;
  this->NumColorTableEntries = -1;
  this->UseExternalColorTableFile = 0; 
  strcpy (this->ColorTableFileName, "");
}

vtkFSSurfaceAnnotationReader::~vtkFSSurfaceAnnotationReader()
{
  if (NULL != this->NamesList)
  {
      free (this->NamesList);
  }
}

void vtkFSSurfaceAnnotationReader::SetColorTableFileName (char* name)
{
  if (NULL != name)
  {
      strcpy (this->ColorTableFileName, name);
  }
}

//void vtkFSSurfaceAnnotationReader::DoUseExternalColorTableFile ()
//{
//  this->UseExternalColorTableFile = true;
//}

//void vtkFSSurfaceAnnotationReader::DontUseExternalColorTableFile ()
//{
//  this->UseExternalColorTableFile = false;
//}



int vtkFSSurfaceAnnotationReader::ReadFSAnnotation()
{
  vtkIntArray *olabels = this->Labels;
  vtkLookupTable *ocolors = this->Colors;
  int result = 0;
  FILE* annotFile = NULL;
  int read;
  int numLabels;
  int* rgbs;
  int* labels;
  int labelIndex;
  int vertexIndex;
  int rgb;
  int error;
  int** colorTableRGBs;
  char** colorTableNames;
  int colorTableEntryIndex;
  int numColorTableEntries = 0;
  int r;
  int g;
  int b;
  bool found;
  bool unassignedEntry;
  int stringLength;

  int thisStep = 0;
  int totalSteps = 1;
  
  vtkDebugMacro( << "starting ReadFSAnnotation\n");
  if (olabels == NULL)
  {
      vtkErrorMacro(<< "ReadFSAnnotation: output is null");
      cerr << "ReadFSAnnotation: output is null" << endl;
      return -1;
  }
  if (ocolors == NULL)
  {
      vtkErrorMacro(<< "ReadFSAnnotation: color table output is null");
      cerr << "ReadFSAnnotation: color table output is null" << endl;
      return -1;
  }
  
  if (NULL == this->FileName) 
  {
      vtkErrorMacro(<< "ReadFSAnnotation: fileName not specified.");
      cerr << "ReadFSAnnotation: fileName not specified." << endl;
      return -1;
  }
  
  vtkDebugMacro( << "ReadFSAnnotation: Reading surface annotation data... from " << this->FileName << "\n");
  
  // Try to open the file.
  annotFile = fopen (this->FileName, "rb");
  if (NULL == annotFile) 
  {
      vtkErrorMacro (<< "\nReadFSAnnotation: could not open file\n " 
                     << this->FileName);
      cerr << "\nReadFSAnnotation: could not open file\n " 
           << this->FileName << endl;
      return FS_ERROR_LOADING_ANNOTATION;
  }  
  
  // Read the number of label elements in the file.
  read = vtkFSIO::ReadInt (annotFile, numLabels);
  vtkDebugMacro( << "ReadFSAnnotation:  after ReadInt, numLabels = " << numLabels << endl);
  
  if (read != 1)
  {
      vtkErrorMacro(<< "ReadFSAnnotation: error reading number of labels");
      cerr << "ReadFSAnnotation: error reading number of labels" << endl;
      fclose (annotFile);
      return FS_ERROR_PARSING_ANNOTATION;
  }
  if (numLabels <= 0) 
  {
      vtkErrorMacro (<< "\nReadFSAnnotation: number of labels is "
                     << "0 or negative, can't process file.");
      cerr << "\nReadFSAnnotation: number of labels is "
           << "0 or negative, can't process file." << endl;
      fclose (annotFile);
      return FS_ERROR_PARSING_ANNOTATION;
  }
  
  // Allocate our arrays to hold the rgb values from the annotation
  // file and the label indices into which we'll transform them.
  vtkDebugMacro( << "nReadFSAnnotation: Callocing rgbs and labels to be numlabels ints " << numLabels << endl);
  rgbs = (int*) calloc (numLabels, sizeof(int));
  labels = (int*) calloc (numLabels, sizeof(int));
  if (NULL == labels || NULL == rgbs)
  {
      vtkErrorMacro (<< "\nReadFSAnnotation: couldn't allocate array with\n "
                     << numLabels << " ints.");
      cerr << "\nReadFSAnnotation: couldn't allocate array with\n "
           << numLabels << " ints." << endl;
      fclose (annotFile);
      return -1;
  }
  
  // Read in all the label rgb values.
  vtkDebugMacro( << "ReadFSAnnotation:  About to read in all the label rgb values, up to " << numLabels << ".\n");

  // this calc will be wrong, as it doesn't count the setting up of the colour
  // table stuff.
  totalSteps = numLabels*2;
      
  for (labelIndex = 0; labelIndex < numLabels; labelIndex ++ )
  {    
      if (feof (annotFile) ) 
      {
          vtkErrorMacro (<< "\nReadFSAnnotation: unexpected EOF after\n " 
                         << numLabels << " values read.");
          cerr << "\nReadFSAnnotation: unexpected EOF after\n " 
               << numLabels << " values read." << endl;
          fclose (annotFile);
          free (rgbs);
          free (labels);
          return FS_ERROR_PARSING_ANNOTATION;
      }
      
      // Read a vertex index and an rgb value. Set the appropriate
      // value in the rgb array.
      read = vtkFSIO::ReadInt (annotFile, vertexIndex);
      if (read != 1)
      {
          vtkErrorMacro (<< "\nReadFSAnnotation: error reading vertex index at\n"
                         << labelIndex);
          cerr << "\nReadFSAnnotation: error reading vertex index at\n"
               << labelIndex << endl;
          fclose (annotFile);
          free (rgbs);
          free (labels);
          return FS_ERROR_PARSING_ANNOTATION;
      }
      read = vtkFSIO::ReadInt (annotFile, rgb);
      if (read != 1)
      {
          vtkErrorMacro (<< "\nReadFSAnnotation: error reading rgb value at\n"
                         << labelIndex);
          cerr << "\nReadFSAnnotation: error reading rgb value at\n"
               << labelIndex << endl;
          fclose (annotFile);
          free (rgbs);
          free (labels);
          return FS_ERROR_PARSING_ANNOTATION;
      }
      
    rgbs[vertexIndex] = rgb;
    thisStep++;
    if (thisStep % 100 == 0)
    {
        this->UpdateProgress(1.0*thisStep/totalSteps);
    }
  }
  
  
  // Are we using an embedded or an external color table?
  vtkDebugMacro( << "ReadFSAnnotation: Are we using an external color table?\n\t" << this->UseExternalColorTableFile << endl);
  
  if (this->UseExternalColorTableFile != 0)
  {
      vtkDebugMacro(<< "ReadFSAnnotation: Using external color table file\n");
      error = ReadExternalColorTable (this->ColorTableFileName,
                      &numColorTableEntries,
                      &colorTableRGBs,
                      &colorTableNames);
      if (0 != error)
      {
          cerr << "ReadFSAnnotation: Got an error on reading external colour table " << error << endl;
          fclose (annotFile);
          free (rgbs);
          free (labels);
          return error;
      }
  } 
  else 
  {
      vtkDebugMacro( << "ReadFSAnnotation: Not using external color table file\n");
      error = ReadEmbeddedColorTable (annotFile,
                                      &numColorTableEntries,
                                      &colorTableRGBs,
                                      &colorTableNames);
      if (0 != error)
      {
          vtkDebugMacro( << "ReadFSAnnotation: Got an error on reading embeded colour table " << error << endl);
          // Return FS_NO_COLOR_TABLE here so that the caller can
          // see that while the annotation file was loaded
          // correctly, there was no embedded color table, so the
          // user must specify one.
          fclose (annotFile);
          //free (rgbs);
          //free (labels);
          vtkDebugMacro( << "ReadFSAnnotation: Returning fs no color table: " << FS_NO_COLOR_TABLE << endl);
          return FS_NO_COLOR_TABLE;
      }
  }

  
      
  // Now match up rgb values with table entries to find the label
  // indices for each vertex.
  vtkDebugMacro( << "ReadFSAnnotation: Now match up rgb values with table entries to find the label indices for each vertex, numLabels = " << numLabels << ", numColorTableEntries = " << numColorTableEntries << endl);
  
  unassignedEntry = false;
  for (labelIndex = 0; labelIndex < numLabels; labelIndex++)
  {
      vtkDebugMacro( << "ReadFSAnnotation: LabelIndex = " << labelIndex << endl);
      // Expand the rgb value into separate values.
      r = rgbs[labelIndex] & 0xff;
      //cerr << "\t r " << r;
      g = (rgbs[labelIndex] >> 8) & 0xff;
      //cerr <<  " g " << g;
      b = (rgbs[labelIndex] >> 16) & 0xff;
      //cerr << " b " << b << endl;

      // Look for this rgb value in the table.
      found = false;
      for (colorTableEntryIndex = 0; 
           (found == false) && (colorTableEntryIndex < numColorTableEntries); 
           colorTableEntryIndex++)
      {
          if (colorTableRGBs[colorTableEntryIndex] != NULL &&
              r == colorTableRGBs[colorTableEntryIndex][0] &&
              g == colorTableRGBs[colorTableEntryIndex][1] &&
              b == colorTableRGBs[colorTableEntryIndex][2])
          {
              labels[labelIndex] = colorTableEntryIndex;
              found = true;
              continue;
          }
          else
          {
              if ( colorTableRGBs[colorTableEntryIndex] == NULL)
              {
                  // let's fail silently for now, as the scan through the
                  // colour table may encounter an index where the colour
                  // hasn't been initialised
                  vtkDebugMacro(<<"ReadFSAnnotation ERROR: null entry at " << colorTableEntryIndex << " of the color table (label index " << labelIndex << ")\n");
                  cerr << "ReadFSAnnotation ERROR: null entry at " << colorTableEntryIndex << " of the color table (label index " << labelIndex << ")\n";
                  //fclose (annotFile);
                  //return FS_ERROR_PARSING_ANNOTATION;
              }
          }
          
      }
      thisStep++;
      if (thisStep % 100 == 0)
      {
          this->UpdateProgress(1.0*thisStep/totalSteps);
      }
      
      // Didn't find an entry so just set it to -1.
      if (!found)
      {
          vtkDebugMacro(<< "ReadFSAnnotation: Not found, returning a -1 in labels[" << labelIndex << "]\n");
          unassignedEntry = true;
          labels[labelIndex] = -1;
      }
  }
  
  // reset total steps, as have to do stuff for the colour table entries
  totalSteps += 3*numColorTableEntries;
  
  // Copy the names as a list into a new string. First find the
  // length that the string should be.
  vtkDebugMacro(<< "ReadFSAnnotation: Copy the names as a list into a new string\n");
  stringLength = 0;
  for (colorTableEntryIndex = 0; 
       colorTableEntryIndex < numColorTableEntries; 
       colorTableEntryIndex++)
  {
      if (colorTableNames[colorTableEntryIndex] != NULL)
      {
          stringLength += strlen (colorTableNames[colorTableEntryIndex]);
          // for the other chars in the name: "nn {} " where nn is an index
          stringLength += 6;
      }
      else
      { cerr << "WARNING: null colour table names entry at index " << colorTableEntryIndex << endl; }
      thisStep++;
      if (thisStep % 100 == 0)
      {
          this->UpdateProgress(1.0*thisStep/totalSteps);
      }
  }
  // Allocate the string and copy all the names in, along with
  // their index. This makes it possible to use the string to
  // allocate a tcl array.
  if (NULL != this->NamesList)
  {
      free (this->NamesList);
  }
  this->NamesList = (char*) calloc (stringLength+1, sizeof(char));
  for (colorTableEntryIndex = 0; 
       colorTableEntryIndex < numColorTableEntries; 
       colorTableEntryIndex++)
  {
      if (colorTableNames[colorTableEntryIndex] != NULL)
      {
          sprintf (this->NamesList, "%s%d {%s} ", this->NamesList,
                   colorTableEntryIndex, 
                   colorTableNames[colorTableEntryIndex]);
      }
      thisStep++;
      if (thisStep % 100 == 0)
      {
          this->UpdateProgress(1.0*thisStep/totalSteps);
      }
  }
  this->NumColorTableEntries = numColorTableEntries;
  
  
  // We're done, so now start setting the values in our output objects.
  
  // Set all the values in the output array.
  olabels->SetArray (labels, numLabels, 0);
  
  // Now convert the color table arrays to a real lookup table.
  ocolors->SetNumberOfColors (numColorTableEntries);
  ocolors->SetTableRange (0.0, 255.0);
  for (colorTableEntryIndex = 0; 
       colorTableEntryIndex < numColorTableEntries; 
       colorTableEntryIndex++)
  {
      if (colorTableRGBs[colorTableEntryIndex] != NULL)
      {
          ocolors->SetTableValue (colorTableEntryIndex, 
                                  colorTableRGBs[colorTableEntryIndex][0],
                                  colorTableRGBs[colorTableEntryIndex][1],
                                  colorTableRGBs[colorTableEntryIndex][2],
                                  1.0);
      }
      else
      {
          // use a default value
          cerr << "WARNING: colorTableRGBs null at entry index " <<  colorTableEntryIndex << ", using default value of 0 0 0" << endl;
          ocolors->SetTableValue(colorTableEntryIndex, 0, 0, 0, 1.0);
      }
      thisStep++;
      if (thisStep % 100 == 0)
      {
          this->UpdateProgress(1.0*thisStep/totalSteps);
      }
  }
  
  if (unassignedEntry)
  {
      result = FS_WARNING_UNASSIGNED_LABELS;
  }

  this->SetProgressText("");
  this->UpdateProgress(0.0);
  
  // Close the file.
  fclose (annotFile);
  
  // Delete the stuff we allocated. This causes a crash in Slicer, so comment
  // it out for now
  /*
  vtkDebugMacro(<< "ReadFSAnnotation: Freeing rgbs, colorTableRGBs, colorTableNames\n");
  free (rgbs);
  vtkDebugMacro( << "done rgbs\n");
  for (colorTableEntryIndex = 0; 
       colorTableEntryIndex < numColorTableEntries; 
       colorTableEntryIndex++)
  {
      // both loops are causing bus errors/seg faults
      vtkDebugMacro( << colorTableEntryIndex << "\t");
      if (colorTableNames[colorTableEntryIndex] != NULL)
      {
          free (colorTableNames[colorTableEntryIndex]);
      }
      else
      {
          vtkDebugMacro( << "WARNING: tried to free null entry in colorTableNames at index " << colorTableEntryIndex << endl);
      }
     
      if (colorTableRGBs[colorTableEntryIndex] != NULL)
      {
          free (colorTableRGBs[colorTableEntryIndex]);
      }
      else
      {
          vtkDebugMacro( << "WARNING: tried to free null entry in colorTableRGBs at index " << colorTableEntryIndex << endl);
      }
      vtkDebugMacro( << "rgbs\n");
  }
  //vtkDebugMacro( << "done looping\n");
  free (colorTableNames);
  vtkDebugMacro( << " done colortablenames\n");
  free (colorTableRGBs);
  vtkDebugMacro( << " done colortablergbs\n");
  */
  vtkDebugMacro( << "ReadFSAnnotation: Returning " << result << endl);
  return result;
}

int vtkFSSurfaceAnnotationReader::ReadEmbeddedColorTable (FILE* annotFile,
                              int* onumEntries, 
                              int*** orgbValues, 
                              char*** onames)
{
  int read;
  int label;
  int tag;
  int error;
  int numColorTableEntries;
  int nameLength;
  char tableName[FS_COLOR_TABLE_NAME_LENGTH];
  int entryIndex;
  int** rgbValues;
  char** names;
  int flag;
  int deleteIndex;

  vtkDebugMacro( << "Starting ReadEmbeddedColorTable\n");
  // Embedded color tables are identified by a tag at the end of
  // the normal data. Right now there's only one kind of tag, so
  // just check for it. If there is no embedded table, we'll get
  // an eof error here.
  read = vtkFSIO::ReadInt (annotFile, tag);
  if (read != 1)
  {
      return FS_NO_COLOR_TABLE;
  }
  
  if (FS_COLOR_TABLE_TAG == tag)
  {
      // Read the number of entries.
      read = vtkFSIO::ReadInt (annotFile, numColorTableEntries);
      if (read != 1)
      {
          vtkErrorMacro (<< "\nReadEmbeddedColorTable: error reading "
                         << "number of entries");
          cerr << "\nReadEmbeddedColorTable: error reading "
               << "number of entries" << endl;
          return FS_ERROR_PARSING_COLOR_TABLE;
      }
      if (numColorTableEntries <= 0)
      {
          vtkErrorMacro (<< "\nReadEmbeddedColorTable: color table has "
                         << numColorTableEntries << " entries.");
          cerr << "\nReadEmbeddedColorTable: color table has "
               << numColorTableEntries << " entries." << endl;
          return FS_ERROR_PARSING_COLOR_TABLE;
      }
      
      // Read the table name.
      read = vtkFSIO::ReadInt (annotFile, nameLength);
      if (read != 1)
      {
          vtkErrorMacro (<< "\nReadEmbeddedColorTable: error reading "
                         << "table name length");
          cerr << "\nReadEmbeddedColorTable: error reading "
               << "table name length" << endl;
          return FS_ERROR_PARSING_COLOR_TABLE;
      }
      read = fread (tableName, sizeof(char), nameLength, annotFile);
      if (read != nameLength)
      {
          vtkErrorMacro (<< "\nReadEmbeddedColorTable: error reading "
                         << "table name");
          cerr << "\nReadEmbeddedColorTable: error reading "
               << "table name" << endl;
          return FS_ERROR_PARSING_COLOR_TABLE;
      }
      
      // Allocate arrays for our r/g/b values and for our names.
      rgbValues = (int**) calloc (numColorTableEntries, sizeof(int) );
      names = (char**) calloc (numColorTableEntries, sizeof(char*) );
      if (NULL == rgbValues || NULL == names)
      {
          vtkErrorMacro (<< "\nReadEmbeddedColorTable: error allocating "
                         << "rgb or name arrays with\n " 
                         << numColorTableEntries << " entries.");
          cerr << "\nReadEmbeddedColorTable: error allocating "
               << "rgb or name arrays with\n " 
               << numColorTableEntries << " entries." << endl;
          return -1;
      }
      
      // Read all the entries.
      for (entryIndex = 0; entryIndex < numColorTableEntries; entryIndex++)
      {
          
          // Allocate the name.
          names[entryIndex] = 
              (char*) calloc (FS_COLOR_TABLE_ENTRY_NAME_LENGTH,sizeof(char));
          if (NULL == names[entryIndex])
          {
              vtkErrorMacro (<< "\nReadEmbeddedColorTable: error allocating\n "
                             << "entry name " << entryIndex);
              cerr << "\nReadEmbeddedColorTable: error allocating\n "
                   << "entry name " << entryIndex << endl;
              free (rgbValues);
              for (deleteIndex = 0; deleteIndex < entryIndex; deleteIndex++)
              {
                  free (names[deleteIndex]);
              }
              free (names);
              return -1;
          }
          
          // Allocate rgb holders.
          //cerr << "\nReadEmbeddedColorTable: allocating "
          //         << "rgb entry " << entryIndex << endl;
          rgbValues[entryIndex] = (int*) calloc (3, sizeof(int));
          if (NULL == rgbValues[entryIndex])
          {
              vtkErrorMacro (<< "\nReadEmbeddedColorTable: error allocating\n "
                             << "rgb entry " << entryIndex);
              cerr << "\nReadEmbeddedColorTable: error allocating\n "
                   << "rgb entry " << entryIndex << endl;
              free (rgbValues);
              for (deleteIndex = 0; deleteIndex <= entryIndex; deleteIndex++)
              {
                  free (names[deleteIndex]);
              }
              free (names);
              return -1;
          }
          
          // Read the name length and name.
          read = vtkFSIO::ReadInt (annotFile, nameLength);
          if (read != 1)
          {
              vtkErrorMacro (<< "\nReadEmbeddedColorTable: error reading\n"
                             << "entry name length for entry " 
                             << entryIndex);
              cerr << "\nReadEmbeddedColorTable: error reading\n"
                   << "entry name length for entry " 
                   << entryIndex << endl;
              free (rgbValues);
              for (deleteIndex = 0; deleteIndex <= entryIndex; deleteIndex++)
              {
                  free (names[deleteIndex]);
              }
              free (names);
              return FS_ERROR_PARSING_COLOR_TABLE;
          }
          read = fread (names[entryIndex], sizeof(char), 
                        nameLength, annotFile);
          if (read != nameLength)
          {
              vtkErrorMacro (<< "\nReadEmbeddedColorTable: error reading\n"
                             << "entry name for entry "
                             << entryIndex);
              cerr << "\nReadEmbeddedColorTable: error reading\n"
                   << "entry name for entry "
                   << entryIndex << endl;
              free (rgbValues);
              for (deleteIndex = 0; deleteIndex <= entryIndex; deleteIndex++)
              {
                  free (names[deleteIndex]);
              }
              free (names);
              return FS_ERROR_PARSING_COLOR_TABLE;
          }
          
          // Read the in the rgb values.
          read = vtkFSIO::ReadInt (annotFile, rgbValues[entryIndex][0]);
          if (read != 1)
          {
              vtkErrorMacro (<< "\nReadEmbeddedColorTable: error reading\n "
                             << "red value for entry " << entryIndex);
              cerr << "\nReadEmbeddedColorTable: error reading\n "
                   << "red value for entry " << entryIndex << endl;
              free (rgbValues);
              for (deleteIndex = 0; deleteIndex <= entryIndex; deleteIndex++)
              {
                  free (names[deleteIndex]);
              }
              free (names);
              return FS_ERROR_PARSING_COLOR_TABLE;
          }
          read = vtkFSIO::ReadInt (annotFile, rgbValues[entryIndex][1]);
          if (read != 1)
          {
              vtkErrorMacro (<< "\nReadEmbeddedColorTable: error reading "
                             << "green value for entry " << entryIndex);
              cerr << "\nReadEmbeddedColorTable: error reading "
                   << "green value for entry " << entryIndex << endl;
              free (rgbValues);
              for (deleteIndex = 0; deleteIndex <= entryIndex; deleteIndex++)
              {
                  free (names[deleteIndex]);
              }
              free (names);
              return FS_ERROR_PARSING_COLOR_TABLE;
          }
          read = vtkFSIO::ReadInt (annotFile, rgbValues  [entryIndex][2]);
          if (read != 1)
          {
              vtkErrorMacro (<< "\nReadEmbeddedColorTable: error reading "
                             << "blue value for entry " << entryIndex);
              cerr << "\nReadEmbeddedColorTable: error reading "
                   << "blue value for entry " << entryIndex << endl;
              free (rgbValues);
              for (deleteIndex = 0; deleteIndex <= entryIndex; deleteIndex++)
              {
                  free (names[deleteIndex]);
              }
              free (names);
              return FS_ERROR_PARSING_COLOR_TABLE;
          }
          
          // A flag value, but we'll just ignore it.
          read = vtkFSIO::ReadInt (annotFile, flag);
          if (read != 1)
          {
              vtkErrorMacro (<< "\nReadEmbeddedColorTable: error reading "
                             << "flag value for entry " << entryIndex);
              cerr << "\nReadEmbeddedColorTable: error reading "
                   << "flag value for entry " << entryIndex << endl;
              free (rgbValues);
              for (deleteIndex = 0; deleteIndex <= entryIndex; deleteIndex++)
              {
                  free (names[deleteIndex]);
              }
              free (names);
              return FS_ERROR_PARSING_COLOR_TABLE;
          }
      }
  } 
  else // if (tag == FS_COLOR_TABLE_TAG)
  {
      return FS_ERROR_PARSING_COLOR_TABLE;
  }
  
  *onumEntries = numColorTableEntries;
  *orgbValues = rgbValues;
  *onames = names;
  
  return 0;
}

int vtkFSSurfaceAnnotationReader::ReadExternalColorTable (char* fileName,
                              int* onumEntries, 
                              int*** orgbValues, 
                              char*** onames)
{
  
  FILE* file = NULL;
  int highestIndex;
  int lineNumber;
  char lineText[1024];
  int read;
  char* got;
  int numColorTableEntries;
  int entryIndex;
  int** rgbValues;
  char** names;
  char name[1024];
  int r;
  int g;
  int b;
  int deleteIndex;

  vtkDebugMacro(<< "Starting ReadExternalColorTable with file " << fileName << endl);
  file = fopen (fileName, "r");
  if (NULL == file)
  {
      vtkErrorMacro (<< "\nReadExternalColorTable: could not open file\n "
                     << fileName);
      cerr << "\nReadExternalColorTable: could not open file\n "
           << fileName << endl;
      return FS_ERROR_LOADING_COLOR_TABLE;
  }
  
  highestIndex = 0;
  lineNumber = 0;
  while (!feof (file)) 
  {
      got = fgets (lineText, 1024, file);
      if (got)
      {
          read = sscanf (lineText, "%d %*s %d %d %d %*s",
                         &entryIndex, &r, &g, &b);
          if (4 != read && -1 != read) 
          {
              vtkWarningMacro(<< "ReadExternalColorTable: error parsing "
                              << fileName << ": Malformed line "
                              << lineNumber );
              return FS_ERROR_PARSING_COLOR_TABLE;
          }  
          
          if (entryIndex > highestIndex)
          {
              highestIndex = entryIndex;
          }
          
          lineNumber++;
      }
  }
  
  fclose (file);
  file = NULL;
  
  // We got the highest zero-based index, so our number of entries is
  // that plus one.
  numColorTableEntries = highestIndex + 1;
  
  // Allocate our arrays.
  vtkDebugMacro(<< "\nReadExternalColorTable: callocing rgbValues as int** x " << numColorTableEntries << endl);
  
  rgbValues = (int**) calloc (numColorTableEntries, sizeof(int) );
  names = (char**) calloc (numColorTableEntries, sizeof(char*) );
  if (NULL == rgbValues || NULL == names)
  {
      vtkErrorMacro (<< "\nReadExternalColorTable: error allocating "
                     << "rgb or name arrays with " 
                     << numColorTableEntries << " entries.");
      cerr << "\nReadExternalColorTable: error allocating "
           << "rgb or name arrays with " 
           << numColorTableEntries << " entries." << endl;
      return -1;
  }
  
  // Read all the entries.
  file = fopen (fileName, "r" );
  if (NULL == file)
  {
      vtkErrorMacro (<< "\nReadExternalColorTable: could not open file\n " 
                     << fileName);
      cerr << "\nReadExternalColorTable: could not open file\n " 
           << fileName << endl;
      free (rgbValues);
      free (names);
      return FS_ERROR_LOADING_ANNOTATION;
  }
  
  lineNumber = 0;
  while (!feof (file) ) 
  {
      
      got = fgets (lineText, 1024, file);
      if (got)
      {
          read = sscanf (lineText, "%d %s %d %d %d %*s",
                         &entryIndex, name, &r, &g, &b);
          if (5 != read && -1 != read) 
          {
              vtkWarningMacro(<< "ReadExternalColorTable: error parsing "
                              << fileName << ": Malformed line "
                              << lineNumber );
              free (rgbValues);
              free (names);
              return FS_ERROR_PARSING_COLOR_TABLE;
          }  
          
          // Allocate the name.
          names[entryIndex] = 
              (char*) calloc (FS_COLOR_TABLE_ENTRY_NAME_LENGTH,sizeof(char));
          if (NULL == names[entryIndex])
          {
              vtkErrorMacro (<< "\nReadExternalColorTable: error allocating "
                             << "entry name " << entryIndex);
              cerr << "\nReadExternalColorTable: error allocating "
                   << "entry name " << entryIndex << endl;
              free (rgbValues);
              for (deleteIndex = 0; deleteIndex < entryIndex; deleteIndex++)
              {
                  free (names[deleteIndex]);
              }
              free (names);
              return -1;
          }
          
          // Allocate rgb holders.
          //cerr << "ReadExternalColorTable: allocating colour table 3 ints at " << entryIndex << endl;
          rgbValues[entryIndex] = (int*) calloc (3, sizeof(int));
          if (NULL == rgbValues[entryIndex])
          {
              vtkErrorMacro (<< "\nReadExternalColorTable: error allocating "
                             << "rgb entry " << entryIndex);
              cerr << "\nReadExternalColorTable: error allocating "
                   << "rgb entry " << entryIndex << endl;
              free (rgbValues);
              for (deleteIndex = 0; deleteIndex <= entryIndex; deleteIndex++)
              {
                  free (names[deleteIndex]);
              }
              free (names);
              return -1;
          }
          
          // Set values.
          rgbValues[entryIndex][0] = r;
          rgbValues[entryIndex][1] = g;
          rgbValues[entryIndex][2] = b;
          
          // Copy the name.
          strcpy (names[entryIndex], name);
          
          lineNumber++;
      }
      
  }
  vtkDebugMacro(<< "ReadExternalColorTable: got num color table entries = " << numColorTableEntries << ", and line number = " << lineNumber << endl);
  *onumEntries = numColorTableEntries;
  *orgbValues = rgbValues;
  *onames = names;
  
  return 0;
}

char* vtkFSSurfaceAnnotationReader::GetColorTableNames()
{
  if (NULL == this->NamesList || this->NumColorTableEntries < 0)
  {
      return NULL;
  }
  
  return this->NamesList;
}


void vtkFSSurfaceAnnotationReader::PrintSelf(ostream& os, vtkIndent indent)
{
  vtkDataReader::PrintSelf(os,indent);
}
