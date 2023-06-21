/*

//////////////////////////TEAM//////////////////////////
BITS ID             Member Name
2020A7PS0073P       Shashank Agrawal
2020A7PS0096P       Akshat Gupta
2020A7PS0129P       Anish Ghule
2020A7PS0134P       Shadan Hussain
2020A7PS1513P       Tarak P V S

*/
#include "codegen.h"
#include "global.h"

char *asmOutputFile = "code.asm";
void generateExitAndErrorLabels(FILE *fp)
{
    // make label for err_index_out_of_bounds
    fprintf(fp, "err_index_out_of_bounds:\n");
    fprintf(fp, "mov rdi, err_index_out_of_bounds_msg\n");
    pushAllRegisters(fp);
    fprintf(fp, "call printf\n");
    popAllRegisters(fp);
    fprintf(fp, "mov rdi, 1\n");// exit status code 1
    fprintf(fp, "call exit\n");
}

char *generateLabel()
{
    static int labelNo = 0;
    char *label = malloc(10 * sizeof(char));
    sprintf(label, "L%d", labelNo);
    labelNo++;
    return label;
}

void zeroAllRegisters(FILE *fp)
{
    fprintf(fp, "mov rax, 0\n");
    fprintf(fp, "mov rbx, 0\n");
    fprintf(fp, "mov rcx, 0\n");
    fprintf(fp, "mov rdx, 0\n");
}

void generateExpressionCodeForModuleCall(astNode *root, symbolTable *currTable, symbolTable *globalTable, FILE *fp)
{
}

