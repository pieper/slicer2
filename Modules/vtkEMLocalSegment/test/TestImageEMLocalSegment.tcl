# This script allows the direct access of the vtkImageEMLocalSegmenter Filter. It is very useful for batch processing.
# Function can be called through
# setenv SLICER_HOME /projects/hpc/active/pohl/slicer_devel/slicer2
# setenv LD_LIBRARY_PATH ${SLICER_HOME}/Base/builds/Solaris8/bin:${SLICER_HOME}/Modules/EMLocalSegment/builds/Solaris8/bin:${LD_LIBRARY_PATH}
# setenv VTK_SRC_DIR /local/os/src/vtk-4.0/Wrapping/Tcl 
# setenv TCLLIBPATH "${SLICER_HOME}/Base/Wrapping/Tcl/vtkSlicerBase ${SLICER_HOME}/Modules/EMLocalSegment/cxx/ /local/os/src/vtk-4.0/Wrapping/Tcl"
# /local/os/src/vtk-4.0/bin/vtk TestImageEMLocalSegment.tcl 
# Test case: case2; slices: 60 - 62
package require vtk
package require vtkSlicerBase
package require vtkEMLocalSegment

#-------------------------------------------------------------------------------
# 1. Step Initialize 
#-------------------------------------------------------------------------------
# vtkImageViewer viewer
#
puts "Result will be written as TestImageEMLocalSegmentTestResult.* - compare it too TestImageEMLocalSegmentResult.*"
set EMSegment(XMLDir)        TestImageEMLocalSegmentData.xml
set EMSegment(DisplayWindow) 0
if {$EMSegment(DisplayWindow) == 0} {
    wm withdraw .
}  
set EMSegment(debug) 0
set EMSegment(CIMList) {West North Up East South Down}
set EMSegment(BiasPrint) 0
set EMSegment(ImgTestNo) -1
set EMSegment(ImgTestDivision) 0
set EMSegment(ImgTestPixel) 0
set EMSegment(DeltedVolumeList) ""
set EMSegment(PrintPCAParameters) 0
set EMSegment(PrintDICEResults)   0
set Volume(idNone) 0

if {$argc < 1} {set Mode 4
} else {set Mode  [lindex $argv 0]}

puts "================================== General Information ========================"
puts "Please note - the test software can be called in different modes."
puts "Execute ./TestImageEMLocalSegment <mode> where <mode>:"
puts "1 = Just the E-Step               (alpha = 0 and iter = 1)" 
puts "2 = E-Step with MFA               (alpha = 0.9, iter = 1, and MFAIter = 1)"
puts "3 = E-Step and M-Step without MFA (alpha = 0.9, iter = 2, MFAIter = 0)" 
puts "4 = E-Step and M-Step with MFA    (alpha = 0.9, iter = 2, MFAIter = 2)"
puts "5 = E-Step and M-Step with MFA    (alpha = 0.9, iter = 1, MFAIter = 1 and ProbDataWeight of WM = 0.80488)"
puts "6 = E-Step and M-Step with MFA    (alpha = 0.9, iter = 2, MFAIter = 1 and InputChannelWeight = 1.0 0.0 | 0.0 0.0)"
puts "7 = E-Step and M-Step with MFA    (alpha = 0.9, iter = 2, MFAIter = 2, BoundaryMin = (80,70,1), and  BoundaryMax=(200, 180,2))"
puts "8 = A two level hierarchical segmentation"
puts "By default <mode> is set to 4"
puts ""
puts  "================================== Start Segmentation ========================"

#-------------------------------------------------------------------------------
# 2. Step Define Programs 
#-------------------------------------------------------------------------------
#-------------------------------------------------------------------------------
# .PROC EMSegmentStartEM
# Starts the EM Algorithm (depending on the Segmentation Mode it start 
# the one channel input or mutli channel input Segmenter Filter
# .ARGS
# .END
#-------------------------------------------------------------------------------
# Kilian Split up structure : Create an own EMSegmentStartem file so we only have to change it once for batch processing !!
# Might be now a good idea for testing - think about it

# --------------------------------------------------------------------------------
proc EMSegmentSetVtkGenericClassSetting {vtkGenericClass Sclass} {
  global EMSegment
  $vtkGenericClass SetNumInputImages $EMSegment(NumInputChannel) 
  eval $vtkGenericClass SetSegmentationBoundaryMin $EMSegment(SegmentationBoundaryMin,0) $EMSegment(SegmentationBoundaryMin,1) $EMSegment(SegmentationBoundaryMin,2)
  eval $vtkGenericClass SetSegmentationBoundaryMax $EMSegment(SegmentationBoundaryMax,0) $EMSegment(SegmentationBoundaryMax,1) $EMSegment(SegmentationBoundaryMax,2)

  $vtkGenericClass SetProbDataWeight $EMSegment(Cattrib,$Sclass,LocalPriorWeight)
  $vtkGenericClass SetTissueProbability $EMSegment(Cattrib,$Sclass,Prob)

  for {set y 0} {$y < $EMSegment(NumInputChannel)} {incr y} {
      if {[info exists EMSegment(Cattrib,$Sclass,InputChannelWeights,$y)]} {$vtkGenericClass SetInputChannelWeights $EMSegment(Cattrib,$Sclass,InputChannelWeights,$y) $y}
  }
}

