#=auto==========================================================================
# (c) Copyright 2003 Massachusetts Institute of Technology
#
# Permission is hereby granted, without payment, to copy, modify, display 
# and distribute this software and its documentation, if any, for any purpose, 
# provided that the above copyright notice and the following three paragraphs 
# appear on all copies of this software.  Use of this software constitutes 
# acceptance of these terms and conditions.
#
# IN NO EVENT SHALL MIT BE LIABLE TO ANY PARTY FOR DIRECT, INDIRECT, SPECIAL, 
# INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT OF THE USE OF THIS SOFTWARE 
# AND ITS DOCUMENTATION, EVEN IF MIT HAS BEEN ADVISED OF THE POSSIBILITY OF 
# SUCH DAMAGE.
#
# MIT SPECIFICALLY DISCLAIMS ANY EXPRESS OR IMPLIED WARRANTIES INCLUDING, 
# BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR 
# A PARTICULAR PURPOSE, AND NON-INFRINGEMENT.
#
# THE SOFTWARE IS PROVIDED "AS IS."  MIT HAS NO OBLIGATION TO PROVIDE 
# MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS. 
#
#===============================================================================
# FILE:        EMSegmentAlgorithm.tcl
# PROCEDURES:  
#   EMSegmentSetVtkSuperClassSetting
#   EMSegmentAlgorithmStart
#==========================================================================auto=
#=auto==========================================================================
# (c) Copyright 2002 Massachusetts Institute of Technology
#
# Permission is hereby granted, without payment, to copy, modify, display 
# and distribute this software and its documentation, if any, for any purpose, 
# provided that the above copyright notice and the following three paragraphs 
# appear on all copies of this software.  Use of this software constitutes 
# acceptance of these terms and conditions.
#
# IN NO EVENT SHALL MIT BE LIABLE TO ANY PARTY FOR DIRECT, INDIRECT, SPECIAL, 
# INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT OF THE USE OF THIS SOFTWARE 
# AND ITS DOCUMENTATION, EVEN IF MIT HAS BEEN ADVISED OF THE POSSIBILITY OF 
# SUCH DAMAGE.
#
# MIT SPECIFICALLY DISCLAIMS ANY EXPRESS OR IMPLIED WARRANTIES INCLUDING, 
# BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR 
# A PARTICULAR PURPOSE, AND NON-INFRINGEMENT.
#
# THE SOFTWARE IS PROVIDED "AS IS."  MIT HAS NO OBLIGATION TO PROVIDE 
# MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS. 
#
#===============================================================================
 
#-------------------------------------------------------------------------------
# .PROC  EMSegmentSetVtkGenericClassSetting
# Settings defined by vtkImageEMGenericClass, i.e. variables that have to be set for both CLASS and SUPERCLASS 
# Only loaded for private version 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EMSegmentSetVtkGenericClassSetting {vtkGenericClass Sclass} {
  global EMSegment
  $vtkGenericClass SetNumInputImages $EMSegment(NumInputChannel) 
  eval $vtkGenericClass SetSegmentationBoundaryMin $EMSegment(SegmentationBoundaryMin,0) $EMSegment(SegmentationBoundaryMin,1) $EMSegment(SegmentationBoundaryMin,2)
  eval $vtkGenericClass SetSegmentationBoundaryMax $EMSegment(SegmentationBoundaryMax,0) $EMSegment(SegmentationBoundaryMax,1) $EMSegment(SegmentationBoundaryMax,2)

  $vtkGenericClass SetProbDataWeight $EMSegment(Cattrib,$Sclass,LocalPriorWeight)
  $vtkGenericClass SetTissueProbability $EMSegment(Cattrib,$Sclass,Prob)

  $vtkGenericClass SetPrintWeights $EMSegment(Cattrib,$Sclass,PrintWeights)

  for {set y 0} {$y < $EMSegment(NumInputChannel)} {incr y} {
      if {[info exists EMSegment(Cattrib,$Sclass,InputChannelWeights,$y)]} {$vtkGenericClass SetInputChannelWeights $EMSegment(Cattrib,$Sclass,InputChannelWeights,$y) $y}
  }
}