void generateExpressionCode(astNode *root, symbolTable *currTable, symbolTable *globalTable, FILE *fp)
{
    if(root->variableNo == search(CharToNoMapping, "<U>")|| root->variableNo == search(CharToNoMapping, "<element_index_with_expressions>"))
    {
        astNode* signNode = root->children->children;
        int signFactor = 1;
        if(signNode->token->tokenType == MINUS)
            signFactor = -1;

        // evaluate expression
        generateExpressionCode(root->children->sibling, currTable, globalTable, fp);
        fprintf(fp, "pop rax\n");
        // multiply rax with signFactor
        fprintf(fp, "imul rax, %d\n", signFactor);
        fprintf(fp, "push rax\n");
    }
    else
    {
        if(root->variableNo == PLUS)
        {
            generateExpressionCode(root->children, currTable, globalTable, fp);
            generateExpressionCode(root->children->sibling, currTable, globalTable, fp);

            fprintf(fp, "pop rbx\n");
            fprintf(fp, "pop rax\n");
            fprintf(fp, "add rax, rbx\n");
            fprintf(fp, "push rax\n");
        }
        else if(root->variableNo == MINUS)
        {
            generateExpressionCode(root->children, currTable, globalTable, fp);
            generateExpressionCode(root->children->sibling, currTable, globalTable, fp);

            fprintf(fp, "pop rbx\n");
            fprintf(fp, "pop rax\n");
            fprintf(fp, "sub rax, rbx\n");
            fprintf(fp, "push rax\n");
        }
        else if(root->variableNo == MUL)
        {
            generateExpressionCode(root->children, currTable, globalTable, fp);
            generateExpressionCode(root->children->sibling, currTable, globalTable, fp);

            fprintf(fp, "pop rbx\n");
            fprintf(fp, "pop rax\n");
            fprintf(fp, "imul rax, rbx\n");
            fprintf(fp, "push rax\n");
        }
        else if(root->variableNo == DIV)
        {
            generateExpressionCode(root->children, currTable, globalTable, fp);
            generateExpressionCode(root->children->sibling, currTable, globalTable, fp);

            fprintf(fp, "pop rbx\n");
            fprintf(fp, "pop rax\n");
            fprintf(fp, "idiv rax, rbx\n");
            fprintf(fp, "push rax\n");
        }
        else if(root->variableNo == AND)
        {
            generateExpressionCode(root->children, currTable, globalTable, fp);
            fprintf(fp, "pop rax\n");
            fprintf(fp, "cmp rax, 0\n");
            char* newLabel = generateLabel();
            fprintf(fp, "push rax\njz exit_expr_%s\npop rax\n", newLabel);
            generateExpressionCode(root->children->sibling, currTable, globalTable, fp);
            fprintf(fp, "pop rbx\n");
            fprintf(fp, "and rax, rbx\n");
            fprintf(fp, "push rax\n");
            fprintf(fp, "exit_expr_%s:\n", newLabel);
        }
        else if(root->variableNo == OR)
        {
            generateExpressionCode(root->children, currTable, globalTable, fp);
            fprintf(fp, "pop rax\n");
            fprintf(fp, "cmp rax, 1\n");
            char* newLabel = generateLabel();
            fprintf(fp, "push rax\njz exit_expr_%s\npop rax\n", newLabel);
            generateExpressionCode(root->children->sibling, currTable, globalTable, fp);
            fprintf(fp, "pop rbx\n");
            fprintf(fp, "or rax, rbx\n");
            fprintf(fp, "push rax\n");
            fprintf(fp, "exit_expr_%s:\n", newLabel);
        }
        else if(root->variableNo == LT)
        {
            generateExpressionCode(root->children, currTable, globalTable, fp);
            generateExpressionCode(root->children->sibling, currTable, globalTable, fp);
            fprintf(fp, "pop rbx\n");
            fprintf(fp, "pop rax\n");
            fprintf(fp, "cmp rax,rbx\n");
            char* newLabel = generateLabel();
            fprintf(fp, "jl lt_expr_%s\n", newLabel);
            fprintf(fp, "mov rax, 0\n");
            fprintf(fp, "push rax\n");
            fprintf(fp, "jmp exit_expr_%s\n", newLabel);
            fprintf(fp, "lt_expr_%s:\n", newLabel);
            fprintf(fp, "mov rax, 1\n");
            fprintf(fp, "push rax\n");
            fprintf(fp, "exit_expr_%s:\n", newLabel);
        }
        else if(root->variableNo == LE)
        {
            generateExpressionCode(root->children, currTable, globalTable, fp);
            generateExpressionCode(root->children->sibling, currTable, globalTable, fp);
            fprintf(fp, "pop rbx\n");
            fprintf(fp, "pop rax\n");
            fprintf(fp, "cmp rax,rbx\n");
            char* newLabel = generateLabel();
            fprintf(fp, "jle le_expr_%s\n", newLabel);
            fprintf(fp, "mov rax, 0\n");
            fprintf(fp, "push rax\n");
            fprintf(fp, "jmp exit_expr_%s\n", newLabel);
            fprintf(fp, "le_expr_%s:\n", newLabel);
            fprintf(fp, "mov rax, 1\n");
            fprintf(fp, "push rax\n");
            fprintf(fp, "exit_expr_%s:\n", newLabel);
        }
        else if(root->variableNo == GT)
        {
            generateExpressionCode(root->children, currTable, globalTable, fp);
            generateExpressionCode(root->children->sibling, currTable, globalTable, fp);
            fprintf(fp, "pop rbx\n");
            fprintf(fp, "pop rax\n");
            fprintf(fp, "cmp rax,rbx\n");
            char* newLabel = generateLabel();
            fprintf(fp, "jg gt_expr_%s\n", newLabel);
            fprintf(fp, "mov rax, 0\n");
            fprintf(fp, "push rax\n");
            fprintf(fp, "jmp exit_expr_%s\n", newLabel);
            fprintf(fp, "gt_expr_%s:\n", newLabel);
            fprintf(fp, "mov rax, 1\n");
            fprintf(fp, "push rax\n");
            fprintf(fp, "exit_expr_%s:\n", newLabel);
        }
        else if(root->variableNo == GE)
        {
            generateExpressionCode(root->children, currTable, globalTable, fp);
            generateExpressionCode(root->children->sibling, currTable, globalTable, fp);
            fprintf(fp, "pop rbx\n");
            fprintf(fp, "pop rax\n");
            fprintf(fp, "cmp rax,rbx\n");
            char* newLabel = generateLabel();
            fprintf(fp, "jge ge_expr_%s\n", newLabel);
            fprintf(fp, "mov rax, 0\n");
            fprintf(fp, "push rax\n");
            fprintf(fp, "jmp exit_expr_%s\n", newLabel);
            fprintf(fp, "ge_expr_%s:\n", newLabel);
            fprintf(fp, "mov rax, 1\n");
            fprintf(fp, "push rax\n");
            fprintf(fp, "exit_expr_%s:\n", newLabel);
        }
        else if(root->variableNo == EQ)
        {
            generateExpressionCode(root->children, currTable, globalTable, fp);
            generateExpressionCode(root->children->sibling, currTable, globalTable, fp);
            fprintf(fp, "pop rbx\n");
            fprintf(fp, "pop rax\n");
            fprintf(fp, "cmp rax,rbx\n");
            char* newLabel = generateLabel();
            fprintf(fp, "je eq_expr_%s\n", newLabel);
            fprintf(fp, "mov rax, 0\n");
            fprintf(fp, "push rax\n");
            fprintf(fp, "jmp exit_expr_%s\n", newLabel);
            fprintf(fp, "eq_expr_%s:\n", newLabel);
            fprintf(fp, "mov rax, 1\n");
            fprintf(fp, "push rax\n");
            fprintf(fp, "exit_expr_%s:\n", newLabel);
        }
        else if(root->variableNo == NE)
        {
            generateExpressionCode(root->children, currTable, globalTable, fp);
            generateExpressionCode(root->children->sibling, currTable, globalTable, fp);
            fprintf(fp, "pop rbx\n");
            fprintf(fp, "pop rax\n");
            fprintf(fp, "cmp rax,rbx\n");
            char* newLabel = generateLabel();
            fprintf(fp, "jne ne_expr_%s\n", newLabel);
            fprintf(fp, "mov rax, 0\n");
            fprintf(fp, "push rax\n");
            fprintf(fp, "jmp exit_expr_%s\n", newLabel);
            fprintf(fp, "ne_expr_%s:\n", newLabel);
            fprintf(fp, "mov rax, 1\n");
            fprintf(fp, "push rax\n");
            fprintf(fp, "exit_expr_%s:\n", newLabel);
        }
        else if(root->variableNo == NUM)
        {
            // move the number to rax
            fprintf(fp, "mov rax, %s\n", root->token->lexeme);
            // push rax to the stack
            fprintf(fp, "push rax\n");
        }
        else if(root->variableNo == RNUM)
        {
            // move the number to rax
            fprintf(fp, "mov rax, %s\n", root->token->lexeme);
            // push rax to the stack
            fprintf(fp, "push rax\n");
        }
        else if(root->variableNo == TRUE)
        {
            // mov 1 to rax
            fprintf(fp, "mov rax, 1\n");
            // push rax to the stack
            fprintf(fp, "push rax\n");
        }
        else if(root->variableNo == FALSE)
        {
            // mov 0 to rax
            fprintf(fp, "mov rax, 0\n");
            // push rax to the stack
            fprintf(fp, "push rax\n");
        }
        else if(root->variableNo == ID)
        {
            if(root->children == NULL)
            {
                // this is not an array
                symbolTableEntry* entry = searchInTables(currTable, root->token->lexeme, root->token->lineNo);
                int offset = entry->info->ifVariable.actualOffset;
                // move rbp to rax
                fprintf(fp, "mov rax, rbp\n");
                // subtract offset from rax
                fprintf(fp, "sub rax, %d\n", offset);
                // subtract 8 from rax
                fprintf(fp, "sub rax, 8\n");
                // mov the value in [rax] to rbx
                fprintf(fp, "mov rax, [rax]\n");
                // push rbx to the stack
                fprintf(fp, "push rax\n");

            }
            else
            {
                // the id is an array
                symbolTableEntry* entry = searchInTables(currTable, root->token->lexeme, root->token->lineNo);

                if(entry->info->ifVariable.type.arrType == STATIC)
                {
                    astNode* indexNode = root->children;
                    generateExpressionCode(indexNode, currTable, globalTable, fp);
                    fprintf(fp, "pop rax\n");
                    //bound checking
                    // comparerax with left and right bounds
                    fprintf(fp, "cmp rax, %d\n", entry->info->ifVariable.type.leftBound);
                    fprintf(fp, "jl err_index_out_of_bounds\n");
                    fprintf(fp, "cmp rax, %d\n", entry->info->ifVariable.type.rightBound);
                    fprintf(fp, "jg err_index_out_of_bounds\n");
                    // now we have to calculate the offset
                    int offset = entry->info->ifVariable.actualOffset;
                    // move rbp to rbx
                    fprintf(fp, "mov rbx, rbp\n");
                    // subtract offset from rbx
                    fprintf(fp, "sub rbx, %d\n", offset);
                    // subtract 8 from rbx
                    fprintf(fp, "sub rbx, 8\n");
                    // subtract lower bound from rax
                    fprintf(fp, "sub rax, %d\n", entry->info->ifVariable.type.leftBound);
                    // multiply rax by 8
                    fprintf(fp, "imul rax, 8\n");
                    // subtract rax from rbx
                    fprintf(fp, "sub rbx, rax\n");
                    // subtract 8 from rbx
                    fprintf(fp, "sub rbx, 8\n");
                    // mov the value in [rbx] to rax
                    fprintf(fp, "mov rax, [rbx]\n");
                    // push rax to the stack
                    fprintf(fp, "push rax\n");

                }
                else
                {
                    //todo
                }
            }
        }


    }
}

void generateRecursivePop(astNode* output_listNode, symbolTable* currTable, symbolTable* globalTable, FILE* fp)
{
    if(output_listNode == NULL)
        return;

    generateRecursivePop(output_listNode->sibling, currTable, globalTable, fp);

    // now we have to generate code for the current node
    symbolTableEntry* entry = searchInTables(currTable, output_listNode->token->lexeme, output_listNode->token->lineNo);
    int offset = entry->info->ifVariable.actualOffset;
    int size = 8;
    // move rbp to rax
    fprintf(fp, "mov rax, rbp\n");
    // subtract offset from rax
    fprintf(fp, "sub rax, %d\n", offset);
    // subtract size from rax
    fprintf(fp, "sub rax, %d\n", size);
    // pop the value from the stack and store it in rbx
    fprintf(fp, "pop rbx\n");
    // store the value in rbx in the address in rax
    fprintf(fp, "mov [rax], rbx\n");

}

char* generateEndLabel()
{
    static int labelNo = 0;
    char *label = malloc(10 * sizeof(char));
    sprintf(label, "END_%d", labelNo);
    labelNo++;
    return label;
}

void pushAllRegisters(FILE* fp)
{
    fprintf(fp, "\npush rax\n");
    fprintf(fp, "push rbx\n");
    fprintf(fp, "push rcx\n");
    fprintf(fp, "push rdx\n\n");
}

