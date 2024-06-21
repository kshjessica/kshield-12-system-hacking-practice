#include "packet.h"

int process_packet(unsigned char *buf, uint32_t len) {
  if (len == 0) {
    fprintf(stderr, "invalid length\n");
    return -1;
  }

  Packet *pkt = (Packet *)buf;
  switch (pkt->call_num) {
  case CALL_NUM_NEW_ACCOUNT: {
    if (len < 3) {
      // 1 for call_num, 1 for type, 1 for name
      fprintf(stderr, "invalid length\n");
      return -1;
    }

    NewAccount *na = &pkt->data.new_account;
    uint8_t account_id = new_account(na->type, na->name);

    printf("%c", account_id);
    return account_id;
  }
  case CALL_NUM_DEL_ACCOUNT: {
    if (len != 2) {
      // 1 for call_num, 1 for account_id
      fprintf(stderr, "invalid packet length\n");
      return -1;
    }

    DelAccount *da = &pkt->data.del_account;
    uint8_t res = del_account(da->account_id);

    printf("%c", res);
    return res;
  }
  case CALL_NUM_FIX_ACCOUNT: {
    if (len < 4) {
      // 1 for call_num, 1 for type, 1 for account_id, 1 for name
      fprintf(stderr, "invalid packet length\n");
      return -1;
    }

    FixAccount *fa = &pkt->data.fix_account;
    uint8_t res = fix_account(fa->type, fa->account_id, fa->name);

    printf("%c", res);
    return res;
  }
  case CALL_NUM_NEW_GROUP: {
    uint8_t group_id = new_group();

    printf("%c", group_id);
    return group_id;
  }
  case CALL_NUM_DEL_GROUP: {
    if (len != 2) {
      // 1 for call_num, 1 for group_id
      fprintf(stderr, "invalid packet length\n");
      return -1;
    }

    DelGroup *dg = &pkt->data.del_group;

    if (dg->group_id >= 0x10) {
      fprintf(stderr, "invalid group id\n");
      return -1;
    }

    Group *g = groups[dg->group_id];
    if (!g) {
      fprintf(stderr, "invalid group id\n");
      return -1;
    }

    uint8_t res = g->vtable->del_group(dg->group_id);
    printf("%c", res);
    return res;
  }
  case CALL_NUM_ADD_GROUP: {
    if (len != 3) {
      // 1 for call_num, 1 for group_id, 1 for account_id
      fprintf(stderr, "invalid packet length\n");
      return -1;
    }

    AddGroup *ag = &pkt->data.add_group;

    if (ag->group_id >= 0x10) {
      fprintf(stderr, "invalid group id\n");
      return -1;
    }

    Group *g = groups[ag->group_id];
    if (!g) {
      fprintf(stderr, "invalid group id\n");
      return -1;
    }

    uint8_t res = g->vtable->add_group(ag->group_id, ag->account_id);
    printf("%c", res);
    return res;
  }
  case CALL_NUM_SUB_GROUP: {
    if (len != 3) {
      // 1 for call_num, 1 for group_id, 1 for account_id
      fprintf(stderr, "invalid packet length\n");
      return -1;
    }

    SubGroup *sg = &pkt->data.sub_group;

    if (sg->group_id >= 0x10) {
      fprintf(stderr, "invalid group id\n");
      return -1;
    }

    Group *g = groups[sg->group_id];
    if (!g) {
      fprintf(stderr, "invalid group id\n");
      return -1;
    }

    uint8_t res = g->vtable->sub_group(sg->group_id, sg->account_id);
    printf("%c", res);
    return res;
  }
  case CALL_NUM_LIST_GROUP: {
    if (len != 2) {
      // 1 for call_num, 1 for group_id
      fprintf(stderr, "invalid packet length\n");
      return -1;
    }

    ListGroup *lg = &pkt->data.list_group;

    if (lg->group_id >= 0x10) {
      fprintf(stderr, "invalid group id\n");
      return -1;
    }

    Group *g = groups[lg->group_id];
    if (!g) {
      fprintf(stderr, "invalid group id\n");
      return -1;
    }

    uint8_t res = g->vtable->list_group(lg->group_id);
    printf("%c", res);
    return res;
  }
  default:
    fprintf(stderr, "invalid call_number\n");
    return -1;
  }

  fprintf(stderr, "unreachable\n");
  return -1;
}
