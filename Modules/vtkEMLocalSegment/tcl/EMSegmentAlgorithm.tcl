#=auto==========================================================================
# (c) Copyright 2005 Massachusetts Institute of Technology (MIT) All Rights Reserved.
#
# This software ("3D Slicer") is provided by The Brigham and Women's 
# Hospital, Inc. on behalf of the copyright holders and contributors. 
# Permission is hereby granted, without payment, to copy, modify, display 
# and distribute this software and its documentation, if any, for 
# research purposes only, provided that (1) the above copyright notice and 
# the following four paragraphs appear on all copies of this software, and 
# (2) that source code to any modifications to this software be made 
# publicly available under terms no more restrictive than those in this 
# License Agreement. Use of this software constitutes acceptance of these 
# terms and conditions.
# 
# 3D Slicer Software has not been reviewed or approved by the Food and 
# Drug Administration, and is for non-clinical, IRB-approved Research Use 
# Only.  In no event shall data or images generated through the use of 3D 
# Slicer Software be used in the provision of patient care.
# 
# IN NO EVENT SHALL THE COPYRIGHT HOLDERS AND CONTRIBUTORS BE LIABLE TO 
# ANY PARTY FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL 
# DAMAGES ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, 
# EVEN IF THE COPYRIGHT HOLDERS AND CONTRIBUTORS HAVE BEEN ADVISED OF THE 
# POSSIBILITY OF SUCH DAMAGE.
# 
# THE COPYRIGHT HOLDERS AND CONTRIBUTORS SPECIFICALLY DISCLAIM ANY EXPRESS 
# OR IMPLIED WARRANTIES INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
# WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, AND 
# NON-INFRINGEMENT.
# 
# THE SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS 
# IS." THE COPYRIGHT HOLDERS AND CONTRIBUTORS HAVE NO OBLIGATION TO 
# PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
# 
#
#===============================================================================
# FILE:        EMSegmentAlgorithm.tcl
# PROCEDURES:  
#   EMSegmentSetVtkGenericClassSetting vtkGenericClass Sclass
#   EMSegmentSetVtkPrivateSuperClassSetting SuperClass
#   EMSegmentSetVtkLocalSuperClassSetting SuperClass
#   EMSegmentAlgorithmStart
#   EMSegmentSuperClassChildren  SuperClass
#   EMSegmentTrainCIMField
#   EMSegmentAlgorithmDeletevtkEMSuperClass Superclass
#   EMSegmentAlgorithmDeletevtkEMSegment
#==========================================================================auto=

 
#-------------------------------------------------------------------------------
# .PROC  EMSegmentSetVtkGenericClassSetting
# Settings defined by vtkImageEMGenericClass, i.e. variables that have to be set for both CLASS and SUPERCLASS 
# Only loaded for private version 
# .ARGS
# string vtkGenericClass
# string Sclass
# .END
#-------------------------------------------------------------------------------
proc EMSegmentSetVtkGenericClassSetting {vtkGenericClass Sclass} {
  global EMSegment Volume
  $vtkGenericClass SetNumInputImages $EMSegment(NumInputChannel) 
  eval $vtkGenericClass SetSegmentationBoundaryMin $EMSegment(SegmentationBoundaryMin,0) $EMSegment(SegmentationBoundaryMin,1) $EMSegment(SegmentationBoundaryMin,2)
  eval $vtkGenericClass SetSegmentationBoundaryMax $EMSegment(SegmentationBoundaryMax,0) $EMSegment(SegmentationBoundaryMax,1) $EMSegment(SegmentationBoundaryMax,2)

  $vtkGenericClass SetProbDataWeight $EMSegment(Cattrib,$Sclass,LocalPriorWeight)

  $vtkGenericClass SetTissueProbability $EMSegment(Cattrib,$Sclass,Prob)
  $vtkGenericClass SetPrintWeights $EMSegment(Cattrib,$Sclass,PrintWeights)

  for {set y 0} {$y < $EMSegment(NumInputChannel)} {incr y} {
      if {[info exists EMSegment(Cattrib,$Sclass,InputChannelWeights,$y)]} {$vtkGenericClass SetInputChannelWeights $EMSegment(Cattrib,$Sclass,InputChannelWeights,$y) $y}
  }

  if {$EMSegment(SegmentMode)}  {
    eval $vtkGenericClass SetRegistrationTranslation $EMSegment(Cattrib,$Sclass,RegistrationTranslation)
    eval $vtkGenericClass SetRegistrationRotation    $EMSegment(Cattrib,$Sclass,RegistrationRotation)
    eval $vtkGenericClass SetRegistrationScale       $EMSegment(Cattrib,$Sclass,RegistrationScale)
    eval $vtkGenericClass SetRegistrationCovariance  $EMSegment(Cattrib,$Sclass,RegistrationCovariance)   
    eval $vtkGenericClass SetPrintRegistrationParameters                $EMSegment(Cattrib,$Sclass,PrintRegistrationParameters)   
    eval $vtkGenericClass SetPrintRegistrationSimularityMeasure         $EMSegment(Cattrib,$Sclass,PrintRegistrationSimularityMeasure) 
    eval $vtkGenericClass SetRegistrationClassSpecificRegistrationFlag  $EMSegment(Cattrib,$Sclass,RegistrationClassSpecificRegistrationFlag) 
    $vtkGenericClass      SetExcludeFromIncompleteEStepFlag             $EMSegment(Cattrib,$Sclass,ExcludeFromIncompleteEStepFlag) 

    if {$EMSegment(Cattrib,$Sclass,ProbabilityData) != $Volume(idNone) } {
    # Pipeline does not automatically update volumes bc of fake first input  
    Volume($EMSegment(Cattrib,$Sclass,ProbabilityData),vol) Update
    $vtkGenericClass SetProbDataPtr [Volume($EMSegment(Cattrib,$Sclass,ProbabilityData),vol) GetOutput]
      
    # Kilian: Currently LocalPriorWeight is also used for shape parameters - should change it later
    } elseif {($EMSegment(Cattrib,$Sclass,IsSuperClass) == 0 ) && ($EMSegment(Cattrib,$Sclass,PCAMeanData) ==  $Volume(idNone))} {
    # set EMSegment(Cattrib,$Sclass,LocalPriorWeight) 0.0
    }
  }

}