#-------------------------------------------------------------------------------
# .PROC  EMSegmentSetVtkPrivateSuperClassSetting
# Setting up everything for the super classes  
# Only loaded for private version 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EMSegmentSetVtkPrivateSuperClassSetting {SuperClass} {
  global EMSegment Volume
  # Reads in the value for each class individually
  # puts "EMSegmentSetVtkSuperClassSetting $SuperClass"
  catch { EMSegment(Cattrib,$SuperClass,vtkImageEMSuperClass) Delete}
  vtkImageEMPrivateSuperClass EMSegment(Cattrib,$SuperClass,vtkImageEMSuperClass)      

  # Define SuperClass specific parameters
  EMSegmentSetVtkGenericClassSetting EMSegment(Cattrib,$SuperClass,vtkImageEMSuperClass) $SuperClass

  EMSegment(Cattrib,$SuperClass,vtkImageEMSuperClass) SetPrintFrequency $EMSegment(Cattrib,$SuperClass,PrintFrequency)
  EMSegment(Cattrib,$SuperClass,vtkImageEMSuperClass) SetPrintBias      $EMSegment(Cattrib,$SuperClass,PrintBias)
  EMSegment(Cattrib,$SuperClass,vtkImageEMSuperClass) SetPrintLabelMap  $EMSegment(Cattrib,$SuperClass,PrintLabelMap)

  set ClassIndex 0
  foreach i $EMSegment(Cattrib,$SuperClass,ClassList) {
    if {$EMSegment(Cattrib,$i,IsSuperClass)} {
        if {[EMSegmentSetVtkPrivateSuperClassSetting $i]} {return [EMSegment(Cattrib,$i,vtkImageEMSuperClass) GetErrorFlag]}
          EMSegment(Cattrib,$SuperClass,vtkImageEMSuperClass) AddSubClass EMSegment(Cattrib,$i,vtkImageEMSuperClass) $ClassIndex
    } else {
      catch {EMSegment(Cattrib,$i,vtkImageEMClass) destroy}
      vtkImageEMPrivateClass EMSegment(Cattrib,$i,vtkImageEMClass)      
      EMSegmentSetVtkGenericClassSetting EMSegment(Cattrib,$i,vtkImageEMClass) $i

      EMSegment(Cattrib,$i,vtkImageEMClass) SetLabel             $EMSegment(Cattrib,$i,Label) 
      EMSegment(Cattrib,$i,vtkImageEMClass) SetShapeParameter    $EMSegment(Cattrib,$i,ShapeParameter)

      if {$EMSegment(Cattrib,$i,ProbabilityData) != $Volume(idNone)} {
          EMSegment(Cattrib,$i,vtkImageEMClass) SetProbDataPtr [Volume($EMSegment(Cattrib,$i,ProbabilityData),vol) GetOutput]
      } 
      for {set y 0} {$y < $EMSegment(NumInputChannel)} {incr y} {
          EMSegment(Cattrib,$i,vtkImageEMClass) SetLogMu $EMSegment(Cattrib,$i,LogMean,$y) $y
          for {set x 0} {$x < $EMSegment(NumInputChannel)} {incr x} {
            EMSegment(Cattrib,$i,vtkImageEMClass) SetLogCovariance $EMSegment(Cattrib,$i,LogCovariance,$y,$x) $y $x
          }
      }
      if {$EMSegment(IntensityAvgClass) == $EMSegment(Cattrib,$i,Label)} {
          # Transfere Intensity correction filter stuff
          set index 0
          EMSegment(vtkEMSegment) EMSetIntensityAvgClass  EMSegment(Cattrib,$i,vtkImageEMClass)
          foreach v $EMSegment(SelVolList,VolumeList) {       
             EMSegment(vtkEMSegment) SetIntensityAvgValuePreDef $EMSegment(IntensityAvgValue,$v) $index
             incr index
          } 
      }
      # Setup Quality Related information
      if {($EMSegment(Cattrib,$i,ReferenceStandardData) !=  $Volume(idNone)) && $EMSegment(Cattrib,$i,PrintQuality) } {
      EMSegment(Cattrib,$i,vtkImageEMClass) SetReferenceStandard [Volume($EMSegment(Cattrib,$i,ReferenceStandardData),vol) GetOutput]
      } 
      # Setup PCA parameter
      if {$EMSegment(Cattrib,$i,PCAMeanData) !=  $Volume(idNone) } {
            set NumEigenModes [llength $EMSegment(Cattrib,$i,PCAEigen)]
            # Kilan: first Rotate and translate the image before setting them 
            # Remember to first calculathe first the inverse of the two because we go from case2 to patient and data is given form patient to case2
            EMSegment(Cattrib,$i,vtkImageEMClass) SetPCANumberOfEigenModes $NumEigenModes
            EMSegment(Cattrib,$i,vtkImageEMClass) SetPCAMeanShape [Volume($EMSegment(Cattrib,$i,PCAMeanData),vol) GetOutput]

            set NumInputImagesSet 0
            foreach EigenList $EMSegment(Cattrib,$i,PCAEigen) {
              EMSegment(Cattrib,$i,vtkImageEMClass) SetPCAEigenVector [Volume([lindex $EigenList 2],vol) GetOutput] $NumInputImagesSet  
              incr NumInputImagesSet
            } 
          
            # Have to do it seperate otherwise EigenValues get deleted 
            foreach EigenList $EMSegment(Cattrib,$i,PCAEigen) {
              EMSegment(Cattrib,$i,vtkImageEMClass)  SetPCAEigenValue [lindex $EigenList 0] [lindex $EigenList 1] 
           }
           eval EMSegment(Cattrib,$i,vtkImageEMClass) SetPCAScale   $EMSegment(Cattrib,$i,PCAScale)
           EMSegment(Cattrib,$i,vtkImageEMClass) SetPCAMaxDist      $EMSegment(Cattrib,$i,PCAMaxDist)
           EMSegment(Cattrib,$i,vtkImageEMClass) SetPCADistVariance $EMSegment(Cattrib,$i,PCADistVariance)
      } 

      EMSegment(Cattrib,$i,vtkImageEMClass) SetPrintQuality $EMSegment(Cattrib,$i,PrintQuality)
      EMSegment(Cattrib,$i,vtkImageEMClass) SetPrintPCA $EMSegment(Cattrib,$i,PrintPCA)

      # After everything is defined add CLASS to its SUPERCLASS
      EMSegment(Cattrib,$SuperClass,vtkImageEMSuperClass) AddSubClass EMSegment(Cattrib,$i,vtkImageEMClass) $ClassIndex
    }
    incr ClassIndex
  }

  # After attaching all the classes we can defineMRF parameters
  set x 0  
  foreach i $EMSegment(Cattrib,$SuperClass,ClassList) {
      set y 0
      foreach j $EMSegment(Cattrib,$SuperClass,ClassList) {
        for {set k 0} { $k < 6} {incr k} {
           EMSegment(Cattrib,$SuperClass,vtkImageEMSuperClass) SetMarkovMatrix $EMSegment(Cattrib,$SuperClass,CIMMatrix,$i,$j,[lindex $EMSegment(CIMList) $k]) $k $y $x
        }
        incr y
      }
      incr x
  }
  # Automatically all the subclass are updated too and checked if values are set correctly 
  EMSegment(Cattrib,$SuperClass,vtkImageEMSuperClass) Update
  return [EMSegment(Cattrib,$SuperClass,vtkImageEMSuperClass) GetErrorFlag] 
}

