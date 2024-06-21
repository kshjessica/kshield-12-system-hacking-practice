from pwn import *
context.log_level = 'info'

elf = ELF('./bin/hash_map')
libc = ELF('/lib/x86_64-linux-gnu/libc.so.6')

p = elf.process(env={'LD_PRELOAD': libc.path})

def send(data):
    p.recvuntil('Command > '.encode())
    p.sendline(data.encode())
    return p.recvline()[9:-1].decode()

"""
create ["object" | "integer_array" | "object_array" | "hash_map"]
object [object_index] ["set"|"get"] ["property"|"item"] [data_index] (value)
hash_map ["set"|"get"] [key] (value)
list
quit
"""

send('create hash_map')
send('hash_map set 0 1337')
print(send('hash_map get 0'))

send('create object_array')  # 0
send('create object')        # 1
send('create integer_array') # 2

send('object 0 set item 0 1')
send('object 0 set item 1 2')

send('object 1 set property 0 1338')
print(send('object 1 get property 0'))

send('object 2 set item 0 1339')
print(send('object 2 get item 0'))

send('list')

p.interactive()
p.close()
