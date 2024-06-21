#include "group.h"

Group *groups[0x10];

static GroupVTable group_vtable = {
    .new_group = new_group,
    .del_group = del_group,
    .add_group = add_group,
    .sub_group = sub_group,
    .list_group = list_group,
};

uint8_t new_group() {
  for (int i = 0; i < 0x10; i++) {
    if (groups[i] == NULL) {
      Group *group = (Group *)mm_alloc(sizeof(Group));
      if (group == NULL) {
        fprintf(stderr, "failed to allocate memory\n");
        return -1;
      }

      group->count = 0;
      group->members = (Account **)mm_alloc(sizeof(Account *) * 0x10);
      if (group->members == NULL) {
        fprintf(stderr, "failed to allocate memory\n");
        return -1;
      }
      memset(group->members, 0x00, sizeof(Account *) * 0x10);
      group->vtable = &group_vtable;

      groups[i] = group;
      return i;
    }
  }

  fprintf(stderr, "no more group\n");
  return -1;
}

int del_group(uint8_t group_id) {
  if (group_id >= 0x10) {
    fprintf(stderr, "invalid group id\n");
    return -1;
  }

  Group *g = groups[group_id];
  if (!g) {
    fprintf(stderr, "invalid group id\n");
    return -1;
  }

  if (g->count != 0) {
    fprintf(stderr, "group is not empty\n");
    return -1;
  }

  mm_free(g->members);
  mm_free(g);
  groups[group_id] = NULL;

  return 0;
}

uint8_t add_group(uint8_t group_id, uint8_t account_id) {
  if (group_id >= 0x10) {
    fprintf(stderr, "invalid group id\n");
    return -1;
  }

  if (account_id >= 0x10) {
    fprintf(stderr, "invalid account id\n");
    return -1;
  }

  Group *g = groups[group_id];
  if (!g) {
    fprintf(stderr, "invalid group id\n");
    return -1;
  }

  if (g->count >= 0x10) {
    fprintf(stderr, "group is full\n");
    return -1;
  }

  Account *ac = accounts[account_id];
  if (!ac) {
    fprintf(stderr, "invalid account id\n");
    return -1;
  }

  for (int i = 0; i < 0x10; i++) {
    if (g->members[i] == ac) {
      fprintf(stderr, "account is already in the group\n");
      return -1;
    }
  }

  for (int i = 0; i < 0x10; i++) {
    if (g->members[i] == NULL) {
      g->members[i] = ac;
      g->count++;
      add_ref(account_id);
      return 0;
    }
  }

  return 0;
}

uint8_t sub_group(uint8_t group_id, uint8_t account_id) {
  if (group_id >= 0x10) {
    fprintf(stderr, "invalid group id\n");
    return -1;
  }

  if (account_id >= 0x10) {
    fprintf(stderr, "invalid account id\n");
    return -1;
  }

  Group *g = groups[group_id];
  if (!g) {
    fprintf(stderr, "invalid group id\n");
    return -1;
  }

  if (g->count == 0) {
    fprintf(stderr, "group is empty\n");
    return -1;
  }

  Account *ac = accounts[account_id];
  if (!ac) {
    fprintf(stderr, "invalid account id\n");
    return -1;
  }

  for (int i = 0; i < 0x10; i++) {
    if (g->members[i] == ac) {
      g->members[i] = NULL;
      g->count--;
      release(account_id);
      return 0;
    }
  }

  fprintf(stderr, "account is not in the group\n");

  return -1;
}

int list_group(uint8_t group_id) {
  if (group_id >= 0x10) {
    fprintf(stderr, "invalid group id\n");
    return -1;
  }

  Group *g = groups[group_id];
  if (!g) {
    fprintf(stderr, "invalid group id\n");
    return -1;
  }

  if (g->count == 0) {
    fprintf(stderr, "group is empty\n");
    return -1;
  }

  for (int i = 0; i < 0x10; i++) {
    if (g->members[i]) {
      _show_account(g->members[i]);
    }
  }

  return 0;
}
