# This script allows the direct access of the vtkImageEMLocalSegmenter Filter. It is very useful for batch processing.
# Function can be called through
# setenv EMBATCH_HOME /home/ai2/kpohl/slicer_devel/vtk3.2/local/examplesTcl
# setenv LD_LIBRARY_PATH /home/ai2/kpohl/slicer_devel/pkg/lib:${LD_LIBRARY_PATH}
# setenv SLICER_USE_LOCAL_ONLY_CODE 1
# exec /home/ai2/kpohl/slicer_devel/pkg/bin/vtk $EMBATCH_HOME/BatchProcessEMLocalSegmenter.tcl <Mrml File Defining Segmentation> <Display Graph Afterwards>

#-------------------------------------------------------------------------------
# 1. Step Initialize 
#-------------------------------------------------------------------------------

# This is just so it works on PCs
catch {load vtktcl}
# vtkImageViewer viewer
#
set EMSegment(SegmentMode) 0
puts "Result will be written as TestImageEMSegmentTestResult.* - compare it too TestImageEMSegmentResult.*"
source ../../imaging/examplesTcl/vtkImageInclude.tcl  
set EMSegment(XMLDir)        TestImageEMSegmentData.xml
set EMSegment(DisplayWindow) 0
if {$EMSegment(DisplayWindow) == 0} {
    wm withdraw .
}  
set EMSegment(debug) 0
set EMSegment(CIMList) {West North Up East South Down}

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
proc EMSegmentStartEM {} {
   global EMSegment Volume
   if {$EMSegment(SegmentMode) > 0} {
       # EMLocalSegmentation: Multiple Input Images
       vtkImageEMLocalSegmenter EMStart 
       # How many input images do you have
       EMStart SetNumInputImages $EMSegment(NumInputChannel)  
       EMStart SetNumClasses $EMSegment(NumClasses)  
       for {set i 1} { $i<= $EMSegment(NumClasses)} {incr i} {
           EMStart SetTissueProbability $EMSegment(Cattrib,$i,Prob) $i
       } 
       EMStart SetNumberOfTrainingSamples $EMSegment(NumberOfTrainingSamples)
       # Transefer probability map
       set NumInputImagesSet 0
       set i 0
       while {$i< $EMSegment(NumClasses)} {
          incr i
          # Kilian Changed here from standard
          if {$EMSegment(Cattrib,$i,ProbabilityData) != 0} {
             EMStart SetUseLocalPrior 1 $i
             EMStart SetInputIndex $NumInputImagesSet [Volume($EMSegment(Cattrib,$i,ProbabilityData),vol) GetOutput]
             incr NumInputImagesSet
          } else {
            EMStart SetUseLocalPrior 0 $i 
      }
       }
       # Transfer image information
       foreach v $EMSegment(SelVolList,VolumeList) {
       EMStart SetInputIndex $NumInputImagesSet [Volume($v,vol) GetOutput]
       incr NumInputImagesSet
       }
   } else {
       # EM Specific Information - Simple EM Algorithm
       vtkImageEMSegmenter EMStart    
       EMStart SetNumClasses      $EMSegment(NumClasses)  

       EMStart SetImgTestNo       -1 
       EMStart SetImgTestDivision 0
       EMStart SetImgTestPixel    0
       for {set i 1} { $i<= $EMSegment(NumClasses)} {incr i} {
          EMStart SetProbability  $EMSegment(Cattrib,$i,Prob) $i
       }
       # Transfer image information
       # Kilian: Changed here from normal input 
       EMStart SetInput [Volume([lindex $EMSegment(SelVolList,VolumeList) 0],vol) GetOutput]
   }

   #----------------------------------------------------------------------------
   # Transfering General Information
   #----------------------------------------------------------------------------
   EMStart SetNumIter         $EMSegment(EMiteration)  
   EMStart SetNumRegIter      $EMSegment(MFAiteration) 
   EMStart SetAlpha           $EMSegment(Alpha) 

   EMStart SetSmoothingWidth  $EMSegment(SmWidth)    
   EMStart SetSmoothingSigma  $EMSegment(SmSigma)      

   EMStart SetStartSlice      $EMSegment(StartSlice)
   EMStart SetEndSlice        $EMSegment(EndSlice)

   EMStart SetPrintIntermediateResults  $EMSegment(PrintIntermediateResults) 
   EMStart SetPrintIntermediateSlice  $EMSegment(PrintIntermediateSlice) 
   EMStart SetPrintIntermediateFrequency  $EMSegment(PrintIntermediateFrequency) 

   for {set i 1} { $i<= $EMSegment(NumClasses)} {incr i} {
       set yindex 1
       for {set y 0} {$y < $EMSegment(NumInputChannel)} {incr y} {
       if {$EMSegment(SegmentMode)} {
           EMStart SetLogMu $EMSegment(Cattrib,$i,LogMean,$y) $i $yindex
           for {set x 0} {$x < $EMSegment(NumInputChannel)} {incr x} {
           EMStart SetLogCovariance $EMSegment(Cattrib,$i,LogCovariance,$y,$x) $i $yindex [expr $x+1]
           }
           incr yindex
       } else {
           EMStart SetMu $EMSegment(Cattrib,$i,LogMean,$y) $i
           EMStart SetSigma $EMSegment(Cattrib,$i,LogCovariance,$y,$y) $i 
       }
       }
       EMStart SetLabel  $EMSegment(Cattrib,$i,Label) $i 
       # Reads in the value for each class individually
       for {set j 1} { $j<= $EMSegment(NumClasses)} {incr j} {
       for {set k 0} { $k< 6} {incr k} {
           EMStart SetMarkovMatrix $EMSegment(CIMMatrix,$i,$j,[lindex $EMSegment(CIMList) $k]) [expr $k+1] $j $i
       }
       }
   }
   return [EMStart GetOutput]
}

