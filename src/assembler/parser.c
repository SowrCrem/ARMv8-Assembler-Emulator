#include <string.h>

void parser(char *line)
{

    char *opcode;
    opcode = strtok(line, " ");

    if ((strcmp(opcode, "add") == 0) || (strcmp(opcode, "sub") == 0) || (strcmp(opcode, "adds") == 0) || (strcmp(opcode, "subs") == 0))
    {
        printf("Reading line: %s \n ---- This is an add/adds/sub/subs instruction..\n");
        addSubInstr(line);
    }

    else if ((strcmp(opcode, "cmp") == 0) || (strcmp(opcode, "cmn") == 0))
    {
        printf("Reading line: %s \n ---- This is a cmp/cmn instruction..\n");
        compareInstr(line);
    }

    else if ((strcmp(opcode, "neg") == 0) || (strcmp(opcode, "negs") == 0))
    {
        printf("Reading line: %s \n ---- This is a neg/negs instruction..\n");
        negInstr(line);
    }

    else if ((strcmp(opcode, "eor") == 0) || (strcmp(opcode, "eon") == 0) || (strcmp(opcode, "orr") == 0) || (strcmp(opcode, "orn") == 0) || (strcmp(opcode, "bic") == 0) || (strcmp(opcode, "bics") == 0) || (strcmp(opcode, "and") == 0) || (strcmp(opcode, "ands") == 0))
    {
        printf("Reading line: %s \n ---- This is an and(s)/bic(s)/or(r/n)/eo(r/n) instruction..\n");
        booleanInstr(line);
    }

    else if (strncmp(opcode, "mov", 3) == 0)
    {
        if (line[3] == ' ')
        {
            printf("Reading line: %s \n ---- This is a mov instruction..\n");
            movInstr(line);
        }

        else
        {
            printf("Reading line: %s \n ---- This is a mov(k/n/z) instruction..\n");
            movXInstr(line);
        }
    }

    else if (strcmp(opcode, "mvn") == 0)
    {
        printf("Reading line: %s \n ---- This is a mvn instruction..\n");
        mvnInstr(line);
    }

    else if (strcmp(opcode, "madd") == 0 || strcmp(opcode, "msub") == 0)
    {
        printf("Reading line: %s \n ---- This is a madd/msub instruction..\n");
        marithInstr(line);
    }

    else if (strcmp(opcode, "mul") == 0 || strcmp(opcode, "mneg") == 0)
    {
        printf("Reading line: %s \n ---- This is a mul/mneg instruction..\n");
        marithInstr(line);
    }

    else if (strcmp(opcode, "str") == 0 || strcmp(opcode, "ldr") == 0)
    {
        printf("Reading line: %s \n ---- This is a ldr/str instruction..\n");
        loadOrStore(line);
    }

    else if (strcmp(opcode, "nop") == 0 || strcmp(opcode, ".int") == 0)
    {
        printf("Reading line: %s \n ---- This is a special instruction instruction..\n");
        specialInstr(line);
    }

    else if (strcmp(opcode, "br") == 0)
    {
        printf("Reading line: %s \n ---- This is a br instruction..\n");
        branchRegisterInstr(line);
    }

    else if ((strcmp(opcode, "b") == 0) || (strcmp(opcode, "b.cond") == 0))
    {
        printf("Reading line: %s \n ---- This is a b(.cond) instruction..\n");
        branchInstr(line);
    }

    else
    {
        printf("------ ERROR! Could not parse through line ------ \n\n %s\n\n ------ Unknown instruction ------", line);
    }

    // if ((strncmp(line, "add", 4) == 0) || (strncmp(line, "sub", 4) == 0)) {
    //     printf("Reading line: %s \n ---- This is an add/adds/sub/subs instruction..\n");
    //     addSubInstr(line);
    // }

    // else if ((strncmp(line, "cm", 3) == 0)) {
    //     printf("Reading line: %s \n ---- This is a cmp/cmn instruction..\n");
    //     compareInstr(line);
    // }

    // else if ((strncmp(line, "neg", 4) == 0)) {
    //     printf("Reading line: %s \n ---- This is a neg/negs instruction..\n");
    //     negInstr(line);
    // }

    // else if ((strncmp(line, "eo", 3) == 0) || (strncmp(line, "or", 3) == 0) || (strncmp(line, "bic", 4) == 0) || (strncmp(line, "and", 4) == 0) ) {
    //     printf("Reading line: %s \n ---- This is an and(s)/bic(s)/or(r/n)/eo(r/n) instruction..\n");
    //     booleanInstr(line);
    // }

    // else if (strncmp(line, "mov", 4) == 0) {
    //     if (line[3] == ' ') {
    //         printf("Reading line: %s \n ---- This is a mov instruction..\n");
    //         movInstr(line);
    //     }

    //     else {
    //         printf("Reading line: %s \n ---- This is a mov(k/n/z) instruction..\n");
    //         movXInstr(line);
    //     }
    // }

    // else if (strncmp(line, "mvn", 4) == 0) {
    //     printf("Reading line: %s \n ---- This is a mvn instruction..\n");
    //     mvnInstr(line);
    // }

    // else if (strncmp(line, "madd", 5) == 0 || strncmp(line, "msub", 5) == 0) {
    //     printf("Reading line: %s \n ---- This is a madd/msub instruction..\n");
    //     marithInstr(line);
    // }

    // else if (strncmp(line, "mul", 4) == 0 || strncmp(line, "mneg", 5) == 0) {
    //     printf("Reading line: %s \n ---- This is a mul/mneg instruction..\n");
    //     marithInstr(line);
    // }

    // else if (strncmp(line, "str", 4) == 0 || strncmp(line, "ldr", 5) == 0) {
    //     printf("Reading line: %s \n ---- This is a ldr/str instruction..\n");
    //     loadOrStore(line);
    // }

    // else if (strncmp(line, "nop", 4) == 0 || strncmp(line, ".int", 5) == 0) {
    //     printf("Reading line: %s \n ---- This is a special instruction instruction..\n");
    //     specialInstr(line);
    // }

    // else if (strncmp(line, "br", 3) == 0) {
    //     printf("Reading line: %s \n ---- This is a br instruction..\n");
    //     branchRegisterInstr(line);
    // }

    // else if (strncmp(line, "b", 2) == 0) {
    //     printf("Reading line: %s \n ---- This is a b(.cond) instruction..\n");
    //     branchInstr(line);
    // }

    // else {
    //     printf("------ ERROR! Could not parse through line ------ \n\n %s\n\n ------ Unknown instruction ------", line);
    // }
}

void addSubInstr(char *line)
{
    return;
}

void compareInstr(char *line)
{
    return;
}

void negInstr(char *line)
{
    return;
}

void booleanInstr(char *line)
{
    return;
}

void movInstr(char *line)
{
    return;
}

void movXInstr(char *line)
{
    return;
}

void mvnInstr(char *line)
{
    return;
}

void marithInstr(char *line)
{
    return;
}

void mulNeginstr(char *line)
{
    return;
}

void specialInstr(char *line)
{
    return;
}

void loadOrStoreInstr(char *line)
{
    return;
}

void branchRegisterInstr(char *line)
{
    return;
}

void branchInstr(char *line)
{
    return;
}