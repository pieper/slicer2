#!/local/os/bin/tclsh
#
# make slicerbase and all the modules using the same configurations
# - sp 2003-01-25
#

switch $tcl_platform(os) {
    "SunOS" {
        set SLICER_HOME /projects/birn/nicole/slicer2
        set VTK_BINARY_PATH /projects/birn/slicer2/Lib/solaris8/vtk/VTK-build
        set BUILD solaris8
        set VTKSLICERBASE_BUILD_LIB $SLICER_HOME/Base/builds/$BUILD/bin/vtkSlicerBase.so
        set GENERATOR "Unix Makefiles"
        set COMPILER "/usr/bin/g++"
    }
    "Linux" {
        set SLICER_HOME /home/nicole/slicer2
        set VTK_BINARY_PATH /usr/local/include/vtk
        set BUILD redhat7.3
        set VTKSLICERBASE_BUILD_LIB $SLICER_HOME/Base/builds/$BUILD/bin/vtkSlicerBase.so
        set GENERATOR "Unix Makefiles" 
    set COMPILER "g++"
    }
    "Darwin" {
        set SLICER_HOME /Users/pieper/slicer2/latest/slicer2
        set VTK_BINARY_PATH /Users/pieper/downloads/vtk/vtk4.2/VTK-4.2.1-build
        set BUILD Darwin
        set VTKSLICERBASE_BUILD_LIB $SLICER_HOME/Base/builds/$BUILD/bin/vtkSlicerBase.dylib
        set GENERATOR "Unix Makefiles" 
        set COMPILER "c++"
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
        set COMPILER "cl"
    }
}


set TARGETS { Base }

# Add any modules here, as found in the Modules/vtk* directories of SLICER_HOME
regsub -all {\\} $SLICER_HOME / slicer_home
set baseModulePath ${slicer_home}/Modules
set modulePaths ""
catch {set modulePaths [glob $baseModulePath/vtk*]}

foreach dir $modulePaths {
    # get the module name
    if {[regexp "$baseModulePath/(\.\*)" $dir match moduleName] == 1} {
        # if it's not the custom one, append it to the list of targets
        if {[string first Custom $moduleName] == -1} {
            if {[file isdirectory ${baseModulePath}/${moduleName}] == 1} {
                puts "Adding module to target list: ${moduleName}"
                lappend TARGETS Modules/${moduleName}
            }
        }
    }
}
# puts "Using TARGETS = ${TARGETS}"

# use an already built version of vtk
set VTK_ARG1 "-DUSE_BUILT_VTK:BOOL=ON"
set VTK_ARG2 "-DVTK_BINARY_PATH:PATH=$VTK_BINARY_PATH"
switch $tcl_platform(os) {
    "SunOS" {
        # in order to bypass warnings about Source files
        set VTK_ARG3 "-DCMAKE_BACKWARDS_COMPATIBILITY:STRING=1.2"
        # explicitly specify the compiler used to compile the version of vtk that 
        # we link with
        set VTK_ARG4 "-DCMAKE_CXX_COMPILER:STRING=g++"
        set VTK_ARG5 "-DCMAKE_CXX_COMPILER_FULLPATH:FILEPATH=/usr/bin/g++"
    }
    default {
        set VTK_ARG3 ""
        set VTK_ARG4 ""
        set VTK_ARG5 ""
    }
}
# make sure to generate shared libraries
set VTK_ARG6 "-DBUILD_SHARED_LIBS:BOOL=ON"

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
    puts "cmake $SLICER_HOME/$target -G$GENERATOR \
        $VTK_ARG1 $VTK_ARG2 $VTK_ARG3 $VTK_ARG4 $VTK_ARG5 $VTK_ARG6 \
        $SLICER_ARG1 $SLICER_ARG2 $SLICER_ARG3"
    exec cmake $SLICER_HOME/$target -G$GENERATOR \
        $VTK_ARG1 $VTK_ARG2 $VTK_ARG3 $VTK_ARG4 $VTK_ARG5 $VTK_ARG6 \
        $SLICER_ARG1 $SLICER_ARG2 $SLICER_ARG3

    switch $tcl_platform(os) {
        "SunOS" -
        "Darwin" -
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