#-------------------------------------------------------------------------------
# .PROC LoadFile 
#  Loads all the paramter necessary to start the segmentation process
#  Put together from parse.tcl - MainMrmlReadVersion2.x {fileName} 
#  and MainMrml.tcl - MainMrmlBuildTreesVersion2.0
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc LoadFile {fileName} {
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
            tk_messageBox -message "$errmsg"
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
            [lsearch "Segmenter /Segmenter" $tag] != -1 } {
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

        # Append to List of tags1
        lappend tags1 "$tag {$attr} {$stuffing}"

        # Strip leading white space
        regsub "^\[\n\t \]*" $mrml "" mrml
    }

    # Parse the attribute list for each tag
    set tags ""
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
            "littleendian" {
                if {$val == "yes" || $val == "true"} {
                Volume($num,vol) SetDataByteOrderToLittleEndian
                } else {
                Volume($num,vol) SetDataByteOrderToBigEndian
                }
            }
            }
            set DataDim3 [lindex $DataDimension 2]
            set DataDimension [lreplace $DataDimension 2 2 [expr $DataSpacing +$DataDim3]]  
            eval Volume($num,vol) SetDataSpacing $DataDimension
        }
        puts "===================== Volume $Volume($num,Name) defined ====================="
        if {$EMSegment(debug)} {puts "[Volume($num,vol) Print]"}    
        }
        "Segmenter" {
        set EMSegment(NumClasses) 0 
        set EMSegment(MaxInputChannelDef) 0
        set EMSegment(EMiteration) 0 
        set EMSegment(MFAiteration) 0
        set EMSegment(Alpha) 0
        set EMSegment(SmWidth) 0
        set EMSegment(SmSigma) 0
        set EMSegment(PrintIntermediateResults) 0
        set EMSegment(PrintIntermediateSlice) -1 
        set EMSegment(PrintIntermediateFrequency) -1 
        set EMSegment(StartSlice) 0 
        set EMSegment(EndSlice) 0 
        set EMSegment(NumberOfTrainingSamples) 0 
        foreach a $attr {
            set key [lindex $a 0]
            set val [lreplace $a 0 0]
            switch [string tolower $key] {
            "numclasses"         {set EMSegment(NumClasses) $val}
            "maxinputchanneldef" {set EMSegment(MaxInputChannelDef) $val}
            "emiteration"        {set EMSegment(EMiteration) $val}
            "mfaiteration"       {set EMSegment(MFAiteration) $val}
            "alpha"              {set EMSegment(Alpha) $val}
            "smwidth"            {set EMSegment(SmWidth) $val}
            "smsigma"            {set EMSegment(SmSigma) $val}
            "printintermediateresults"   {set EMSegment(PrintIntermediateResults) $val}
            "printintermediateslice"     {set EMSegment(PrintIntermediateSlice) $val}
            "printintermediatefrequency" {set EMSegment(PrintIntermediateFrequency) $val}
            "startslice"                 {set EMSegment(StartSlice) $val}
            "endslice"                   {set EMSegment(EndSlice) $val}
            "numberoftrainingsamples"    {set EMSegment(NumberOfTrainingSamples) $val}
            }
        }
        puts "===================== EM Segmenter Parameters  ====================="
        if {$EMSegment(debug)} {puts "[array get EMSegment]"}
        # Define default Class Parameters
        for {set i 1} { $i <= $EMSegment(NumClasses)} {incr i} {
            set EMSegment(Cattrib,$i,Name) ""
            set EMSegment(Cattrib,$i,ProbabilityData) 0
            for {set y 0} {$y < $EMSegment(MaxInputChannelDef)} {incr y} {
            set EMSegment(Cattrib,$i,LogMean,$y) 0.0
            for {set x 0} {$x < $EMSegment(MaxInputChannelDef)} {incr x} {
                set EMSegment(Cattrib,$i,LogCovariance,$y,$x)  0.0     
            }
            }
            set EMSegment(Cattrib,$i,Label) 0
            set EMSegment(Cattrib,$i,Prob) 0.0
        }
        foreach Name $EMSegment(CIMList) {
            for {set y 1} { $y<= $EMSegment(NumClasses)} {incr y} {
            for {set x 1} { $x<= $EMSegment(NumClasses)} {incr x} {
                set EMSegment(CIMMatrix,$x,$y,$Name) 0.0
            }
            }
        }
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
            }
        }
        for {set i 1} {$i <= $EMSegment(VolNumber)} {incr i} {
            if {$FilePrefix == $Volume($i,FilePrefix) && $FileName == $Volume($i,Name) && $ImageRange == $Volume($i,ImageRange)} { 
            incr EMSegment(NumInputChannel)
            lappend EMSegment(SelVolList,VolumeList) $i
            puts "===================== EM Segmenter Input Channel $Name  ====================="
            break
            }
        }    
        }
        "SegmenterClass" {
        incr NumClassesDef 
        set LocalPriorPrefix ""
        set LocalPriorName   ""
        set LocalPriorRange  ""
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
            for {set y 1} { $y<= $EMSegment(NumClasses)} {incr y} {
            for {set x 1} { $x<= $EMSegment(NumClasses)} {incr x} {
                set EMSegment(CIMMatrix,$x,$y,$Name) [lindex $CIMMatrix $i]
                incr i
            }
            incr i
            }
            puts "===================== EM Segmenter CIM $Name  ========================"
            if {$EMSegment(debug)} {puts "[array get EMSegment CIMMatrix,*,*,$Name]"}
        }
        }
    }
    }
    return 1
}


