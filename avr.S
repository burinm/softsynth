; Approximate timing in ms for spin_loop
; cycles:
; ldi - 1
; dec - n
; bne - 2(n-1) + 1 
;
; 16Mhz clock tick= 62.5ns
; t = (3n - 1)(62.5ns) 
; n = t/187.5ns + 1/3

.equ    _1_5us, 8
.equ    _3us,   16
.equ    _5us,   27
.equ    _40us,  213

.macro  spin_loop us
    ldi r18, \us
    dec r18
    brne .-4
.endm


.equ    TCNT1L, 0x84
.equ    TCNT1H, 0x85
.equ    PORTD,  (0x2b - 0x20)

.global DELAY_5
DELAY_5:
        push    r18
        spin_loop(_5us)
        pop     r18
        ret

.global jitter_undo 
jitter_undo:
;        sbi     PORTD,1
        push    r1                                  ;2 cycles
        push    r16                                 ;2 cycles
        push    r17                                 ;2 cycles
        push    r30                                 ;2 cycles
        push    r31                                 ;2 cycles
        eor     r1, r1  ;set this to zero            1 cycle

        lds     r16, TCNT1L                         ;2 cycles
        lds     r17, TCNT1H                         ;2 cycles
                                                    ;--------
                                                    ;15 cycles

        mov     r30, r16
                    ; 76 idle 
                    ; 76  -max - including UART read
        subi    r30, 76                             ;1 cycle 
        brsh    adjust                              ;2 cycles on true
                        ;error - timer is way overdue
timer_error:
        mov r18, r30  ; 1 cycle         4 per loop
diag_loop:
        dec r18       ; 1 cycle
        sbi PORTD,1   ; 2 cycle
        cbi PORTD,1   ; 2 cycle
        brne diag_loop      ; 2 cycles true, 1 false
        rjmp timer_error ; 2 cycles
adjust:
        subi    r16, 78 ;<--don't think this system has jitter  
        brsh    timer_error
        ;sbc     r17, r1 ; really don't need this because the case above
        call    jitter  ;pushes PC+2 on to stack
jitter:
        pop     r31     ; high byte of address      ; 2 bytes, 1 PC
        pop     r30     ; low byte of address       ; 2 bytes, 1 PC 

        add     r30, r16 ;add low byte of timer     ; 2 bytes, 1 PC
        adc     r31, r1                             ; 2 bytes, 1 PC
        adiw    r30, 6                              ; 2 bytes, 1 PC 
        ijmp                                        ; 2 bytes, 1 PC 
                                                    ; -------------
        nop                                  ; <---  12 bytes  6 PC
        nop
        nop         ;2
        nop
        nop
        nop

jitter_2:
        pop    r31
        pop    r30
        pop    r17
        pop    r16
        pop    r1
;        cbi     PORTD,1
        ret
