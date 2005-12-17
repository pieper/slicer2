



proc blur_run { {mem 1} } {

    catch "blur_iss Delete"
    catch "blur_igs Delete"

    vtkImageSinusoidSource blur_iss

    set dim [expr $mem * 200]
    blur_iss SetWholeExtent 0 200 0 200 0 $dim

    vtkImageGaussianSmooth blur_igs
    blur_igs SetInput [blur_iss GetOutput]
    [blur_igs GetOutput] SetWholeExtent 0 $dim 0 $dim 0 $dim
    [blur_igs GetOutput] SetUpdateExtentToWholeExtent

    puts "blur updating 200x200x$dim..."; update
    [blur_igs GetOutput] Update
    set memory_used [[blur_igs GetOutput] GetActualMemorySize]
    puts "blur done on $memory_used"; update

    catch "blur_iss Delete"
    catch "blur_igs Delete"

    return 0
}
