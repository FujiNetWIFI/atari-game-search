        .export _load_app

        .import clear_menu
        .import load_setup
        .import load_init
        .import dosiov
        .import bindcb
        .import stadcb

        .include "atari.inc"

.struct IO_DCB
        .org $0300
        ddevic  .byte
        dunit   .byte
        dcomnd  .byte
        dstats  .byte
        dbuflo  .byte
        dbufhi  .byte
        dtimlo  .byte
        dunuse  .byte
        dbytlo  .byte
        dbythi  .byte
        daux1   .byte
        daux2   .byte
.endstruct

.segment "LOADER"

_load_app:
        JSR     LOAD_READ2
        JSR     LOAD_CHKFF
        ; is this an xex file? C=1 if it is not.
        BCS     R

        JSR     clear_menu
        JSR     load_setup
        INC     BIN_1ST
        ; Process each payload
GETFIL: JSR     LOAD_READ2      ; Get two bytes (binary header)
        BMI     R               ; Exit if EOF hit
        JSR     load_init       ; Set init default
        LDX     #$01
        JSR     LOAD_CHKFF      ; Check if header (and start addr, too)
        JSR     LOAD_STRAD      ; Put start address in
        JSR     LOAD_READ2      ; Get to more butes (end addr)
        JSR     LOAD_ENDAD      ; Put end address in
        JSR     LOAD_BUFLEN     ; Calculate buffer length
        JSR     LOAD_GETDAT     ; Get the data record
        BCC     :+              ; Was EOF detected?

        JSR     JSTART          ; Yes. Go to RUNAD
:       JSR     JINIT           ; Attempt initialization
        JMP     GETFIL          ; Process next payload

JINIT:  JMP     (INITAD)        ; Will either RTS or perform INIT
JSTART: JMP     (RUNAD)         ; Godspeed.
R:      RTS                     ; Stunt-double for (INITAD),(RUNAD)

;---------------------------------------
LOAD_READ2:
;---------------------------------------

    ;---------------------------------------
    ; This is accomplished by abusing the LOAD_GETDAT
    ; routine by stuffing the buffer addr (BAL/H)
    ; into the payload Start/End addrs. We're doing
    ; this in case a payload  header straddles a
    ; cache boundary. LOAD_GETDAT has the logic for
    ; dealing with that.
    ;---------------------------------------
        LDA     #<BAL
        STA     STL         ; Payload start address
        LDA     #>BAL
        STA     STH

        LDA     #<BAH
        STA     ENL         ; Payload end address
        LDA     #>BAH
        STA     ENH

        LDX     #$02
        STX     BLL         ; Payload size (2)
        LDA     #$00
        STA     BLH

        JMP     LOAD_GETDAT ; Read 2 bytes

;---------------------------------------
LOAD_CHKFF:
;---------------------------------------
    ; On 1st pass, check for binary signature (FF FF)
    ; On 2..n passes, Skip FF FF (if found)
    ; and read next 2 bytes
    ;---------------------------------------

        LDA     BAL         ; Is 1st byte FF?
        AND     BAH         ; Is 2nd byte FF?
        CMP     #$FF
        BNE     NOTFF       ; If no, skip down.

    ;---------------------------------------
    ; Here if FF FF tags found.
    ; On 1st pass, we're done.
    ; On 2..n passes, read next 2 bytes and leave.
    ;---------------------------------------
        BIT     BIN_1ST     ; Is this 1st pass? If so, N is set
        BMI     NOTFF_DONE  ; If yes, then we're done here.
        JMP     LOAD_READ2  ;

    ;---------------------------------------
    ; Here if FF FF tags NOT found.
    ; On 1st pass,return failure via C.
    ; On 2..n passes, the 2 bytes = payload start addr.
    ;---------------------------------------
NOTFF:
        BIT     BIN_1ST     ; A still has FF. BIN_1ST = FF on first pass
        BPL     NOTFF_DONE  ; Not 1st pass, ok to proceed.

        SEC     ; Set C=1 for failure
        RTS
NOTFF_DONE:
        CLC     ; Clear C=0 for success
        RTS

;---------------------------------------
LOAD_STRAD:
;---------------------------------------
    ; Save payload start address into STL2/STLH2.
    ; Otherwise it will get clobbered
    ; when reading payload end address.
        LDX     #$00        ; BAL/BAH
        LDY     #$0E        ; STL2/STH2
        JMP     COPY_VAR

