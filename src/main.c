#include <stdio.h>
#include <stdlib.h>
#include "lexer.h"
#include "parser.h"
#include "astrid_ir.h"
#include "codegen.h"

int main() {
    const char* input = "let result = ( (2 + 3) * (10 - 4) / 2 + 50 - (8 * 2) );\n";

    Lexer* l = NewLexer(input);
    Parser* p = NewParser(l);
    Program* program = ParseProgram(p);
    IR_Listing* ir = GenerateIR(program);

    char* assembly_code = GenerateAssembly(ir);

    FILE* out_file = fopen("output.asm", "w");
    if (out_file) {
        fprintf(out_file, "%s", assembly_code);
        fclose(out_file);
        printf("Assembly code written to output.asm\n");
    }

    free(assembly_code);
    FreeIR(ir);
    FreeNode((Node*)program);
    FreeParser(p);
    FreeLexer(l);
    
    return 0;
}