#-------------------------------------------------------------------------------
# 3. Step Execute 
#-------------------------------------------------------------------------------
puts "Load File TestImageEMSegmentData.xml"
if {[LoadFile TestImageEMSegmentData.xml] < 1} {
  puts "Error:: Could not load the file TestImageEMSegmentData.xml"
  exit 1 
}

# Check Values 
if {$EMSegment(NumInputChannel) == 0} {
    puts "Error::EMSegmentStartEM: No Input channels defined !"
    exit 1
} 
set SegmentationResult [EMSegmentStartEM]

#-------------------------------------------------------------------------------
# 4. Write Result to a File 
#-------------------------------------------------------------------------------
vtkImageWriter WriteResult 
WriteResult  SetInput $SegmentationResult 
EMStart Update  
WriteResult  SetFilePrefix TestImageEMSegmentTestResult
WriteResult  SetFilePattern %s.%03d
# Write volume data
puts "Writing TestImageEMSegmentTestResult ..."
   WriteResult  Write
puts " ...done."
WriteResult Delete

for {set i $EMSegment(EndSlice)} {$i >= $EMSegment(StartSlice)} {incr i -1} {
    set targetFile TestImageEMSegmentTestResult.[format %03d $i]
    if {[file exists $targetFile]} {file delete $targetFile}
    file rename TestImageEMSegmentTestResult.[format %03d [expr $i-1]] $targetFile
}

#-------------------------------------------------------------------------------
# 6. Delete all the old instances 
#-------------------------------------------------------------------------------
for {set i 1} {$i <= $EMSegment(VolNumber)} {incr i} {
  Volume($i,vol) Delete
}  
#-------------------------------------------------------------------------------
# 7. Display window if necessary 
#-------------------------------------------------------------------------------
if {$EMSegment(DisplayWindow) } {
    vtkImageViewer viewer
    viewer SetZSlice  0
    # Find out Maximum Label Number
    set max $EMSegment(Cattrib,1,Label)
    for {set i 2} { $i<= $EMSegment(NumClasses)} {incr i} {
    if {$EMSegment(Cattrib,$i,Label) > $max} {
        set max $EMSegment(Cattrib,$i,Label)
    }
    }
    # How many values should be displayed
    set max_half [expr round($max / 2.0)]   
    viewer SetColorWindow [expr 2*$max_half]
    # Where are they centered around 
    viewer SetColorLevel $max_half
    viewer SetInput $SegmentationResult
    source ../../imaging/examplesTcl/WindowLevelInterface.tcl 
} else {
    EMStart Delete
    exit 1
}





