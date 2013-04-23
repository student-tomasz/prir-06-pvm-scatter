#include <stdlib.h>
#include <stdio.h>
#include "pvm3.h"

#include "communication.h"
#include "vector.h"

extern const int v_len;
extern const double v[];

void exit_with_msg(const char *msg, const int id, const int tid);

int main(int argc, char *argv[])
{
    int id = 0;
    int tid = pvm_mytid();
    int barrier_info, leave_info;

    id = pvm_joingroup(GRP_NAME);
    if (id == GRP_ROOT_ID) exit_with_msg("SLV%d(%d) failed at pvm_joingroup\n", id, tid);
    barrier_info = pvm_barrier(GRP_NAME, GRP_LEN);
    if (barrier_info < 0) exit_with_msg("SLV%d(%d) failed at initial pvm_barrier\n", id, tid);

    int u_len = v_len / GRP_LEN;
    double *u = malloc(sizeof(*u) * u_len);
    if (u == NULL) exit_with_msg("SLV%d(%d) failed at malloc\n", id, tid);
    pvm_scatter((void *) u, (void *) v, u_len, PVM_DOUBLE, GRP_SCT_MSG_TAG, GRP_NAME, GRP_ROOT_ID);
    double partial_rslt = sum_u(u, u_len);
    // printf("** SLV%d(%d) partial result = %lg\n", id, tid, partial_rslt);
    pvm_reduce(PvmSum, &partial_rslt, 1, PVM_DOUBLE, GRP_RDC_MSG_TAG, GRP_NAME, GRP_ROOT_ID);

    barrier_info = pvm_barrier(GRP_NAME, GRP_LEN);
    if (barrier_info < 0) exit_with_msg("SLV%d(%d) failed at final pvm_barrier\n", id, tid);
    leave_info = pvm_lvgroup(GRP_NAME);
    if (leave_info < 0) exit_with_msg("SLV%d(%d) failed at pvm_lvgroup\n", id, tid);

    free(u);
    pvm_exit();
    return EXIT_SUCCESS;
}

void exit_with_msg(const char *msg, const int id, const int tid)
{
    fprintf(stderr, msg, id, tid);
    pvm_exit();
    exit(EXIT_FAILURE);
}
