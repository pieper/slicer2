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
option add *isregistration.target         "" widgetDefault
option add *isregistration.source            "" widgetDefault
option add *isregistration.transform         "" widgetDefault
option add *isregistration.resolution       128 widgetDefault
option add *isregistration.iterations         5 widgetDefault
option add *isregistration.samples           50 widgetDefault
option add *isregistration.learningrate     .01 widgetDefault
option add *isregistration.translatescale    64 widgetDefault
option add *isregistration.target_standarddev 1 widgetDefault
option add *isregistration.source_standarddev 1 widgetDefault


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
        itk_option define -iterations iterations Iterations 5
        itk_option define -samples samples Samples 50
        itk_option define -learningrate learningrate Learningrate .01
        itk_option define -target_standarddev target_stardarddev Target_standarddev 1
        itk_option define -source_standarddev source_stardarddev Source_standarddev 1
        itk_option define -translatescale translatescale Translatescale 64
        itk_option define -verbose verbose Verbose 1

        variable _name ""
        variable _lastmatrix ""

        # widgets
        variable _controls ""
        variable _task ""
        variable _targetvol ""
        variable _sourcevol ""

        # vtk instances
        variable _reg ""
        variable _xform ""
        variable _matrix ""
        variable _targetcast ""
        variable _targetnorm ""
        variable _sourcecast ""
        variable _sourcenorm ""

        method step {} {}
        method matrix {} {}
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

    #
    # make the subwidgets
    # - isvolumes for target and source volumes
    # - control area
    #

    set _targetvol    [isvolume $itk_interior.target]
    set _sourcevol    [isvolume $itk_interior.source]

    #puts $itk_interior.target
    #set _targetvol    [isvolume -volume $itk_option(-target) ]
    #set _sourcevol    [isvolume -volume $itk_option(-source) ]
#
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

    #
    # set up the vtk pipeline
    # - matrix and xform that tie to slicer's matrix
    # - image normalizers to prep volumes
    # - the registration itself
    #

    set _xform ::xform_$_name
    set _matrix ::matrix_$_name
    catch "$_xform Delete"
    catch "$_matrix Delete"
    vtkMatrixToLinearTransform $_xform
    vtkMatrix4x4 $_matrix
    $_matrix Identity
    $_xform SetInput $_matrix

    ##
    ## Cast target image to float and normalize
    ##
    global Matrix

    set _targetcast ::targetcast_$_name
    set _targetnorm ::targetnorm_$_name
    catch "$_targetcast Delete"
    catch "$_targetnorm Delete"

    vtkImageCast $_targetcast
    $_targetcast SetOutputScalarTypeToFloat
    $_targetcast SetInput [$_targetvol imagedata]
    ### big hack by Samson -- turned off
    ### $_targetcast SetInput [Volume($Matrix(refVolume),vol) GetOutput]
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
    ### big hack by Samson -- turned off
    ### $_sourcecast SetInput [Volume($Matrix(volume),vol) GetOutput]
    vtkITKNormalizeImageFilter $_sourcenorm
    $_sourcenorm SetInput [$_sourcecast GetOutput]

    ##
    # Create the MI Registration instance
    ##
    set _reg ::reg_$_name
    catch "$_reg Delete"
    vtkITKMutualInformationTransform $_reg
    $_reg Initialize $_xform

    $_reg SetTargetImage [$_targetnorm GetOutput]
    $_reg SetSourceImage [$_sourcenorm GetOutput]

#    $_reg SetTargetImage [$_targetcast GetOutput]
#    $_reg SetSourceImage [$_sourcecast GetOutput]

    eval itk_initialize $args
}

itcl::body isregistration::destructor {} {
        catch "_reg Delete"
        catch "_xform Delete"
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
    $_sourcevol configure -volume $itk_option(-source)
    $_sourcevol configure -resolution $itk_option(-resolution)
    $_sourcevol configure -orientation coronal ;# TODO extra config due to isvolume bug
    $_sourcevol configure -orientation axial
}

#-------------------------------------------------------------------------------
# OPTION: -resolution
#
# DESCRIPTION: set the resolution of the calculation
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

    # set the default values
    $_reg SetNumberOfIterations $itk_option(-iterations)
    $_reg SetNumberOfSamples $itk_option(-samples)
    $_reg SetLearningRate $itk_option(-learningrate)
    $_reg SetTranslateScale $itk_option(-translatescale)
    $_reg SetSourceStandardDeviation $itk_option(-source_standarddev)
    $_reg SetTargetStandardDeviation $itk_option(-target_standarddev)

    #
    # Get the current matrix - if it's different from the
    # the last matrix we set, copy it in and re-init reg 
    #

    set t $itk_option(-transform)

    ## The first time through, we need to set the matrix
    if { $_lastmatrix != [Matrix($t,node) GetMatrix] } {
        $_matrix DeepCopy [[Matrix($t,node) GetTransform] GetMatrix]
        if {$itk_option(-verbose)} {
            set mat [Matrix($t,node) GetMatrix]
            puts "setting matrix $mat"
        }
        $_matrix Invert
        $_reg Initialize $_xform
    }

    $_reg Modified
    $_reg Update
    if {$itk_option(-verbose)} {
        $_reg Print
        puts "Metric [$_reg GetMetricValue]"
    }

    ## Call the matrix command
    set results_mat [$this matrix]
    Matrix($t,node) SetMatrix $results_mat
    if {$itk_option(-verbose)} {
        puts "the last matrix is $_lastmatrix"
        puts "resulting mat: $results_mat"
        puts [Matrix($t,node) GetMatrix]
    }
    set _lastmatrix $results_mat


    # Update MRML and display
    MainUpdateMRML
    RenderAll
}

#-------------------------------------------------------------------------------
# METHOD: matrix
#
# DESCRIPTION: return the current registration matrix as 16 floats
#-------------------------------------------------------------------------------

itcl::body isregistration::matrix {} {

    set m ::tmpmatrix_$_name
    catch "$m Delete"
    vtkMatrix4x4 $m
    $m DeepCopy [$_reg GetMatrix] 
    $m Invert

    set mat ""
    for {set i 0} {$i < 4} {incr i} {
        for {set j 0} {$j < 4} {incr j} {
            set mat "$mat [$m GetElement $i $j]"
        }
    }
    $m Delete
    return [string trimleft $mat " "]
}


proc isregistration_demo {} {

    catch "destroy .isregistrationdemo"
    toplevel .isregistrationdemo
    wm title .isregistrationdemo "isregistrationdemo"

    pack [isregistration .isregistrationdemo.isr -iterations 1] -fill both -expand true
}
