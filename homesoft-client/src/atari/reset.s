        .export         _reset_screen

        .include        "atari.inc"
        .include        "macros.inc"

.code
.proc _reset_screen

        mva     #$00, GRACTL
        sta           PMBASE
        sta           NOCLIK
        sta           SDMCTL
        sta           PCOLR0
        sta           PCOLR1
        sta           PCOLR2
        sta           PCOLR3
        sta           HPOSP0
        sta           HPOSP1
        sta           HPOSP2
        sta           HPOSP3
        sta           HPOSM0
        sta           HPOSM1
        sta           HPOSM2
        sta           HPOSM3
        sta           SIZEP0
        sta           SIZEP1
        sta           SIZEP2
        sta           SIZEP3
        sta           SIZEM
        tax
        mva     #$22, DMACTL

        ; RESET THE SCREEN by closing and opening E: on IOCB#0
        mva     #$0C,       {ICCOM, x}
        jsr     CIOV

        ldx     #$00
        mva     #$03,       {ICCOM, x}  ; open
        mva     #<dev_name, {ICBAL, x}
        mva     #>dev_name, {ICBAH, x}
        mva     #$0C,       {ICAX1, x}
        jmp     CIOV
        ; implicit rts

dev_name:        .byte "E:", 0

.endproc