void popAllRegisters(FILE* fp)
{
    fprintf(fp, "\npop rdx\n");
    fprintf(fp, "pop rcx\n");
    fprintf(fp, "pop rbx\n");
    fprintf(fp, "pop rax\n\n");
}
void generateStartCode(FILE* fp)
{
    fprintf(fp, "segment .text\n");
    fprintf(fp, "global main\n");
    fprintf(fp,"extern printf\n");
    fprintf(fp,"extern scanf\n");
    fprintf(fp,"extern exit\n\n\n");
}

void generateEndCode(FILE *fp)
{
    fprintf(fp, "\n\nsegment .data\n");
    fprintf(fp, "op_int: db \"OUTPUT: %%d\", 10, 0\n");
    fprintf(fp, "op_int_arr: db \"OUTPUT: \", 0\n");
    fprintf(fp, "oup_int: db \"%%d \", 0\n");
    fprintf(fp, "op_false: db \"OUTPUT: FALSE\", 10, 0\n");
    fprintf(fp, "op_true: db \"OUTPUT: TRUE\", 10, 0\n");
    fprintf(fp, "f_in_arr_bool: db \"%%d\", 0\n");
    fprintf(fp, "f_in_arr_real: db \"%%lld\", 0\n");
    fprintf(fp, "f_in_arr_int: db \"%%d\", 0\n");
    fprintf(fp, "ip_arrint1: db \"Input: Enter %%d array elements\",0\n");
    fprintf(fp, "ip_arrint2: db \"of integer type for range %%d\",0\n");
    fprintf(fp, "ip_arrint3: db \"to %%d\",0\n");
    fprintf(fp, "ip_arrbool1: db \"Input: Enter %%d array elements\",0\n");
    fprintf(fp, "ip_arrbool2: db \"of boolean type for range %%d\",0\n");
    fprintf(fp, "ip_arrbool3: db \"to %%d\",0\n");
    fprintf(fp, "ip_arrreal1: db \"Input: Enter %%d array elements\",0\n");
    fprintf(fp, "ip_arrreal2: db \"of real type for range %%d\",0\n");
    fprintf(fp, "ip_arrreal3: db \"to %%d\",0\n");
    fprintf(fp, "f_ip_int: db \"%%d\",0\n");
    fprintf(fp, "f_ip_real: db \"%%lld\",0\n");
    fprintf(fp, "f_ip_bool: db \"%%d\",0\n");
    fprintf(fp, "ip_int: db \"INPUT: Enter an integer value \",0\n");
    fprintf(fp, "ip_real: db \"INPUT: Enter an real value \",0\n");
    fprintf(fp, "ip_bool: db \"INPUT: Enter an boolean value \",0\n");
    fprintf(fp, "err_index_out_of_bounds_msg: db \"Array Index Out of Bound Error!\",10,0\n");
}


