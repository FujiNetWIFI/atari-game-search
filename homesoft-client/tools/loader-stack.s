        opt h-
        org $0120

RUNAD   =   $02E0       ; RUN ADDRESS
INITAD  =   $02E2       ; INIT ADDRESS
DVSTAT  =   $02EA
DCB     =   $0300       ; BASE
DSTATS  =   $0303       ; STATUS

SIOV    =   $E459        
        
STADCB: .BYTE   $71      ; DDEVIC
        .BYTE   $01         ; DUNIT
        .BYTE   'S'         ; DCOMND
        .BYTE   $40         ; DSTATS
        .BYTE   <DVSTAT     ; DBUFL
        .BYTE   >DVSTAT     ; DBUFH
        .BYTE   $0F         ; DTIMLO
        .BYTE   $00         ; DRESVD
        .BYTE   $04         ; DBYTL
        .BYTE   $00         ; DBYTH
        .BYTE   $00         ; DAUX1
        .BYTE   $00         ; DAUX2

CLODCB .BYTE     $71      ; DDEVIC
        .BYTE    $01         ; DUNIT
        .BYTE    'C'         ; DCOMND
        .BYTE    $00         ; DSTATS
        .BYTE    $00         ; DBUFL
        .BYTE    $00         ; DBUFH
        .BYTE    $0F         ; DTIMLO
        .BYTE    $00         ; DRESVD
        .BYTE    $00         ; DBYTL
        .BYTE    $00         ; DBYTH
        .BYTE    $00         ; DAUX1
        .BYTE    $00         ; DAUX2

BINDCB:
        .BYTE    $71      ; DDEVIC
        .BYTE    $01         ; DUNIT
        .BYTE    'R'         ; DCOMND
        .BYTE    $40         ; DSTATS
        .BYTE    $FF         ; DBUFL
        .BYTE    $FF         ; DBUFH
        .BYTE    $0F         ; DTIMLO
        .BYTE    $00         ; DRESVD
        .BYTE    $FF         ; DBYTL
        .BYTE    $FF         ; DBYTH
        .BYTE    $FF         ; DAUX1
        .BYTE    $FF         ; DAUX2
        
CLEAR_MENU:    
    	lda #0
    	ldx #0
    	ldy #$b8
cloop 	sta $0700,x
    	inx
    	bne cloop
    	inc cloop+2  ; increasing HI-byte of the clearing address
    	dey
    	bne cloop
	RTS

LOAD_SETUP:
        LDA     #$C0
        STA     RUNAD
        LDA     #$E4
        STA     RUNAD+1
        RTS

LOAD_INIT:
        LDA     #$C0
        STA     INITAD
        LDA     #$E4
        STA     INITAD+1
        RTS

DOSIOV:
        STA     DODCBL+1
        STY     DODCBL+2
        LDY     #$0B
DODCBL  LDA     $FFFF,Y
        STA     DCB,Y
        DEY
        BPL     DODCBL

SIOVDST:
        JSR     SIOV
        LDY     DSTATS
        TYA
        RTS

LOAD_CLOSE:
        LDA     #<CLODCB
        LDY     #>CLODCB
        JMP     DOSIOV
