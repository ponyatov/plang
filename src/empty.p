# empty program

zero:                               # label
            nop
#            jmp     unresolved
            call    forward
            bye
            jmp     zero
forward:
            ret
:init       jmp     zero