#-------------------------------------------------------------------------------
# .PROC  EMSegmentSetVtkPrivateSuperClassSetting
# Setting up everything for the super classes  
# Only loaded for private version 
# .ARGS
# string SuperClass
# .END
#-------------------------------------------------------------------------------
proc EMSegmentSetVtkPrivateSuperClassSetting {SuperClass} {
  global EMSegment Volume
  # Reads in the value for each class individually
  # puts "EMSegmentSetVtkPrivateSuperClassSetting $SuperClass $EMSegment(Cattrib,$SuperClass,Name)"
  catch { EMSegment(Cattrib,$SuperClass,vtkImageEMSuperClass) Delete}
  vtkImageEMPrivateSuperClass EMSegment(Cattrib,$SuperClass,vtkImageEMSuperClass)      

  # Define SuperClass specific parameters
  EMSegmentSetVtkGenericClassSetting EMSegment(Cattrib,$SuperClass,vtkImageEMSuperClass) $SuperClass

  EMSegment(Cattrib,$SuperClass,vtkImageEMSuperClass) SetPrintFrequency $EMSegment(Cattrib,$SuperClass,PrintFrequency)
  EMSegment(Cattrib,$SuperClass,vtkImageEMSuperClass) SetPrintBias      $EMSegment(Cattrib,$SuperClass,PrintBias)
  EMSegment(Cattrib,$SuperClass,vtkImageEMSuperClass) SetPrintLabelMap  $EMSegment(Cattrib,$SuperClass,PrintLabelMap)

  EMSegment(Cattrib,$SuperClass,vtkImageEMSuperClass) SetPrintEMLabelMapConvergence  $EMSegment(Cattrib,$SuperClass,PrintEMLabelMapConvergence)
  EMSegment(Cattrib,$SuperClass,vtkImageEMSuperClass) SetPrintEMWeightsConvergence   $EMSegment(Cattrib,$SuperClass,PrintEMWeightsConvergence)
  EMSegment(Cattrib,$SuperClass,vtkImageEMSuperClass) SetStopEMType                  $EMSegment(Cattrib,$SuperClass,StopEMType)
  EMSegment(Cattrib,$SuperClass,vtkImageEMSuperClass) SetStopEMValue                 $EMSegment(Cattrib,$SuperClass,StopEMValue)
  # Current Legacy 
  if {$EMSegment(Cattrib,$SuperClass,StopEMMaxIter) == 0} {set EMSegment(Cattrib,$SuperClass,StopEMMaxIter)  $EMSegment(Cattrib,0,StopEMMaxIter) }
  EMSegment(Cattrib,$SuperClass,vtkImageEMSuperClass) SetStopEMMaxIter               $EMSegment(Cattrib,$SuperClass,StopEMMaxIter)

  EMSegment(Cattrib,$SuperClass,vtkImageEMSuperClass) SetPrintMFALabelMapConvergence  $EMSegment(Cattrib,$SuperClass,PrintMFALabelMapConvergence)
  EMSegment(Cattrib,$SuperClass,vtkImageEMSuperClass) SetPrintMFAWeightsConvergence   $EMSegment(Cattrib,$SuperClass,PrintMFAWeightsConvergence)
  EMSegment(Cattrib,$SuperClass,vtkImageEMSuperClass) SetStopMFAType                  $EMSegment(Cattrib,$SuperClass,StopMFAType)
  EMSegment(Cattrib,$SuperClass,vtkImageEMSuperClass) SetStopMFAValue                 $EMSegment(Cattrib,$SuperClass,StopMFAValue)
  EMSegment(Cattrib,$SuperClass,vtkImageEMSuperClass) SetStopBiasCalculation          $EMSegment(Cattrib,$SuperClass,StopBiasCalculation)

  EMSegment(Cattrib,$SuperClass,vtkImageEMSuperClass) SetPrintShapeSimularityMeasure  $EMSegment(Cattrib,$SuperClass,PrintShapeSimularityMeasure)

  EMSegment(Cattrib,$SuperClass,vtkImageEMSuperClass) SetPCAShapeModelType                 $EMSegment(Cattrib,$SuperClass,PCAShapeModelType)
  EMSegment(Cattrib,$SuperClass,vtkImageEMSuperClass) SetRegistrationIndependentSubClassFlag $EMSegment(Cattrib,$SuperClass,RegistrationIndependentSubClassFlag)

  # Current Legacy - I have to fix gui
  if {$EMSegment(Cattrib,$SuperClass,StopMFAMaxIter) == 0} {set EMSegment(Cattrib,$SuperClass,StopMFAMaxIter) $EMSegment(Cattrib,0,StopMFAMaxIter) }
  EMSegment(Cattrib,$SuperClass,vtkImageEMSuperClass) SetStopMFAMaxIter               $EMSegment(Cattrib,$SuperClass,StopMFAMaxIter)

  if {$EMSegment(SegmentMode)} {
      EMSegment(Cattrib,$SuperClass,vtkImageEMSuperClass) SetRegistrationType              $EMSegment(Cattrib,$SuperClass,RegistrationType)
      EMSegment(Cattrib,$SuperClass,vtkImageEMSuperClass) SetGenerateBackgroundProbability $EMSegment(Cattrib,$SuperClass,GenerateBackgroundProbability)
  }

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
 
      for {set y 0} {$y < $EMSegment(NumInputChannel)} {incr y} {
          EMSegment(Cattrib,$i,vtkImageEMClass) SetLogMu $EMSegment(Cattrib,$i,LogMean,$y) $y
          for {set x 0} {$x < $EMSegment(NumInputChannel)} {incr x} {
            EMSegment(Cattrib,$i,vtkImageEMClass) SetLogCovariance $EMSegment(Cattrib,$i,LogCovariance,$y,$x) $y $x
          }
      }
      # if {$EMSegment(IntensityAvgClass) == $EMSegment(Cattrib,$i,Label)} {
      #     # Transfere Intensity correction filter stuff
      #     set index 0
      #     EMSegment(vtkEMSegment) SetIntensityAvgClass  EMSegment(Cattrib,$i,vtkImageEMClass)
      #     foreach v $EMSegment(SelVolList,VolumeList) {       
      #        EMSegment(vtkEMSegment) SetIntensityAvgValuePreDef $EMSegment(IntensityAvgValue,$v) $index
      #        incr index
      #     } 
      # }
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

           # Pipeline does not automatically update volumes bc of fake first input  
            Volume($EMSegment(Cattrib,$i,PCAMeanData),vol) Update
            EMSegment(Cattrib,$i,vtkImageEMClass) SetPCAMeanShape [Volume($EMSegment(Cattrib,$i,PCAMeanData),vol) GetOutput]

            set NumInputImagesSet 1 
            foreach EigenList $EMSegment(Cattrib,$i,PCAEigen) {
          # Pipeline does not automatically update volumes bc of fake first input  
              [Volume([lindex $EigenList 2],vol) GetOutput] Update
              EMSegment(Cattrib,$i,vtkImageEMClass) SetPCAEigenVector [Volume([lindex $EigenList 2],vol) GetOutput] $NumInputImagesSet  
              incr NumInputImagesSet
            } 
          
            # Have to do it seperate otherwise EigenValues get deleted 
            foreach EigenList $EMSegment(Cattrib,$i,PCAEigen) {
              EMSegment(Cattrib,$i,vtkImageEMClass)  SetPCAEigenValues [lindex $EigenList 0] [lindex $EigenList 1] 
           }
           
           EMSegment(Cattrib,$i,vtkImageEMClass) SetPCALogisticSlope      $EMSegment(Cattrib,$i,PCALogisticSlope)
           EMSegment(Cattrib,$i,vtkImageEMClass) SetPCALogisticMin        $EMSegment(Cattrib,$i,PCALogisticMin)
           EMSegment(Cattrib,$i,vtkImageEMClass) SetPCALogisticMax        $EMSegment(Cattrib,$i,PCALogisticMax)
           EMSegment(Cattrib,$i,vtkImageEMClass) SetPCALogisticBoundary   $EMSegment(Cattrib,$i,PCALogisticBoundary)
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
 #  puts "EMSegmentSetVtkPrivateSuperClassSetting $SuperClass $EMSegment(Cattrib,$SuperClass,Name)"
  foreach i $EMSegment(Cattrib,$SuperClass,ClassList) {
      set y 0

      foreach j $EMSegment(Cattrib,$SuperClass,ClassList) {
        for {set k 0} { $k < 6} {incr k} {
       # puts "$EMSegment(Cattrib,$SuperClass,CIMMatrix,$i,$j,[lindex $EMSegment(CIMList) $k]) [lindex $EMSegment(CIMList) $k] $y $x --- $i $j $k"
           EMSegment(Cattrib,$SuperClass,vtkImageEMSuperClass) SetMarkovMatrix $EMSegment(Cattrib,$SuperClass,CIMMatrix,$i,$j,[lindex $EMSegment(CIMList) $k]) $k $y $x
        }
        incr y
      }
      incr x
  }
  # Automatically all the subclass are updated too and checked if values are set correctly 
  # puts  "======== Start Updated here  $SuperClass";
  EMSegment(Cattrib,$SuperClass,vtkImageEMSuperClass) Update
  # puts  "======== Finsihed Updated here  $SuperClass";
  return [EMSegment(Cattrib,$SuperClass,vtkImageEMSuperClass) GetErrorFlag] 
}

