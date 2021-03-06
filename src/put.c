#include "hog.h"

// <cmd> {<len> <column id>} <types> <#kvs> [{<len> <key>} {<len> <value>}]...
void hog_put(server_t *s, grn_ctx *ctx)
{
    uint32_t len;
    receive(s->socket, &len, sizeof(len));
    len = ntohl(len);
    char *buf = malloc(len);
    receive(s->socket, buf, len);
    grn_obj *col, *table;
    col = grn_ctx_get(ctx, buf, len);
    if(grn_obj_is_table(ctx, col)) table = col;
    else table = grn_column_table(ctx, col);
    // get key and value types
    char types[2];
    receive(s->socket, types, 2);
    // receive keys and values
    uint32_t nkeys;
    receive(s->socket, &nkeys, sizeof(nkeys));
    nkeys = ntohl(nkeys);
    for(uint32_t i = 0; i < nkeys; ++i){
        receive(s->socket, &len, sizeof(len));
        len = ntohl(len);
        buf = realloc(buf, len);
        receive(s->socket, buf, len);
        ntoh_buf(buf, len, types[0]);
        grn_id id = grn_table_add(ctx, table, buf, len, NULL);
        receive(s->socket, &len, sizeof(len));
        len = ntohl(len);
        buf = realloc(buf, len);
        receive(s->socket, buf, len);
        ntoh_buf(buf, len, types[1]);
        grn_obj value;
        GRN_OBJ_INIT(&value, GRN_BULK, 0, types[1]);
        grn_bulk_write(ctx, &value, buf, len);
        grn_obj_set_value(ctx, col, id, &value, GRN_OBJ_SET);
        GRN_OBJ_FIN(ctx, &value);
    }
cleanup:
    free(buf);
}
