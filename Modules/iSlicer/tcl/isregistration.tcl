# TODO - won't be needed once iSlicer is a package
package require Iwidgets

if { [info command ::isvolume] == "" } {
    global env
    source $env(SLICER_HOME)/Modules/iSlicer/tcl/isvolume.tcl
}

if { [info command ::istask] == "" } {
    global env
    source $env(SLICER_HOME)/Modules/iSlicer/tcl/istask.tcl
}

#########################################################
#
if {0} { ;# comment

isregistration - a widget for running image registrations

  source : the moving volume
  target : the stationary volume.

# TODO : 
    -- pre-programmed sequence for multires registration
    -- convergence
}

#########################################################

#
# Default resources
# - sets the default colors for the widget components
#
option add *isregistration.target            "" widgetDefault
option add *isregistration.source            "" widgetDefault
option add *isregistration.transform         "" widgetDefault
option add *isregistration.resolution       128 widgetDefault
option add *isregistration.iterations         5 widgetDefault
option add *isregistration.samples           50 widgetDefault
option add *isregistration.learningrate     .01 widgetDefault
option add *isregistration.translatescale    64 widgetDefault
option add *isregistration.target_standarddev 1 widgetDefault
option add *isregistration.source_standarddev 1 widgetDefault
option add *isregistration.target_shrink {1 1 1} widgetDefault
option add *isregistration.source_shrink {1 1 1} widgetDefault

## for debugging
option add *isregistration.verbose         1 widgetDefault

#
# The class definition - define if needed (not when re-sourcing)
#
if { [itcl::find class isregistration] == "" } {

    itcl::class isregistration {
        inherit iwidgets::Labeledwidget

        constructor {args} {}
        destructor {}

        #
        # itk_options for widget options that may need to be
        # inherited or composed as part of other widgets
        # or become part of the option database
        #
        itk_option define -target target Target {}
        itk_option define -source source Source {}
        itk_option define -transform transform Transform {}
        itk_option define -resolution resolution Resolution 128
        itk_option define -samples samples Samples 50
        itk_option define -iterations iterations Iterations 5
        itk_option define -learningrate learningrate Learningrate .01
        itk_option define -target_standarddev target_stardarddev Target_standarddev 1
        itk_option define -source_standarddev source_stardarddev Source_standarddev 1
        itk_option define -target_shrink target_shrink Target_shrink {1 1 1}
        itk_option define -source_shrink source_shrink Source_shrink {1 1 1}

        itk_option define -translatescale translatescale Translatescale 64
        itk_option define -verbose verbose Verbose 1

        variable _name ""
    # is this the first time we are iterating
    variable _firsttime 1

    # the m_time of matrix being altered
    # keep track so we can see if it was changed
        variable _mat_m_time  -1

        # widgets
        variable _controls ""
        variable _task ""
        variable _targetvol ""
        variable _sourcevol ""
        variable _p1 ""
        variable _p2 ""

        # vtk instances
        variable _reg ""
        variable _matrix ""
        variable _targetcast ""
        variable _targetnorm ""
        variable _sourcecast ""
        variable _sourcenorm ""

        method step {} {}
        method StringMatrix { mat4x4 } {}
    method StringToMatrix { mat4x4 str} {}
        method GetSimilarityMatrix { s2 mat s1 } {}
        method getP1 {} {}
        method getP2 {} {}
        method update_slicer_mat {} {}
        method set_init_mat {} {}
        method start {} {$_task on  }
        method stop  {} {$_task off }
    }
}