#-------------------------------------------------------------------------------
# .PROC  EMSegmentSetVtkLocalSuperClassSetting
# Setting up everything for the super classes  
# Only loaded for private version 
# .ARGS
# string SuperClass
# .END
#-------------------------------------------------------------------------------
proc EMSegmentSetVtkLocalSuperClassSetting {SuperClass} {
  global EMSegment Volume
  # Reads in the value for each class individually
  # puts "EMSegmentSetVtkLocalSuperClassSetting $SuperClass $EMSegment(Cattrib,$SuperClass,Name)"
  catch { EMSegment(Cattrib,$SuperClass,vtkImageEMSuperClass) Delete}
  vtkImageEMLocalSuperClass EMSegment(Cattrib,$SuperClass,vtkImageEMSuperClass)      

  # Define SuperClass specific parameters
  EMSegmentSetVtkGenericClassSetting EMSegment(Cattrib,$SuperClass,vtkImageEMSuperClass) $SuperClass

  EMSegment(Cattrib,$SuperClass,vtkImageEMSuperClass) SetPrintFrequency $EMSegment(Cattrib,$SuperClass,PrintFrequency)
  EMSegment(Cattrib,$SuperClass,vtkImageEMSuperClass) SetPrintBias      $EMSegment(Cattrib,$SuperClass,PrintBias)
  EMSegment(Cattrib,$SuperClass,vtkImageEMSuperClass) SetPrintLabelMap  $EMSegment(Cattrib,$SuperClass,PrintLabelMap)
  EMSegment(Cattrib,$SuperClass,vtkImageEMSuperClass) SetProbDataWeight $EMSegment(Cattrib,$SuperClass,LocalPriorWeight)
  
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

      if {$EMSegment(Cattrib,$i,ProbabilityData) != $Volume(idNone)} {
          # Pipeline does not automatically update volumes bc of fake first input  
          Volume($EMSegment(Cattrib,$i,ProbabilityData),vol) Update
          EMSegment(Cattrib,$i,vtkImageEMClass) SetProbDataPtr [Volume($EMSegment(Cattrib,$i,ProbabilityData),vol) GetOutput]
      
      } else {
         set EMSegment(Cattrib,$i,LocalPriorWeight) 0.0
      }
      EMSegment(Cattrib,$i,vtkImageEMClass) SetProbDataWeight $EMSegment(Cattrib,$i,LocalPriorWeight)

      for {set y 0} {$y < $EMSegment(NumInputChannel)} {incr y} {
          EMSegment(Cattrib,$i,vtkImageEMClass) SetLogMu $EMSegment(Cattrib,$i,LogMean,$y) $y
          for {set x 0} {$x < $EMSegment(NumInputChannel)} {incr x} {
            EMSegment(Cattrib,$i,vtkImageEMClass) SetLogCovariance $EMSegment(Cattrib,$i,LogCovariance,$y,$x) $y $x
          }
      }
      # if {$EMSegment(IntensityAvgClass) == $EMSegment(Cattrib,$i,Label)} {
      #     # Transfere Intensity correction filter stuff
      #     set index 0
      #     EMSegment(vtkEMSegment) EMSetIntensityAvgClass  EMSegment(Cattrib,$i,vtkImageEMClass)
      #     foreach v $EMSegment(SelVolList,VolumeList) {       
      #        EMSegment(vtkEMSegment) SetIntensityAvgValuePreDef $EMSegment(IntensityAvgValue,$v) $index
      #        incr index
      #     } 
      # }
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
 #  puts "EMSegmentSetVtkLocalSuperClassSetting $SuperClass $EMSegment(Cattrib,$SuperClass,Name)"
  foreach i $EMSegment(Cattrib,$SuperClass,ClassList) {
      set y 0

      foreach j $EMSegment(Cattrib,$SuperClass,ClassList) {
        for {set k 0} { $k < 6} {incr k} {
       # puts "$EMSegment(Cattrib,$SuperClass,CIMMatrix,$i,$j,[lindex $EMSegment(CIMList) $k]) [lindex $EMSegment(CIMList) $k] $y $x --- $i $j $k"
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
   puts "Start EMSegmentAlgorithmStart"
   if {$EMSegment(SegmentMode)} {
       vtkImageEMPrivateSegmenter EMSegment(vtkEMSegment)
   } else {
       vtkImageEMLocalSegmenter EMSegment(vtkEMSegment)
   }
   # How many input images do you have
   puts "Kilian: Generalize it later"
   EMSegment(vtkEMSegment) SetDisableMultiThreading 0

   EMSegment(vtkEMSegment) SetNumInputImages $EMSegment(NumInputChannel) 
   EMSegment(vtkEMSegment) SetNumberOfTrainingSamples $EMSegment(NumberOfTrainingSamples)

   if {$EMSegment(SegmentMode)} {
       # EMSegment(vtkEMSegment) SetNumEMShapeIter  $EMSegment(EMShapeIter)  
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
   EMSegment(vtkEMSegment) SetAlpha           $EMSegment(Alpha) 

   EMSegment(vtkEMSegment) SetSmoothingWidth  $EMSegment(SmWidth)    
   EMSegment(vtkEMSegment) SetSmoothingSigma  $EMSegment(SmSigma)      

   if {$EMSegment(SegmentMode)}  {
       # New Private Variables 
       EMSegment(vtkEMSegment) SetRegistrationInterpolationType  $EMSegment(RegistrationInterpolationType)      
   } else {
       EMSegment(vtkEMSegment) SetNumIter         $EMSegment(Cattrib,0,StopEMMaxIter) 
       EMSegment(vtkEMSegment) SetNumRegIter      $EMSegment(Cattrib,0,StopMFAMaxIter) 
   }

   return  $EMSegment(NumInputChannel) 
}

#-------------------------------------------------------------------------------
# .PROC  EMSegmentSuperClassChildren 
# Finds out the all children, grandchildren and ... of a super class
# .ARGS
# string SuperClass
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
    if {[info command vtkImageEMMarkov] == ""} {
        DevErrorWindow "vtkImageEMMarkov not included in this build, cannot train the CIM field."
        return
    }
    foreach i $EMSegment(GlobalSuperClassList) {
        catch "EMCIM Delete"
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
               set temp [$data $::getScalarComponentAs $yindex $xindex  $z 0]
               if {[catch {set mytestvar [expr int($temp)]} err]} {
                   # probably temp is too large to be an int, max it
                   set temp 10000
                   if {$::Module(verbose)} { puts "capped temp: $temp" }
               }
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
# string Superclass
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