#-------------------------------------------------------------------------------
# .PROC  EMSegmentSetVtkLocalSuperClassSetting
# Setting up everything for the super classes  
# Only loaded for private version 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EMSegmentSetVtkLocalSuperClassSetting {SuperClass} {
  global EMSegment Volume
  # Reads in the value for each class individually
  # puts "EMSegmentSetVtkSuperClassSetting $SuperClass"
  catch { EMSegment(Cattrib,$SuperClass,vtkImageEMSuperClass) Delete}
  vtkImageEMLocalSuperClass EMSegment(Cattrib,$SuperClass,vtkImageEMSuperClass)      

  # Define SuperClass specific parameters
  EMSegmentSetVtkGenericClassSetting EMSegment(Cattrib,$SuperClass,vtkImageEMSuperClass) $SuperClass

  EMSegment(Cattrib,$SuperClass,vtkImageEMSuperClass) SetPrintFrequency $EMSegment(Cattrib,$SuperClass,PrintFrequency)
  EMSegment(Cattrib,$SuperClass,vtkImageEMSuperClass) SetPrintBias      $EMSegment(Cattrib,$SuperClass,PrintBias)
  EMSegment(Cattrib,$SuperClass,vtkImageEMSuperClass) SetPrintLabelMap  $EMSegment(Cattrib,$SuperClass,PrintLabelMap)

  set ClassIndex 0
  foreach i $EMSegment(Cattrib,$SuperClass,ClassList) {
    if {$EMSegment(Cattrib,$i,IsSuperClass)} {
        if {[EMSegmentSetVtkLocalSuperClassSetting $i]} {return [EMSegment(Cattrib,$i,vtkImageEMSuperClass) GetErrorFlag]}
          EMSegment(Cattrib,$SuperClass,vtkImageEMSuperClass) AddSubClass EMSegment(Cattrib,$i,vtkImageEMSuperClass) $ClassIndex
    } else {
      catch {EMSegment(Cattrib,$i,vtkImageEMClass) destroy}
      vtkImageEMLocalClass EMSegment(Cattrib,$i,vtkImageEMClass)      
      EMSegmentSetVtkGenericClassSetting EMSegment(Cattrib,$i,vtkImageEMClass) $i

      EMSegment(Cattrib,$i,vtkImageEMClass) SetLabel             $EMSegment(Cattrib,$i,Label) 
      EMSegment(Cattrib,$i,vtkImageEMClass) SetShapeParameter    $EMSegment(Cattrib,$i,ShapeParameter)

      if {$EMSegment(Cattrib,$i,ProbabilityData) != $Volume(idNone)} {
          EMSegment(Cattrib,$i,vtkImageEMClass) SetProbDataPtr [Volume($EMSegment(Cattrib,$i,ProbabilityData),vol) GetOutput]
      } 
      for {set y 0} {$y < $EMSegment(NumInputChannel)} {incr y} {
          EMSegment(Cattrib,$i,vtkImageEMClass) SetLogMu $EMSegment(Cattrib,$i,LogMean,$y) $y
          for {set x 0} {$x < $EMSegment(NumInputChannel)} {incr x} {
            EMSegment(Cattrib,$i,vtkImageEMClass) SetLogCovariance $EMSegment(Cattrib,$i,LogCovariance,$y,$x) $y $x
          }
      }
      if {$EMSegment(IntensityAvgClass) == $EMSegment(Cattrib,$i,Label)} {
          # Transfere Intensity correction filter stuff
          set index 0
          EMSegment(vtkEMSegment) EMSetIntensityAvgClass  EMSegment(Cattrib,$i,vtkImageEMClass)
          foreach v $EMSegment(SelVolList,VolumeList) {       
             EMSegment(vtkEMSegment) SetIntensityAvgValuePreDef $EMSegment(IntensityAvgValue,$v) $index
             incr index
          } 
      }
      # Setup Quality Related information
      if {($EMSegment(Cattrib,$i,ReferenceStandardData) !=  $Volume(idNone)) && $EMSegment(Cattrib,$i,PrintQuality) } {
      EMSegment(Cattrib,$i,vtkImageEMClass) SetReferenceStandard [Volume($EMSegment(Cattrib,$i,ReferenceStandardData),vol) GetOutput]
      } 
      # Setup PCA parameter
      if {$EMSegment(Cattrib,$i,PCAMeanData) !=  $Volume(idNone) } {
            set NumEigenModes [llength $EMSegment(Cattrib,$i,PCAEigen)]
            # Kilan: first Rotate and translate the image before setting them 
            # Remember to first calculathe first the inverse of the two because we go from case2 to patient and data is given form patient to case2
            EMSegment(Cattrib,$i,vtkImageEMClass) SetPCANumberOfEigenModes $NumEigenModes
            EMSegment(Cattrib,$i,vtkImageEMClass) SetPCAMeanShape [Volume($EMSegment(Cattrib,$i,PCAMeanData),vol) GetOutput]

            set NumInputImagesSet 0
            foreach EigenList $EMSegment(Cattrib,$i,PCAEigen) {
              EMSegment(Cattrib,$i,vtkImageEMClass) SetPCAEigenVector [Volume([lindex $EigenList 2],vol) GetOutput] $NumInputImagesSet  
              incr NumInputImagesSet
            } 
          
            # Have to do it seperate otherwise EigenValues get deleted 
            foreach EigenList $EMSegment(Cattrib,$i,PCAEigen) {
              EMSegment(Cattrib,$i,vtkImageEMClass)  SetPCAEigenValue [lindex $EigenList 0] [lindex $EigenList 1] 
           }
           eval EMSegment(Cattrib,$i,vtkImageEMClass) SetPCAScale   $EMSegment(Cattrib,$i,PCAScale)
           EMSegment(Cattrib,$i,vtkImageEMClass) SetPCAMaxDist      $EMSegment(Cattrib,$i,PCAMaxDist)
           EMSegment(Cattrib,$i,vtkImageEMClass) SetPCADistVariance $EMSegment(Cattrib,$i,PCADistVariance)
      } 

      EMSegment(Cattrib,$i,vtkImageEMClass) SetPrintQuality $EMSegment(Cattrib,$i,PrintQuality)
      EMSegment(Cattrib,$i,vtkImageEMClass) SetPrintPCA $EMSegment(Cattrib,$i,PrintPCA)

      # After everything is defined add CLASS to its SUPERCLASS
      EMSegment(Cattrib,$SuperClass,vtkImageEMSuperClass) AddSubClass EMSegment(Cattrib,$i,vtkImageEMClass) $ClassIndex
    }
    incr ClassIndex
  }

  # After attaching all the classes we can defineMRF parameters
  set x 0  
  foreach i $EMSegment(Cattrib,$SuperClass,ClassList) {
      set y 0
      foreach j $EMSegment(Cattrib,$SuperClass,ClassList) {
        for {set k 0} { $k < 6} {incr k} {
           EMSegment(Cattrib,$SuperClass,vtkImageEMSuperClass) SetMarkovMatrix $EMSegment(Cattrib,$SuperClass,CIMMatrix,$i,$j,[lindex $EMSegment(CIMList) $k]) $k $y $x
        }
        incr y
      }
      incr x
  }
  # Automatically all the subclass are updated too and checked if values are set correctly 
  EMSegment(Cattrib,$SuperClass,vtkImageEMSuperClass) Update
  return [EMSegment(Cattrib,$SuperClass,vtkImageEMSuperClass) GetErrorFlag] 
}


