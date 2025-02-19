        opt h-
        org $03FD

RUNAD   =   $02E0       ; RUN ADDRESS
INITAD  =   $02E2       ; INIT ADDRESS
MEMLO   =   $02E7       ; MEM LO
DVSTAT  =   $02EA       ; 4 BYTE STATS

DCB     =   $0300       ; BASE
DDEVIC  =   DCB         ; DEVICE #
DUNIT   =   DCB+1       ; UNIT #
DCOMND  =   DCB+2       ; COMMAND
DSTATS  =   DCB+3       ; STATUS/DIR
DBUFL   =   DCB+4       ; BUF ADR L
DBUFH   =   DCB+5       ; BUF ADR H
DTIMLO  =   DCB+6       ; TIMEOUT (S)
DRSVD   =   DCB+7       ; NOT USED
DBYTL   =   DCB+8       ; BUF LEN L
DBYTH   =   DCB+9       ; BUF LEN H
DAUXL   =   DCB+10      ; AUX BYTE L
DAUXH   =   DCB+11      ; AUX BYTE H

SIOV    =   $E459               ; SIO VECTOR
EOF     =   136
        ;; Put DCBs in the stack.
STADCB  =   $0120
CLODCB  =   $012C
BINDCB  =   $0138        
        ;; Put utility functions in stack, check loader-stack.lst.
CLEAR_MENU = $0144
LOAD_SETUP = $0157
LOAD_INIT  = $0162
DOSIOV     = $016D
LOAD_CLOSE = $0186        

        .ENUM   DCB_IDX
                                ;---------------
        DDEVIC              ; 0
        DUNIT               ; 1
        DCOMND              ; 2
        DSTATS              ; 3
        DBUFL               ; 4
        DBUFH               ; 5
        DTIMLO              ; 6
        DRESVD              ; 7
        DBYTL               ; 8
        DBYTH               ; 9
        DAUX1               ; 10
        DAUX2               ; 11
        .ENDE

        .MACRO DCBC
        .LOCAL
        LDY     #$0C
        ?DCBL   LDA     %%1,Y
        STA     DCB,Y
        DEY
        BPL     ?DCBL
        .ENDL
        .ENDM

DO_LOAD:
        JSR     CLEAR_MENU
        JSR     LOAD_SETUP
        LDA     #$FF
        STA     BIN_1ST
        JSR     LOAD_READ2
        JSR     LOAD_CHKFF
        CPY     #$01
        BNE     R

        INC     BIN_1ST
    ; Process each payload
GETFIL: JSR     LOAD_READ2      ; Get two bytes (binary header)
        BMI     R               ; Exit if EOF hit
        JSR     LOAD_INIT       ; Set init default
        LDX     #$01
        JSR     LOAD_CHKFF      ; Check if header (and start addr, too)
        JSR     LOAD_STRAD      ; Put start address in
        JSR     LOAD_READ2      ; Get to more butes (end addr)
        JSR     LOAD_ENDAD      ; Put end address in
        JSR     LOAD_BUFLEN     ; Calculate buffer length
        JSR     LOAD_GETDAT     ; Get the data record
        BPL     @+              ; Was EOF detected?
        JSR     JSTART          ; Yes. Go to RUNAD
@:      JSR     JINIT           ; Attempt initialization
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

        LDA     #$FF
        CMP     BAL         ; Is 1st byte FF?
        BNE     NOTFF       ; If no, skip down.
        CMP     BAH         ; Is 2nd byte FF?
        BNE     NOTFF       ; If no, skip down.

    ;---------------------------------------
    ; Here if FF FF tags found.
    ; On 1st pass, we're done.
    ; On 2..n passes, read next 2 bytes and leave.
    ;---------------------------------------
        CMP     BIN_1ST     ; Is this 1st pass?
        BEQ     NOTFF_DONE  ; If yes, then we're done here.
        JMP     LOAD_READ2  ;

    ;---------------------------------------
    ; Here if FF FF tags NOT found.
    ; On 1st pass, print error.
    ; On 2..n passes, the 2 bytes = payload start addr.
    ;---------------------------------------
NOTFF:  LDY     #$01        ; Preload success return code
        CMP     BIN_1ST     ; A still has FF. BIN_1ST = FF on first pass
        BNE     NOTFF_DONE  ; Not 1st pass, exit with success.

        LDY     #$FF        ; Return failure
NOTFF_DONE:
        RTS

;---------------------------------------
LOAD_STRAD:
;---------------------------------------
    ; Save payload start address into STL2/STLH2.
    ; Otherwise it will get clobbered
    ; when reading payload end address.
        LDA     BAL
        STA     STL2
        LDA     BAH
        STA     STH2
        RTS

