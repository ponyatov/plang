# empty program

init:                               # label
            nop
#            jmp     unresolved
            call    forward
            bye
            jmp     init
forward:
            ret