#-------------------------------------------------------------------------------
# .PROC  EMSegmentSetVtkSuperClassSetting
# Setting up everything for the super classes  
# Only loaded for private version 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EMSegmentSetVtkLocalSuperClassSetting {SuperClass} {
  global EMSegment Volume
  # Reads in the value for each class individually
  # puts "EMSegmentSetVtkSuperClassSetting $SuperClass"
  catch { EMSegment(Cattrib,$SuperClass,vtkImageEMSuperClass) Delete}
  vtkImageEMLocalSuperClass EMSegment(Cattrib,$SuperClass,vtkImageEMSuperClass)      

  # Define SuperClass specific parameters
  EMSegmentSetVtkGenericClassSetting EMSegment(Cattrib,$SuperClass,vtkImageEMSuperClass) $SuperClass

  EMSegment(Cattrib,$SuperClass,vtkImageEMSuperClass) SetPrintFrequency $EMSegment(Cattrib,$SuperClass,PrintFrequency)
  EMSegment(Cattrib,$SuperClass,vtkImageEMSuperClass) SetPrintBias      $EMSegment(Cattrib,$SuperClass,PrintBias)
  EMSegment(Cattrib,$SuperClass,vtkImageEMSuperClass) SetPrintLabelMap  $EMSegment(Cattrib,$SuperClass,PrintLabelMap)

  set ClassIndex 0
  foreach i $EMSegment(Cattrib,$SuperClass,ClassList) {
    if {$EMSegment(Cattrib,$i,IsSuperClass)} {
        if {[EMSegmentSetVtkLocalSuperClassSetting $i]} {return [EMSegment(Cattrib,$i,vtkImageEMSuperClass) GetErrorFlag]}
          EMSegment(Cattrib,$SuperClass,vtkImageEMSuperClass) AddSubClass EMSegment(Cattrib,$i,vtkImageEMSuperClass) $ClassIndex
    } else {
      catch {EMSegment(Cattrib,$i,vtkImageEMClass) destroy}
      vtkImageEMLocalClass EMSegment(Cattrib,$i,vtkImageEMClass)      
      EMSegmentSetVtkGenericClassSetting EMSegment(Cattrib,$i,vtkImageEMClass) $i

      EMSegment(Cattrib,$i,vtkImageEMClass) SetLabel             $EMSegment(Cattrib,$i,Label) 
      EMSegment(Cattrib,$i,vtkImageEMClass) SetShapeParameter    $EMSegment(Cattrib,$i,ShapeParameter)

      if {$EMSegment(Cattrib,$i,ProbabilityData) != $Volume(idNone)} {
          EMSegment(Cattrib,$i,vtkImageEMClass) SetProbDataPtr [Volume($EMSegment(Cattrib,$i,ProbabilityData),vol) GetOutput]
      } 
      for {set y 0} {$y < $EMSegment(NumInputChannel)} {incr y} {
          EMSegment(Cattrib,$i,vtkImageEMClass) SetLogMu $EMSegment(Cattrib,$i,LogMean,$y) $y
          for {set x 0} {$x < $EMSegment(NumInputChannel)} {incr x} {
            EMSegment(Cattrib,$i,vtkImageEMClass) SetLogCovariance $EMSegment(Cattrib,$i,LogCovariance,$y,$x) $y $x
          }
      }
      if {$EMSegment(IntensityAvgClass) == $EMSegment(Cattrib,$i,Label)} {
          # Transfere Intensity correction filter stuff
          set index 0
          EMSegment(vtkEMSegment) EMSetIntensityAvgClass  EMSegment(Cattrib,$i,vtkImageEMClass)
          foreach v $EMSegment(SelVolList,VolumeList) {       
             EMSegment(vtkEMSegment) SetIntensityAvgValuePreDef $EMSegment(IntensityAvgValue,$v) $index
             incr index
          } 
      }
      # Setup Quality Related information
      if {($EMSegment(Cattrib,$i,ReferenceStandardData) !=  $Volume(idNone)) && $EMSegment(Cattrib,$i,PrintQuality) } {
      EMSegment(Cattrib,$i,vtkImageEMClass) SetReferenceStandard [Volume($EMSegment(Cattrib,$i,ReferenceStandardData),vol) GetOutput]
      } 
      EMSegment(Cattrib,$i,vtkImageEMClass) SetPrintQuality $EMSegment(Cattrib,$i,PrintQuality)

      # After everything is defined add CLASS to its SUPERCLASS
      EMSegment(Cattrib,$SuperClass,vtkImageEMSuperClass) AddSubClass EMSegment(Cattrib,$i,vtkImageEMClass) $ClassIndex
    }
    incr ClassIndex
  }

  # After attaching all the classes we can defineMRF parameters
  set x 0  
  foreach i $EMSegment(Cattrib,$SuperClass,ClassList) {
      set y 0
      foreach j $EMSegment(Cattrib,$SuperClass,ClassList) {
        for {set k 0} { $k < 6} {incr k} {
           EMSegment(Cattrib,$SuperClass,vtkImageEMSuperClass) SetMarkovMatrix $EMSegment(Cattrib,$SuperClass,CIMMatrix,$i,$j,[lindex $EMSegment(CIMList) $k]) $k $y $x
        }
        incr y
      }
      incr x
  }
  # Automatically all the subclass are updated too and checked if values are set correctly 
  EMSegment(Cattrib,$SuperClass,vtkImageEMSuperClass) Update
  return [EMSegment(Cattrib,$SuperClass,vtkImageEMSuperClass) GetErrorFlag] 
}

