
#
# ./slicer2-linux-x86 --load-dicom /home/pieper/data/1.2.840.113619.2.5.1762874864.1932.1015502256.640.UID/000004.SER/ --script Modules/iSlicer/tcl/evaluation-movies.tcl --exec "eval_movies /var/tmp/facemovies 10 5; exit"

#
#
source $env(SLICER_HOME)/Modules/iSlicer/tcl/isvolume.tcl
source $env(SLICER_HOME)/Modules/iSlicer/tcl/is3d.tcl

proc eval_movies { {dir /tmp} {steps 120} {skip 1} } {
    
    catch "destroy .eval"
    toplevel .eval
    wm title .eval "Evaluation Movies Render Window"
    wm geometry .eval +0+0

    pack [isvolume .eval.isv] -side left
    .eval.isv configure -resolution 256 -volume 1
    .eval.isv configure -orientation coronal
    .eval.isv configure -orientation axial
    pack [is3d .eval.is3d -isvolume .eval.isv -background #000000] -side left

    raise .eval

    eval_3d_movie $dir $steps
    eval_slice_movie $dir axial $skip
    eval_slice_movie $dir sagittal $skip
    eval_slice_movie $dir coronal $skip

    catch "destroy .eval"
}

proc eval_3d_movie { dir steps } {

    #
    # render and encode the face volume
    #

    set delta [expr 360. / $steps]

    set f 0
    for {set l 0} {$l <= 360} { set l [expr $l + $delta] } {
        puts -nonewline "$f..." ; flush stdout
        .eval.is3d configure -longitude $l
        .eval.is3d expose
        update
        .eval.is3d screensave [format /tmp/is%04d.ppm $f]
        incr f
    }

    puts ""
    puts "encoding..."

    set ret [catch "exec /usr/local/bin/ffmpeg -i /tmp/is%04d.ppm -y $dir/face.mpg" res]
    puts $res

    puts "deleting..."
    for {set ff 0} {$ff <= $f} {incr ff} {
        file delete [format /tmp/is%04d.ppm $ff]
    }
    puts "done."

}

proc eval_slice_movie { dir orientation { step 1 } } {

    #
    # render and encode the slice movies
    #

    .eval.isv configure -orientation $orientation
    .eval.isv expose
    update

    for {set s 0} {$s <= 256} { incr s $step } {
        puts -nonewline "$s..." ; flush stdout
        .eval.isv configure -slice $s
        update
        .eval.isv screensave [format /tmp/is%04d.ppm $s]
    }

    puts ""
    puts "encoding..."

    set ret [catch "exec /usr/local/bin/ffmpeg -i /tmp/is%04d.ppm -y $dir/slices-$orientation.mpg" res]
    puts $res

    puts "deleting..."
    for {set ff 0} {$ff <= 256} {incr ff $step} {
        file delete [format /tmp/is%04d.ppm $ff]
    }
    puts "done."
}

