        ;; DLI

        .include "atari.inc"

        .export _dli, _dli2

_dli:   PHA
        STA WSYNC
        LDA #$B4
        STA COLBK
        LDA #<_dli2
	STA VDSLST 
        LDA #>_dli2
        STA VDSLST+1
        PLA
        RTI

_dli2:	PHA
	LDA #$0
	STA COLBK
        LDA #<_dli
        STA VDSLST
        LDA #>_dli
        STA VDSLST+1
	PLA
	RTI
