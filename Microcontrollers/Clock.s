;Author Kostadin Belchev 10358150

; Exercise 5 for COMP227
; The program outputs a timer on the LCD
; The lower board button is pressed to start and continue execution
; The upper board button is pressed to pause or if held for 1 second to reset
;the timer


Reset           NOP
                B initialise
                B svc_procedure       ; jump to the SVC procedure

;= Memory =====================================================================

Port_area_A     EQU     &10000000
Port_area_B     EQU     &10000004
Timer_port      EQU     &10000008

Enable_bus      EQU     0b00000001    ; used to enable the bus
Disable_bus     EQU     0b11111110    ; used to disable the bus

Enable_RS       EQU     0b00000010    ; used to enable RS
Disable_RS      EQU     0b11111101    ; used to disable Rs

Enable_RW       EQU     0b00000100    ; used to enable RW
Disable_RW      EQU     0b11111011    ; used to disable RW

Lower_button    EQU     0b10000000    ; used to check for the lower button
Upper_button    EQU     0b01000000    ; used to check for the upper_button

Screen_idle     EQU     0b10000000    ; used to check if the screen is idle

Max_SVC         EQU     2             ; maximum defined SVC calls (3 in total)
                                      ; starting from 0

DEFS            100      ; Define 100 bytes for the user stack
ALIGN
user_stack

DEFS            100      ; Define 100 bytes for the supervisor stack
ALIGN
supervisor_stack

;= Initialise =================================================================

initialise        ADR  SP, supervisor_stack     ; init. the supervisor stack
                  MOV  R14, #&D0; User mode
                  MSR  SPSR, R14                ; go into user mode
                  ADR  R14, main
                  MOVS PC, R14                  ; start the main

;= SVC procedure ==============================================================

svc_procedure
                STR LR, [SP, #-4]!            ; store the link register
                LDR R14, [LR, #-4]            ; get the type of SVC call
                BIC R14, R14, #&FF000000      ; Clear the bits


                CMP R14, #Max_SVC             ; Check if such call exists
                BHI end
                ADD R14, PC, R14, LSL #2      ; get label from table
                LDR PC, [R14, #0]

jump_table      DEFW read_timer
                DEFW read_port_b              ; jump_table for svc calls
                DEFW print_char


;= Read timer =================================================================

read_timer      PUSH    {R2}                 ; push working register
                MOV     R2, #Timer_port
                LDRB    R6, [R2]             ; load from the timer

                POP     {R2}

                B back_to_user               ; continue program

;= Read port B ================================================================

read_port_b     PUSH {R2}                    ; push working register
                MOV R2, #Port_area_B
                LDRB R7, [R2]                ; load from the board
                POP     {R2}

                B back_to_user
;= Back to user ===============================================================

back_to_user    MOV  R14, #&D0               ; User mode
                MSR  SPSR, R14               ; go back into user mode
                POP     {PC}^                ; continue program

;= Divide By 10 ===============================================================

Div_By_10       MOV  R2, #0                  ; number of 10s counter
                MOV  R9, #0                  ; count the number of digits

sub_loop
                CMP  R4, #10                 ; check if it is lower than 10
                BLT  singleChar              ; if it is then have a single
                                             ; charachter

                SUB  R4, R4, #10             ; if not then subtract 10 of it
                ADD  R2, R2, #1              ; one more 10 to the overall
                B    sub_loop

singleChar      ADD  R9, R9, #1
                CMP  R2, #0                  ; if it is single char and we have
                BEQ  endOfDivision           ; 10s then it is the last char

again           PUSH {R4}                    ; push remainder to the stack
                MOV R4, R2                   ; get the 10s left
                MOV R2, #0                   ; start from the begining
                B   sub_loop

endOfDivision   PUSH {R4}                    ; push the last character
                MOV PC, LR

;= Print Character ===========================================================

print_char
                PUSH {R7, R8, R9, R10}            ; Push working register and

                MOV  R7, #Port_area_A
                MOV  R8, #Port_area_B


                LDRB R9, [R8]
                ORR  R9, R9, #Enable_RW
                AND  R9, R9, #Disable_RS
                STRB R9, [R8]


state_1         ORR  R9, R9, #Enable_bus
                STRB R9, [R8]

                LDRB R10, [R7]                   ; Get data from the port A

                AND R9, R9, #Disable_bus
                STRB R9, [R8]


                TST R10, #Screen_idle            ; check if able to output
                BNE state_1

                AND  R9, R9, #Disable_RW         ; Change the value of R/W to 0
                STRB R9, [R8]

                CMP R2, #0                       ; check if inst is clear
                BNE  print                       ;screen(#0) or print a char(#1)


                ORR  R9, R9, #Enable_RS          ; Change the value of RS to 1
                STRB R9, [R8]

                ADD  R4, R4, #&30                ; We add 30 hex to it to get
                                                 ; the appropriate ASCII char

print           STRB R4, [R7]                    ; Output to port A


continue        ORR R9, R9, #Enable_bus
                STRB R9, [R8]                    ; Enable the bus


                AND R9, R9, #Disable_bus         ; Disable the bus
                STRB R9, [R8]

                POP {R7, R8, R9, R10}

                B   back_to_user

;= Timer ======================================================================

start_timer

                MOV     R3, #1000                ; has a second passed?
                MOV     R1, #0                   ; seconds counter

                SVC     0                        ; read from the timer into R6
                MOV     R5, R6

hz_loop         SVC     1                        ; read from port B
                TST     R7, #Upper_button        ; check for raised pause
                BNE     halt_procedure

                SVC     0                        ; read from timer
                CMP     R5, R6                   ; if we have 1hz difference
                BEQ     hz_loop                  ; 1 ms has passed

                MOV     R5, R6                   ; start the next checks

                SUBS    R3, R3, #1
                BNE     hz_loop                  ; do until 1 sec has passed
                MOV     R3, #1000

                ADD     R1, R1, #1               ; increment seconds
                MOV     R4, R1
                BL      Div_By_10                ; get them into appropriate
                                                 ; into the stack

                MOV     R2, #1                   ; clear the screen

                MOV     R4, #&01
                SVC     2                        ; print_char

                MOV     R2, #0                   ; we want to print chars now

print_all_chars POP     {R4}
                SVC     2                        ; output the digits
                SUBS    R9, R9, #1
                BNE     print_all_chars

                B       hz_loop                  ; continue to the next second

;= Halt =======================================================================

halt_procedure                                ; procedure to stop the counter

                MOV     R8, #1000             ; checking for seconds

                SVC     0                     ; read from timer
                MOV     R5, R6

                                              ; checking for a second
check_for_reset SVC     0
                CMP     R5, R6
                BEQ     check_for_reset
                MOV     R5, R6


                SVC     1
                TST     R7, #Upper_button    ;  Check if the desired effect is
                BEQ     continue_wait        ; 'pause' or reset

                SUBS    R8, R8, #1           ; Reset if upper button has been
                BEQ     Reset                ; pressed for more than 1 second
                B       check_for_reset

continue_wait   SVC     1                    ; read from port B
                TST     R7, #Lower_button    ; check if start button is pressed
                BNE     hz_loop              ; if it is continue
                B       continue_wait

;= Main =======================================================================

main            ADR     SP, user_stack              ;make the user_stack pointer

start_wait      SVC     1                           ; wait for the lower button
                TST     R7, #Lower_button           ; to be pressed to start
                BNE     start_timer
                B       start_wait

end             B       end
