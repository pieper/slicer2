/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkFSSurfaceAnnotationReader.cxx,v $
  Language:  C++
  Date:      $Date: 2003/04/14 21:49:10 $
  Version:   $Revision: 1.1 $

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
    this->UseExternalColorTableFile = false;
    strcpy (this->ColorTableFileName, "");
}

vtkFSSurfaceAnnotationReader::~vtkFSSurfaceAnnotationReader()
{
}

void vtkFSSurfaceAnnotationReader::SetColorTableFileName (char* name)
{
  if (NULL != name)
    {
      strcpy (this->ColorTableFileName, name);
    }
}

void vtkFSSurfaceAnnotationReader::DoUseExternalColorTableFile ()
{
  this->UseExternalColorTableFile = true;
}

void vtkFSSurfaceAnnotationReader::DontUseExternalColorTableFile ()
{
  this->UseExternalColorTableFile = false;
}


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
  int numColorTableEntries;
  int r;
  int g;
  int b;
  bool found;
  bool unassignedEntry;
  int stringLength;

  try 
    {
      
      if (olabels == NULL)
    {
      vtkErrorMacro(<< "ReadFSAnnotation: output is null");
      throw -1;
    }
      if (ocolors == NULL)
    {
      vtkErrorMacro(<< "ReadFSAnnotation: color table output is null");
      throw -1;
    }
      
      if (NULL == this->FileName) 
    {
      vtkErrorMacro(<< "ReadFSAnnotation: fileName not specified.");
      throw -1;
    }
      
      vtkDebugMacro(<<"Reading surface annotation data...");
      
      // Try to open the file.
      annotFile = fopen (this->FileName, "rb");
      if (NULL == annotFile) 
    {
      vtkErrorMacro (<< "ReadFSAnnotation: could not open file " 
             << this->FileName);
      throw FS_ERROR_LOADING_ANNOTATION;
    }  
      
      // Read the number of label elements in the file.
      read = vtkFSIO::ReadInt (annotFile, numLabels);
      if (read != 1)
    {
      vtkErrorMacro(<< "ReadFSAnnotation: error reading number of labels");
      throw FS_ERROR_PARSING_ANNOTATION;
    }
      if (numLabels <= 0) 
    {
      vtkErrorMacro (<< "ReadFSAnnotation: number of labels is "
             << "0 or negative, can't process file.");
      throw FS_ERROR_PARSING_ANNOTATION;
    }
      
      // Allocate our arrays to hold the rgb values from the annotation
      // file and the label indices into which we'll transform them.
      rgbs = (int*) calloc (numLabels, sizeof(int));
      labels = (int*) calloc (numLabels, sizeof(int));
      if (NULL == labels || NULL == rgbs)
      {
    vtkErrorMacro (<< "ReadFSAnnotation: couldn't allocate array with "
               << numLabels << " ints.");
    throw -1;
      }
      
      // Read in all the label rgb values.
      for (labelIndex = 0; labelIndex < numLabels; labelIndex ++ ) {
    
    if (feof (annotFile) ) 
      {
        vtkErrorMacro (<< "ReadFSAnnotation: unexpected EOF after " 
               << numLabels << " values read.");
        throw FS_ERROR_PARSING_ANNOTATION;
    }
    
    // Read a vertex index and an rgb value. Set the appropriate
    // value in the rgb array.
    read = vtkFSIO::ReadInt (annotFile, vertexIndex);
    if (read != 1)
      {
        vtkErrorMacro (<< "ReadFSAnnotation: error reading vertex index at"
               << labelIndex);
        throw FS_ERROR_PARSING_ANNOTATION;
      }
    read = vtkFSIO::ReadInt (annotFile, rgb);
    if (read != 1)
      {
        vtkErrorMacro (<< "ReadFSAnnotation: error reading rgb value at"
               << labelIndex);
        throw FS_ERROR_PARSING_ANNOTATION;
      }
    
    rgbs[vertexIndex] = rgb;
      }
      
      // Are we using an embedded or an external color table?
      if (this->UseExternalColorTableFile)
    {
      error = ReadExternalColorTable (this->ColorTableFileName,
                      &numColorTableEntries,
                      &colorTableRGBs,
                      &colorTableNames);
      if (0 != error)
        {
          throw error;
        }
    } 
      else 
    {
      error = ReadEmbeddedColorTable (annotFile,
                      &numColorTableEntries,
                      &colorTableRGBs,
                      &colorTableNames);
      if (0 != error)
        {
          // Throw a FS_NO_COLOR_TABLE here so that the caller can
          // see that while the annotation file was loaded
          // correctly, there was no embedded color table, so the
          // user must specify one.
          throw FS_NO_COLOR_TABLE;
        }
    }
      
      // Now match up rgb values with table entries to find the label
      // indices for each vertex.
      unassignedEntry = false;
      for (labelIndex = 0; labelIndex < numLabels; labelIndex++)
    {
      // Expand the rgb value into separate values.
      r = rgbs[labelIndex] & 0xff;
      g = (rgbs[labelIndex] >> 8) & 0xff;
      b = (rgbs[labelIndex] >> 16) & 0xff;

      // Look for this rgb value in the table.
      found = false;
      for (colorTableEntryIndex = 0; 
           colorTableEntryIndex < numColorTableEntries; 
           colorTableEntryIndex++)
        {
          if (r == colorTableRGBs[colorTableEntryIndex][0] &&
          g == colorTableRGBs[colorTableEntryIndex][1] &&
          b == colorTableRGBs[colorTableEntryIndex][2])
        {
          labels[labelIndex] = colorTableEntryIndex;
          found = true;
          continue;
        }
        }

      // Didn't find an entry so just set it to -1.
      if (!found)
        {
          unassignedEntry = true;
          labels[labelIndex] = -1;
        }
    }


      // Copy the names as a list into a new string. First find the
      // length that the string should be.
      stringLength = 0;
      for (colorTableEntryIndex = 0; 
       colorTableEntryIndex < numColorTableEntries; 
       colorTableEntryIndex++)
    {
      stringLength += strlen (colorTableNames[colorTableEntryIndex]);
      // for the other chars in the name: "nn {} " where nn is an index
      stringLength += 6; 
    }
      // Allocate the string and copy all the names in, along with
      // their index. This makes it possible to use the string to
      // allocate a tcl array.
      this->NamesList = (char*) calloc (stringLength+1, sizeof(char));
      for (colorTableEntryIndex = 0; 
       colorTableEntryIndex < numColorTableEntries; 
       colorTableEntryIndex++)
    {
      sprintf (this->NamesList, "%s%d {%s} ", this->NamesList,
           colorTableEntryIndex, 
           colorTableNames[colorTableEntryIndex]);
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
      ocolors->SetTableValue (colorTableEntryIndex, 
                  colorTableRGBs[colorTableEntryIndex][0],
                  colorTableRGBs[colorTableEntryIndex][1],
                  colorTableRGBs[colorTableEntryIndex][2],
                  1.0);
    }

      if (unassignedEntry)
    {
      result = FS_WARNING_UNASSIGNED_LABELS;
    }
    }

  catch (int code)
    {
      result = code;
    }

  // Close the file.
  if (NULL != annotFile)
    {
      fclose (annotFile);
    }

  // Delete the stuff we allocated.
  if (NULL != rgbs)
    {
      free (rgbs);
    }
  if (NULL != colorTableRGBs)
    {
      free (colorTableRGBs);
    }
  if (NULL != colorTableNames)
    {
      for (colorTableEntryIndex = 0; 
       colorTableEntryIndex < numColorTableEntries; 
       colorTableEntryIndex++)
    {
      free (colorTableNames[colorTableEntryIndex]);
    }
      free (colorTableNames);
    }
  
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

  try
    {
      // Embedded color tables are identified by a tag at the end of
      // the normal data. Right now there's only one kind of tag, so
      // just check for it. If there is no embedded table, we'll get
      // an eof error here.
      read = vtkFSIO::ReadInt (annotFile, tag);
      if (read != 1)
    {
      throw FS_NO_COLOR_TABLE;
    }
      
      if (FS_COLOR_TABLE_TAG == tag)
    {
      // Read the number of entries.
      read = vtkFSIO::ReadInt (annotFile, numColorTableEntries);
      if (read != 1)
        {
          vtkErrorMacro (<< "ReadEmbeddedColorTable: error reading "
                 << "number of entries");
          throw FS_ERROR_PARSING_COLOR_TABLE;
        }
      if (numColorTableEntries <= 0)
        {
          vtkErrorMacro (<< "ReadEmbeddedColorTable: color table has "
                 << numColorTableEntries << " entries.");
          throw FS_ERROR_PARSING_COLOR_TABLE;
        }
      
      // Read the table name.
      read = vtkFSIO::ReadInt (annotFile, nameLength);
      if (read != 1)
        {
          vtkErrorMacro (<< "ReadEmbeddedColorTable: error reading "
                 << "table name length");
          throw FS_ERROR_PARSING_COLOR_TABLE;
        }
      read = fread (tableName, sizeof(char), nameLength, annotFile);
      if (read != nameLength)
        {
          vtkErrorMacro (<< "ReadEmbeddedColorTable: error reading "
                 << "table name");
          throw FS_ERROR_PARSING_COLOR_TABLE;
        }
      
      // Allocate arrays for our r/g/b values and for our names.
      rgbValues = (int**) calloc (numColorTableEntries, sizeof(int) );
      names = (char**) calloc (numColorTableEntries, sizeof(char*) );
      if (NULL == rgbValues || NULL == names)
        {
          vtkErrorMacro (<< "ReadEmbeddedColorTable: error allocating "
                 << "rgb or name arrays with " 
                 << numColorTableEntries << " entries.");
          throw -1;
        }

      // Read all the entries.
      for (entryIndex = 0; entryIndex < numColorTableEntries; entryIndex++)
        {
          
          // Allocate the name.
          names[entryIndex] = 
        (char*) calloc (FS_COLOR_TABLE_ENTRY_NAME_LENGTH,sizeof(char));
          if (NULL == names[entryIndex])
        {
          vtkErrorMacro (<< "ReadEmbeddedColorTable: error allocating "
                 << "entry name " << entryIndex);
          throw -1;
        }
         
          // Allocate rgb holders.
          rgbValues[entryIndex] = (int*) calloc (3, sizeof(int));
          if (NULL == rgbValues[entryIndex])
        {
          vtkErrorMacro (<< "ReadEmbeddedColorTable: error allocating "
                 << "rgb entry " << entryIndex);
          throw -1;
        }
 
          // Read the name length and name.
          read = vtkFSIO::ReadInt (annotFile, nameLength);
          if (read != 1)
        {
          vtkErrorMacro (<< "ReadEmbeddedColorTable: error reading "
                 << "entry name length for entry " 
                 << entryIndex);
          throw FS_ERROR_PARSING_COLOR_TABLE;
        }
          read = fread (names[entryIndex], sizeof(char), 
                nameLength, annotFile);
          if (read != nameLength)
        {
          vtkErrorMacro (<< "ReadEmbeddedColorTable: error reading "
                 << "entry name for entry "
                 << entryIndex);
          throw FS_ERROR_PARSING_COLOR_TABLE;
        }
          
          // Read the in the rgb values.
          read = vtkFSIO::ReadInt (annotFile, rgbValues[entryIndex][0]);
          if (read != 1)
        {
          vtkErrorMacro (<< "ReadEmbeddedColorTable: error reading "
                 << "red value for entry " << entryIndex);
          throw FS_ERROR_PARSING_COLOR_TABLE;
        }
          read = vtkFSIO::ReadInt (annotFile, rgbValues[entryIndex][1]);
          if (read != 1)
        {
          vtkErrorMacro (<< "ReadEmbeddedColorTable: error reading "
                 << "green value for entry " << entryIndex);
          throw FS_ERROR_PARSING_COLOR_TABLE;
        }
          read = vtkFSIO::ReadInt (annotFile, rgbValues  [entryIndex][2]);
          if (read != 1)
        {
          vtkErrorMacro (<< "ReadEmbeddedColorTable: error reading "
                 << "blue value for entry " << entryIndex);
          throw FS_ERROR_PARSING_COLOR_TABLE;
        }
          
          // A flag value, but we'll just ignore it.
          read = vtkFSIO::ReadInt (annotFile, flag);
          if (read != 1)
        {
          vtkErrorMacro (<< "ReadEmbeddedColorTable: error reading "
                 << "flag value for entry " << entryIndex);
          throw FS_ERROR_PARSING_COLOR_TABLE;
        }
        }
    } 
      else // if (tag == FS_COLOR_TABLE_TAG)
    {
      throw FS_ERROR_PARSING_COLOR_TABLE;
    }
    }

  catch (int code) 
    {
      return code;
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

  try 
    {
      
      file = fopen (fileName, "r");
      if (NULL == file)
    {
      vtkErrorMacro (<< "ReadFSAnnotation: could not open file "
             << fileName);
      throw FS_ERROR_LOADING_COLOR_TABLE;
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
          throw FS_ERROR_PARSING_COLOR_TABLE;
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
      rgbValues = (int**) calloc (numColorTableEntries, sizeof(int) );
      names = (char**) calloc (numColorTableEntries, sizeof(char*) );
      if (NULL == rgbValues || NULL == names)
    {
      vtkErrorMacro (<< "ReadEmbeddedColorTable: error allocating "
             << "rgb or name arrays with " 
             << numColorTableEntries << " entries.");
      throw -1;
    }
      
      // Read all the entries.
      file = fopen (fileName, "r" );
      if (NULL == file)
    {
      vtkErrorMacro (<< "ReadFSAnnotation: could not open file " 
             << fileName);
      throw FS_ERROR_LOADING_ANNOTATION;
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
          throw FS_ERROR_PARSING_COLOR_TABLE;
        }  
          
          // Allocate the name.
          names[entryIndex] = 
        (char*) calloc (FS_COLOR_TABLE_ENTRY_NAME_LENGTH,sizeof(char));
          if (NULL == names[entryIndex])
        {
          vtkErrorMacro (<< "ReadEmbeddedColorTable: error allocating "
                 << "entry name " << entryIndex);
          throw -1;
        }
          
          // Allocate rgb holders.
          rgbValues[entryIndex] = (int*) calloc (3, sizeof(int));
          if (NULL == rgbValues[entryIndex])
        {
          vtkErrorMacro (<< "ReadEmbeddedColorTable: error allocating "
                 << "rgb entry " << entryIndex);
          throw -1;
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
    }

  catch (int code) 
    {
      return code;
    }

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