# ------------------------------------------------------------------
#                        CONSTRUCTOR/DESTRUCTOR
# ------------------------------------------------------------------
itcl::body isregistration::constructor {args} {
    component hull configure -borderwidth 0

    # make a unique name associated with this object
    set _name [namespace tail $this]
    # remove dots from name so it can be used in widget names
    regsub -all {\.} $_name "_" _name

    #######
    ### make the subwidgets
    ### - isvolumes for target and source volumes
    ### - control area
    ### - note that the volumes are initialized using the -target
    ### -      and -source volumes
    #######

    ### create the two isvolumes, but do not give them any information.
    set _targetvol    [isvolume $itk_interior.target]
    set _sourcevol    [isvolume $itk_interior.source]

    pack $_targetvol -fill both -expand true
    pack $_sourcevol -fill both -expand true
    
    set _controls [iwidgets::scrolledframe $itk_interior.controls]
    pack $_controls -fill both -expand true

    set cs [$_controls childsite]
    set _task $cs.task
    istask $_task -taskcommand "$this step" -labeltext "Registration: " -taskdelay 100
    pack $_task

    set _resmenu $cs.resmenu
    iwidgets::optionmenu $_resmenu -labeltext "Resolution: " -labelpos w \
        -command "$this configure -resolution \[$_resmenu get\]"
    set resolutions { 8 16 32 64 128 256 512 }
    foreach r $resolutions {
        $_resmenu insert end $r
    }
    $_resmenu select [lsearch $resolutions "128"]
    pack $_resmenu 

    # align the control labels
    ::iwidgets::Labeledwidget::alignlabels $_task $_resmenu 

    ######
    ### set up the vtk pipeline
    ### - matrix that tie to slicer's matrix
    ### - image normalizers to prep volumes
    ### - the registration itself
    ######

    set _matrix ::matrix_$_name
    catch "$_matrix Delete"
    vtkMatrix4x4 $_matrix
    $_matrix Identity

    ######
    ##
    ## Cast target image to float and normalize
    ##
    ######
    global Matrix

    set _targetcast ::targetcast_$_name
    set _targetnorm ::targetnorm_$_name
    catch "$_targetcast Delete"
    catch "$_targetnorm Delete"

    ## hack by Samson
    set _targetchangeinfo ::targetchangeinfo_$_name
    set _sourcechangeinfo ::sourcechangeinfo_$_name
    catch "$_targetchangeinfo Delete"
    catch "$_sourcechangeinfo Delete"

    vtkImageCast $_targetcast
    $_targetcast SetOutputScalarTypeToFloat
    $_targetcast SetInput [$_targetvol imagedata]

    ### big hack by Samson -- turned on
    vtkImageChangeInformation $_targetchangeinfo
    $_targetchangeinfo CenterImageOn
    $_targetchangeinfo SetInput [Volume($Matrix(refVolume),vol) GetOutput]
    $_targetcast SetInput [$_targetchangeinfo GetOutput]

    vtkITKNormalizeImageFilter $_targetnorm
    $_targetnorm SetInput [$_targetcast GetOutput]

    ##
    ## Cast source image to float and normalize
    ##
    set _sourcecast ::sourcecast_$_name
    set _sourcenorm ::sourcenorm_$_name
    catch "$_sourcecast Delete"
    catch "$_sourcenorm Delete"

    vtkImageCast $_sourcecast
    $_sourcecast SetOutputScalarTypeToFloat
    $_sourcecast SetInput [$_sourcevol imagedata]

    ### big hack by Samson -- turned on
    vtkImageChangeInformation $_sourcechangeinfo
    $_sourcechangeinfo CenterImageOn
    $_sourcechangeinfo SetInput [Volume($Matrix(volume),vol) GetOutput]
    $_sourcecast SetInput [$_sourcechangeinfo GetOutput]

    vtkITKNormalizeImageFilter $_sourcenorm
    $_sourcenorm SetInput [$_sourcecast GetOutput]

    #######
    ## Create the MI Registration instance
    #######
    set _reg ::reg_$_name
    catch "$_reg Delete"
    vtkITKMutualInformationTransform $_reg
    $_reg Initialize $_matrix

    $_reg SetTargetImage [$_targetnorm GetOutput]
    $_reg SetSourceImage [$_sourcenorm GetOutput]

#    $_reg SetTargetImage [$_targetcast GetOutput]
#    $_reg SetSourceImage [$_sourcecast GetOutput]

    eval itk_initialize $args
}

itcl::body isregistration::destructor {} {
        catch "_reg Delete"
        catch "_matrix Delete"
        catch "_targetcast Delete"
        catch "_targetnorm Delete"
        catch "_sourcecast Delete"
        catch "_sourcenorm Delete"
}