#-------------------------------------------------------------------------------
proc EMSegmentSetVtkSuperClassSetting {SuperClass} {
  global EMSegment Volume
  # Reads in the value for each class individually
  # puts "EMSegmentSetVtkSuperClassSetting $SuperClass"
  catch { EMSegment(Cattrib,$SuperClass,vtkImageEMLocalSuperClass) destroy}
  vtkImageEMLocalSuperClass EMSegment(Cattrib,$SuperClass,vtkImageEMLocalSuperClass)      

  EMSegmentSetVtkGenericClassSetting EMSegment(Cattrib,$SuperClass,vtkImageEMLocalSuperClass) $SuperClass

  set ClassIndex 0
  foreach i $EMSegment(Cattrib,$SuperClass,ClassList) {
    if {$EMSegment(Cattrib,$i,IsSuperClass)} {
        if {[EMSegmentSetVtkSuperClassSetting $i]} {return [EMSegment(Cattrib,$i,vtkImageEMLocalSuperClass) GetErrorFlag]}
          EMSegment(Cattrib,$SuperClass,vtkImageEMLocalSuperClass) AddSubClass EMSegment(Cattrib,$i,vtkImageEMLocalSuperClass) $ClassIndex
    } else {
      catch {EMSegment(Cattrib,$i,vtkImageEMLocalClass) destroy}
      vtkImageEMLocalClass EMSegment(Cattrib,$i,vtkImageEMLocalClass)      
      EMSegmentSetVtkGenericClassSetting EMSegment(Cattrib,$i,vtkImageEMLocalClass) $i

      EMSegment(Cattrib,$i,vtkImageEMLocalClass) SetLabel             $EMSegment(Cattrib,$i,Label) 
      EMSegment(Cattrib,$i,vtkImageEMLocalClass) SetShapeParameter    $EMSegment(Cattrib,$i,ShapeParameter)

      if {$EMSegment(Cattrib,$i,ProbabilityData) != $Volume(idNone)} {
          EMSegment(Cattrib,$i,vtkImageEMLocalClass) SetProbDataPtr [Volume($EMSegment(Cattrib,$i,ProbabilityData),vol) GetOutput]
      } 
      for {set y 0} {$y < $EMSegment(NumInputChannel)} {incr y} {
          EMSegment(Cattrib,$i,vtkImageEMLocalClass) SetLogMu $EMSegment(Cattrib,$i,LogMean,$y) $y
          for {set x 0} {$x < $EMSegment(NumInputChannel)} {incr x} {
            EMSegment(Cattrib,$i,vtkImageEMLocalClass) SetLogCovariance $EMSegment(Cattrib,$i,LogCovariance,$y,$x) $y $x
          }
      }
      if {$EMSegment(IntensityAvgClass) == $EMSegment(Cattrib,$i,Label)} {
          # Transfere Intensity correction filter stuff
          set index 0
          EMSegment(vtkEMSegment) EMSetIntensityAvgClass  EMSegment(Cattrib,$i,vtkImageEMLocalClass)
          foreach v $EMSegment(SelVolList,VolumeList) {       
             EMSegment(vtkEMSegment) SetIntensityAvgValuePreDef $EMSegment(IntensityAvgValue,$v) $index
             incr index
          } 
      }
      # Setup DICE Related information
      # if {($EMSegment(Cattrib,$i,DICEData) !=  $Volume(idNone)) && $EMSegment(PrintDICEResults) } {
      #    EMSegment(Cattrib,$i,vtkImageEMLocalClass) SetReferenceStandardPtr [Volume($EMSegment(Cattrib,$i,DICEData),vol) GetOutput]
      # } 
      # Setup PCA parameter
      if {$EMSegment(Cattrib,$i,PCAMeanData) !=  $Volume(idNone) } {
            set NumEigenModes [llength $EMSegment(Cattrib,$i,PCAEigen)]
            # Kilan: first Rotate and translate the image before setting them 
            # Remember to first calculathe first the inverse of the two because we go from case2 to patient and data is given form patient to case2
            EMSegment(Cattrib,$i,vtkImageEMLocalClass) SetPCANumberOfEigenModes $NumEigenModes
            EMSegment(Cattrib,$i,vtkImageEMLocalClass) SetPCAMeanShape [Volume($EMSegment(Cattrib,$i,PCAMeanData),vol) GetOutput]

            set NumInputImagesSet 0
            foreach EigenList $EMSegment(Cattrib,$i,PCAEigen) {
              EMSegment(Cattrib,$i,vtkImageEMLocalClass) SetPCAEigenVector [Volume([lindex $EigenList 2],vol) GetOutput] $NumInputImagesSet  
              incr NumInputImagesSet
            } 
          
            # Have to do it seperate otherwise EigenValues get deleted 
            foreach EigenList $EMSegment(Cattrib,$i,PCAEigen) {
              EMSegment(Cattrib,$i,vtkImageEMLocalClass)  SetPCAEigenValue [lindex $EigenList 0] [lindex $EigenList 1] 
           }
           eval EMSegment(Cattrib,$i,vtkImageEMLocalClass) SetPCAScale   $EMSegment(Cattrib,$i,PCAScale)
           EMSegment(Cattrib,$i,vtkImageEMLocalClass) SetPCAMaxDist      $EMSegment(Cattrib,$i,PCAMaxDist)
           EMSegment(Cattrib,$i,vtkImageEMLocalClass) SetPCADistVariance $EMSegment(Cattrib,$i,PCADistVariance)
      } 
      EMSegment(Cattrib,$SuperClass,vtkImageEMLocalSuperClass) AddSubClass EMSegment(Cattrib,$i,vtkImageEMLocalClass) $ClassIndex
    }
    incr ClassIndex
  }

  # After attaching all the classes we can defineMRF parameters
  set x 0  
  foreach i $EMSegment(Cattrib,$SuperClass,ClassList) {
      set y 0
      foreach j $EMSegment(Cattrib,$SuperClass,ClassList) {
        for {set k 0} { $k < 6} {incr k} {
           EMSegment(Cattrib,$SuperClass,vtkImageEMLocalSuperClass) SetMarkovMatrix $EMSegment(Cattrib,$SuperClass,CIMMatrix,$i,$j,[lindex $EMSegment(CIMList) $k]) $k $y $x
        }
        incr y
      }
      incr x
  }
  # Automatically all the subclass are updated too and checked if values are set correctly 
  EMSegment(Cattrib,$SuperClass,vtkImageEMLocalSuperClass) Update
  return [EMSegment(Cattrib,$SuperClass,vtkImageEMLocalSuperClass) GetErrorFlag] 
}

