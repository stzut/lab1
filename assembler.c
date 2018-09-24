/*
Name 1: Tongzhou Shi

UTEID 1: ts36947 

*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

int char2dec(char* hex)
{
    int result=0;
    int sign = 1;
    if (*(++hex) == '-')
        sign = -1;
    hex--;
    if (*hex == 'X') {
        hex++;
        while ((*hex) != '\0') {
            if (('0' <= (*hex)) && ((*hex) <= '9'))
                result = result * 16 + (*hex) - '0';
            else if (('a' <= (*hex)) && ((*hex) <= 'f'))
                result = result * 16 + (*hex) - 'a' + 10;
            else if (('A' <= (*hex)) && ((*hex) <= 'F'))
                result = result * 16 + (*hex) - 'A' + 10;
            hex++;
        }
        result = result * sign;
    }
    else if (*hex == '#')
    {
        hex++;
        result = atoi(hex);
    }
    else exit(3);
    return result;
}


int reg2int(char *reg)
{
    int result = atoi(++reg);
    if (result < 0 || result > 7)
        exit(4);
    return result;
}

int main(int argc, char *argv[])
{
    int ins[100];                       //instructions
    char prog[500];                     //the string reading from the input file
    char buf[5000];

    for (int l = 0; l < 5000; ++l) {
        buf[l] = '\0';
    }

    int pc = 0;
    char *labname, *reg1, *reg2, *op, *key, *ss;     //elements of decoded ins
    FILE *f = fopen(argv[1], "r");
    //FILE *f = fopen("shuffle.asm", "r");
    int addr[100];
    int cons = 0;
    int end = 0;


    struct label
    {
        char *label;
        int addr;
    };
    struct label labtable[100];
    for (int w = 0; w < 100;w++)
        labtable[w].addr = 0;
    int label_count = 0;
    int exist = 0;
    char line[200];
    char sapc = ' ';

    struct jmp
    {
        int pc;
        int labelnum;
        int opcode;
    };
    struct jmp jumptable[50];
    int jmp_count = 0;


    if(f == NULL)
    {
        printf("No such input file.\n");
        exit(4);
    }

    while(fgets(prog, 500, f) != NULL)
    {
        strcpy(line, prog);
        for (int g = 0; g < 200; g++) {
            line[g] = (char) toupper(line[g]);
            if(line[g] == '\0'||line[g] == ';'||line[g] == '\n') {
                line[g] = ' ';
                line[g + 1] = '\0';
                break;
            }
        }
        strcat(buf,line);
    }
    //fgets(prog, 500, f);

    //fscanf(f,"%s",prog);

                    /* Initialization*/

        key = strtok(buf, " \r\n\t,");
        if (strcmp(key, ".ORIG") != 0)      //first command is not .ORIG
            exit(4);

        key = strtok(NULL, " \n\t\r,");
        cons = char2dec(key);
        ins[0] = cons;
        pc++;

        if (cons % 2 != 0 || cons < 0 || cons >= 65536)
            exit(3);

        addr[0] = 0;
        for(int e = 1;e <100; e++)              //configure address of instructions
        {
            addr[e] = cons + 2*(e-1);
        }



                         /* First Pass*/

        key = strtok(NULL, " \n\t\r,"); //skip .ORIG and its value
        while (key != NULL)
        {
            if (strcmp(key, "ADD") == 0)
            {
                reg1 = strtok(NULL, " \n\t\r,");
                reg2 = strtok(NULL, " \n\t\r,");
                op = strtok(NULL, " \n\t\r,");
                ins[pc] = 0x1000 + reg2int(reg1) * 512 + reg2int(reg2)*64;
                if (*op == 'R')
                    ins[pc] += reg2int(op);
                else
                {
                    cons = char2dec(op);
                    if (cons < - 16 || cons > 15)
                        exit(3);
                    if(cons < 0)
                        ins[pc] = ins[pc] + 32 + 32+cons;
                    else ins[pc] = ins[pc] + 32 + cons;
                }
                pc++;
                key = strtok(NULL, " \n\r\t,");
            }
            else if (strcmp(key, "AND") == 0)
            {
                reg1 = strtok(NULL, " \n\t\r,");
                reg2 = strtok(NULL, " \n\t\r,");
                op = strtok(NULL, " \n\t\r,");
                ins[pc] = 0x5000 + reg2int(reg1) * 512 + reg2int(reg2) * 64;
                if (*op == 'R')
                    ins[pc] += reg2int(op);
                else
                {
                    cons = char2dec(op);
                    if (cons < - 16 || cons > 15)
                        exit(3);
                    if(cons < 0)
                        ins[pc] = ins[pc] + 32 + 32+cons;
                    else ins[pc] = ins[pc] + 32 + cons;
                }
                pc++;
                key = strtok(NULL, " \n\r\t,");
            }
            else if(strcmp(key, "BR") == 0 ||strcmp(key, "BRNZP") == 0)
            {
                exist = 0;
                labname = strtok(NULL, " \n\t\r,");
                if (*labname == 'X' && (*(++labname)- '0') < 10 && (*(labname)- '0') >0){
                    labname--;
                    exit(4);
                }
                else if(*labname == '#')
                    exit(4);

                for (cons = 0; cons < label_count;cons++)
                {
                    if (strcmp(labname, labtable[cons].label) == 0)
                    {
                        exist = 1;
                        jumptable[jmp_count].labelnum = cons;
                    }
                }

                if (exist == 1)
                {

                }
                else
                {
                    labtable[label_count].label = labname;
                    labtable[label_count].addr = -1;
                    jumptable[jmp_count].labelnum = label_count;
                    label_count++;
                }
                jumptable[jmp_count].pc = pc;
                jumptable[jmp_count].opcode = 7 * 512;

                jmp_count++;
                pc++;
                key = strtok(NULL, " \n\r\t,");
            }
            else if(strcmp(key, "BRN") == 0)
            {
                exist = 0;
                labname = strtok(NULL, " \n\t\r,");
                if (*labname == 'X' && (*(++labname)- '0') < 10 && (*(labname)- '0') >0){
                    labname--;
                    exit(4);
                }
                else if(*labname == '#')
                    exit(4);
                for (cons = 0; cons < label_count;cons++)
                {
                    if (strcmp(labname, labtable[cons].label) == 0)
                    {
                        exist = 1;
                        jumptable[jmp_count].labelnum = cons;
                    }
                }

                if (exist == 1)
                {

                }
                else
                {
                    labtable[label_count].label = labname;
                    labtable[label_count].addr = -1;
                    jumptable[jmp_count].labelnum = label_count;
                    label_count++;
                }
                jumptable[jmp_count].pc = pc;
                jumptable[jmp_count].opcode = 2048;

                jmp_count++;
                pc++;
                key = strtok(NULL, " \n\r\t,");
            }
            else if(strcmp(key, "BRZ") == 0)
            {
                exist = 0;
                labname = strtok(NULL, " \n\t\r,");
                if (*labname == 'X' && (*(++labname)- '0') < 10 && (*(labname)- '0') >0){
                    labname--;
                    exit(4);
                }
                else if(*labname == '#')
                    exit(4);
                for (cons = 0; cons < label_count;cons++)
                {
                    if (strcmp(labname, labtable[cons].label) == 0)
                    {
                        exist = 1;
                        jumptable[jmp_count].labelnum = cons;
                    }
                }

                if (exist == 1)
                {

                }
                else
                {
                    labtable[label_count].label = labname;
                    labtable[label_count].addr = -1;
                    jumptable[jmp_count].labelnum = label_count;
                    label_count++;
                }
                jumptable[jmp_count].pc = pc;
                jumptable[jmp_count].opcode = 1024;

                jmp_count++;
                pc++;
                key = strtok(NULL, " \n\r\t,");
            }
            else if(strcmp(key, "BRP") == 0)
            {
                exist = 0;
                labname = strtok(NULL, " \n\t\r,");
                if (*labname == 'X' && (*(++labname)- '0') < 10 && (*(labname)- '0') >0){
                    labname--;
                    exit(4);
                }
                else if(*labname == '#')
                    exit(4);
                for (cons = 0; cons < label_count;cons++)
                {
                    if (strcmp(labname, labtable[cons].label) == 0)
                    {
                        exist = 1;
                        jumptable[jmp_count].labelnum = cons;
                    }
                }

                if (exist == 1)
                {

                }
                else
                {
                    labtable[label_count].label = labname;
                    labtable[label_count].addr = -1;
                    jumptable[jmp_count].labelnum = label_count;
                    label_count++;
                }
                jumptable[jmp_count].pc = pc;
                jumptable[jmp_count].opcode = 512;

                jmp_count++;
                pc++;
                key = strtok(NULL, " \n\r\t,");
            }
            else if(strcmp(key, "BRNZ") == 0)
            {
                exist = 0;
                labname = strtok(NULL, " \n\t\r,");
                if (*labname == 'X' && (*(++labname)- '0') < 10 && (*(labname)- '0') >0){
                    labname--;
                    exit(4);
                }
                else if(*labname == '#')
                    exit(4);
                for (cons = 0; cons < label_count;cons++)
                {
                    if (strcmp(labname, labtable[cons].label) == 0)
                    {
                        exist = 1;
                        jumptable[jmp_count].labelnum = cons;
                    }
                }

                if (exist == 1)
                {

                }
                else
                {
                    labtable[label_count].label = labname;
                    labtable[label_count].addr = -1;
                    jumptable[jmp_count].labelnum = label_count;
                    label_count++;
                }
                jumptable[jmp_count].pc = pc;
                jumptable[jmp_count].opcode = 3 * 1024;

                jmp_count++;
                pc++;
                key = strtok(NULL, " \n\r\t,");
            }
            else if(strcmp(key, "BRNP") == 0)
            {
                exist = 0;
                labname = strtok(NULL, " \n\t\r,");
                if (*labname == 'X' && (*(++labname)- '0') < 10 && (*(labname)- '0') >0){
                    labname--;
                    exit(4);
                }
                else if(*labname == '#')
                    exit(4);
                for (cons = 0; cons < label_count;cons++)
                {
                    if (strcmp(labname, labtable[cons].label) == 0)
                    {
                        exist = 1;
                        jumptable[jmp_count].labelnum = cons;
                    }
                }

                if (exist == 1)
                {

                }
                else
                {
                    labtable[label_count].label = labname;
                    labtable[label_count].addr = -1;
                    jumptable[jmp_count].labelnum = label_count;
                    label_count++;
                }
                jumptable[jmp_count].pc = pc;
                jumptable[jmp_count].opcode = 5 * 512;

                jmp_count++;
                pc++;
                key = strtok(NULL, " \n\r\t,");
            }
            else if(strcmp(key, "BRZP") == 0)
            {
                exist = 0;
                labname = strtok(NULL, " \n\t\r,");
                if (*labname == 'X' && (*(++labname)- '0') < 10 && (*(labname)- '0') >0){
                    labname--;
                    exit(4);
                }
                else if(*labname == '#')
                    exit(4);
                for (cons = 0; cons < label_count;cons++)
                {
                    if (strcmp(labname, labtable[cons].label) == 0)
                    {
                        exist = 1;
                        jumptable[jmp_count].labelnum = cons;
                    }
                }

                if (exist == 1)
                {

                }
                else
                {
                    labtable[label_count].label = labname;
                    labtable[label_count].addr = -1;
                    jumptable[jmp_count].labelnum = label_count;
                    label_count++;
                }
                jumptable[jmp_count].pc = pc;
                jumptable[jmp_count].opcode = 3 * 512;

                jmp_count++;
                pc++;
                key = strtok(NULL, " \n\r\t,");
            }
            else if (strcmp(key, "JMP") == 0)
            {
                reg1 = strtok(NULL, " \n\t\r,");
                ins[pc] = 0xC000 + reg2int(reg1) * 64;
                pc++;
                key = strtok(NULL, " \n\r\t,");
            }
            else if (strcmp(key, "RET") == 0)
            {
                ins[pc] = 0xC000 + 7 * 64;
                pc++;
                key = strtok(NULL, " \n\r\t,");
            }

            else if(strcmp(key, "JSR") == 0)
            {
                exist = 0;
                labname = strtok(NULL, " \n\t\r,");
                if (*labname == 'X' && (*(++labname)- '0') < 10 && (*(labname)- '0') >0){
                    labname--;
                    exit(4);
                }
                else if(*labname == '#')
                    exit(4);
                for (cons = 0; cons < label_count;cons++)
                {
                    if (strcmp(labname, labtable[cons].label) == 0)
                    {
                        exist = 1;
                        jumptable[jmp_count].labelnum = cons;
                    }
                }

                if (exist == 1)
                {

                }
                else
                {
                    labtable[label_count].label = labname;
                    labtable[label_count].addr = -1;
                    jumptable[jmp_count].labelnum = label_count;
                    label_count++;
                }
                jumptable[jmp_count].pc = pc;
                jumptable[jmp_count].opcode = 0x4800;

                jmp_count++;
                pc++;
                key = strtok(NULL, " \n\r\t,");
            }
            else if (strcmp(key, "JSRR") == 0)
            {
                reg1 = strtok(NULL, " \n\t\r,");
                ins[pc] = 0x4000 + reg2int(reg1) * 64;
                pc++;
                key = strtok(NULL, " \n\r\t,");
            }
            else if (strcmp(key, "LDB") == 0)
            {
                reg1 = strtok(NULL, " \n\t\r,");
                reg2 = strtok(NULL, " \n\t\r,");
                op = strtok(NULL, " \n\t\r,");
                ins[pc] = 0x2000 + reg2int(reg1) * 512 + reg2int(reg2) * 64;

                cons = char2dec(op);
                if (cons < - 32 || cons > 31)
                    exit(3);
                if(cons < 0)
                    ins[pc] = ins[pc] + 64 + cons;
                else ins[pc] = ins[pc] + cons;
                pc++;
                key = strtok(NULL, " \n\r\t,");
            }
            else if (strcmp(key, "LDW") == 0)
            {
                reg1 = strtok(NULL, " \n\t\r,");
                reg2 = strtok(NULL, " \n\t\r,");
                op = strtok(NULL, " \n\t\r,");
                ins[pc] = 0x6000 + reg2int(reg1) * 512 + reg2int(reg2) * 64;

                cons = char2dec(op);
                if (cons < - 32 || cons > 31)
                    exit(3);
                if(cons < 0)
                    ins[pc] = ins[pc] + 64 + cons;
                else ins[pc] = ins[pc] + cons;
                pc++;
                key = strtok(NULL, " \n\r\t,");
            }
            else if (strcmp(key, "LEA") == 0)
            {
                exist = 0;
                reg1 = strtok(NULL, " \n\t\r,");
                labname = strtok(NULL, " \n\t\r,");
                if (*labname == 'X' && (*(++labname)- '0') < 10 && (*labname - '0') >0){
                    labname--;
                    exit(4);

                }
                else if(*labname == '#')
                    exit(4);
                for (cons = 0; cons < label_count;cons++)
                {
                    if (strcmp(labname, labtable[cons].label) == 0)
                    {
                        exist = 1;
                        jumptable[jmp_count].labelnum = cons;
                    }
                }

                if (exist == 1)
                {

                }
                else
                {
                    labtable[label_count].label = labname;
                    labtable[label_count].addr = -1;
                    jumptable[jmp_count].labelnum = label_count;
                    label_count++;
                }
                jumptable[jmp_count].pc = pc;
                jumptable[jmp_count].opcode = 0xE000 + reg2int(reg1) * 512;

                jmp_count++;
                pc++;
                key = strtok(NULL, " \n\r\t,");
            }
            else if (strcmp(key, "NOT") == 0)
            {
                reg1 = strtok(NULL, " \n\t\r,");
                reg2 = strtok(NULL, " \n\t\r,");

                ins[pc] = 0x9000 + reg2int(reg1) * 512 + reg2int(reg2) * 64 + 63;
                pc++;
                key = strtok(NULL, " \n\r\t,");
            }
            else if (strcmp(key, "RTI") == 0)
            {
                ins[pc] = 0x8000;
                pc++;
                key = strtok(NULL, " \n\r\t,");
            }
            else if (strcmp(key, "LSHF") == 0)
            {
                reg1 = strtok(NULL, " \n\t\r,");
                reg2 = strtok(NULL, " \n\t\r,");
                op = strtok(NULL, " \n\t\r,");
                ins[pc] = 0xD000 + reg2int(reg1) * 512 + reg2int(reg2) * 64;
                cons = char2dec(op);
                if (cons < 0 || cons > 15)
                    exit(3);
                ins[pc] += cons;
                pc++;
                key = strtok(NULL, " \n\r\t,");
            }
            else if (strcmp(key, "RSHFL") == 0)
            {
                reg1 = strtok(NULL, " \n\t\r,");
                reg2 = strtok(NULL, " \n\t\r,,");
                op = strtok(NULL, " \n\t\r");
                ins[pc] = 0xD000 + reg2int(reg1) * 512 + reg2int(reg2) * 64 + 16;
                cons = char2dec(op);
                if (cons < 0 || cons > 15)
                    exit(3);
                ins[pc] += cons;
                pc++;
                key = strtok(NULL, " \n\r\t,");
            }
            else if (strcmp(key, "RSHFA") == 0)
            {
                reg1 = strtok(NULL, " \n\t\r,");
                reg2 = strtok(NULL, " \n\t\r,");
                op = strtok(NULL, " \n\t\r,");
                ins[pc] = 0xD000 + reg2int(reg1) * 512 + reg2int(reg2) * 64 + 48;
                cons = char2dec(op);
                if (cons < 0 || cons > 15)
                    exit(3);
                ins[pc] += cons;
                pc++;
                key = strtok(NULL, " \n\r\t,");
            }
            else if (strcmp(key, "STB") == 0)
            {
                reg1 = strtok(NULL, " \n\t\r,");
                reg2 = strtok(NULL, " \n\t\r,");
                op = strtok(NULL, " \n\t\r,");
                ins[pc] = 0x3000 + reg2int(reg1) * 512 + reg2int(reg2) * 64;
                cons = char2dec(op);
                if (cons < -32 || cons > 31)
                    exit(3);
                if(cons < 0)
                    ins[pc] = ins[pc] + 64 + cons;
                else ins[pc] = ins[pc] + cons;
                pc++;
                key = strtok(NULL, " \n\r\t,");
            }
            else if (strcmp(key, "STW") == 0)
            {
                reg1 = strtok(NULL, " \n\t\r,");
                reg2 = strtok(NULL, " \n\t\r,");
                op = strtok(NULL, " \n\t\r,");
                ins[pc] = 0x7000 + reg2int(reg1) * 512 + reg2int(reg2) * 64;
                cons = char2dec(op);
                if (cons < -32 || cons > 31)
                    exit(3);
                if(cons < 0)
                    ins[pc] = ins[pc] + 64 + cons;
                else ins[pc] = ins[pc] + cons;
                pc++;
                key = strtok(NULL, " \n\r\t,");
            }
            else if (strcmp(key, "TRAP") == 0)
            {
                op = strtok(NULL, " \n\t\r,");
                cons = char2dec(op);
                if (cons < 0)
                    exit(3);
                ins[pc] = 0xF000 + cons;
                pc++;
                key = strtok(NULL, " \n\r\t,");
            }
            else if (strcmp(key, "HALT") == 0)
            {
                ins[pc] = 0xF000 + 0x25;
                pc++;
                key = strtok(NULL, " \n\r\t,");
            }
            else if (strcmp(key, "XOR") == 0)
            {
                reg1 = strtok(NULL, " \n\t\r,");
                reg2 = strtok(NULL, " \n\t\r,");
                op = strtok(NULL, " \n\t\r,");
                ins[pc] = 0x9000 + reg2int(reg1) * 512 + reg2int(reg2) * 64;
                if (*op == 'R')
                    ins[pc] += reg2int(op);
                else
                {
                    cons = char2dec(op);
                    if (cons < - 16 || cons > 15)
                        exit(3);
                    if(cons < 0)
                        ins[pc] = ins[pc] + 64 + cons;
                    else ins[pc] = ins[pc] + 32+cons;
                }
                pc++;
                key = strtok(NULL, " \n\r\t,");
            }
            else if (strcmp(key, ".FILL") == 0)
            {
                op = strtok(NULL, " \n\t\r,");
                ins[pc] = char2dec(op);
                pc++;
                key = strtok(NULL, " \n\r\t,");
            }
            else if (strcmp(key, ".END") == 0){
                end = 1;
                key = strtok(NULL, " \n\r\t,");
            }
            else
            {
                char * poss = strtok(NULL, " \n\r\t,");
                if (*poss == 'X' || *poss == '#')
                    exit(2);
                else if (*poss == 'R' && (*(++poss) - '0') < 8)
                    exit(2);
                else
                {
                    exist = 0;
                    for (cons = 0; cons < label_count;cons++)
                    {
                        if (strcmp(key, labtable[cons].label) == 0) {
                            exist = 1;
                            labtable[cons].addr = addr[pc];
                        }
                    }

                    if (exist == 0)
                    {
                        labtable[label_count].label = key;
                        labtable[label_count].addr = addr[pc];
                        label_count++;
                    }

                    key = poss;
                }
            

        }
    }
                            /* The Second pass */
    int labeladdr = 0;
    int insaddr = 0;
    for (int d = 0; d < label_count;d++)
    {
        if(labtable[d].addr == -1)
            exit(1);
    }
    for (int i = 0; i < jmp_count;i++)
    {
        if(jumptable[i].pc != 0)
        {
            labeladdr = labtable[jumptable[i].labelnum].addr;
            insaddr = addr[jumptable[i].pc];
            if (labeladdr > insaddr)
            {
                ins[jumptable[i].pc] = jumptable[i].opcode + (labeladdr - insaddr - 2) / 2;
                if (jumptable[i].opcode == 0x4800 && (labeladdr - insaddr - 2) / 2 > 1023)
                    exit(4);
                else if (jumptable[i].opcode != 0x4800 && (labeladdr - insaddr - 2) / 2 > 255)
                    exit(4);
            }
            else
            {
                if (jumptable[i].opcode == 0x4800)
                {
                    if (((insaddr - labeladdr + 2) / 2) > 1024)
                        exit(4);

                    ins[jumptable[i].pc] = jumptable[i].opcode +  2048 + ((labeladdr - insaddr - 2) / 2);
                }
                else {
                    if ((insaddr - labeladdr + 2) / 2 > 256)
                        exit(4);
                    ins[jumptable[i].pc] = jumptable[i].opcode + 512 + ((labeladdr - insaddr - 2) / 2);
                }
            }
        }
    }
    fclose(f);
    fopen(argv[2],"w");
    for(int ww = 0; ww < pc;ww++)
    {
        fprintf(f,"0x%.4X\n", ins[ww]);
    }
    if(end == 0)
        exit(4);

}