#-------------------------------------------------------------------------------
# OPTION: -target
#
# DESCRIPTION: the stationary target volume 
# - name of a slicer volume
#-------------------------------------------------------------------------------
itcl::configbody isregistration::target {

    # TODO - this should be handled by passing the option through using the 
    # itk hide and related stuff

    if {$itk_option(-target) == ""} {
        return
    }
    puts "got here in target"
    $_targetvol configure -volume $itk_option(-target)
    $_targetvol configure -resolution $itk_option(-resolution)
    $_targetvol configure -orientation coronal ;# TODO extra config due to isvolume bug
    $_targetvol configure -orientation axial
}

#-------------------------------------------------------------------------------
# OPTION: -source
#
# DESCRIPTION: the source volume that the matrix applies to
# - name of a slicer volume
#-------------------------------------------------------------------------------
itcl::configbody isregistration::source {

    # TODO - this should be handled by passing the option through using the 
    # itk hide and related stuff

    if {$itk_option(-source) == ""} {
        return
    }
    puts "got here in source"
    $_sourcevol configure -volume $itk_option(-source)
    $_sourcevol configure -resolution $itk_option(-resolution)
    $_sourcevol configure -orientation coronal ;# TODO extra config due to isvolume bug
    $_sourcevol configure -orientation axial
}

#-------------------------------------------------------------------------------
# OPTION: -resolution
#
# DESCRIPTION: set the resolution of the calculation
#  this is not used at all
#-------------------------------------------------------------------------------
itcl::configbody isregistration::resolution {

    if {$itk_option(-source) != ""} {
        $_sourcevol configure -resolution $itk_option(-resolution)
        $_sourcevol configure -orientation coronal ;# TODO extra config due to isvolume bug
        $_sourcevol configure -orientation axial
    }
    if {$itk_option(-target) != ""} {
        $_targetvol configure -resolution $itk_option(-resolution)
        $_targetvol configure -orientation coronal ;# TODO extra config due to isvolume bug
        $_targetvol configure -orientation axial
    }
}

#-------------------------------------------------------------------------------
# METHOD: step
#
# DESCRIPTION: run an interation of the registration
#-------------------------------------------------------------------------------

itcl::body isregistration::step {} {
    global Matrix


#     puts "0 [Volume(0,vol) GetOutput]"
#     puts "1 [Volume(1,vol) GetOutput]"
#     puts "2 [Volume(2,vol) GetOutput]"
#     puts "target c getinfo [::changeinfo__mi_reg_lwchildsite_target GetInput]"
#
#    catch "aw Delete"
#    vtkImageWriter aw
#      aw SetInput [::changeinfo__mi_reg_lwchildsite_target GetOutput]
#      aw SetFilePrefix "/bigscratch/tmp/ftarget"
#      aw SetFilePattern "%s.%03d"
#      aw Write
#      aw Delete
#
#    catch "aw Delete"
#    vtkImageWriter aw
#      aw SetInput [::changeinfo__mi_reg_lwchildsite_source GetOutput]
#      aw SetFilePrefix "/bigscratch/tmp/fsource"
#      aw SetFilePattern "%s.%03d"
#      aw Write
#      aw Delete

    #######
    ## set the default values
    #######

    $_reg SetTranslateScale $itk_option(-translatescale)

    $_reg SetSourceStandardDeviation $itk_option(-source_standarddev)
    $_reg SetTargetStandardDeviation $itk_option(-target_standarddev)

    set i [lindex $itk_option(-source_shrink) 0 ]
    set j [lindex $itk_option(-source_shrink) 1 ]
    set k [lindex $itk_option(-source_shrink) 2 ]
    puts "$i $j $k $itk_option(-source_shrink)"
    $_reg SetSourceShrinkFactors $i $j $k

    set i [lindex $itk_option(-target_shrink) 0 ]
    set j [lindex $itk_option(-target_shrink) 1 ]
    set k [lindex $itk_option(-target_shrink) 2 ]
    puts "$i $j $k $itk_option(-target_shrink)"
    $_reg SetTargetShrinkFactors $i $j $k

    $_reg SetNumberOfSamples $itk_option(-samples)

    ## Reset for MultiResSettings
    $_reg ResetMultiResolutionSettings

    # set for MultiResStuff
    foreach iter  $itk_option(-iterations) {
        $_reg SetNextMaxNumberOfIterations $iter
    }
    foreach rate $itk_option(-learningrate) {
        $_reg SetNextLearningRate  $rate
    }

    if {[llength $itk_option(-iterations) ] != \
        [llength $itk_option(-learningrate)] } {
       DevErrorWindow "Must Have same number of levels of iterations as learning rates"
       return
     }

    #
    # Get the current matrix - if it's different from the
    # the last matrix we set, copy it in and re-init reg 
    #

    set t $itk_option(-transform)

     $this set_init_mat

    $_reg Modified
    $_reg Update
    if {$itk_option(-verbose)} {
        $_reg Print
        puts "Metric [$_reg GetMetricValue]"
    }

    $this update_slicer_mat

    # Update MRML and display
    MainUpdateMRML
    RenderAll

    # the next iteration will not be the first iter...
    set _firsttime 0;
}