#-------------------------------------------------------------------------------
# .PROC EMSegmentAlgorithmStart
# Sets up the segmentation algorithm
# Returns 0 if an Error Occured and 1 if it was successfull 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EMSegmentAlgorithmStart { } {
   global EMSegment Volume 
   set NumInputImagesSet 0
   # EMLocalSegmentation: Multiple Input Images

   if {$EMSegment(SegmentMode)} {
       vtkImageEMPrivateSegmenter EMSegment(vtkEMSegment)
   } else {
       vtkImageEMLocalSegmenter EMSegment(vtkEMSegment)
   }
   # How many input images do you have
   EMSegment(vtkEMSegment) SetNumInputImages $EMSegment(NumInputChannel) 
   EMSegment(vtkEMSegment) SetNumberOfTrainingSamples $EMSegment(NumberOfTrainingSamples)
   # EMSegment(vtkEMSegment) SetBiasPrint $EMSegment(BiasPrint)

   if {$EMSegment(SegmentMode)} {
       if {[EMSegmentSetVtkPrivateSuperClassSetting 0]} { return 0 }
   }  else {
       if {[EMSegmentSetVtkLocalSuperClassSetting 0]} { return 0 }
   }


   # Transfer image information
   set NumInputImagesSet 0
   foreach v $EMSegment(SelVolList,VolumeList) {       
   EMSegment(vtkEMSegment) SetImageInput $NumInputImagesSet [Volume($v,vol) GetOutput]
   incr NumInputImagesSet
   }
   # Transfer Bias Print out Information
   EMSegment(vtkEMSegment) SetPrintDir $EMSegment(PrintDir)
   EMSegment(vtkEMSegment) SetNumEMShapeIter  $EMSegment(EMShapeIter)  
   
   # This is for debuging purposes so extra volumes can be loaded into the segmentation process 
   if {$EMSegment(DebugVolume)} {
   set index 1 
   set foundindex 0
   while {$foundindex > -1} {
       set foundindex [lsearch -exact $EMSegment(VolumeNameList)  EMDEBUG${index}] 
       if {$foundindex > -1} {
       EMSegment(vtkEMSegment) SetInputIndex $NumInputImagesSet [Volume([lindex $Volume(idList) $foundindex],vol) GetOutput]
       incr NumInputImagesSet
       incr index
       }
   }
   }
   EMSegment(vtkEMSegment) SetHeadClass          EMSegment(Cattrib,0,vtkImageEMSuperClass)

   #----------------------------------------------------------------------------
   # Transfering General Information
   #----------------------------------------------------------------------------
   EMSegment(vtkEMSegment) SetNumIter         $EMSegment(EMiteration)  
   EMSegment(vtkEMSegment) SetNumRegIter      $EMSegment(MFAiteration) 
   EMSegment(vtkEMSegment) SetAlpha           $EMSegment(Alpha) 

   EMSegment(vtkEMSegment) SetSmoothingWidth  $EMSegment(SmWidth)    
   EMSegment(vtkEMSegment) SetSmoothingSigma  $EMSegment(SmSigma)      

  
   # EMSegment(vtkEMSegment) SetPrintIntermediateResults    $EMSegment(PrintIntermediateResults) 
   # EMSegment(vtkEMSegment) SetPrintIntermediateSlice      $EMSegment(PrintIntermediateSlice) 
   # EMSegment(vtkEMSegment) SetPrintIntermediateFrequency  $EMSegment(PrintIntermediateFrequency) 

   return  $EMSegment(NumInputChannel) 
}

