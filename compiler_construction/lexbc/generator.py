import sys

from random import randint, random

def gen_ws():
    buf = ""
    ws = randint(0, 20)
    for j in xrange(ws):
        wstype = randint(0, 4)
        if wstype == 0:
            buf += " "
        elif wstype == 1:
            buf += "\t"
        else:
            buf += "\n"
            

    sys.stdout.write(buf)

def gen_expr():
    buf = ""
    num_of_args=2
    while (num_of_args % 2) != 1:
        num_of_args = randint(1, 20)
    for i in xrange(num_of_args):
        if (i % 2) == 0:
            mode = randint(0, 1)
            smode = ["f", "e"][mode]
            gen_ws()
            num = random() * randint(1, 5)
            fmt = "%s%" + smode
            sys.stdout.write(fmt % (buf, num))
            gen_ws()
        else:
            op = randint(0, 2)
            op = [ "+", "-", "*" ][op]
            sys.stdout.write(op)
            gen_ws()
    sys.stdout.write("=")
    gen_ws()

gen_expr()
        
