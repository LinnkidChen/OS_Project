debug = True

if(debug):
    block = 100
else:
    block = 64000

for i in range(0, block):
    str = "DISK/{}.disk".format(i)
    f = open(str, "w")
    f.close()

f = open("DISK/assed.disk", "w")
for i in range(0, 4):
    f.write(str(1))
for i in range(4, block):
    f.write(str(0))
f.close()

f = open("DISK/0.disk", "w")
f.write("root@0@1@0@1000@-1@-1@-1@-1@-1@-1@-1")
f.close()

f = open("DISK/1.disk", "w")
f.write("system@0@1@0@2000@-1@-1@-1@-1@-1@-1@-1#home@0@1@0@3000@-1@-1@-1@-1@-1@-1@-1#")