;---------------------------------------
LOAD_ENDAD:
;---------------------------------------
    ; Save payload end address
        LDA     STL2
        STA     STL
        LDA     STH2
        STA     STH

        LDA     BAL
        STA     ENL
        LDA     BAH
        STA     ENH
        RTS

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
        CLC
        LDA     BLL
        ADC     #$01
        STA     BLL

        LDA     BLH
        ADC     #$00    ; Take care of any carry
        STA     BLH

        RTS

;---------------------------------------
LOAD_GETDAT:
;---------------------------------------
    ; Definitions:
    ; HEAD = requested bytes that will be found in current cache (< 512 bytes)
    ; BODY = contiguous 512 byte sections. BODY = n * 512 bytes)
    ; TAIL = any bytes remaining after BODY (< 512 bytes)

        JSR     GETDAT_CHECK_EOF    ; Check EOF before proceeding
        BPL     GETDAT_NEXT1        ; If true, then EOF found. Exit
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
        LDA     BLL
        STA     HEADL
        LDA     BLH
        STA     HEADH
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
        LDA     HEADL
        STA     BLL
        LDA     HEADH
        STA     BLH
        JSR     GETDAT_DOSIOV
        BPL     GETDAT_BODY ; Skip ahead if no problems
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
        BPL     @+          ; Skip ahead if no problems
        PLA                 ; Here if problem. Clean up stack
        TYA                 ; Reset N status flag before returning
        RTS                 ; Bail if error

@:      PLA                 ; Retrieve our loop index
        TAX                 ; and xfer it back into X
        DEX                 ; -2 (we pull 0200 bytes at a time)
        DEX                 ;
        BNE     GETDAT_BODY_LOOP

GETDAT_TAIL:
    ;---------------------------------------
    ; Read TAIL bytes
    ;---------------------------------------
        LDA     TAILL
        STA     BLL
        LDA     TAILH
        STA     BLH

;---------------------------------------
GETDAT_DOSIOV:
;---------------------------------------
    ; Bail if BL = 0
        LDA     BLL
        BNE     @+
        LDA     BLH
        BEQ     CHECK_EOF_DONE

@:
    ; SIO READ
        LDA     STL
        STA     BINDCB+DCB_IDX.DBUFL    ; Start Address Lo
        LDA     STH
        STA     BINDCB+DCB_IDX.DBUFH    ; Start Address Hi
        LDA     BLL
        STA     BINDCB+DCB_IDX.DBYTL    ; Buffer Size Lo
        STA     BINDCB+DCB_IDX.DAUX1
        LDA     BLH
        STA     BINDCB+DCB_IDX.DBYTH    ; Buffer Size Hi
        STA     BINDCB+DCB_IDX.DAUX2

    ;---------------------------------------
    ; Send Read request to SIO
    ;---------------------------------------
        LDA     #<BINDCB
        LDY     #>BINDCB
        JSR     DOSIOV

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

GETDAT_CHECK_EOF:
    ; Get status (updates DVSTAT, DSTATS)
        LDA     BINDCB+DCB_IDX.DUNIT
        STA     STADCB+DCB_IDX.DUNIT
        LDA     #<STADCB
        LDY     #>STADCB
        JSR     DOSIOV

    ; Return -1 if DVSTAT == $0000 and DVSTAT+3 == EOF
        LDA     DVSTAT
        BNE     CHECK_EOF_DONE

        LDA     DVSTAT+1
        BNE     CHECK_EOF_DONE

        LDA     #EOF
        CMP     DVSTAT+3        ; Is it EOF
        BNE     CHECK_EOF_DONE  ; No? Go to success
        LDY     #$FF            ; Yes? Return -1
        RTS

CHECK_EOF_DONE:
        LDY     #$01        ; Return success
        RTS

                                ; Binary loader working variables
BAL     .ds 1
BAH     .ds 1    ;
STL     .ds 1      ; Payload Start address
STH     .ds 1
ENL     .ds 1    ; Payload End address
ENH     .ds 1
HEADL   .ds 1    ; Bytes read from existing cache
HEADH   .ds 1
BODYL   .ds 1    ; Total bytes read in contiguous 512-byte blocks
BODYH   .ds 1
BLL     .ds 1    ; Payload Buffer Length
BLH     .ds 1
TAILL   .ds 1   ; Bytes read from last cache
TAILH   .ds 1
BODYSZL .ds 1   ; # Bytes to read at a time in Body
BODYSZH .ds 1
STL2    .ds 1   ; Payload Start address (working var)
STH2    .ds 1
BIN_1ST .ds 1   ; Flag for binary loader signature (FF -> 1st pass)
        
