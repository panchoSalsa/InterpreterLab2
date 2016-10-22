set 0, read
jumpt 5, D[0] == 0
set write, D[D[0]]+2
set 0, D[0] - 1
jump 1
halt