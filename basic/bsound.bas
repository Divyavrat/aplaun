cls
print ""
print "Sound Blaster 16"
print " Init "
gosub clrreg
print "Success."
a = 32
b = 1
gosub setreg
a = 64
b = 16
gosub setreg
a = 96
b = 240
gosub setreg
a = 128
b = 119
gosub setreg
a = 160
b = 152
gosub setreg
a = 35
b = 1
gosub setreg
a = 67
b = 0
gosub setreg
a = 99
b = 240
gosub setreg
a = 131
b = 119
gosub setreg
a = 176
b = 49
gosub setreg
print " Sound ON"
print "Waiting.."
pause 10
a = 176
b = 17
gosub setreg
print " Sound OFF"
gosub clrreg
print "Finished."
end

setreg:
port out 904 a
for i = 1 to 6
port in 904 x
next i
port out 905 b
for i = 1 to 35
port in 904 x
next i
return

clrreg:
for j = 0 to 245
a = j
b = 0
gosub setreg
next j
return
 