package provide tclVectorUtils 1.0

namespace eval tclVectorUtils {
    proc VCross {a b} {
        foreach {a0 a1 a2} $a {}
        foreach {b0 b1 b2} $b {}

        return [list [expr {$a1*$b2 - $a2*$b1}] \
                [expr {$a2*$b0 - $a0*$b2}] \
                [expr {$a0*$b1 - $a1*$b0}]]
    }

    proc VDist {a b} {
        foreach {a0 a1 a2} $a {}
        foreach {b0 b1 b2} $b {}
        return [expr {sqrt(pow($a0-$b0,2) + pow($a1-$b1,2) + pow($a2-$b2,2))}]
    }

    proc VScale {s v} {
        foreach {v0 v1 v2} $v {}
        return [list [expr {$s*$v0}] [expr {$s*$v1}] [expr {$s*$v2}]]
    }

    proc VAdd {a b} {
        foreach {a0 a1 a2} $a {}
        foreach {b0 b1 b2} $b {}

        return [list [expr {$a0+$b0}] [expr {$a1+$b1}] [expr {$a2+$b2}]]
    }

    proc VLen {a} {
        foreach {a0 a1 a2} $a {}

        return [expr {sqrt(pow($a0*$a0,2) + pow($a1*$a1,2) + pow($a2*$a2,2))}]
    }

    proc VSub {a b} {
        foreach {a0 a1 a2} $a {}
        foreach {b0 b1 b2} $b {}

        return [list [expr {$a0-$b0}] [expr {$a1-$b1}] [expr {$a2-$b2}]]
    }

    proc VNorm {a} {
        set len [VLen $a]
        return [VScale [expr {1.0/$len}] $a]
    }

    namespace export VAdd VSub VScale VCross VNorm
}
