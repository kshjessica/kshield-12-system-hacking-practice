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


new_account(0, b"AAAA")

new_group()

add_group(0, 0)

print(list_group(0))

sub_group(0, 0)

del_group(0)

del_account(0)

p.interactive()
p.close()
