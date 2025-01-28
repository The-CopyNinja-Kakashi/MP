%macro io 4
    ; Macro for system call (syscall)
    ; Arguments:
    ; %1 - syscall number (e.g., 1 for write, 0 for read)
    ; %2 - file descriptor (e.g., 1 for stdout, 0 for stdin)
    ; %3 - buffer (memory address of data to read/write)
    ; %4 - length (number of bytes to read/write)
    mov rax, %1    ; Set syscall number
    mov rdi, %2    ; Set file descriptor
    mov rsi, %3    ; Set buffer address
    mov rdx, %4    ; Set length
    syscall         ; Perform syscall
%endmacro

%macro exit 0
    ; Macro to exit the program
    mov rax, 60    ; Exit syscall number
    mov rdi, 0     ; Exit code (0 for success)
    syscall         ; Perform syscall
%endmacro

section .data
    ; Data section (static strings and constants)
    msg1 db "Write an x86/64 ALP to accept 5 hexadecimal numbers from user and store them in an array and display the accepted numbers", 10, \
            'Name - Abhishek bachchis', 10, \
            'Roll no - 7205', 10 ; Program description
    msg1len equ $-msg1               ; Length of msg1

    msg2 db "Enter 5 64bit hexadecimal numbers (0-9,A-F only): ", 10
    msg2len equ $-msg2               ; Length of msg2

    msg3 db "5 64bit hexadecimal numbers are: ", 10
    msg3len equ $-msg3               ; Length of msg3

    newline db 10                    ; Newline character

section .bss
    ; Uninitialized data section
    asciinum resb 17                 ; Buffer for ASCII input (16 characters + null terminator)
    hexnum resq 5                    ; Array to store 5 64-bit numbers

section .text
    global _start
_start:
    ; Display the program description and prompt
    io 1, 1, msg1, msg1len           ; Write msg1 to stdout
    io 1, 1, msg2, msg2len           ; Write msg2 to stdout

    ; Input loop: Read 5 hexadecimal numbers
    mov rcx, 5                       ; Loop counter (5 numbers to input)
    mov rsi, hexnum                  ; Pointer to the hexnum array
next1:
    push rsi                         ; Save current hexnum pointer
    push rcx                         ; Save current loop counter
    io 0, 0, asciinum, 17            ; Read input from stdin into asciinum buffer
    call ascii_hex64                 ; Convert ASCII input to 64-bit hexadecimal number
    pop rcx                          ; Restore loop counter
    pop rsi                          ; Restore hexnum pointer
    mov [rsi], rbx                   ; Store converted number in hexnum array
    add rsi, 8                       ; Move to the next element in hexnum
    loop next1                       ; Repeat until 5 numbers are read

    ; Display the prompt for output
    io 1, 1, msg3, msg3len           ; Write msg3 to stdout

    ; Output loop: Display 5 hexadecimal numbers
    mov rsi, hexnum                  ; Pointer to the hexnum array
    mov rcx, 5                       ; Loop counter (5 numbers to display)
next2:
    push rsi                         ; Save current hexnum pointer
    push rcx                         ; Save current loop counter
    mov rbx, [rsi]                   ; Load the number from hexnum array
    call hex_ascii64                 ; Convert 64-bit number to ASCII
    pop rcx                          ; Restore loop counter
    pop rsi                          ; Restore hexnum pointer
    add rsi, 8                       ; Move to the next element in hexnum
    loop next2                       ; Repeat until 5 numbers are displayed

    ; Exit the program
    exit

ascii_hex64:
    ; Convert a 16-character hexadecimal string to a 64-bit number
    ; Input: asciinum buffer (rsi points to the string)
    ; Output: rbx contains the 64-bit number
    mov rsi, asciinum                ; Point to the input buffer
    mov rbx, 0                       ; Initialize result to 0
    mov rcx, 16                      ; Loop counter (16 characters)
next3:
    rol rbx, 4                       ; Shift existing value left by 4 bits
    mov al, [rsi]                    ; Load the current character
    cmp al, 39h                      ; Check if character is '9' or below
    jbe sub30h                       ; If yes, handle as numeric digit
    sub al, 7h                       ; Adjust for A-F characters
sub30h:
    sub al, 30h                      ; Convert ASCII to numerical value
    add bl, al                       ; Add to result
    inc rsi                          ; Move to the next character
    loop next3                       ; Repeat for all 16 characters
    ret                              ; Return to caller

hex_ascii64:
    ; Convert a 64-bit number to a 16-character hexadecimal string
    ; Input: rbx contains the 64-bit number
    ; Output: asciinum buffer (rsi points to the output string)
    mov rsi, asciinum                ; Point to the output buffer
    mov rcx, 16                      ; Loop counter (16 characters)
next4:
    rol rbx, 4                       ; Rotate value left by 4 bits
    mov al, bl                       ; Extract the lowest nibble
    and al, 0fh                      ; Mask to get the last 4 bits
    cmp al, 9                        ; Check if value is 9 or below
    jbe add30h                       ; If yes, handle as numeric digit
    add al, 7h                       ; Adjust for A-F characters
add30h:
    add al, 30h                      ; Convert to ASCII character
    mov [rsi], al                    ; Store character in output buffer
    inc rsi                          ; Move to the next position
    loop next4                       ; Repeat for all 16 characters
    io 1, 1, asciinum, 16            ; Write the hexadecimal string to stdout
    io 1, 1, newline, 1              ; Write a newline character to stdout
    ret                              ; Return to caller