;---------------------------------------
LOAD_ENDAD:
;---------------------------------------
    ; Save payload end address
        LDX     #$0E        ; STL2/STH2
        LDY     #$02        ; STL/STH
        JSR     COPY_VAR

        LDX     #$00        ; BAL/BAH
        LDY     #$04        ; ENL/ENH
        JMP     COPY_VAR

;---------------------------------------
LOAD_BUFLEN:
;---------------------------------------
    ; Calculate buffer length (end-start+1)

    ; Calc buffer size Lo
        LDA     ENL
        SEC
        SBC     STL
        STA     BLL     ; Buffer Length Lo

    ; Calc buffer size Hi
        LDA     ENH     ; Calc buffer size Hi
        SBC     STH
        STA     BLH     ; Buffer Length Hi

    ; Add 1
        INC     BLL
        BNE     :+
        INC     BLH
:       RTS


;---------------------------------------
LOAD_GETDAT:
;---------------------------------------
    ; Definitions:
    ; HEAD = requested bytes that will be found in current cache (< 512 bytes)
    ; BODY = contiguous 512 byte sections. BODY = n * 512 bytes
    ; TAIL = any bytes remaining after BODY (< 512 bytes)

        JSR     GETDAT_CHECK_EOF    ; Check EOF before proceeding
        BCC     GETDAT_NEXT1        ; If C is clear there's more data to read
        ; If C is set, then EOF found, so exit
        RTS

    ; Check if bytes requested BL < DVSTAT (bytes waiting in cache)
GETDAT_NEXT1:
        LDA     DVSTAT
        CMP     BLL
        LDA     DVSTAT+1
        SBC     BLH
        BCS     GETDAT_OPT2     ; BL <= BW (bytes waiting)

GETDAT_OPT1:
    ;--------------------------------
    ; Here if bytes requested > bytes
    ; remaining in cache
    ;--------------------------------

    ;-------------------------------
    ; Head = BW (bytes waiting)
    ;-------------------------------
        LDA     DVSTAT
        STA     HEADL
        LDA     DVSTAT+1
        STA     HEADH

    ;-------------------------------
    ; Tail = (BL - HEAD) mod 512
    ;-------------------------------
        SEC
        LDA     BLL
        SBC     HEADL
        AND     #$FF
        STA     TAILL
        LDA     BLH
        SBC     HEADH
        AND     #$01
        STA     TAILH

    ;-----------------------------------
    ; Body = BL - HEAD - TAIL
    ;-----------------------------------
        ; 1. Body = BL - HEAD
        ;-------------------------------
        SEC
        LDA     BLL
        SBC     HEADL
        STA     BODYL
        LDA     BLH
        SBC     HEADH
        STA     BODYH

        ;-------------------------------
        ; 2. Body = Body - HEAD
        ;-------------------------------
        SEC
        LDA     BODYL
        SBC     TAILL
        STA     BODYL
        LDA     BODYH
        SBC     TAILH
        STA     BODYH

        JMP     GETDAT_READ

GETDAT_OPT2:
    ;--------------------------------
    ; Here if bytes requested <= bytes
    ; remaining in cache
    ;--------------------------------
    ; Head = BL, TAIL = BODY = 0
    ;--------------------------------
        LDX     #$0A        ; BLL/BLH
        LDY     #$06        ; HEADL/HEADH
        JSR     COPY_VAR

        LDA     #$00
        STA     TAILL
        STA     TAILH
        STA     BODYL
        STA     BODYH

;---------------------------------------
GETDAT_READ:
;---------------------------------------
    ;---------------------------------------
    ; Read HEAD bytes
    ;---------------------------------------
        LDX     #$06        ; HEADL/HEADH
        LDY     #$0A        ; BLL/BLH
        JSR     COPY_VAR

        JSR     GETDAT_DOSIOV
        BCC     GETDAT_BODY ; Skip ahead if no problems
        RTS                 ; Bail if error

    ;---------------------------------------
    ; Read BODY bytes
    ;---------------------------------------
GETDAT_BODY:
        LDX     BODYH
