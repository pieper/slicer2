
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

# TODO : 
    - flesh out control panel
    - pre-programmed sequence for multires registration
}

#
#########################################################

#
# Default resources
# - sets the default colors for the widget components
#
option add *isregistration.reference "" widgetDefault
option add *isregistration.moving "" widgetDefault
option add *isregistration.transform "" widgetDefault
option add *isregistration.resolution 128 widgetDefault
option add *isregistration.iterations 5 widgetDefault
option add *isregistration.samples 50 widgetDefault
option add *isregistration.learningrate .01 widgetDefault
option add *isregistration.translatescale 64 widgetDefault

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
        itk_option define -reference reference Reference {}
        itk_option define -moving moving Moving {}
        itk_option define -transform transform Transform {}
        itk_option define -resolution resolution Resolution 128
        itk_option define -iterations iterations Iterations 5
        itk_option define -samples samples Samples 50
        itk_option define -learningrate learningrate Learningrate .01
        itk_option define -translatescale translatescale Translatescale 64

        variable _name ""
        variable _lastmatrix ""

        # widgets
        variable _controls ""
        variable _task ""
        variable _referencevol ""
        variable _movingvol ""

        # vtk instances
        variable _reg ""
        variable _xform ""
        variable _matrix ""
        variable _refcast ""
        variable _refnorm ""
        variable _movcast ""
        variable _movnorm ""

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
    # - isvolumes for reference and moving volumes
    # - control area
    #

    set _referencevol [isvolume $itk_interior.ref]
    set _movingvol [isvolume $itk_interior.mov]

    pack $_referencevol -fill both -expand true
    pack $_movingvol -fill both -expand true

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

    set _refcast ::refcast_$_name
    set _refnorm ::refnorm_$_name
    catch "$_refcast Delete"
    catch "$_refnorm Delete"
    vtkImageCast $_refcast
    $_refcast SetOutputScalarTypeToFloat
    $_refcast SetInput [$_referencevol imagedata]
    vtkITKNormalizeImageFilter $_refnorm
    $_refnorm SetInput [$_refcast GetOutput]

    set _movcast ::movcast_$_name
    set _movnorm ::movnorm_$_name
    catch "$_movcast Delete"
    catch "$_movnorm Delete"
    vtkImageCast $_movcast
    $_movcast SetOutputScalarTypeToFloat
    $_movcast SetInput [$_movingvol imagedata]
    vtkITKNormalizeImageFilter $_movnorm
    $_movnorm SetInput [$_movcast GetOutput]

    # Create the MI Registration instance
    set _reg ::reg_$_name
    catch "$_reg Delete"
    vtkITKMutualInformationTransform $_reg
    $_reg Initialize $_xform

    $_reg SetTargetImage [$_refnorm GetOutput]
    $_reg SetSourceImage [$_movnorm GetOutput]

    eval itk_initialize $args
}


itcl::body isregistration::destructor {} {
        catch "_reg Delete"
        catch "_xform Delete"
        catch "_matrix Delete"
        catch "_refcast Delete"
        catch "_refnorm Delete"
        catch "_movcast Delete"
        catch "_movnorm Delete"
}


#-------------------------------------------------------------------------------
# OPTION: -reference
#
# DESCRIPTION: the stationary reference volume 
# - name of a slicer volume
#-------------------------------------------------------------------------------
itcl::configbody isregistration::reference {

    # TODO - this should be handled by passing the option through using the 
    # itk hide and related stuff

    if {$itk_option(-reference) == ""} {
        return
    }
    $_referencevol configure -volume $itk_option(-reference)
    $_referencevol configure -resolution $itk_option(-resolution)
    $_referencevol configure -orientation coronal ;# TODO extra config due to isvolume bug
    $_referencevol configure -orientation axial
}

#-------------------------------------------------------------------------------
# OPTION: -moving
#
# DESCRIPTION: the moving volume that the matrix applies to
# - name of a slicer volume
#-------------------------------------------------------------------------------
itcl::configbody isregistration::moving {

    # TODO - this should be handled by passing the option through using the 
    # itk hide and related stuff

    if {$itk_option(-moving) == ""} {
        return
    }
    $_movingvol configure -volume $itk_option(-moving)
    $_movingvol configure -resolution $itk_option(-resolution)
    $_movingvol configure -orientation coronal ;# TODO extra config due to isvolume bug
    $_movingvol configure -orientation axial
}

#-------------------------------------------------------------------------------
# OPTION: -resolution
#
# DESCRIPTION: set the resolution of the calculation
#-------------------------------------------------------------------------------
itcl::configbody isregistration::resolution {

    if {$itk_option(-moving) != ""} {
        $_movingvol configure -resolution $itk_option(-resolution)
        $_movingvol configure -orientation coronal ;# TODO extra config due to isvolume bug
        $_movingvol configure -orientation axial
    }
    if {$itk_option(-reference) != ""} {
        $_referencevol configure -resolution $itk_option(-resolution)
        $_referencevol configure -orientation coronal ;# TODO extra config due to isvolume bug
        $_referencevol configure -orientation axial
    }
}

#-------------------------------------------------------------------------------
# METHOD: step
#
# DESCRIPTION: run an interation of the registration
#-------------------------------------------------------------------------------

itcl::body isregistration::step {} {
    global Matrix

    # set the default values
    $_reg SetNumberOfIterations $itk_option(-iterations)
    $_reg SetNumberOfSamples $itk_option(-samples)
    $_reg SetLearningRate $itk_option(-learningrate)
    $_reg SetTranslateScale $itk_option(-translatescale)

    #
    # Get the current matrix - if it's different from the
    # the last matrix we set, copy it in and re-init reg 
    #
    set t $itk_option(-transform)
    if { $_lastmatrix != [Matrix($t,node) GetMatrix] } {
        puts "setting matrix"
        $_matrix DeepCopy [[Matrix($t,node) GetTransform] GetMatrix]
        $_matrix Invert
        $_reg Initialize $_xform
    }

    $_reg Modified
    $_reg Update
    puts "[$_reg GetMetricValue]"

    Matrix($t,node) SetMatrix [$this matrix]
    set _lastmatrix [$this matrix]

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
    return [string trimleft $mat " "]
    $m Delete
}



proc isregistration_demo {} {

    catch "destroy .isregistrationdemo"
    toplevel .isregistrationdemo
    wm title .isregistrationdemo "isregistrationdemo"

    pack [isregistration .isregistrationdemo.isr -iterations 1] -fill both -expand true
}
