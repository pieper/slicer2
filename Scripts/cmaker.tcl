
#
# make slicerbase and all the modules using the same configurations
# - sp 2003-01-25
#

switch $tcl_platform(os) {
    "SunOS" {
        # TODO
    }
    "Linux" {
        set SLICER_HOME /home/pieper/slicer2/latest/slicer2
        set VTK_BINARY_PATH /projects/birn/slicer2/Lib/redhat7.3/vtk/VTK-build
        set BUILD redhat7.3
        set VTKSLICERBASE_BUILD_LIB $SLICER_HOME/Base/builds/$BUILD/bin/vtkSlicerBase.so
        set GENERATOR "Unix Makefiles" 
    }
    default {
        # different windows machines say different things, so assume
        # that if it doesn't match above it must be windows
        # (VC7 is Visual C++ 7.0, also known as the .NET version)

        set SLICER_HOME c:/pieper/bwh/slicer2/latest/slicer2
        set VTK_BINARY_PATH c:/downloads/vtk/latest/Win32VC7
        set BUILD Win32VC7
        set VTKSLICERBASE_BUILD_LIB $SLICER_HOME/Base/builds/$BUILD/bin/debug/vtkSlicerBase.lib
        set GENERATOR "Visual Studio 7" 
    }
}



set TARGETS {
        Base
        Modules/vtkFastMarching
        Modules/vtkFluxDiffusion
        Modules/vtkFreeSurferReaders
        Modules/vtkITK
        Modules/vtkSlicerTensors
    }


set VTK_ARG1 "-DUSE_BUILT_VTK:BOOL=ON"
set VTK_ARG2 "-DVTK_BINARY_PATH:PATH=$VTK_BINARY_PATH"
set SLICER_ARG1 "-DVTKSLICERBASE_SOURCE_DIR:PATH=$SLICER_HOME/Base"
set SLICER_ARG2 "-DVTKSLICERBASE_BUILD_DIR:PATH=$SLICER_HOME/Base/builds/$BUILD"
set SLICER_ARG3 "-DVTKSLICERBASE_BUILD_LIB:PATH=$SLICER_HOME/Base/builds/$BUILD/bin/debug/vtkSlicerBase.lib"


foreach target $TARGETS {

    puts "processing $target..."

    set build $SLICER_HOME/$target/builds/$BUILD 
    catch "file mkdir $build"
    cd $build
    puts "enter directory $build..."

    puts "running cmake ..."
    exec cmake $SLICER_HOME/$target -G$GENERATOR \
        $VTK_ARG1 $VTK_ARG2 \
        $SLICER_ARG1 $SLICER_ARG2 $SLICER_ARG3

    switch $tcl_platform(os) {
        "SunOS" -
        "Linux" {
            puts "running: make"
            puts [catch "exec make" res]
            puts $res
        }
        default {
            if { $target == "Base" } {
                set sln VTKSLICER.sln
            } else {
                set sln [string toupper [file tail $target]].sln
            }
            puts "running: devenv $sln /build debug"
            exec devenv $sln /build debug
        }
    }
}


exit