#-------------------------------------------------------------------------------
# METHOD: update_slicer_mat
#
# DESCRIPTION: set the slicer matrix appropriately
#
# The matrix M returned by the registration takes vectors r1 from a space 
# centered on the center of the image, in mm space, and maps it to space r2
# centered on the center of the target image, in mm space
#
#  r2 <- M r1
#
# We want a matrix M' from Ras to Ras space. This is simply the Position Matrix
# P1 and P2 (for each volume)
#
#  R2 <- M' R1,  R1 = P1 r1, R2 = P2 r2
#  P2 r2 <- M' P1 r1
# So we conclude
#  M = P2^-1 M'P1
#
# or M' = P2 M P1^-1
#
#-------------------------------------------------------------------------------

itcl::body isregistration::update_slicer_mat {} {

    set t $itk_option(-transform)

    ## Call the matrix command

    set mat ::tmpmatrix_$_name
    catch "$mat Delete"
    vtkMatrix4x4 $mat

    $mat DeepCopy [$_reg GetMatrix]
    puts "The real mat"
#   0.968893 0.247481 5.18321e-05 -18.2546 
#   -0.247481 0.968893 0.000201605 -25.8306 
#   -3.26387e-07 -0.000208161 1 0.0294268 
#   0 0 0 1
    puts [$this StringMatrix $mat]

#
# General Rot
# 0.979268 0.132425 0.153287 
# 7.96021 -0.153178 0.979264 
# 0.132584 -6.05893 -0.132551 
# -0.153316 0.979247 0.952764 
# 0 0 0 1

# rot z
#
# 0.968908 0.247421 0.000125456 -18.2419 -0.247421 0.968908 0.00015346 -25.8546 -8.35858e-05 -0.000179729 1 -0.0039064 0 0 0 1
#
#    $mat Zero
#    $mat SetElement 0 0 0.9689
#    $mat SetElement 1 1 0.9689
#    $mat SetElement 2 2 1
#    $mat SetElement 3 3 1
#    $mat SetElement 0 1  0.2474
#    $mat SetElement 1 0 -0.2474
#    $mat SetElement 0 3 -18.28
#    $mat SetElement 1 3 -25.872

# rot y
# 0.968786 8.79436e-05 0.2479 29.5348 -5.47718e-05 1 -0.000140708 -0.00208282 -0.2479 0.000122738 0.968786 -31.8745 0 0 0 1
#
#    $mat Zero
#    $mat SetElement 0 0 0.9689
#    $mat SetElement 1 1  1
#    $mat SetElement 2 2  0.9689
#    $mat SetElement 3 3  1
#    $mat SetElement 0 2  0.2479
#    $mat SetElement 2 0 -0.2479
#    $mat SetElement 0 3 29.5348
#    $mat SetElement 2 3 -31.8745

# rotx
#1 -4.74651e-05 -5.65028e-05 14.999 5.99604e-05 0.968959 0.24722 21.9452 4.30146e-05 -0.24722 0.968959 27.2486 0 0 0 1
#    $mat SetElement 0 0 1
#    $mat SetElement 1 1 0.9689
#    $mat SetElement 2 2 0.9689
#    $mat SetElement 3 3  1
#    $mat SetElement 1 2  0.2479
#    $mat SetElement 2 1 -0.2479
#    $mat SetElement 0 3 15
#    $mat SetElement 1 3 27.2486
#    $mat SetElement 2 3 -31.8745


# pieper data
#1 -4.74651e-05 -5.65028e-05 14.999 5.99604e-05 0.968959 0.24722 21.9452 4.30146e-05 -0.24722 0.968959 27.2486 0 0 0 1
#    $mat SetElement 0 0 1
#    $mat SetElement 1 1 0.9689
#    $mat SetElement 2 2 0.9689
#    $mat SetElement 3 3  1
#    $mat SetElement 1 2  0.2479
#    $mat SetElement 2 1 -0.2479
#    $mat SetElement 0 3 15
#    $mat SetElement 1 3 27.2486
#    $mat SetElement 2 3 -31.8745


## Pieper data
# 0.998666 -0.0111754 -0.0504106 -22.8 
# 0.00955995 0.999436 -0.0321874 -11.1 
# 0.0507421 0.0316625 0.99821 -4 
# 0 0 0 1 

    puts "Starting slicer updated matrix"
    puts "The mat"
    puts [$this StringMatrix $mat]
    puts "P1"
    puts [$this StringMatrix [$this getP1]]
    puts "P2"
    puts [$this StringMatrix [$this getP2]]

    set p2mat [$this getP2]
    $p2mat Invert
    $this GetSimilarityMatrix $p2mat $mat [$this getP1]

#    $mat Multiply4x4 $mat [$this getP1] $mat
#    puts "intermediate"
#    puts [$this StringMatrix $mat]

#    set tmpnode ::tmpnode_$_name
#    catch "$tmpnode Delete"
#    vtkMrmlVolumeNode $tmpnode
#    $tmpnode SolveABeqCforB [$this getP2] $mat $mat
#    set tmpmat [$this getP2]
#    $tmpmat Invert
#    $mat Multiply4x4 $tmpmat $mat $mat

    Matrix($t,node) SetMatrix [$this StringMatrix $mat]

    if {$itk_option(-verbose)} {
    set results_mat [$this StringMatrix [$_reg GetMatrix] ]
        puts "resulting mat: $results_mat"
    set tmp_mat [Matrix($t,node) GetMatrix]
        puts "actually set $tmp_mat"
    }

    #$tmpnode Delete
    $mat Delete

    set _mat_m_time [[Matrix($t,node) GetTransform] GetMTime]
}

