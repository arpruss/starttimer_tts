import sys

with open(sys.argv[1],"rb") as f:
    data = f.read()
for i in range(len(data)):
    if data[i] != 0x80:
        data = data[i:]
        break
for i in range(len(data)-1,-1,-1):
    if data[i] != 0x80:
        data = data[:i+1]
        break
with open(sys.argv[1],"wb") as f:
    f.write(data)
    