GETDAT_BODY_LOOP:
        BEQ     GETDAT_TAIL ; Skip if less than a page to read

        LDA     #$00
        STA     BLL         ; Buffer length
        LDA     #$02        ; 512 bytes at a time
        STA     BLH

        TXA                 ; Stash our loop index (X)
        PHA                 ; onto the stack
        JSR     GETDAT_DOSIOV
        BCC     :+          ; Skip ahead if no problems
        PLA                 ; Here if problem. Clean up stack
        TYA                 ; Reset N status flag before returning
        RTS                 ; Bail if error

:       PLA                 ; Retrieve our loop index
        TAX                 ; and xfer it back into X
        DEX                 ; -2 (we pull 0200 bytes at a time)
        DEX                 ;
        BNE     GETDAT_BODY_LOOP

GETDAT_TAIL:
    ;---------------------------------------
    ; Read TAIL bytes
    ;---------------------------------------
        LDX     #$0C        ; TAILL/TAILH
        LDY     #$0A        ; BLL/BLH
        JSR     COPY_VAR

;---------------------------------------
GETDAT_DOSIOV:
;---------------------------------------
    ; Bail if BL = 0
        LDA     BLL
        ORA     BLH
        BEQ     CHECK_EOF_DONE

    ; SIO READ
        LDA     STL
        STA     bindcb + IO_DCB::dbuflo - DCB    ; Start Address Lo
        LDA     STH
        STA     bindcb + IO_DCB::dbufhi - DCB    ; Start Address Hi
        LDA     BLL
        STA     bindcb + IO_DCB::dbytlo - DCB    ; Buffer Size Lo
        STA     bindcb + IO_DCB::daux1 - DCB
        LDA     BLH
        STA     bindcb + IO_DCB::dbythi - DCB    ; Buffer Size Hi
        STA     bindcb + IO_DCB::daux2 - DCB

    ;---------------------------------------
    ; Send Read request to SIO
    ;---------------------------------------
        LDA     #<bindcb
        LDY     #>bindcb
        JSR     dosiov

    ;---------------------------------------
    ; Advance start address by buffer length
    ;---------------------------------------
        CLC
        LDA     STL
        ADC     BLL
        STA     STL

        LDA     STH
        ADC     BLH
        STA     STH

; Check if EOF, and return C=1 if so
GETDAT_CHECK_EOF:
    ; Get status (updates DVSTAT, DSTATS)
        LDA     bindcb + IO_DCB::dunit - DCB
        STA     stadcb + IO_DCB::dunit - DCB
        LDA     #<stadcb
        LDY     #>stadcb
        JSR     dosiov

    ; Return C=1 if DVSTAT == $0000 and DVSTAT+3 == EOF
        LDA     DVSTAT
        BNE     CHECK_EOF_DONE

        LDA     DVSTAT+1
        BNE     CHECK_EOF_DONE

        LDA     #EOFERR         ; 136 defined in atari.inc
        CMP     DVSTAT+3        ; Is it EOF
        BNE     CHECK_EOF_DONE  ; No? Go to success

        ; C = 1
        ; Yes? Return with C set to indicate EOF met
        RTS

CHECK_EOF_DONE:
        CLC        ; Return success
        RTS

COPY_VAR:
        LDA     VARSPACE, X
        STA     VARSPACE, Y
        LDA     VARSPACE + 1, X
        STA     VARSPACE + 1, Y
        RTS

; these have to be part of the LOADER segment
VARSPACE:
BAL:            .res 1  ; 00
BAH:            .res 1  ; 01
STL:            .res 1  ; 02 ; Payload Start address
STH:            .res 1  ; 03
ENL:            .res 1  ; 04 ; Payload End address
ENH:            .res 1  ; 05
HEADL:          .res 1  ; 06 ; Bytes read from existing cache
HEADH:          .res 1  ; 07
BODYL:          .res 1  ; 08 ; Total bytes read in contiguous 512-byte blocks
BODYH:          .res 1  ; 09
BLL:            .res 1  ; 0A ; Payload Buffer Length
BLH:            .res 1  ; 0B
TAILL:          .res 1  ; 0C ; Bytes read from last cache
TAILH:          .res 1  ; 0D
STL2:           .res 1  ; 0E ; Payload Start address (working var)
STH2:           .res 1  ; 0F

BIN_1ST:        .byte $FF    ; Flag for binary loader signature (FF -> 1st pass)