#-------------------------------------------------------------------------------
# .PROC  EMSegmentSuperClassChildren 
# Finds out the all children, grandchildren and ... of a super class
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EMSegmentSuperClassChildren {SuperClass} {
    global EMSegment
    if {$EMSegment(Cattrib,$SuperClass,IsSuperClass) == 0} {
    return     $EMSegment(Cattrib,$SuperClass,Label)
    }
    set result ""
    foreach i $EMSegment(Cattrib,$SuperClass,ClassList) {
    if {$EMSegment(Cattrib,$i,IsSuperClass)} {
           # it is defined as SetType<TYPE> <ID>  
       set result "$result [EMSegmentSuperClassChildren $i]" 
    } else {
        lappend result $EMSegment(Cattrib,$i,Label)  
    }
     } 
     return $result
 }

#-------------------------------------------------------------------------------
# .PROC EMSegmentTrainCIMField
# Traines the CIM Field with a given Image
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EMSegmentTrainCIMField {} {
    global EMSegment Volume
    # Transferring Information
    foreach i $EMSegment(GlobalSuperClassList) {
       vtkImageEMMarkov EMCIM    
       # EM Specific Information
       set NumClasses [llength $EMSegment(Cattrib,$i,ClassList)]
       EMCIM SetNumClasses     $NumClasses  
       EMCIM SetStartSlice     $EMSegment(SegmentationBoundaryMin,2)
       EMCIM SetEndSlice       $EMSegment(SegmentationBoundaryMax,2)

       # Kilian : Get rid of those 
       EMCIM SetImgTestNo       -1 
       EMCIM SetImgTestDivision  0 
       EMCIM SetImgTestPixel     0 
       set index 0
       foreach j $EMSegment(Cattrib,$i,ClassList) {
          set LabelList [EMSegmentSuperClassChildren $j]
          EMCIM SetLabelNumber $index [llength $LabelList]
          foreach l $LabelList {
            EMCIM SetLabel $index $l
          }
          incr index
       }

       # Transfer image information
       EMCIM SetInput [Volume($Volume(activeID),vol) GetOutput]
       set data [EMCIM GetOutput]
       # This Command calls the Thread Execute function
       $data Update
       set xindex 0 
       foreach x $EMSegment(Cattrib,$i,ClassList) {
           set EMSegment(Cattrib,$x,Prob) [EMCIM GetProbability $xindex]
           set yindex 0
           # EMCIM traines the matrix (y=t, x=t-1) just the other way EMSegment (y=t-1, x=t) needs it - Sorry !
           foreach y $EMSegment(Cattrib,$i,ClassList) {
           for {set z 0} {$z < 6} {incr z} {
               # Error made in x,y coordinates in EMSegment - I checked everything - it workes in XML and CIM Display in correct order - so do not worry - it is just a little bit strange - but strange things happen
               set temp [$data GetScalarComponentAsFloat $yindex $xindex  $z 0]
               set EMSegment(Cattrib,$i,CIMMatrix,$x,$y,[lindex $EMSegment(CIMList) $z]) [expr round($temp*100000)/100000.0]        
           }
           incr yindex
           }
           incr xindex
       }
       # Delete instance
       EMCIM Delete
    }
    # Jump to edit field 
    # EMSegmentExecuteCIM Edit
    # Move Button to Edit
    # set EMSegment(TabbedFrame,$EMSegment(Ma-tabCIM),tab) Edit
}

#-------------------------------------------------------------------------------
# .PROC EMSegmentAlgorithmDeletevtkEMSuperClass
# Delete vtkImageEMSuperClass and children attached to it 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EMSegmentAlgorithmDeleteVtkEMSuperClass { SuperClass } {
   global EMSegment
   EMSegment(Cattrib,$SuperClass,vtkImageEMSuperClass) Delete
   foreach i $EMSegment(Cattrib,$SuperClass,ClassList) {
         if {$EMSegment(Cattrib,$i,IsSuperClass)} {
            EMSegmentAlgorithmDeleteVtkEMSuperClass  $i
         } else {
            EMSegment(Cattrib,$i,vtkImageEMClass) Delete
         }
   }  
}
#-------------------------------------------------------------------------------
# .PROC EMSegmentAlgorithmDeletevtkEMSegment
# Delete vtkEMSegment related parameters 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EMSegmentAlgorithmDeletevtkEMSegment { } {
     global EMSegment
     EMSegment(vtkEMSegment) Delete
     EMSegmentAlgorithmDeleteVtkEMSuperClass 0
}