proc EMSegmentStartEM {} {
   global EMSegment Volume 
   set NumInputImagesSet 0
   # EMLocalSegmentation: Multiple Input Images
   vtkImageEMLocalSegmenter EMSegment(vtkEMSegment) 

   # How many input images do you have
   EMSegment(vtkEMSegment) SetNumInputImages $EMSegment(NumInputChannel) 
   EMSegment(vtkEMSegment) SetNumberOfTrainingSamples $EMSegment(NumberOfTrainingSamples)
   # EMSegment(vtkEMSegment) SetBiasPrint $EMSegment(BiasPrint)

   if {[EMSegmentSetVtkSuperClassSetting 0]} { return 0 }
    
   # Transfer image information
   set NumInputImagesSet 0
   foreach v $EMSegment(SelVolList,VolumeList) {       
     EMSegment(vtkEMSegment) SetImageInput $NumInputImagesSet [Volume($v,vol) GetOutput]
     incr NumInputImagesSet
   }
   # Transfer Bias Print out Information
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
   EMSegment(vtkEMSegment) SetHeadClass          EMSegment(Cattrib,0,vtkImageEMLocalSuperClass)

   #----------------------------------------------------------------------------
   # Transfering General Information
   #----------------------------------------------------------------------------
   EMSegment(vtkEMSegment) SetNumIter         $EMSegment(EMiteration)  
   EMSegment(vtkEMSegment) SetNumRegIter      $EMSegment(MFAiteration) 
   EMSegment(vtkEMSegment) SetAlpha           $EMSegment(Alpha) 

   EMSegment(vtkEMSegment) SetSmoothingWidth  $EMSegment(SmWidth)    
   EMSegment(vtkEMSegment) SetSmoothingSigma  $EMSegment(SmSigma)      

  
   return 1 
}

