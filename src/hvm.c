#include <signal.h>
#include <stdio.h>

#include "instruction.h"
#include "vm.h"

int main_loop;

void core_dump (struct _vm * vm)
{
    FILE * fh;

    fh = fopen("core", "wb");
    if (fh == NULL)
        return;

    fwrite(vm->mem, 1, 65536, fh);

    fclose(fh);
}

void int_handler (int interrupt)
{
    main_loop = 0;
}

int main (int argc, char * argv [])
{
    struct _vm * vm;
    int error;

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <hvm program>\n", argv[0]);
        return 0;
    }

    signal(SIGINT, int_handler);

    vm = vm_load(argv[1]);
    main_loop = 1;

    while (main_loop) {
        //printf("%s\n", vm_ins_str(vm));
        error = vm_step(vm);
        if (error) {
            switch (error) {
            case VM_HALTED :
                break;
            case VM_BAD_INSTRUCTION :
                printf("VM Encountered Bad Instruction\n");
                printf("%s\n", vm_registers_str(vm));
                break;
            }
            break;
        }
    }

    core_dump(vm);
    vm_destroy(vm);

    return 0;
}