void generateCode(astNode *root, symbolTable *currTable, symbolTable *globalTable, FILE *fp)
{
    if (root->variableNo == search(CharToNoMapping, "<program>"))
    {
        generateStartCode(fp);
        // first, we just need to generate labels and code for each of the module
        astNode *child = root->children->sibling; // we can skip module declarations

        while (child)
        {
            // generate code for each child
            generateCode(child, currTable, globalTable, fp);

            child = child->sibling;
        }

        // now that we have generated code for all the modules and the driver, we can now have to create labels for exit
        generateExitAndErrorLabels(fp);
        generateEndCode(fp);
    }
    else if (root->variableNo == DRIVER)
    {
        // generate code for the driver module

        // first, we need to create a label for the driver module
        fprintf(fp, "main:\n");

        // Then we locate the entry for the driver module in the symbol table
        symbolTableEntry *driverEntry = lookUpInSymbolTable(currTable, "driver");

        // we allocate memory for all the local variables
        fprintf(fp, "push rbp\n");
        fprintf(fp, "mov rbp, rsp\n");
        fprintf(fp, "sub rsp, %d\n", 2*driverEntry->info->ifModule.nested_table->actualOffset);

        // now we generate code for the statements in the driver module
        astNode *stmtsNode = root->children->children;
        generateCode(stmtsNode, driverEntry->info->ifModule.nested_table, globalTable, fp);

        // now we need to return from the driver module
        // put goto to the label for exit
        zeroAllRegisters(fp);
        fprintf(fp, "mov rsp, rbp\n");
        fprintf(fp, "pop rbp\n");
        fprintf(fp, "ret\n");
    }

    else if (root->variableNo == search(CharToNoMapping, "<otherModules"))
    {
        // go to each module and recursively call on each module
        astNode *child = root->children;

        while (child)
        {
            // generate code for each child
            generateCode(child, currTable, globalTable, fp);

            // move to next child
            child = child->sibling;
        }
        // when we have no more children, we can return
    }
    else if (root->variableNo == MODULE)
    {
        // generate code for the module
        astNode* outputListNode = root->children->sibling->sibling->children;

        // first, we need to create a label for the module
        fprintf(fp, "%s:\n", root->children->token->lexeme);

        // We have the return address at the top of the stack
        // We pop the top of the stack and store it in rax
        fprintf(fp, "pop rax\n");

        // We have allocated space for return address at rbp + 8
        // We store rax at rbp + 8
        fprintf(fp, "mov [rbp + 8], rax\n");

        // Then we locate the entry for the module in the symbol table
        symbolTableEntry *moduleEntry = lookUpInSymbolTable(currTable, root->children->token->lexeme);
        int offset_including_IO = moduleEntry->info->ifModule.nested_table->actualOffset;

        // subtract rsp from rbp and store it in rax
        fprintf(fp, "mov rax, rbp\n");
        fprintf(fp, "sub rax, rsp\n");
        // rax gives the already allocated offset for the module
        // move the table actual offset to rbx
        fprintf(fp, "mov rbx, %d\n", offset_including_IO);
        // subtract rbx from rax and store it in rax
        fprintf(fp, "sub rax, rbx\n");
        // add rax to rsp
        // now rax is the negative of required allocation
        // we allocate memory for all the local variables
        fprintf(fp, "add rax, rsp\n");
        fprintf(fp, "mov rsp, rax\n");

        // now we generate code for the statements in the module
        astNode *stmtsNode = root->children->sibling->sibling->sibling->children;
        generateCode(stmtsNode, moduleEntry->info->ifModule.nested_table, globalTable, fp);

        while( outputListNode != NULL && outputListNode->sibling != NULL)
        {
            outputListNode = outputListNode->sibling;
        }
        if(outputListNode != NULL)
        {
            // get the symbol table entry for output List Node
            symbolTableEntry *outputEntry = lookUpInSymbolTable(moduleEntry->info->ifModule.nested_table, outputListNode->token->lexeme);

            // get the offset of the output variable
            int offset = outputEntry->info->ifVariable.actualOffset;

            // move the offset to rax
            fprintf(fp, "mov rax, %d\n", offset);
            // add 8 to offset
            fprintf(fp, "add rax, 8\n");
            // subtract rbp from rax
            fprintf(fp, "sub rax, rbp\n");
            // take negative of rax
            fprintf(fp, "neg rax\n");
            // move rax tp rsp
            fprintf(fp, "mov rsp, rax\n");
        }

        // return address is stored in rbp+4
        // we move it to rax
        fprintf(fp, "mov rax, [rbp + 8]\n");
        // we push it to the top of the stack
        fprintf(fp, "push rax\n");
        // We need to return from the module
        //  Zero all registers and use ret to return
        zeroAllRegisters(fp);
        fprintf(fp, "ret\n");
    }
    else if (root->variableNo == search(CharToNoMapping, "<statements>"))
    {
        // go to each statement and recursively call on each statement
        astNode *child = root->children;

        while (child)
        {
            // generate code for each child
            generateCode(child, currTable, globalTable, fp);

            // move to next child
            child = child->sibling;
        }
    }
    else if (root->variableNo == search(CharToNoMapping, "<moduleReuseStmt>"))
    {
        // generate code for the module reuse statement

        astNode *optionalNode = root->children;
        astNode *idNode = optionalNode->sibling;
        astNode *actualParameterList = idNode->sibling;

        // get the entry for the module in the symbol table
        symbolTableEntry *moduleEntry = lookUpInSymbolTable(globalTable, idNode->token->lexeme);
        DATA_TYPE_NODE *inputTypesListNode = moduleEntry->info->ifModule.input_plist;
        DATA_TYPE_NODE *outputTypesListNode = moduleEntry->info->ifModule.output_plist;

        // first we need to allocate space for the return address
        // We subtract 8 from rsp
        fprintf(fp, "sub rsp, 8\n");

        // Now we need to push the base pointer to the stack
        // We push rbp to the stack
        fprintf(fp, "push rbp\n");
        // We move rsp to rbp
        fprintf(fp, "mov rbp, rsp\n");

        // Now we need to push the input variables to the stack top
        astNode *inputListNode = actualParameterList->children;

        while (inputListNode)
        {
            // generate code for each input
            if (inputListNode->variableNo == TRUE)
            {
                // move 1 to rax
                fprintf(fp, "mov rax, 1\n");
                // push rax to the stack
                fprintf(fp, "push rax\n");
            }
            else if (inputListNode->variableNo == FALSE)
            {
                // move 0 to rax
                fprintf(fp, "mov rax, 0\n");
                // push rax to the stack
                fprintf(fp, "push rax\n");
            }
            else
            {
                // this means that the node is <sign>

                astNode *signNode = inputListNode->children;

                if (signNode->children != NULL)
                {
                    // This means that there a unary operator before the ID
                    // We have already done the type checking, so we know that the resulting value will either be a real or an integer

                    // We can use the generateExpression Code function to store the result of the expression in rax
                    generateExpressionCodeForModuleCall(signNode->children, currTable, globalTable, fp);

                    // The result is stored in rax
                    // We need to push it to the stack
                    fprintf(fp, "push rax\n");
                }
                else
                {
                    // This means that the resulting expression can be anything
                    // We can get Type from inputTypesListNode

                    if ((inputTypesListNode->type.primitiveType == INTEGER || inputTypesListNode->type.primitiveType == BOOLEAN) && inputTypesListNode->type.arrType == NOT_ARRAY)
                    {
                        // We can use the generateExpression Code function to store the result of the expression in rax
                        generateExpressionCodeForModuleCall(signNode->children, currTable, globalTable, fp);

                        // The result is stored in rax
                        // We need to push it to the stack
                        fprintf(fp, "push rax\n");
                    }
                    else if (inputTypesListNode->type.primitiveType == REAL && inputTypesListNode->type.arrType == NOT_ARRAY)
                    {
                        // we need to use floating point registers
                    }
                    else
                    {
                        // This means that the input is an array
                        // We need to push the address of the array to the stack

                        // move the value stored in rbp to rax
                        fprintf(fp, "mov rax, [rbp]\n");
                        // subtract rbp from rax
                        fprintf(fp, "sub rax, rbp\n");

                        // get the symbol table entry for the variable
                        symbolTableEntry *IDentry = lookUpInSymbolTable(currTable, signNode->sibling->token->lexeme);
                        int offset = IDentry->info->ifVariable.actualOffset;

                        // subtract offset from rax
                        fprintf(fp, "sub rax, %d\n", offset);

                        // add rax to rbp
                        fprintf(fp, "add rax, rbp\n");
                        // subtract 8 from rax
                        fprintf(fp, "sub rax, 8\n");
                        // push rax to the stack
                        fprintf(fp, "push rax\n");

                        if (IDentry->info->ifVariable.type.arrType == STATIC)
                        {
                            // we need to push left and right bounds to the stack
                            // move left bound to rax
                            fprintf(fp, "mov rax, %d\n", IDentry->info->ifVariable.type.leftBound);
                            // push rax to the stack
                            fprintf(fp, "push rax\n");
                            // move right bound to rax
                            fprintf(fp, "mov rax, %d\n", IDentry->info->ifVariable.type.rightBound);
                            // push rax to the stack
                            fprintf(fp, "push rax\n");
                        }
                        else
                        {
                            // The left and right bounds are stored just after the array address
                            // mov the value stored at rbp to rax
                            fprintf(fp, "mov rax, [rbp]\n");
                            // move value stored in rax - 8 to rbx
                            fprintf(fp, "mov rbx, [rax - 8]\n");
                            // push rbx to the stack
                            fprintf(fp, "push rbx\n");
                            // move value stored in rax - 16 to rbx
                            fprintf(fp, "mov rbx, [rax - 16]\n");
                            // push rbx to the stack
                            fprintf(fp, "push rbx\n");
                        }

                        // The result is stored in rax
                        // We need to push it to the stack
                        fprintf(fp, "push rax\n");
                    }
                }
            }

            // move to next input
            inputListNode = inputListNode->sibling;
            inputTypesListNode = inputTypesListNode->next;
        }

        // now we have pushed the input variables on to the stack top
        //next we allocate memory for the output variables
        while(outputTypesListNode)
        {
            // we need to allocate memory for each output variable
            // we donot need to check if the output is an array or not because we have already done the type checking

            // move 0 to rax
            fprintf(fp, "mov rax, 0\n");
            // push rax to the stack
            fprintf(fp, "push rax\n");

            outputTypesListNode = outputTypesListNode->next;
        }
        // we need to call the function

        // call the label of the function
        fprintf(fp, "call %s\n",idNode->token->lexeme);

        // now we need to pop the output variables from the stack and assign them to the output variables
        // we need to pop the output variables in the reverse order

        // we need to pop the output variables in the reverse order
        if(optionalNode->children)
            generateRecursivePop(optionalNode->children->children, currTable, globalTable, fp);

        // now we need to pop the input variables from the stack
        // just move the stack pointer to base pointer
        fprintf(fp, "mov rsp, rbp\n");
        // pop the base pointer
        fprintf(fp, "pop rbp\n");
        // pop the return address
        fprintf(fp, "pop rax\n");
        // make rax 0
        fprintf(fp, "mov rax, 0\n");
    }
    else if(root->variableNo == FOR)
    {
        // we need to generate code for the loop
        char* lexeme = root->token->lexeme;
        // get the symbol table entry for the lexeme
        symbolTableEntry* forEntry = lookUpInSymbolTable(currTable, lexeme);


        astNode* idNode = root->children;
        symbolTableEntry* idEntry = searchInTables(forEntry->info->ifLoop.nested_table, idNode->token->lexeme, idNode->token->lineNo);
        int leftBound = forEntry->info->ifLoop.leftBound;
        int rightBound = forEntry->info->ifLoop.rightBound;

        int offset = idEntry->info->ifVariable.actualOffset;
        // move rbp to rax
        fprintf(fp, "mov rax, rbp\n");
        // subtract offset from rax
        fprintf(fp, "sub rax, %d\n", offset);
        // subtract 8 from rax
        fprintf(fp, "sub rax, 8\n");
        // move left bound to memory pointed by rax
        fprintf(fp, "mov rbx, %d\nmov [rax], rbx\n", leftBound);

        // put the label for the for loop
        fprintf(fp, "%s:\n", lexeme);

        // move rbp to rax
        fprintf(fp, "mov rax, rbp\n");
        // subtract offset from rax
        fprintf(fp, "sub rax, %d\n", offset);
        // subtract 8 from rax
        fprintf(fp, "sub rax, 8\n");
        // move the calue stored in memory pointed by rax to rcx
        fprintf(fp, "mov rcx, [rax]\n");

        // mov right bound to rbx
        if(leftBound < rightBound)
            fprintf(fp, "mov rbx, %d\n", rightBound+1);
        else
            fprintf(fp, "mov rbx, %d\n", rightBound-1);
        // compare rcx and rbx
        fprintf(fp, "cmp rcx, rbx\n");
        // if equal to zero jum to exit
        fprintf(fp, "jz end_%s\n", lexeme);

        // generate code for the statements
        generateCode(root->children->sibling->sibling, forEntry->info->ifLoop.nested_table, globalTable, fp);

        // get the value of variable
        fprintf(fp, "mov rax, rbp\n");
        // subtract offset from rax
        fprintf(fp, "sub rax, %d\n", offset);
        // subtract 8 from rax
        fprintf(fp, "sub rax, 8\n");
        // move the value pointed by rax to rcx
        fprintf(fp, "mov rcx, [rax]\n");


        if(leftBound < rightBound)
        {
            // increment rcx by 1
            fprintf(fp, "inc rcx\n");
        }
        else
        {
            // decrement rcx by 1
            fprintf(fp, "dec rcx\n");
        }

        // move rcx to memory pointed by rax
        fprintf(fp, "mov [rax], rcx\n");

        // jump to the label of the for loop
        fprintf(fp, "jmp %s\n", lexeme);

        // put the label for the exit of the for loop
        fprintf(fp, "end_%s:\n", lexeme);
    }
    else if(root->variableNo == SWITCH)
    {
        symbolTableEntry* switchEntry = lookUpInSymbolTable(currTable, root->token->lexeme);

        astNode* idNode = root->children;
        astNode* caseStmts = idNode->sibling;
        astNode* defaultNode = caseStmts->sibling;

        // get symbolTable entry for the id
        symbolTableEntry* idEntry = searchInTables(currTable, idNode->token->lexeme, idNode->token->lineNo);

        // get the offset of the variable
        int offset = idEntry->info->ifVariable.actualOffset;

        // move rbp to rax
        fprintf(fp, "mov rax, rbp\n");
        // subtract offset from rax
        fprintf(fp, "sub rax, %d\n", offset);
        // subtract 8 from rax
        fprintf(fp, "sub rax, 8\n");
        // move the value pointed by rax to rcx
        fprintf(fp, "mov rcx, [rax]\n");

        astNode* caseStmtsNode = caseStmts->children;


        while (caseStmtsNode)
        {
            // label for the case
            fprintf(fp, "case_%s_%s:\n", caseStmtsNode->token->lexeme, root->token->lexeme);
            if(caseStmtsNode->token->tokenType == TRUE)
                fprintf(fp, "cmp rcx, 1\n");
            else if(caseStmtsNode->token->tokenType == FALSE)
                fprintf(fp, "cmp rcx, 0\n");
            else
                fprintf(fp, "cmp rcx, %s\n", caseStmtsNode->token->lexeme);

            if(caseStmtsNode->sibling)
                fprintf(fp, "jnz case_%s_%s\n", caseStmtsNode->sibling->token->lexeme, root->token->lexeme);
            else if(defaultNode)
                fprintf(fp, "jnz default_%s\n", root->token->lexeme);
            // else
            //     fprintf(fp, "jnz end_%s\n", root->token->lexeme);

            symbolTableEntry* caseEntry = lookUpInSymbolTable(switchEntry->info->ifConditional.nested_table, caseStmtsNode->token->lexeme);

            generateCode(caseStmtsNode->children, caseEntry->info->ifCase.nested_table, globalTable, fp);
            caseStmtsNode = caseStmtsNode->sibling; fprintf(fp, "jmp end_%s\n", root->token->lexeme);
        }

        if(defaultNode)
        {
            fprintf(fp, "default_%s:\n", root->token->lexeme);
            symbolTableEntry* defaultEntry = lookUpInSymbolTable(switchEntry->info->ifConditional.nested_table, "default");

            generateCode(defaultNode->children, defaultEntry->info->ifCase.nested_table, globalTable, fp);
            fprintf(fp, "jmp end_%s\n", root->token->lexeme);
        }

        fprintf(fp, "end_%s:\n", root->token->lexeme);
    }
        //todo
    else if(root->variableNo == search(CharToNoMapping, "<assignmentStmt>"))
    {
        astNode* idNode = root->children;
        astNode* assnType = idNode->sibling;
        symbolTableEntry* idEntry = searchInTables(currTable, idNode->token->lexeme, idNode->token->lineNo);
        int offset = idEntry->info->ifVariable.actualOffset;

        if(assnType->variableNo == search(CharToNoMapping, "<lvalueIDStmt>"))
        {
            astNode* exprNode = assnType->children;


            if(idEntry->info->ifVariable.type.arrType != NOT_ARRAY)
            {
                // todo
                if(idEntry->info->ifVariable.type.arrType == STATIC)
                {
                    if(exprNode->variableNo == ID)
                    {
                        symbolTableEntry* rhsEntry = searchInTables(currTable, exprNode->token->lexeme, exprNode->token->lineNo);
                        if(rhsEntry->info->ifVariable.type.arrType == STATIC)
                        {
                            int rhsOffset = rhsEntry->info->ifVariable.type.leftBound;
                        }
                    }
                }
            }
            else
            {
                generateExpressionCode(exprNode, currTable, globalTable, fp);
                fprintf(fp, "pop rbx\n");
                fprintf(fp, "mov rax, rbp\n");
                fprintf(fp, "sub rax, 8\n");
                fprintf(fp, "sub rax, %d\n", offset);
                fprintf(fp, "mov [rax], rbx\n");
            }

        }
        else
        {

        }

    }
    else if(root->variableNo == search(CharToNoMapping, "<ioStmt>"))
    {
        if(root->children->variableNo == GET_VALUE)
        {
            astNode* idNode = root->children->sibling;

            symbolTableEntry* idEntry = searchInTables(currTable, idNode->token->lexeme,  idNode->token->lineNo);
            int offset = idEntry->info->ifVariable.actualOffset;

            DATA_TYPE idType = idEntry->info->ifVariable.type;
            pushAllRegisters(fp);

            if(idType.arrType == NOT_ARRAY)
            {
                if(idType.primitiveType == BOOLEAN_TYPE)
                {
                    fprintf(fp, "mov rdi, ip_bool\n");
                    pushAllRegisters(fp);
                    fprintf(fp, "call printf\n");
                    popAllRegisters(fp);
                    // get the offset of the variable
                    // mov rbp to rax
                    fprintf(fp, "mov rax, rbp\n");
                    // subtract offset from rax
                    fprintf(fp, "sub rax, %d\n", offset);
                    // subtract 8 from rax
                    fprintf(fp, "sub rax, 8\n");
                    fprintf(fp, "lea rsi, [rax]\n");
                    fprintf(fp, "mov rdi, f_ip_bool");
                    fprintf(fp, "call scanf\n");

                }
                else if(idType.primitiveType == INTEGER_TYPE)
                {
                    fprintf(fp, "mov rdi, ip_int\n");
                    pushAllRegisters(fp);
                    fprintf(fp, "call printf\n");
                    popAllRegisters(fp);

                    // get the offset of the variable
                    // mov rbp to rax
                    fprintf(fp, "mov rax, rbp\n");
                    // subtract offset from rax
                    fprintf(fp, "sub rax, %d\n", offset);
                    // subtract 8 from rax
                    fprintf(fp, "sub rax, 8\n");
                    fprintf(fp, "lea rsi, [rax]\n");
                    fprintf(fp, "mov rdi, f_ip_int\n");
                    fprintf(fp, "call scanf\n");
                }
                else if(idType.primitiveType == REAL_TYPE)
                {
                    fprintf(fp, "mov rdi, ip_real\n");
                    pushAllRegisters(fp);
                    fprintf(fp, "call printf\n");
                    popAllRegisters(fp);
                    // get the offset of the variable
                    // mov rbp to rax
                    fprintf(fp, "mov rax, rbp\n");
                    // subtract offset from rax
                    fprintf(fp, "sub rax, %d\n", offset);
                    // subtract 8 from rax
                    fprintf(fp, "sub rax, 8\n");
                    fprintf(fp, "lea rsi, [rax]\n");
                    fprintf(fp, "mov rdi, f_ip_real");
                    fprintf(fp, "call scanf\n");
                }

            }
            else if(idType.arrType == STATIC)
            {
                int leftBound = idType.leftBound;
                int rightBound = idType.rightBound;



                if(idType.primitiveType == BOOLEAN_TYPE)
                {
                    fprintf(fp, "mov rdi, ip_arrbool1\n");
                    fprintf(fp, "mov rsi, %d\n", rightBound-leftBound+1);
                    pushAllRegisters(fp);
                    fprintf(fp, "call printf\n");
                    popAllRegisters(fp);
                    fprintf(fp, "mov rdi, ip_arrbool2\n");
                    fprintf(fp, "mov rsi, %d", leftBound);
                    pushAllRegisters(fp);
                    fprintf(fp, "call printf\n");
                    popAllRegisters(fp);
                    fprintf(fp, "mov rdi, ip_arrbool3\n");
                    fprintf(fp, "mov rsi, %d\n", rightBound);
                    pushAllRegisters(fp);
                    fprintf(fp, "call printf\n");
                    popAllRegisters(fp);

                    // get the offset of the variable
                    // mov rbp to rax
                    fprintf(fp, "mov rax, rbp\n");
                    // subtract offset from rax
                    fprintf(fp, "sub rax, %d\n", offset);
                    // subtract 8 from rax
                    fprintf(fp, "sub rax, 8\n");
                    fprintf(fp, "mov rcx, %d\n", leftBound);
                    fprintf(fp, "for_%s_%d:\n", idNode->token->lexeme, idNode->token->lineNo);
                    fprintf(fp, "cmp rcx, %d\n", rightBound+1);
                    fprintf(fp, "jz end_for_%s_%d\n", idNode->token->lexeme, idNode->token->lineNo);
                    fprintf(fp, "sub rax, 8\n");
                    fprintf(fp, "lea rsi, [rax]\n");
                    fprintf(fp, "mov rdi, f_in_arr_bool\n");
                    fprintf(fp, "push rax\n");
                    fprintf(fp, "push rcx\n");
                    fprintf(fp, "call scanf\n");
                    fprintf(fp, "pop rcx\n");
                    fprintf(fp, "pop rax\n");
                    fprintf(fp, "inc rcx\n");
                    fprintf(fp, "jmp for_%s_%d\n", idNode->token->lexeme, idNode->token->lineNo);
                    fprintf(fp, "end_for_%s_%d:\n", idNode->token->lexeme, idNode->token->lineNo);


                }
                else if(idType.primitiveType == INTEGER_TYPE)
                {
                    fprintf(fp, "mov rdi, ip_arrint1\n");
                    fprintf(fp, "mov rsi, %d", rightBound-leftBound+1);
                    pushAllRegisters(fp);
                    fprintf(fp, "call printf\n");
                    popAllRegisters(fp);
                    fprintf(fp, "mov rdi, ip_arrint2\n");
                    fprintf(fp, "mov rsi, %d", leftBound);
                    pushAllRegisters(fp);
                    fprintf(fp, "call printf\n");
                    popAllRegisters(fp);
                    fprintf(fp, "mov rdi, ip_arrint3\n");
                    fprintf(fp, "mov rsi, %d", rightBound);
                    pushAllRegisters(fp);
                    fprintf(fp, "call printf\n");
                    popAllRegisters(fp);

                    // get the offset of the variable
                    // mov rbp to rax
                    fprintf(fp, "mov rax, rbp\n");
                    // subtract offset from rax
                    fprintf(fp, "sub rax, %d\n", offset);
                    // subtract 8 from rax
                    fprintf(fp, "sub rax, 8\n");
                    fprintf(fp, "mov rcx, %d\n", leftBound);
                    fprintf(fp, "for_%s_%d:\n", idNode->token->lexeme, idNode->token->lineNo);
                    fprintf(fp, "cmp rcx, %d\n", rightBound+1);
                    fprintf(fp, "jz end_for_%s_%d\n", idNode->token->lexeme, idNode->token->lineNo);
                    fprintf(fp, "sub rax, 8\n");
                    fprintf(fp, "lea rsi, [rax]\n");
                    fprintf(fp, "mov rdi, f_in_arr_int\n");
                    fprintf(fp, "push rax\n");
                    fprintf(fp, "push rcx\n");
                    fprintf(fp, "call scanf\n");
                    fprintf(fp, "pop rcx\n");
                    fprintf(fp, "pop rax\n");
                    fprintf(fp, "inc rcx\n");
                    fprintf(fp, "jmp for_%s_%d\n", idNode->token->lexeme, idNode->token->lineNo);
                    fprintf(fp, "end_for_%s_%d:\n", idNode->token->lexeme, idNode->token->lineNo);
                }
                else if(idType.primitiveType == REAL_TYPE)
                {
                    fprintf(fp, "mov rdi, ip_arrreal1\n");
                    fprintf(fp, "mov rsi, %d", rightBound-leftBound+1);
                    pushAllRegisters(fp);
                    fprintf(fp, "call printf\n");
                    popAllRegisters(fp);
                    fprintf(fp, "mov rdi, ip_arrreal2\n");
                    fprintf(fp, "mov rsi, %d", leftBound);
                    pushAllRegisters(fp);
                    fprintf(fp, "call printf\n");
                    popAllRegisters(fp);
                    fprintf(fp, "mov rdi, ip_arrreal3\n");
                    fprintf(fp, "mov rsi, %d", rightBound);
                    pushAllRegisters(fp);
                    fprintf(fp, "call printf\n");
                    popAllRegisters(fp);

                    // get the offset of the variable
                    // mov rbp to rax
                    fprintf(fp, "mov rax, rbp\n");
                    // subtract offset from rax
                    fprintf(fp, "sub rax, %d\n", offset);
                    // subtract 8 from rax
                    fprintf(fp, "sub rax, 8\n");
                    fprintf(fp, "mov rcx, %d\n", leftBound);
                    fprintf(fp, "for_%s_%d:\n", idNode->token->lexeme, idNode->token->lineNo);
                    fprintf(fp, "cmp rcx, %d\n", rightBound+1);
                    fprintf(fp, "jz end_for_%s_%d\n", idNode->token->lexeme, idNode->token->lineNo);
                    fprintf(fp, "sub rax, 8\n");
                    fprintf(fp, "lea rsi, [rax]\n");
                    fprintf(fp, "mov rdi, f_in_arr_real\n");
                    fprintf(fp, "push rax\n");
                    fprintf(fp, "push rcx\n");
                    fprintf(fp, "call scanf\n");
                    fprintf(fp, "pop rcx\n");
                    fprintf(fp, "pop rax\n");
                    fprintf(fp, "inc rcx\n");
                    fprintf(fp, "jmp for_%s_%d\n", idNode->token->lexeme, idNode->token->lineNo);
                    fprintf(fp, "end_for_%s_%d:\n", idNode->token->lexeme, idNode->token->lineNo);
                }


            }
            else
            {
                // dynamic array
                //todo


            }
            popAllRegisters(fp);
        }
        else
        {
            // root->child = print

            astNode* idNode = root->children->sibling;

            if(idNode->variableNo == NUM)
            {
                int num = idNode->token->tokenData.ifInt;
                fprintf(fp, "mov rdi, op_int\n");
                fprintf(fp, "mov rsi, %d\n", num);
                pushAllRegisters(fp);
                fprintf(fp, "call printf\n");
                popAllRegisters(fp);
            }
            else if(idNode->variableNo == RNUM)
            {
                //todo
            }
            else if(idNode->variableNo == TRUE)
            {
                fprintf(fp, "mov rdi, op_true\n");
                pushAllRegisters(fp);
                fprintf(fp, "call printf\n");
                popAllRegisters(fp);
            }
            else if(idNode->variableNo == FALSE)
            {
                fprintf(fp, "mov rdi, op_false\n");
                pushAllRegisters(fp);
                fprintf(fp, "call printf\n");
                popAllRegisters(fp);
            }
            else if( idNode->variableNo == ID)
            {
                symbolTableEntry* idEntry = searchInTables(currTable, idNode->token->lexeme, idNode->token->lineNo);
                int offset = idEntry->info->ifVariable.actualOffset;

                if(idEntry->info->ifVariable.type.arrType == NOT_ARRAY && idEntry->info->ifVariable.type.primitiveType == INTEGER_TYPE)
                {
                    fprintf(fp, "mov rdi, op_int\n");
                    // get the offset of the variable
                    // mov rbp to rax
                    fprintf(fp, "mov rax, rbp\n");
                    // subtract offset from rax
                    fprintf(fp, "sub rax, %d\n", offset);
                    // subtract 8 from rax
                    fprintf(fp, "sub rax, 8\n");
                    fprintf(fp, "mov rsi, [rax]\n");
                    pushAllRegisters(fp);
                    fprintf(fp, "call printf\n");
                    popAllRegisters(fp);
                }
                else if(idEntry->info->ifVariable.type.arrType == NOT_ARRAY && idEntry->info->ifVariable.type.primitiveType == REAL_TYPE)
                {
                    //todo
                }
                else if(idEntry->info->ifVariable.type.arrType == NOT_ARRAY && idEntry->info->ifVariable.type.primitiveType == BOOLEAN_TYPE)
                {
                    // get the offset of the variable
                    // mov rbp to rax
                    fprintf(fp, "mov rax, rbp\n");
                    // subtract offset from rax
                    fprintf(fp, "sub rax, %d\n", offset);
                    // subtract 8 from rax
                    fprintf(fp, "sub rax, 8\n");
                    fprintf(fp, "mov rcx, [rax]\n");

                    // compare rcx with 0
                    fprintf(fp, "cmp rcx, 0\n");
                    fprintf(fp, "jz falsep_%s_%d\n", idNode->token->lexeme, idNode->token->lineNo);
                    fprintf(fp, "mov rdi, op_true\n");
                    pushAllRegisters(fp);
                    fprintf(fp, "call printf\n");
                    popAllRegisters(fp);
                    fprintf(fp, "jmp endp_%s_%d\n", idNode->token->lexeme, idNode->token->lineNo);
                    fprintf(fp, "falsep_%s_%d:\n", idNode->token->lexeme, idNode->token->lineNo);
                    fprintf(fp, "mov rdi, op_false\n");
                    pushAllRegisters(fp);
                    fprintf(fp, "call printf\n");
                    popAllRegisters(fp);
                    fprintf(fp, "endp_%s_%d:\n", idNode->token->lexeme, idNode->token->lineNo);
                }
                else if(idEntry->info->ifVariable.type.arrType == STATIC && idEntry->info->ifVariable.type.primitiveType == INTEGER_TYPE && idNode->sibling == NULL)
                {
                    int leftBound = idEntry->info->ifVariable.type.leftBound;
                    int rightBound = idEntry->info->ifVariable.type.rightBound;
                    // get the offset of the variable
                    // mov rbp to rax
                    fprintf(fp, "mov rax, rbp\n");
                    // subtract offset from rax
                    fprintf(fp, "sub rax, %d\n", offset);
                    // subtract 8 from rax
                    fprintf(fp, "sub rax, 8\n");
                    fprintf(fp, "mov rcx, %d\n", leftBound);
                    fprintf(fp, "mov rdi, op_int_arr\n");
                    fprintf(fp, "mov rsi, [rax]\n");
                    pushAllRegisters(fp);
                    fprintf(fp, "call printf\n");
                    popAllRegisters(fp);
                    fprintf(fp, "for_%s_%d:\n", idNode->token->lexeme, idNode->token->lineNo);
                    fprintf(fp, "cmp rcx, %d\n", rightBound+1);
                    fprintf(fp, "jz end_for_%s_%d\n", idNode->token->lexeme, idNode->token->lineNo);
                    fprintf(fp, "sub rax, 8\n");
                    fprintf(fp, "mov rdi, oup_int\n");
                    fprintf(fp, "mov rsi, [rax]\n");
                    pushAllRegisters(fp);
                    fprintf(fp, "call printf\n");
                    popAllRegisters(fp);
                    fprintf(fp, "inc rcx\n");
                    fprintf(fp, "jmp for_%s_%d\n", idNode->token->lexeme, idNode->token->lineNo);
                    fprintf(fp, "end_for_%s_%d:\n", idNode->token->lexeme, idNode->token->lineNo);
                }
                else if(idEntry->info->ifVariable.type.arrType == STATIC && idEntry->info->ifVariable.type.primitiveType == REAL_TYPE)
                {
                    //todo
                }
                else if(idEntry->info->ifVariable.type.arrType == STATIC && idEntry->info->ifVariable.type.primitiveType == BOOLEAN_TYPE && idNode->sibling == NULL)
                {
                    int leftBound = idEntry->info->ifVariable.type.leftBound;
                    int rightBound = idEntry->info->ifVariable.type.rightBound;
                    // get the offset of the variable
                    // mov rbp to rax
                    fprintf(fp, "mov rax, rbp\n");
                    // subtract offset from rax
                    fprintf(fp, "sub rax, %d\n", offset);
                    // subtract 8 from rax
                    fprintf(fp, "sub rax, 8\n");
                    fprintf(fp, "mov rcx, %d\n", leftBound);
                    fprintf(fp, "for_%s_%d:\n", idNode->token->lexeme, idNode->token->lineNo);
                    fprintf(fp, "cmp rcx, %d\n", rightBound+1);
                    fprintf(fp, "jz end_for_%s_%d\n", idNode->token->lexeme, idNode->token->lineNo);
                    fprintf(fp, "sub rax, 8\n");
                    fprintf(fp, "mov rbx, [rax]\n");
                    fprintf(fp, "cmp rbx, 0\n");
                    fprintf(fp, "jz falsep_%s_%d\n", idNode->token->lexeme, idNode->token->lineNo);
                    fprintf(fp, "mov rdi, op_true\n");
                    pushAllRegisters(fp);
                    fprintf(fp, "call printf\n");
                    popAllRegisters(fp);
                    fprintf(fp, "jmp endp_%s_%d\n", idNode->token->lexeme, idNode->token->lineNo);
                    fprintf(fp, "falsep_%s_%d:\n", idNode->token->lexeme, idNode->token->lineNo);
                    fprintf(fp, "mov rdi, op_false\n");
                    pushAllRegisters(fp);
                    fprintf(fp, "call printf\n");
                    popAllRegisters(fp);
                    fprintf(fp, "endp_%s_%d:\n", idNode->token->lexeme, idNode->token->lineNo);
                    fprintf(fp, "inc rcx\n");
                    fprintf(fp, "jmp for_%s_%d\n", idNode->token->lexeme, idNode->token->lineNo);
                    fprintf(fp, "end_for_%s_%d:\n", idNode->token->lexeme, idNode->token->lineNo);
                }
                else if(idEntry->info->ifVariable.type.arrType == STATIC && idEntry->info->ifVariable.type.primitiveType == INTEGER_TYPE)
                {
                    astNode* indexArrNode = idNode->sibling;
                    astNode* signNode = indexArrNode->children;
                    astNode* indexNode = signNode->sibling;

                    int leftBound = idEntry->info->ifVariable.type.leftBound;
                    int rightBound = idEntry->info->ifVariable.type.rightBound;
                    int signFactor = 1;
                    if(signNode->children && signNode->children->token->tokenType == MINUS)
                    {
                        signFactor = -1;
                    }

                    if(indexNode->variableNo == NUM)
                    {
                        // mov NUM to rax
                        fprintf(fp, "mov rbx, %d\n", indexNode->token->tokenData.ifInt);
                        fprintf(fp, "imul rbx, %d\n", signFactor);
                        fprintf(fp, "cmp rbx, %d\n", leftBound);
                        fprintf(fp, "jl err_index_out_of_bounds\n");
                        fprintf(fp, "cmp rbx, %d\n", rightBound);
                        fprintf(fp, "jg err_index_out_of_bounds\n");
                        fprintf(fp, "mov rax, rbp\n");
                        // subtract offset from rax
                        fprintf(fp, "sub rax, %d\n", offset);
                        // subtract 8 from rax
                        fprintf(fp, "sub rax, 8\n");
                        // subtract leftBound from index
                        fprintf(fp, "sub rbx, %d\n", leftBound);
                        // multiply by 8
                        fprintf(fp, "imul rbx, 8\n");
                        // subtract it from rax
                        fprintf(fp, "sub rax, rbx\n");
                        // subtract 8 from rax
                        fprintf(fp, "sub rax, 8\n");
                        // get the value at rax
                        fprintf(fp, "mov rbx, [rax]\n");
                        fprintf(fp, "mov rdi, op_int\n");
                        fprintf(fp, "mov rsi, rbx\n");
                        pushAllRegisters(fp);
                        fprintf(fp, "call printf\n");
                        popAllRegisters(fp);
                    }
                    else
                    {

                        symbolTableEntry* indexEntry = searchInTables(currTable, indexNode->token->lexeme, indexNode->token->lineNo);
                        int indexOffset = indexEntry->info->ifVariable.actualOffset;
                        // get the offset of the index
                        // mov rbp to rax
                        fprintf(fp, "mov rax, rbp\n");
                        // subtract offset from rax
                        fprintf(fp, "sub rax, %d\n", indexOffset);
                        // subtract 8 from rax
                        fprintf(fp, "sub rax, 8\n");
                        // compare with left bound and right bound of array  and exit if error
                        fprintf(fp, "mov rbx, [rax]\n");
                        // multiply by signFactor
                        fprintf(fp, "imul rbx, %d\n", signFactor);
                        fprintf(fp, "cmp rbx, %d\n", leftBound);
                        fprintf(fp, "jl err_index_out_of_bounds\n");
                        fprintf(fp, "cmp rbx, %d\n", rightBound);
                        fprintf(fp, "jg err_index_out_of_bounds\n");
                        // get the offset of the variable
                        // mov rbp to rax
                        fprintf(fp, "mov rax, rbp\n");
                        // subtract offset from rax
                        fprintf(fp, "sub rax, %d\n", offset);
                        // subtract 8 from rax
                        fprintf(fp, "sub rax, 8\n");
                        // subtract leftBound from index
                        fprintf(fp, "sub rbx, %d\n", leftBound);
                        // multiply by 8
                        fprintf(fp, "imul rbx, 8\n");
                        // subtract it from rax
                        fprintf(fp, "sub rax, rbx\n");
                        // subtract 8 from rax
                        fprintf(fp, "sub rax, 8\n");
                        // get the value at rax
                        fprintf(fp, "mov rbx, [rax]\n");
                        fprintf(fp, "mov rdi, op_int\n");
                        fprintf(fp, "mov rsi, rbx\n");
                        pushAllRegisters(fp);
                        fprintf(fp, "call printf\n");
                        popAllRegisters(fp);
                    }
                }
                else
                {
                    //todo
                }
            }
        }
    }
    else
    {
        return;
    }
}


