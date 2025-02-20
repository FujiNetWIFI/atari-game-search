        .export _bar_show
        .export _bar_y
        .export _bar_clear
        .export _bar_up
        .export _bar_down
        .export _bar_get

        .include "atari.inc"

; TODO: could convert this to a value from CFG, and then update bar-setup-regs.s
BAR_PMBASE = $7c00

.proc _bar_show
    sta     _bar_y

    asl     a
    asl     a
    adc     #$10

    pha
    jsr     _bar_clear
    pla

    tax
    ldy     #$04
    lda     #$FF

:
    sta     BAR_PMBASE + $180, x  ; missiles
    sta     BAR_PMBASE + $200, x  ; p0
    sta     BAR_PMBASE + $280, x  ; p1
    sta     BAR_PMBASE + $300, x  ; p2
    sta     BAR_PMBASE + $380, x  ; p3

    inx
    dey
    bne     :-
    rts

.endproc

.proc _bar_clear
    ldx     #$00
    lda     #$00
loop:
    sta     BAR_PMBASE       , x
    sta     BAR_PMBASE + $100, x
    sta     BAR_PMBASE + $200, x
    sta     BAR_PMBASE + $300, x
    inx
    bne     loop
    rts
.endproc

.proc _bar_up
    dec     _bar_y
    lda     _bar_y
    jsr     _bar_show
    rts
.endproc

.proc _bar_down
    inc     _bar_y
    lda     _bar_y
    jsr     _bar_show
    rts
.endproc

.proc _bar_get
    lda     _bar_y
    rts
.endproc

.segment "DATA"
_bar_y:     .byte 0