org 0x6000
use16
mov ah,0x0B
int 0x61
mov ah,0x03
mov dx,currentstr
int 0x61
mov ah,0x13
int 0x61
push dx
mov ah,0x24
int 0x61
mov ah,0x0B
int 0x61
mov ah,0x03
mov dx,currentitemstr
int 0x61
pop bx
;dec bx
sub bx,11
mov cx,20
.loop:
pusha
mov dl,[bx]
push dx
mov ah,0x22
int 0x61
mov ah,0x02
mov dl,':'
int 0x21
pop dx
mov ah,0x02
int 0x21
popa
cmp cx,0
jle .quit
mov ah,0x02
mov dl,','
int 0x21
inc bx
loop .loop
.quit:
ret
stackloc:
dw 0x8000
currentstr:
db 'Current stack location : ',0
currentitemstr:
db 'Current stack : ',0
times 512-($-$$) db 0