proc CreateSuperClass {SuperClass NumClasses StartIndex} {
  global EMSegment Volume
  set max  [expr  $NumClasses + $StartIndex]
  set EMSegment(Cattrib,$SuperClass,IsSuperClass) 1
  lappend EMSegment(GlobalClassList) $SuperClass
  for {set i $StartIndex} { $i < $max} {incr i} {
      lappend EMSegment(Cattrib,$SuperClass,ClassList) $i

      set EMSegment(Cattrib,$i,IsSuperClass) 0
      set EMSegment(Cattrib,$i,ClassList) ""

      set EMSegment(Cattrib,$i,Name) ""
      set EMSegment(Cattrib,$i,ProbabilityData) $Volume(idNone)

      for {set y 0} {$y < $EMSegment(MaxInputChannelDef)} {incr y} {
          set EMSegment(Cattrib,$i,LogMean,$y) 0.0
          for {set x 0} {$x < $EMSegment(MaxInputChannelDef)} {incr x} {
             set EMSegment(Cattrib,$i,LogCovariance,$y,$x)  0.0     
          }
      }
      set EMSegment(Cattrib,$i,Label) 0
      set EMSegment(Cattrib,$i,Prob) 0.0
      set EMSegment(Cattrib,$i,ShapeParameter) 0.0

      foreach Name $EMSegment(CIMList) {
      for {set y $StartIndex} { $y < $max} {incr y} {
          set EMSegment(Cattrib,$SuperClass,CIMMatrix,$i,$y,$Name) 0.0
      }
      set EMSegment(Cattrib,$SuperClass,CIMMatrix,$i,$i,$Name) 1.0
      }

      # PCA Parameter
      set EMSegment(Cattrib,$i,PCAFileRange) "0 0"
      set EMSegment(Cattrib,$i,PCAMeanData) $Volume(idNone)
      set EMSegment(Cattrib,$i,PCATranslation)  "0.0 0.0 0.0" 
      set EMSegment(Cattrib,$i,PCARotation)     "0.0 0.0 0.0" 
      set EMSegment(Cattrib,$i,PCAScale)        "1.0 1.0 1.0"
      set EMSegment(Cattrib,$i,PCAMaxDist)      "0.0"  
      set EMSegment(Cattrib,$i,PCADistVariance) "0.0" 

      set EMSegment(Cattrib,$i,DICEData) $Volume(idNone)

      # Members of PCAEigen) are defined by (Number, EigenValue, EigenVectorData, NodeID)     
      set EMSegment(Cattrib,$i,PCAEigen) ""
  }
}


# Set everything for mode 6

proc EMSegmentSetClassWeightSetting {SuperClass NumInputImages } {
  global EMSegment

  foreach i $EMSegment(Cattrib,$SuperClass,ClassList) {
      set EMSegment(Cattrib,$i,InputChannelWeights,0) 1.0
      for {set x 1} {$x < $NumInputImages} {incr x } {
      set EMSegment(Cattrib,$i,InputChannelWeights,$x) 0.0
      }

      if {$EMSegment(Cattrib,$i,IsSuperClass)} {
       EMSegmentSetClassWeightSetting $i $NumInputImages
      }  
  }
}