#-------------------------------------------------------------------------------
# METHOD: set_init_mat
#
# DESCRIPTION: set the matrix from the slicer matrix
#
# Using the logic from update_slicer_mat, M = P2^-1 M'P1
#
#
#-------------------------------------------------------------------------------

itcl::body isregistration::set_init_mat {} {

    #
    # Get the current matrix - if this is the first time through
    # OR someone has edited it since the last iteration
    #

    set t $itk_option(-transform)

    if { $_firsttime == 1 || [[Matrix($t,node) GetTransform] GetMTime] != $_mat_m_time } {
    set mat ::tmpmatrix_$_name
    catch "$mat Delete"
    vtkMatrix4x4 $mat

        $mat DeepCopy [[Matrix($t,node) GetTransform] GetMatrix]
    puts "Initting Matrix"
    puts [$this StringMatrix $mat]
    puts [$this StringMatrix [$this getP1]]
    puts [$this StringMatrix [$this getP2]]

#     $mat Multiply4x4 [$this getP2] $mat $mat
#     puts "multiply 1"
#     puts [$this StringMatrix $mat]
#
#     ## solve the system of equations
#     #set tmpnode ::tmpnode_$_name
#     #catch "$tmpnode Delete"
#     #vtkMrmlVolumeNode $tmpnode
#     #$tmpnode SolveABeqCforB [$this getP2] $_matrix $mat
#     #
#     #$tmpnode Delete
#
#     set tmpmat [$this getP1]
#     $tmpmat Invert
#     $mat Multiply4x4 $mat $tmpmat $_matrix

    set p1mat [$this getP1]
    $p1mat Invert
    $this GetSimilarityMatrix [$this getP2] $mat $p1mat

    $_matrix DeepCopy $mat
    $mat Delete
    puts [$_matrix Determinant]

#     ### hacking to identity
#     $_matrix Identity

#     puts "major hack"
#     $_matrix Identity
#
#    $_matrix Zero
#    $_matrix SetElement 0 0 0.9689
#    $_matrix SetElement 1 1 0.9689
#    $_matrix SetElement 2 2 1
#    $_matrix SetElement 3 3 1
#    $_matrix SetElement 0 1  0.2474
#    $_matrix SetElement 1 0 -0.2474
#    $_matrix SetElement 0 3 -18.28
#    $_matrix SetElement 1 3 -25.872


        $_reg Initialize $_matrix
    
        if {$itk_option(-verbose)} {
            set matstring [Matrix($t,node) GetMatrix]
            puts "input matrix $matstring"
        set matstring [$this StringMatrix $_matrix ]
        puts "transformed input matrix $matstring"
#        set matstring [$this StringMatrix [$_reg GetMatrix ]]
#        puts "transformed input matrix $matstring"
        }
    
    ## keep track of MTime just in case user updates that transform...
    set _mat_m_time [[Matrix($t,node) GetTransform] GetMTime]
    } else {
    puts "Not initting matrix"
    }
}

