#include <stdlib.h>
#include <stdio.h>
#include "pvm3.h"

#include "communication.h"
#include "vector.h"

extern const int v_len;
extern const double v[];

void exit_with_msg(const char *msg);

int main(int argc, char *argv[])
{
    int id;
    int slv_tids[SLV_NUM];
    int actual_slv_num;
    int barrier_info, leave_info;

    /*
    Dlugosc wektora i liczba procesow, pomiedzy ktore jest rozdzielany, sa
    ustalone na sztwyno. Jezeli alokacja wymaganej liczby procesow lub czesci
    wektora zawiedzie, to program krzyczy i sie przewraca.
    */

    id = pvm_joingroup(GRP_NAME);
    if (id != GRP_ROOT_ID) exit_with_msg("MSTR failed at pvm_joingroup\n");
    actual_slv_num = pvm_spawn(SLV_NAME, NULL, PvmTaskDefault, "", SLV_NUM, slv_tids);
    if (actual_slv_num < SLV_NUM) exit_with_msg("MSTR failed at pvm_spawn\n");
    barrier_info = pvm_barrier(GRP_NAME, GRP_LEN);
    if (barrier_info < 0) exit_with_msg("MSTR failed at initial pvm_barrier\n");

    int u_len = v_len / GRP_LEN;
    double *u = malloc(sizeof(*u) * u_len);
    if (u == NULL) exit_with_msg("MSTR failed at malloc\n");
    pvm_scatter((void *) u, (void *) v, u_len, PVM_DOUBLE, GRP_SCT_MSG_TAG, GRP_NAME, GRP_ROOT_ID);
    double partial_rslt = sum_u(u, u_len);
    // printf("** MSTR partial result = %lg\n", partial_rslt);
    pvm_reduce(PvmSum, &partial_rslt, 1, PVM_DOUBLE, GRP_RDC_MSG_TAG, GRP_NAME, GRP_ROOT_ID);
    printf("**** MSTR result = %lg\n", partial_rslt);

    barrier_info = pvm_barrier(GRP_NAME, GRP_LEN);
    if (barrier_info < 0) exit_with_msg("MSTR failed at final pvm_barrier\n");
    leave_info = pvm_lvgroup(GRP_NAME);
    if (leave_info < 0) exit_with_msg("MSTR failed at pvm_lvgroup\n");

    free(u);
    pvm_exit();
    return EXIT_SUCCESS;
}

void exit_with_msg(const char *msg)
{
    fprintf(stderr, msg);
    pvm_exit();
    exit(EXIT_FAILURE);
}