#-------------------------------------------------------------------------------
# .PROC LoadFile 
#  Loads all the paramter necessary to start the segmentation process
#  Put together from parse.tcl - MainMrmlReadVersion2.x {fileName} 
#  and MainMrml.tcl - MainMrmlBuildTreesVersion2.0
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc LoadFile {fileName Mode} {
    global EMSegment Volume
    # Returns list of tags on success else 0

    # Read file
    if {[catch {set fid [open $fileName r]} errmsg] == 1} {
        puts $errmsg
        return -1 
    }
    set mrml [read $fid]
    if {[catch {close $fid} errorMessage]} {
    puts "Aborting due to : ${errorMessage}"
    exit 1
    }

    # accepts all versions from MRML 2.0 to 2.5
    if {[regexp {<!DOCTYPE MRML SYSTEM ['"]mrml2[0-5].dtd['"]>} $mrml match] == 0} {
        puts "The file \"$fileName\" is NOT MRML version 2.x"
        return -1
    }

    # Strip off everything but the body
    if {[regexp {<MRML>(.*)</MRML>} $mrml match mrml] == 0} {
        puts "There's no content in the file"
        return -1
    }

    # Strip leading white space
    regsub "^\[\n\t \]*" $mrml "" mrml

    set tags1 ""
    while {$mrml != ""} {

        # Find next tag
        if {[regexp {^<([^ >]*)([^>]*)>([^<]*)} $mrml match tag attr stuffing] == 0} {
        #([^/>]*)
                set errmsg "Invalid MRML file. Can't parse tags:\n$mrml"
            puts "$errmsg"
            return 0
        }

        # Strip off this tag, so we can continue.
         if {[lsearch "Transform /Transform" $tag] != -1 || \
            [lsearch "Fiducials /Fiducials" $tag] != -1 || \
            [lsearch "Hierarchy /Hierarchy" $tag] != -1 || \
            [lsearch "ModelGroup /ModelGroup" $tag] != -1 || \
            [lsearch "Scenes /Scenes" $tag] != -1 || \
            [lsearch "VolumeState /VolumeState" $tag] != -1 || \
            [lsearch "Path /Path" $tag] != -1 || \
            [lsearch "Segmenter /Segmenter" $tag] != -1 || \
            [lsearch "SegmenterSuperClass /SegmenterSuperClass" $tag] != -1 } {
            # set str "<$tag>" doesn't work with tags which have attributes
            set str ">"
        } else {
            set str "</$tag>"
            set str2 " />"
        }
        set str2_used 0
        set i [string first $str $mrml]
        if {($i<=0) && ([info exists str2])} {
            set i [string first $str2 $mrml]
            set str2_used 1
        }
        if {!$str2_used} {
            set mrml [string range $mrml [expr $i + [string length $str]] end]
        } else {
            set mrml [string range $mrml [expr $i + [string length $str2]] end]
        }

        # Give the EndTransform tag a name
        if {$tag == "/Transform"} {
            set tag EndTransform
        }

        # Give the EndFiducials tag a name
        if {$tag == "/Fiducials"} {
            set tag EndFiducials
        }

        # Give the EndPath tag a name
        if {$tag == "/Path"} {
            set tag EndPath
        }

        # Give the EndHierarchy tag a name
        if {$tag == "/Hierarchy"} {
            set tag EndHierarchy
        }
        
        # Give the EndModelGroup tag a name
        if {$tag == "/ModelGroup"} {
            set tag EndModelGroup
        }
        
        # Give the EndScenes tag a name
        if {$tag == "/Scenes"} {
            set tag EndScenes
        }

        # Give the EndVolumeState tag a name
        if {$tag == "/VolumeState"} {
            set tag EndVolumeState
        }
        # Give the EndSegmenter tag a name
        if {$tag == "/Segmenter"} {
            set tag EndSegmenter
        }

        # Give the EndSegmenter tag a name
        if {$tag == "/SegmenterSuperClass"} {
            set tag EndSegmenterSuperClass
        }
        # Append to List of tags1
        lappend tags1 "$tag {$attr} {$stuffing}"

        # Strip leading white space
        regsub "^\[\n\t \]*" $mrml "" mrml
    }

    # Parse the attribute list for each tags
    # Current SupperClass
    set EMSegment(SuperClass) 0 
    set SclassMemory ""

    set tags ""
 
    set NumClassesDef 0
    set EMSegment(SelVolList,VolumeList) ""
    set EMSegment(NumInputChannel) 0
    set EMSegment(GraphNum)        0
    set EMSegment(EMShapeIter)     1
    set EMSegment(GlobalSuperClassList) ""
    set EMSegment(DebugVolume) 0

    foreach pair $tags1 {
        set tag [lindex $pair 0]
        set attr [lindex $pair 1]
        set stuffing [lindex $pair 2]
        
        # Add the options (the "stuffing" from inside the start and end tags)
        set attrList ""
        lappend attrList "options $stuffing"

        # Strip leading white space
        regsub "^\[\n\t \]*" "$attr $stuffing" "" attr

        while {$attr != ""} {
        
            # Find the next key=value pair (and also strip it off... all in one step!)
            if {([regexp "^(\[^=\]*)\[\n\t \]*=\[\n\t \]*\['\"\](\[^'\"\]*)\['\"\](.*)$" \
                $attr match key value attr] == 0) && ([string equal -length 1 $attr "/"] == 0)} {
                set errmsg "Invalid MRML file. Can't parse attributes:\n$attr"
                puts "$errmsg"
                return 0
            }
            if {[string equal -length 1 $attr "/"] == 0} {
                lappend attrList "$key $value"
            } else {
                # Strip the / at then end of an XML tag
                regsub "/" $attr "" attr
            }

            # Strip leading white space
            regsub "^\[\n\t \]*" $attr "" attr
        }

        # Add this tag
        lappend tags "$tag $attrList"
    }
    # -------------------------------------------------------
    # Finished loading # we could just use parse for that 
    # For the following part we cannot just use the MainMrml file
    # ------------------------------------------------------
    set EMSegment(VolNumber)   0
    set NumClassesDef 0
    set EMSegment(SelVolList,VolumeList) ""
    set EMSegment(NumInputChannel) 0
    foreach pair $tags {
        set tag  [lindex $pair 0]
        set attr [lreplace $pair 0 0]
        switch $tag {
        
        "Volume" {
          incr EMSegment(VolNumber)
          set num $EMSegment(VolNumber)
          set Volume($num,Name) ""
          set DataDimension "0.9375 0.9375 1.5"
          set DataSpacing 0.0
          vtkImageReader Volume($num,vol)
          # Default
          Volume($num,vol) ReleaseDataFlagOff
          Volume($num,vol) SetDataScalarTypeToShort
          Volume($num,vol) SetDataExtent 0 255 0 255 1 124 
          foreach a $attr {
            set key [lindex $a 0]
            set val [lreplace $a 0 0]
            switch [string tolower $key] {
              "name"            {set Volume($num,Name) $val}
              "filepattern"     {Volume($num,vol) SetFilePattern    $val}
              "fileprefix"      {set Volume($num,FilePrefix) $val; Volume($num,vol) SetFilePrefix $val}
              "imagerange"      {set Volume($num,ImageRange) $val; eval Volume($num,vol) SetDataExtent 0 255 0 255 $val}
              "spacing"         {set DataSpacing $val}
              "dimensions"      {set DataDimensions $val}
              "scalartype"      {Volume($num,vol) SetDataScalarTypeTo$val}
              "numscalars"      {Volume($num,vol) SetNumberOfScalarComponents $val}
              "littleendian"    {if {$val == "yes" || $val == "true"} {
                                    Volume($num,vol) SetDataByteOrderToLittleEndian
                                 } else {
                                    Volume($num,vol) SetDataByteOrderToBigEndian
                                 }
                                }
        }
          }
          set DataDim3 [lindex $DataDimension 2]
          set DataDimension [lreplace $DataDimension 2 2 [expr $DataSpacing +$DataDim3]]  
          eval Volume($num,vol) SetDataSpacing $DataDimension
          Volume($num,vol) Update

          puts "===================== Volume $Volume($num,Name) defined ====================="
          if {$EMSegment(debug)} {puts "[Volume($num,vol) Print]"}    
        }
        "Segmenter" {
          set NumClasses 0 
          set EMSegment(MaxInputChannelDef) 0
          set EMSegment(EMiteration) 0 
          set EMSegment(MFAiteration) 0
          set EMSegment(Alpha) 0
          set EMSegment(SmWidth) 0
          set EMSegment(SmSigma) 0
          set EMSegment(PrintIntermediateResults) 0
          set EMSegment(PrintIntermediateSlice) -1 
          set EMSegment(PrintIntermediateFrequency) -1 
          set EMSegment(PrintIntermediateDir) "."
          set EMSegment(BiasPrint) 0

          set EMSegment(SegmentationBoundaryMin,0) 1 
          set EMSegment(SegmentationBoundaryMin,1) 1 
          set EMSegment(SegmentationBoundaryMin,2) 0
          set EMSegment(SegmentationBoundaryMax,0) 256  
          set EMSegment(SegmentationBoundaryMax,1) 256 
          set EMSegment(SegmentationBoundaryMin,2) 0  

          set EMSegment(NumberOfTrainingSamples) 0 
          set EMSegment(IntensityAvgClass) -1
          lappend EMSegment(GlobalSuperClassList) 0     
          foreach a $attr {
            set key [lindex $a 0]
            set val [lreplace $a 0 0]
            switch [string tolower $key] {
              "numclasses"         {set NumClasses $val}
              "maxinputchanneldef" {set EMSegment(MaxInputChannelDef) $val}
              "emiteration"        {set EMSegment(EMiteration) $val}
              "mfaiteration"       {set EMSegment(MFAiteration) $val}
              "alpha"              {set EMSegment(Alpha) $val}
              "smwidth"            {set EMSegment(SmWidth) $val}
              "smsigma"            {set EMSegment(SmSigma) $val}
              "printintermediateresults"   {set EMSegment(PrintIntermediateResults) $val}
              "printintermediateslice"     {set EMSegment(PrintIntermediateSlice) $val}
              "printintermediatefrequency" {set EMSegment(PrintIntermediateFrequency) $val}
              "startslice"                 {set EMSegment(SegmentationBoundaryMin,2) $val}
              "endslice"                   {set EMSegment(SegmentationBoundaryMax,2) $val}
              "numberoftrainingsamples"    {set EMSegment(NumberOfTrainingSamples) $val}
              "intensityavgclass"          {set EMSegment(IntensityAvgClass) $val}
            }
          }
          puts "===================== EM Segmenter Parameters  ====================="
          if {$EMSegment(debug)} {puts "[array get EMSegment]"}
          # Define default Class Parameters 
          set EMSegment(SuperClass) 0
          set EMSegment(Cattrib,0,Name) "Head"
          set EMSegment(Cattrib,0,Prob) 0.0 
          set EMSegment(Cattrib,0,ProbabilityData) $Volume(idNone)
          set EMSegment(Cattrib,0,PCAMeanData) $Volume(idNone)
          set EMSegment(Cattrib,0,DICEData) $Volume(idNone)
          set EMSegment(Cattrib,0,LocalPriorWeight) 0 

          CreateSuperClass 0 $NumClasses 1
          set StartIndex [expr $NumClasses+1]
          set CurrentClassList $EMSegment(Cattrib,0,ClassList)       
        }    
        "SegmenterInput" {
           foreach a $attr {
             set key [lindex $a 0]
             set val [lreplace $a 0 0]
             switch [string tolower $key] {
               "name"        {set Name $val}
               "fileprefix"  {set FilePrefix $val}
               "filename"    {set FileName $val}
               "imagerange"  {set ImageRange  $val}
               "intensityavgvaluepredef" {set intensity $val}
             }
           }
           for {set i 1} {$i <= $EMSegment(VolNumber)} {incr i} {
             if {$FilePrefix == $Volume($i,FilePrefix) && $FileName == $Volume($i,Name) && $ImageRange == $Volume($i,ImageRange)} { 
                incr EMSegment(NumInputChannel)
                set EMSegment(IntensityAvgValue,$i) $intensity 
                lappend EMSegment(SelVolList,VolumeList) $i
                puts "===================== EM Segmenter Input Channel $Name  ====================="
                break
              }
           }    
        }
        "SegmenterSuperClass" {
        set NumClass [lindex $CurrentClassList 0]
        set CurrentClassList [lrange $CurrentClassList 1 end]
            set NumClasses 0
            set EMSegment(Cattrib,$NumClass,LocalPriorWeight) 1.0
        for {set y 0} {$y < $EMSegment(MaxInputChannelDef)} {incr y} {
                   set EMSegment(Cattrib,$NumClass,InputChannelWeights,$y) 1.0
        }
            if {$NumClass == ""} { puts "Error in XML File : Super class $EMSegment(SuperClass)  has not a sub-classes defined" }
            foreach a $attr {
                set key [lindex $a 0]
                set val [lreplace $a 0 0]
                switch [string tolower $key] {
                    "numclasses" {set NumClasses $val}
                    "name"       {set EMSegment(Cattrib,$NumClass,Name) $val}
            "prob"       {set EMSegment(Cattrib,$NumClass,Prob) $val}
                }
            }
            lappend SclassMemory [list "$EMSegment(SuperClass)" "$CurrentClassList"]
        set EMSegment(SuperClass) $NumClass
        CreateSuperClass $NumClass $NumClasses $StartIndex
        incr StartIndex [expr $NumClasses+1]
        set CurrentClassList $EMSegment(Cattrib,$NumClass,ClassList)
        puts "===================== EM Segmenter SuperClass $EMSegment(Cattrib,$NumClassesDef,Name) ========================"
        if {$EMSegment(debug)} {puts "[array get EMSegment Cattrib,$EMSegment(SuperClass),*]"}

         }
         "EndSegmenterSuperClass" {
          # Pop the last parent from the Stack
          set temp [lindex $SclassMemory end]
          set SclassMemory [lreplace $SclassMemory end end]
          set CurrentClassList [lindex $temp 1] 
          set EMSegment(SuperClass) [lindex $temp 0]
          puts "===================== EM Segmenter EndSuperClass  ========================"
          }
          "SegmenterClass" {
         set NumClassesDef [lindex $CurrentClassList 0]
         set CurrentClassList [lrange $CurrentClassList 1 end]   

             set LocalPriorPrefix ""
             set LocalPriorName   ""
             set LocalPriorRange  ""
             set EMSegment(Cattrib,$NumClassesDef,LocalPriorWeight) 1.0

         for {set y 0} {$y < $EMSegment(MaxInputChannelDef)} {incr y} {
                   set EMSegment(Cattrib,$NumClassesDef,InputChannelWeights,$y) 1.0
         }

             
             foreach a $attr {
               set key [lindex $a 0]
               set val [lreplace $a 0 0]
               switch [string tolower $key] {
                 "name"              {set EMSegment(Cattrib,$NumClassesDef,Name) $val}
                 "localpriorprefix"  {set LocalPriorPrefix $val}
                 "localpriorname"    {set LocalPriorName $val}
                 "localpriorrange"   {set LocalPriorRange  $val}
                 "logmean"           {set LogMean $val}
                 "logcovariance"     {set LogCovariance $val}
                 "label"             {set EMSegment(Cattrib,$NumClassesDef,Label) $val}
                 "prob"              {set EMSegment(Cattrib,$NumClassesDef,Prob) $val}
         "localpriorweight"  {set LocalPriorWeight $val}
                 "inputchannelweights"  {set InputChannelWeights $val}
               }
            }
            set index 0
            for {set y 0} {$y < $EMSegment(MaxInputChannelDef)} {incr y} {
              set EMSegment(Cattrib,$NumClassesDef,LogMean,$y) [lindex $LogMean $y]
              for {set x 0} {$x < $EMSegment(MaxInputChannelDef)} {incr x} {
                set EMSegment(Cattrib,$NumClassesDef,LogCovariance,$y,$x)  [lindex $LogCovariance $index]
                incr index
              }
              incr index
            }
            for {set i 1} {$i <= $EMSegment(VolNumber)} {incr i} {
              if {$LocalPriorPrefix == $Volume($i,FilePrefix) && $LocalPriorName == $Volume($i,Name) && $LocalPriorRange == $Volume($i,ImageRange)} { 
                set EMSegment(Cattrib,$NumClassesDef,ProbabilityData) $i
                break
              }
            }
            puts "===================== EM Segmenter Class $EMSegment(Cattrib,$NumClassesDef,Name) ========================"
            if {$EMSegment(debug)} {puts "[array get EMSegment Cattrib,$NumClassesDef,*]"}
          }
          "SegmenterCIM" {
             set Name ""
             set CIMMatrix ""
             foreach a $attr {
               set key [lindex $a 0]
               set val [lreplace $a 0 0]
               switch [string tolower $key] {
                 "name"       {set Name $val}
                 "cimmatrix"  {set CIMMatrix $val}
               }
             }
             if {[lsearch $EMSegment(CIMList) $Name] > -1} { 
               set i 0
               foreach y $EMSegment(Cattrib,$EMSegment(SuperClass),ClassList) {
          foreach x $EMSegment(Cattrib,$EMSegment(SuperClass),ClassList) {
            set EMSegment(Cattrib,$EMSegment(SuperClass),CIMMatrix,$x,$y,$Name) [lindex $CIMMatrix $i]
            incr i
          }
          incr i
               }
               puts "===================== EM Segmenter CIM $Name for $EMSegment(SuperClass)  ========================"
               if {$EMSegment(debug)} {puts "[array get EMSegment CIMMatrix,*,*,$Name]"}
           }
        }
      }
   }
   # If is set to Mode for it stays like it is.
   switch $Mode {
       1 { set  EMSegment(Alpha) 0; set EMSegment(EMiteration) 1}
       2 { set  EMSegment(Alpha) 0.9; set EMSegment(EMiteration) 1; set EMSegment(MFAiteration) 1}
       3 { set  EMSegment(Alpha) 0.9; set EMSegment(MFAiteration) 0 }
       5 { set  EMSegment(Alpha) 0.9; set EMSegment(EMiteration) 1; set EMSegment(MFAiteration) 1; set EMSegment(Cattrib,2,LocalPriorWeight) 0.80488 }
       6 { set  EMSegment(Alpha) 0.9; set EMSegment(EMiteration) 2; set EMSegment(MFAiteration) 1;  EMSegmentSetClassWeightSetting 0 $EMSegment(MaxInputChannelDef)}
       7 {  set EMSegment(SegmentationBoundaryMin,0) 80 
        set EMSegment(SegmentationBoundaryMin,1) 70
         set EMSegment(SegmentationBoundaryMin,2) 1
            set EMSegment(SegmentationBoundaryMax,0) 200 
            set EMSegment(SegmentationBoundaryMax,1) 180 
        set EMSegment(SegmentationBoundaryMax,2) 2 } 
   }
   return 1
}


#-------------------------------------------------------------------------------
# 3. Step Execute 
#-------------------------------------------------------------------------------
if {$Mode == 8} {
    set XMLFile TestImageEMLocalSegmentDataHir.xml
} else {
    set XMLFile TestImageEMLocalSegmentData.xml
}

puts "Load File $XMLFile"

if {[LoadFile $XMLFile $Mode] < 1} {
  puts "Error:: Could not load the file TestImageEMLocalSegmentData.xml"
  exit 1 
}

# Check Values 
if {$EMSegment(NumInputChannel) == 0} {
    puts "Error::EMSegmentStartEM: No Input channels defined !"
    exit 1
}
 
EMSegmentStartEM
 
#-------------------------------------------------------------------------------
# 4. Write Result to a File 
#-------------------------------------------------------------------------------
# Remove old files if exist
set i 1
while {[file exists TestImageEMLocalSegmentTestResult.[format %03d $i]]} {
    file delete -force TestImageEMLocalSegmentTestResult.[format %03d $i]
    incr i; 
}  

vtkImageWriter WriteResult 
WriteResult  SetInput [EMSegment(vtkEMSegment) GetOutput]
EMSegment(vtkEMSegment) Update
# puts [EMSegment(vtkEMSegment) Print]
WriteResult  SetFilePrefix TestImageEMLocalSegmentTestResult
WriteResult  SetFilePattern %s.%03d
# Write volume data
puts "Writing TestImageEMLocalSegmentTestResult ..."
WriteResult  Write
puts " ...done."
WriteResult Delete

# Find Maximum File extension 
set max 0
while {[file exists TestImageEMLocalSegmentTestResult.[format %03d $max]]} {incr max}  
for {set i $max} {$i > 0} {incr i -1} {
     file rename TestImageEMLocalSegmentTestResult.[format %03d [expr $i-1]] TestImageEMLocalSegmentTestResult.[format %03d $i] 
}

#-------------------------------------------------------------------------------
# 6. Delete all the old instances 
#-------------------------------------------------------------------------------
for {set i 1} {$i <= $EMSegment(VolNumber)} {incr i} {
  Volume($i,vol) Delete
}  
EMSegment(vtkEMSegment) Delete

#-------------------------------------------------------------------------------
# 7. Test Images for correctness 
#-------------------------------------------------------------------------------


if {[file exists $env(SCRIPT_HOME)/tcl/MathImage.tcl]} {
  puts  "\n================================== Analyse Test Results ========================"
  puts "The report below should display Rate(%): 0.00"
  if { $Mode == 4 } { set TestFiles TestImageEMLocalSegmentResult 
  } else {
    set TestFiles TestImageEMLocalSegmentResultMode$Mode
  } 
  if {$tcl_platform(byteOrder) == "littleEndian"} {set Endian yes 
  } else {set Endian no}

  puts [exec $env(VTK_EXE) $env(SCRIPT_HOME)/tcl/MathImage.tcl -pr $TestFiles -p2 TestImageEMLocalSegmentTestResult -le yes -l2 $Endian  -fc com -ir "1 3"]

} else {
  puts "Could not automatically check correctness of test results because $env(SCRIPT_HOME)/tcl/MathImage.tcl did not exists !"
}
exit 0