void generateFinalCode(char *input, char *output)
{

    // initialize lookup table
    lookupTable = initializeLookupTable();
    FILE *fp = fopen(input, "r");

    if (fp == NULL)
    {
        printf("Error opening source code file.");
        return;
    }

    // creating and populating buffers
    // freeMappings();
    buff1 = malloc(BUFF_SIZE * sizeof(char));
    buff2 = malloc(BUFF_SIZE * sizeof(char));
    populateBuffer(fp, buff1);
    if (!readCodeCompletely)
        populateBuffer(fp, buff2);
    ParseTree *parseTree = readGrammar(output, fp);
    if (__SYNTAX_ERROR__)
    {
        printf("\033[31mSyntax Errors Found.. Symbol Table not generated\033[0m");
        fseek(fp, 0, SEEK_SET);
        fclose(fp);
        free(buff1);
        free(buff2);
        return;
    }
    AST *ast = createAST(parseTree);
    symbolTable *globalTable = createSymbolTable(NULL, NULL);
    symbolTable *defTable = createSymbolTable(NULL, NULL);


    PopulateSymbolTable(ast->root, globalTable, defTable);
    printf("Checking Semantic Errors...\n");
    semanticCheck(ast->root, globalTable, globalTable, defTable);
    if (__SEMANTIC_ERROR__){
        printf("\033[31mSemantic Errors Found...\033[0m\n");
        fseek(fp, 0, SEEK_SET);
        fclose(fp);
        free(buff1);
        free(buff2);
        return;
    }
    else
        printf("\033[32mCode Compiles Successfully...\033[0m\n");
//    printST(globalTable, "global", 0);
    FILE* asmfp = fopen(asmOutputFile, "w");


    generateCode(ast->root, globalTable, globalTable, asmfp);
    printf("ASM FILE GENERATED SUCCESSFULLY!! NAME: %s\n\n", asmOutputFile);
    fclose(asmfp);
    // printf("Hello \n%d \n", search(CharToNoMapping, "<program'>"));
    fseek(fp, 0, SEEK_SET);
    fclose(fp);
    free(buff1);
    free(buff2);
    // return ast;
}