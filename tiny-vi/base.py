from pwn import *
context.log_level = "info"

elf = ELF("./bin/tiny_vi")
libc = ELF("/lib/x86_64-linux-gnu/libc.so.6")

p = elf.process(env={"LD_PRELOAD": libc.path})

CTRL_E = b"\x05"
CTRL_N = b"\x0E"
CTRL_O = b"\x0F"
CTRL_X = b"\x18"
CTRL_Y = b"\x19"
CTRL_P = b"\x10"
CTRL_F = b"\x06"
CTRL_G = b"\x07"
CTRL_3 = b"\x1B"
CTRL_5 = b"\x1D"
CTRL_6 = b"\x1E"
CTRL_7 = b"\x1F"

def get_buf():
    buf = []

    for i in range(24):
        p.recvuntil(b"\x1b[K") # clear_line
        buf.append(p.recv(80))

    return buf

def move_cursor(x, y):
    payload = b""
    payload += b"h" * 80
    payload += b"k" * 24
    payload += b"l" * x
    payload += b"j" * y
    return payload

get_buf()

payload = b""
payload += CTRL_O
payload += b"Hello, World!"
payload += CTRL_O
payload += move_cursor(0, 0)
p.send(payload)
get_buf()

p.interactive()
p.close()
