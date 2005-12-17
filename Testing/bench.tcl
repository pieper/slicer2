
#
# utility code for running benchmarks
#

proc bench_init {} {

    package require vtk

    catch "bench_mt Delete"
    vtkMultiThreader bench_mt

    set ::BENCH(numThreads) [bench_mt GetNumberOfThreads]
    set ::BENCH(benchmarks) [glob -nocomplain $::env(SLICER_HOME)/Testing/Benchmarks/*.tcl]

    puts "will use up to $::BENCH(numThreads) threads"
}


proc bench_run {} {

    bench_init

    foreach benchmark [lsort -dictionary $::BENCH(benchmarks)] {
        set bench [file root [file tail $benchmark]]
        
        for {set memMultiple 1} {$memMultiple < 3} {incr memMultiple} {

            catch "iss Delete"
            vtkImageSinusoidSource iss
            set dim [expr $memMultiple * 200]
            iss SetWholeExtent 0 200 0 200 0 $dim
            [iss GetOutput] Update 
            set id [iss GetOutput]

            for {set nthreads 1} {$nthreads <= $::BENCH(numThreads)} {incr nthreads} {

                bench_mt SetGlobalMaximumNumberOfThreads $nthreads

                puts "running $bench on $nthreads at memory multiple $memMultiple"; update

                source $benchmark
                set ret [catch {time "${bench}_run $id"} res]

                if { $ret } {
                    puts "failed: $res"; update
                    set ::BENCH($bench,$nthreads,$memMultiple) "failed"
                    break
                } else {
                    puts $res; update
                    set ::BENCH($bench,$nthreads,$memMultiple) [expr [lindex $res 0] / 1000000.]
                }

            }
            set percent [expr 100. * $::BENCH($bench,$::BENCH(numThreads),$memMultiple) / (1. * $::BENCH($bench,1,$memMultiple))]
            puts "--> $::BENCH(numThreads) threads is $percent % of the speed of 1 thread"
            puts ""

            update
        }
    }

    parray ::BENCH
}

bench_run
