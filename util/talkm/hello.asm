org 0x8000
use16
mov ah,0x0B
int 0x61
mov dx,showstr
mov ah,0x03
int 0x61
ret
showstr:
db "Well hello to you too sir.",0
times (512)-($-$$) db 0