#-------------------------------------------------------------------------------
# METHOD: GetSimilarityMatrix
#
# DESCRIPTION: Updates a Matrix with a similarity transform
#
# 
# s2 * mat * s1. Result updates mat
#
#-------------------------------------------------------------------------------

itcl::body isregistration::GetSimilarityMatrix { s2 mat s1 } {
    $mat Invert
    $mat Multiply4x4 $mat $s1 $mat
    $mat Multiply4x4 $s2 $mat $mat
}

#-------------------------------------------------------------------------------
# METHOD: getP1
#
# DESCRIPTION: return a matrix for ...
#-------------------------------------------------------------------------------

itcl::body isregistration::getP1 {  } {
    global Matrix

#    return [Volume($Matrix(volume),node) GetPosition]

    set _p1 ::matrixp1_$_name
    catch  "$_p1 Delete"
    vtkMatrix4x4 $_p1

    GetSlicerToItkMatrix Volume($Matrix(volume),node) $_p1
    puts [$this StringMatrix $_p1]
    return $_p1
}


#-------------------------------------------------------------------------------
# METHOD: getP2
#
# DESCRIPTION: return a matrix for ...
#-------------------------------------------------------------------------------

itcl::body isregistration::getP2 {  } {
    global Matrix

# return [Volume($Matrix(refVolume),node) GetPosition]

    set _p2 ::matrixp2_$_name
    catch  "$_p2 Delete"
    vtkMatrix4x4 $_p2


    GetSlicerToItkMatrix Volume($Matrix(refVolume),node) $_p2
    puts [$this StringMatrix $_p2]
    return $_p2
}


#-------------------------------------------------------------------------------
# METHOD: StringMatrix
#
# DESCRIPTION: return a matrix as 16 floats in a string
#-------------------------------------------------------------------------------

itcl::body isregistration::StringMatrix { mat4x4 } {

    set mat ""
    for {set i 0} {$i < 4} {incr i} {
        for {set j 0} {$j < 4} {incr j} {
            set mat "$mat [$mat4x4 GetElement $i $j]"
        }
    }
    return [string trimleft $mat " "]
}


#-------------------------------------------------------------------------------
# METHOD: StringToMatrix
#
# DESCRIPTION: fills in a matrix with a string
#-------------------------------------------------------------------------------

itcl::body isregistration::StringToMatrix { mat4x4 str} {

    for {set i 0} {$i < 4} {incr i} {
        for {set j 0} {$j < 4} {incr j} {
            $mat4x4 SetElement $i $j [lindex $str [expr $i*4+$j]
        }
    }
}



proc isregistration_demo {} {

    catch "destroy .isregistrationdemo"
    toplevel .isregistrationdemo
    wm title .isregistrationdemo "isregistrationdemo"

    pack [isregistration .isregistrationdemo.isr -iterations 1] -fill both -expand true
}
