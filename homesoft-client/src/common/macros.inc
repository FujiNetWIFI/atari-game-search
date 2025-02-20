; various macros to make asm easier to read

.macro  mwa    arg1, arg2
    .if (.match (.left (1, {arg1}), #))
        ; arg1 is immediate mode
        lda     #<(.right (.tcount ({arg1})-1, {arg1}))
        sta     arg2
        lda     #>(.right (.tcount ({arg1})-1, {arg1}))
        ; have to use "1+..." to stop assembler thinking it's an addressing mode
        sta     1+(arg2)
    .else
        lda     arg1
        sta     arg2
        lda     1+(arg1)
        sta     1+(arg2)
    .endif
.endmacro

.macro  mva     arg1, arg2
    ; this could be immediate or absolute, doesn't matter.
    ; commas are arg separators, so `mva #$00, {($80),y}` is how to use this with addressing modes
    lda     arg1
    sta     arg2
.endmacro

