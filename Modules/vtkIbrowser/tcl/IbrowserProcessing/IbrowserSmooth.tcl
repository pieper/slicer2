

proc IbrowserBuildSmoothGUI { f  master} {

    place $f -in $master -relheight 1.0 -relwidth 1.0
    #--- set a global variable for frame
    set ::Ibrowser(fProcessSmooth) $f
}
