/*

//////////////////////////TEAM//////////////////////////
BITS ID             Member Name
2020A7PS0073P       Shashank Agrawal
2020A7PS0096P       Akshat Gupta
2020A7PS0129P       Anish Ghule
2020A7PS0134P       Shadan Hussain
2020A7PS1513P       Tarak P V S

*/

#include <stdlib.h>
#include <stdio.h>
#include "ast.h"
#include "global.h"
#include "hashmap.h"
#include "lexer.h"
#include "global.h"
#include "parser.h"


//GLOBAL
char *astOutput = "astOutput.txt";
char *testfile = "semanicAnalysis_testcases/t4.txt";
char *treeOutput = "parseTreeOutput.txt";

astNode *createASTNode(tokenInfo *token, int variableNo)
{
    astNode *newNode = (astNode *)malloc(sizeof(astNode));
    newNode->token = token;
    newNode->variableNo = variableNo;
    newNode->parent = NULL;
    newNode->children = NULL;
    newNode->sibling = NULL;
    newNode->ignore = false;
    newNode->scope.start = -1;
    newNode->scope.end = -1;
    return newNode;
}

void joinNodes(astNode **siblings, int count, astNode *parent)
{
    // joining all the siblings
    siblings[0]->parent = parent;
    if (parent != NULL)
        parent->children = siblings[0];
    for (int i = 1; i < count; i++)
    {
        siblings[i - 1]->sibling = siblings[i];
        siblings[i]->parent = parent;
    }
}

astNode *addNodeToEnd(astNode *list, astNode *node, astNode *parent)
{
    if (node == NULL)
        return list;

    node->parent = parent;

    if (list == NULL)
        return list = node;
    else
    {
        astNode *curr = list;
        while (curr->sibling != NULL)
        {
            curr = curr->sibling;
        }
        curr->sibling = node;
    }
    return list;
}

astNode *addNodeToBeg(astNode *list, astNode *node, astNode *parent)
{
    if (node == NULL)
        return list;

    node->parent = parent;
    node->sibling = list;
    list = node;

    return list;
}

// takes a parseTreeNode and find ast of subtree rooted at that node
astNode *applyASTRules(ParseTreeNode *parentNode)
{
    astNode *node = NULL;

    // printf("Variable = %s, Rule = %d starting !\n", NoToCharMapping[parentNode->variableNo], parentNode->ruleNo);

    switch (parentNode->ruleNo)
    {
    case 0:
    {
        // <program'> -> <program> $
        node = applyASTRules(parentNode->children->head);
        break;
    }

    case 1:
    {
        // <program> -> <moduleDeclarations> <otherModules> <driverModule> <otherModules>

        astNode *children[4]; // array of addresses of 4 nodes md om dm om
        ParseTreeNode *curr = parentNode->children->head;
        node = createASTNode(parentNode->token, parentNode->variableNo);
        node->scope.start = 1;
        node->scope.end = lineNo;

        int i = 0;
        while (curr != NULL)
        {
            children[i] = applyASTRules(curr);
            curr = curr->sibling;
            i++;
        }

        joinNodes(children, 4, node);

        break;
    }

    case 2:
    {
        // <moduleDeclarations> -> <moduleDeclaration> <moduleDeclarations>

        // top down do nothing

        // for children
        // take node generated from bottom - up as node
        //  node -> children will act as syn
        node = applyASTRules(parentNode->children->head->sibling); // node with children as list

        // Bottom up step
        astNode *newNode = applyASTRules(parentNode->children->head); // new node for new declaration
        // newNode->parent = node; // setting the parent node to the new declaration node
        node->children = addNodeToBeg(node->children, newNode, node); // adding new node to the list of declarations
        break;
    }

    case 3:
    {
        //<moduleDeclarations> -> epsilon
        node = createASTNode(parentNode->token, parentNode->variableNo);
        break;
    }

    case 4:
    {
        // <moduleDeclaration> -> DECLARE MODULE ID SEMICOL
        ParseTreeNode *curr = parentNode->children->head->sibling->sibling;
        node = createASTNode(curr->token, curr->variableNo); // create a node corresponding to id
        break;
    }

    case 5:
    {
        //<otherModules> -> <module> <otherModules>
        // same as <moduleDeclarations> -> <moduleDeclaration> <moduleDeclarations>
        node = applyASTRules(parentNode->children->head->sibling);
        astNode *newNode = applyASTRules(parentNode->children->head);
        // newNode->parent = node;
        node->children = addNodeToBeg(node->children, newNode, node);
        break;
    }

    case 6:
    {
        //<otherModules> -> epsilon
        // same as <moduleDeclarations> -> epsilon
        node = createASTNode(parentNode->token, parentNode->variableNo);
        break;
    }

    case 7:
    {
        // <driverModule> -> DRIVERDEF DRIVER PROGRAM DRIVERENDDEF <moduleDef>
        // astNode for DRIVER
        ParseTreeNode *driverKeywordNode = parentNode->children->head->sibling;

        node = createASTNode(driverKeywordNode->token, driverKeywordNode->variableNo); // creating a node for <driverModule>
        // applying ast rules on <moduleDef> and joining to <driverModule>
        node->children = applyASTRules(parentNode->children->head->sibling->sibling->sibling->sibling);
        node->children->parent = node;
        node->scope.start = driverKeywordNode->token->lineNo;
        node->scope.end = node->children->scope.end;
        break;
    }

    case 8:
    {
        // <module> -> DEF MODULE /ID/ ENDDEF TAKES INPUT SQBO /<input_plist>/ SQBC SEMICOL /<ret>/ /<moduleDef>/
        // similar to <program> -> <moduleDeclarations> <otherModules> <driverModule> <otherModules>
        // ast Node for MODULE
        ParseTreeNode *moduleKeywordNode = parentNode->children->head->sibling;
        node = createASTNode(moduleKeywordNode->token, moduleKeywordNode->variableNo);

        ParseTreeNode *idNode = parentNode->children->head->sibling->sibling;
        ParseTreeNode *inputList = idNode->sibling->sibling->sibling->sibling->sibling;
        ParseTreeNode *ret = inputList->sibling->sibling->sibling;
        ParseTreeNode *moduleDef = ret->sibling;

        astNode *children[4];
        children[0] = createASTNode(idNode->token, idNode->variableNo); // chg_1
        // children[0]->parent = node;
        node->children = children[0];

        children[1] = applyASTRules(inputList);
        // children[1]->parent = node;

        children[2] = applyASTRules(ret);
        // children[2]->parent = node;

        children[3] = applyASTRules(moduleDef);
        // children[3]->parent = node;

        node->scope.start = node->token->lineNo;
        node->scope.end = children[3]->scope.end;

        joinNodes(children, 4, node);
        break;
    }

    case 9:
    {
        // <ret> -> RETURNS SQBO <output_plist> SQBC SEMICOL
        node = createASTNode(parentNode->token, parentNode->variableNo);
        node->children = applyASTRules(parentNode->children->head->sibling->sibling);

        astNode *curr = node->children;

        while (curr != NULL)
        {
            curr->parent = node;
            curr = curr->sibling;
        }

        break;
    }

    case 10:
    {
        // <ret> ->epsilon
        node = createASTNode(parentNode->token, parentNode->variableNo);
        break;
    }

    case 11:
    {
        // <input_plist> -> ID COLON <dataType> <N1>
        node = createASTNode(parentNode->token, parentNode->variableNo);

        ParseTreeNode *idNode = parentNode->children->head;
        ParseTreeNode *dataType = idNode->sibling->sibling;
        ParseTreeNode *n1 = dataType->sibling;

        astNode *children[1];

        children[0] = createASTNode(idNode->token, idNode->variableNo);
        node->children = children[0];

        children[0]->children = applyASTRules(dataType);
        children[0]->children->parent = children[0];

        joinNodes(children, 1, node);

        // joining the list generated from <N1> to input_plist
        // synList is the list pointer generated from <N1>
        astNode *synList = applyASTRules(n1);
        children[0]->sibling = synList;

        // setting the parent of synthesized list nodes to <input_plist> node
        while (synList != NULL)
        {
            synList->parent = node;
            synList = synList->sibling;
        }

        break;
    }

    case 12:
    {
        // <N1> -> COMMA ID COLON <dataType> <N1>

        ParseTreeNode *idNode = parentNode->children->head->sibling;
        ParseTreeNode *dataType = idNode->sibling->sibling;
        ParseTreeNode *n1 = dataType->sibling;

        astNode *children[1];
        children[0] = createASTNode(idNode->token, idNode->variableNo);

        children[0]->children = applyASTRules(dataType);
        children[0]->children->parent = children[0];

        joinNodes(children, 1, NULL);

        // joining the list generated from <N1> to input_plist
        astNode *synList = applyASTRules(n1);
        children[0]->sibling = synList;
        node = children[0];

        break;
    }

    case 13:
    {
        // <N1> -> epsilon
        node = NULL;
        break;
    }

    case 14:
    {
        // TOP DOWN APPROACH -- FOR LISTS
        // <output_plist> -> ID COLON <type> <N2>
        // similar to <input_plist> -> ID COLON <dataType> <N1>
        node = NULL;

        ParseTreeNode *idNode = parentNode->children->head;
        ParseTreeNode *type = idNode->sibling->sibling;
        ParseTreeNode *n2 = type->sibling;

        astNode *children[2];

        children[0] = createASTNode(idNode->token, idNode->variableNo);

        children[0]->children = applyASTRules(type);
        children[0]->children->parent = children[0];

        joinNodes(children, 1, NULL);

        // joining the list generated from <N2> to output_plist
        astNode *synList = applyASTRules(n2);
        children[0]->sibling = synList;

        node = children[0];
        break;
    }

    case 15:
    {
        // TOP DOWN ONLY
        // <N2> -> COMMA ID COLON <type> <N2>
        // similar to <N1> -> COMMA ID COLON <dataType> <N1>

        ParseTreeNode *idNode = parentNode->children->head->sibling;
        ParseTreeNode *type = idNode->sibling->sibling;
        ParseTreeNode *n2 = type->sibling;

        astNode *children[2];
        children[0] = createASTNode(idNode->token, idNode->variableNo);

        children[0]->children = applyASTRules(type);
        children[0]->children->parent = children[0];

        joinNodes(children, 1, NULL);

        // joining the list generated from <N1> to input_plist
        astNode *synList = applyASTRules(n2);
        children[0]->sibling = synList;
        node = children[0];

        break;
    }

    case 16:
    {
        // <N2> -> epsilon
        node = NULL;
        break;
    }

    case 17:
    {
        // <dataType> -> ARRAY SQBO <range_arrays> SQBC OF <type>

        ParseTreeNode *rangeArray = parentNode->children->head->sibling->sibling;
        ParseTreeNode *type = rangeArray->sibling->sibling->sibling;

        node = applyASTRules(type);
        node->children = applyASTRules(rangeArray);
        node->children->parent = node;

        break;
    }

    case 18:
    {
        // <dataType> -> <type>
        node = applyASTRules(parentNode->children->head);
        break;
    }

    case 19:
    {
        // <range_arrays> -> <index_arr> RANGEOP <index_arr>

        node = createASTNode(parentNode->token, parentNode->variableNo);

        ParseTreeNode *indexArr1 = parentNode->children->head;
        ParseTreeNode *indexArr2 = indexArr1->sibling->sibling;

        astNode *children[2];

        children[0] = applyASTRules(indexArr1);
        node->children = children[0];

        children[1] = applyASTRules(indexArr2);

        joinNodes(children, 2, node);

        break;
    }

    case 20:
    {
        // <type> -> INTEGER

        ParseTreeNode *typeNode = parentNode->children->head;
        node = createASTNode(typeNode->token, typeNode->variableNo);

        break;
    }

    case 21:
    {
        // <type> -> REAL
        // same as case 20

        ParseTreeNode *typeNode = parentNode->children->head;
        node = createASTNode(typeNode->token, typeNode->variableNo);

        break;
    }

    case 22:
    {
        // <type> -> BOOLEAN
        // same as case 21

        ParseTreeNode *typeNode = parentNode->children->head;
        node = createASTNode(typeNode->token, typeNode->variableNo);

        break;
    }

    case 23:
    {
        // <index_arr> -> <sign> <new_index>
        node = createASTNode(parentNode->token, parentNode->variableNo);

        ParseTreeNode *sign = parentNode->children->head;
        ParseTreeNode *newIndex = sign->sibling;

        astNode *children[2];
        children[0] = applyASTRules(sign);
        // node->children = children[0];
        children[1] = applyASTRules(newIndex);

        joinNodes(children, 2, node);

        break;
    }

    case 24:
    {
        // <new_index> -> NUM
        ParseTreeNode *numNode = parentNode->children->head;
        node = createASTNode(numNode->token, numNode->variableNo);
        break;
    }

    case 25:
    {
        // <new_index> -> ID
        ParseTreeNode *idNode = parentNode->children->head;
        node = createASTNode(idNode->token, idNode->variableNo);
        break;
    }

    case 26:
    {
        // <sign> -> PLUS
        ParseTreeNode *plusNode = parentNode->children->head;
        node = createASTNode(parentNode->token, parentNode->variableNo);
        node->children = createASTNode(plusNode->token, plusNode->variableNo);
        node->children->parent = node;
        break;
    }

    case 27:
    {
        // <sign> -> MINUS
        ParseTreeNode *minusNode = parentNode->children->head;
        node = createASTNode(parentNode->token, parentNode->variableNo);
        node->children = createASTNode(minusNode->token, minusNode->variableNo);
        node->children->parent = node;
        break;
    }

    case 28:
    {
        // <sign> -> epsilon
        node = createASTNode(parentNode->token, parentNode->variableNo);
        break;
    }

    case 29:
    {
        //<moduleDef> -> START_TK <statements> END

        ParseTreeNode *startNode = parentNode->children->head;
        ParseTreeNode *endNode = startNode->sibling->sibling;

        node = createASTNode(parentNode->token, parentNode->variableNo);
        node->children = applyASTRules(parentNode->children->head->sibling);
        node->children->parent = node;
        node->scope.start = startNode->token->lineNo;
        node->scope.end = endNode->token->lineNo;
        break;
    }

    case 30:
    {
        //<statements> -> <statement> <statements>
        // similar to <otherModules> -> <module> <otherModules>

        node = applyASTRules(parentNode->children->head->sibling);

        astNode *newNode = applyASTRules(parentNode->children->head);
        node->children = addNodeToBeg(node->children, newNode, node);
        break;
    }

    case 31:
    {
        //<statements> -> epsilon
        node = createASTNode(parentNode->token, parentNode->variableNo);
        break;
    }

    case 32:
    {
        //<statement> -> <ioStmt>
        node = applyASTRules(parentNode->children->head);
        break;
    }

    case 33:
    {
        //<statement> -> <simpleStmt>
        node = applyASTRules(parentNode->children->head);
        break;
    }

    case 34:
    {
        //<statement> -> <declareStmt>
        node = applyASTRules(parentNode->children->head);
        break;
    }

    case 35:
    {
        //<statement> -> <conditionalStmt>
        node = applyASTRules(parentNode->children->head);
        break;
    }

    case 36:
    {
        //<statement> -> <iterativeStmt>
        node = applyASTRules(parentNode->children->head);
        break;
    }

    case 37:
    {
        // <ioStmt> -> /GET_VALUE/ BO /ID/ BC SEMICOL

        node = createASTNode(parentNode->token, parentNode->variableNo);

        // ParseTreenode for token GET_VALUE and ID
        ParseTreeNode *getValNode = parentNode->children->head;
        ParseTreeNode *idNode = getValNode->sibling->sibling;

        // astNode for token GET_VALUE and ID
        astNode *children[2];
        children[0] = createASTNode(getValNode->token, getValNode->variableNo);
        children[1] = createASTNode(idNode->token, idNode->variableNo);

        joinNodes(children, 2, node);
        break;
    }

    case 38:
    {
        // <ioStmt> -> PRINT BO <var_print> BC SEMICOL

        node = createASTNode(parentNode->token, parentNode->variableNo);

        // ParseTreenode for token PRINT and var
        ParseTreeNode *printNode = parentNode->children->head;
        ParseTreeNode *varNode = printNode->sibling->sibling;

        // astNode for token PRINT and var
        astNode *children[2];
        children[0] = createASTNode(printNode->token, printNode->variableNo);
        children[1] = applyASTRules(varNode);

        if(children[1]->sibling != NULL)
        {
            children[1]->sibling->parent = node;
        }

        joinNodes(children, 2, node);
        break;
    }

    case 39:
    {
        // <boolConstt> -> TRUE
        ParseTreeNode *trueNode = parentNode->children->head;
        node = createASTNode(trueNode->token, trueNode->variableNo);
        break;
    }

    case 40:
    {
        // <boolConstt> -> FALSE
        ParseTreeNode *falseNode = parentNode->children->head;
        node = createASTNode(falseNode->token, falseNode->variableNo);
        break;
    }

    case 41:
    {
        // <var_print> -> ID <P1>

        // parse tree node for ID and P1
        ParseTreeNode *idNode = parentNode->children->head;
        ParseTreeNode *p1Node = idNode->sibling;

        // ast node for ID and P1
        node = createASTNode(idNode->token, idNode->variableNo);
        node->sibling = applyASTRules(p1Node);

        if (node->sibling)
            node->sibling->parent = node;
        break;
    }

    case 42:
    {
        // <var_print> -> NUM
        ParseTreeNode *numNode = parentNode->children->head;
        node = createASTNode(numNode->token, numNode->variableNo);
        break;
    }

    case 43:
    {
        // <var_print> -> RNUM
        ParseTreeNode *rnumNode = parentNode->children->head;
        node = createASTNode(rnumNode->token, rnumNode->variableNo);
        break;
    }

    case 44:
    {
        // <var_print> -> <boolConstt>
        node = applyASTRules(parentNode->children->head);
        break;
    }

    case 45:
    {
        // <P1> -> SQBO <index_arr> SQBC

        // parse tree node for <index_arr>
        ParseTreeNode *indexNode = parentNode->children->head->sibling;

        node = applyASTRules(indexNode);

        break;
    }

    case 46:
    {
        // <P1> -> epsilon
        node = NULL;
        break;
    }

    case 47:
    {
        // <simpleStmt> -> <assignmentStmt>
        node = applyASTRules(parentNode->children->head);
        break;
    }

    case 48:
    {
        // <simpleStmt> -> <moduleReuseStmt>
        node = applyASTRules(parentNode->children->head);
        break;
    }

    case 49:
    {
        // <assignmentStmt> -> ID <whichStmt>

        // ast node for assignmentStmt
        node = createASTNode(parentNode->token, parentNode->variableNo);

        // parse tree node for ID and whichStmt
        ParseTreeNode *idNode = parentNode->children->head;
        ParseTreeNode *whichStmtNode = idNode->sibling;

        // ast node for ID and whichStmt
        astNode *children[2];
        children[0] = createASTNode(idNode->token, idNode->variableNo);
        children[1] = applyASTRules(whichStmtNode);

        joinNodes(children, 2, node);
        break;
    }

    case 50:
    {
        // <whichStmt> -> <lvalueIDStmt>
        node = applyASTRules(parentNode->children->head);
        break;
    }

    case 51:
    {
        // <whichStmt> -> <lvalueARRStmt>
        node = applyASTRules(parentNode->children->head);
        break;
    }

    case 52:
    {
        // <lvalueIDStmt> -> ASSIGNOP <expression> SEMICOL

        // ast node for lvalueIDStmt
        node = createASTNode(parentNode->token, parentNode->variableNo);

        // parse tree node for expression
        ParseTreeNode *exprNode = parentNode->children->head->sibling;

        // ast node for expression
        astNode *children[1];
        children[0] = applyASTRules(exprNode);

        joinNodes(children, 1, node);
        break;
    }

    case 53:
    {
        // <lvalueARRStmt> -> SQBO /<element_index_with_expressions>/ SQBC ASSIGNOP /<expression>/ SEMICOL

        // ast node for lvalueARRStmt
        node = createASTNode(parentNode->token, parentNode->variableNo);

        // parse tree node for element_index_with_expressions and expression
        ParseTreeNode *indexNode = parentNode->children->head->sibling;
        ParseTreeNode *exprNode = indexNode->sibling->sibling->sibling;

        // ast node for element_index_with_expressions and expression
        astNode *children[2];
        children[0] = applyASTRules(indexNode);
        children[1] = applyASTRules(exprNode);

        joinNodes(children, 2, node);

        break;
    }

    case 54:
    {
        // <moduleReuseStmt> -> <optional> USE MODULE ID WITH PARAMETERS <actual_para_list> SEMICOL

        // ast node for moduleReuseStmt
        node = createASTNode(parentNode->token, parentNode->variableNo);

        // parse tree node for optional, ID and idList
        ParseTreeNode *optionalNode = parentNode->children->head;
        ParseTreeNode *idNode = optionalNode->sibling->sibling->sibling;
        ParseTreeNode *idListNode = idNode->sibling->sibling->sibling;

        // ast node for optional, ID and idList
        astNode *children[3];
        children[0] = applyASTRules(optionalNode);
        children[1] = createASTNode(idNode->token, idNode->variableNo);
        children[2] = applyASTRules(idListNode);

        joinNodes(children, 3, node);

        break;
    }

    case 55:
    {
        // <actual_para_list> -> <list_item> <actual_para_list'>
        // ast node for actual_para_list
        node = createASTNode(parentNode->token, parentNode->variableNo);

        // parse tree node for list_item and actual_para_list'
        ParseTreeNode *listItemNode = parentNode->children->head;
        ParseTreeNode *actualParaListNode = listItemNode->sibling;

        // ast node for list_item and actual_para_list'
        astNode *children[2];
        children[0] = applyASTRules(listItemNode);
        children[1] = applyASTRules(actualParaListNode);

        if (children[1])
            joinNodes(children, 2, node);
        else
            joinNodes(children, 1, node);

        astNode *temp = children[1];

        while (temp != NULL)
        {
            temp->parent = node;
            temp = temp->sibling;
        }

        break;
    }

    case 56:
    {
        // <actual_para_list'> -> COMMA <list_item> <actual_para_list'>

        // parse tree node for list_item and actual_para_list'
        ParseTreeNode *listItemNode = parentNode->children->head->sibling;
        ParseTreeNode *actualParaListNode = listItemNode->sibling;

        // ast node for list_item and actual_para_list'
        astNode *children[2];
        children[0] = applyASTRules(listItemNode);
        children[1] = applyASTRules(actualParaListNode);

        if (children[1])
            joinNodes(children, 2, NULL);
        node = children[0];
        break;
    }

    case 57:
    {
        // <actual_para_list'> -> epsilon
        node = NULL;
        break;
    }

    case 58:
    {
        // <list_item> -> <sign> <actual_list_item>

        // ast node for list_item
        node = createASTNode(parentNode->token, parentNode->variableNo);

        // parse tree node for sign and actual_list_item
        ParseTreeNode *signNode = parentNode->children->head;
        ParseTreeNode *actualListItemNode = signNode->sibling;

        // ast node for sign and actual_list_item
        astNode *children[2];
        children[0] = applyASTRules(signNode);
        children[1] = applyASTRules(actualListItemNode);

        joinNodes(children, 2, node);

        astNode *curr = children[1]->sibling;
        while (curr != NULL)
        {
            curr->parent = node;
            curr = curr->sibling;
        }

        break;
    }

    case 59:
    {
        // <list_item> -> <boolConstt>
        node = createASTNode(parentNode->token, parentNode->variableNo);

        node->children = applyASTRules(parentNode->children->head);
        node->children->parent = node;

        break;
    }

    case 60:
    {
        //<actual_list_item> -> NUM
        ParseTreeNode *numNode = parentNode->children->head;

        // ast node for NUM
        node = createASTNode(numNode->token, numNode->variableNo);

        break;
    }

    case 61:
    {
        //<actual_list_item> -> RNUM
        ParseTreeNode *rnumNode = parentNode->children->head;

        node = createASTNode(rnumNode->token, rnumNode->variableNo);

        break;
    }

    case 62:
    {
        // <actual_list_item> -> ID <N_11>
        // parse tree nodes for ID and N_11
        ParseTreeNode *idNode = parentNode->children->head;
        ParseTreeNode *n_11Node = idNode->sibling;

        // ast node for factor
        node = createASTNode(idNode->token, idNode->variableNo);
        node->children = applyASTRules(n_11Node);
        if (node->children != NULL)
            node->children->parent = node;

        break;
    }

    case 63:
    {
        // <N_11> -> SQBO <element_index_with_expressions> SQBC

        // parseTree node for element_index_with_expressions
        ParseTreeNode *elementIndexWithExpressionsNode = parentNode->children->head->sibling;

        // ast node for N_11
        node = applyASTRules(elementIndexWithExpressionsNode);

        break;
    }

    case 64:
    {
        // <N_11> -> epsilon
        node = NULL;
        break;
    }

    case 65:
    {
        // <optional> -> SQBO <idList> SQBC ASSIGNOP

        // parse tree node for idList
        ParseTreeNode *idListNode = parentNode->children->head->sibling;

        // ast node for optional
        node = createASTNode(parentNode->token, parentNode->variableNo);

        // ast node for idList
        astNode *idList = applyASTRules(idListNode);

        // join the nodes
        node->children = idList;
        node->children->parent = node;

        break;
    }

    case 66:
    {
        // <optional> -> epsilon

        // ast node for optional
        node = createASTNode(parentNode->token, parentNode->variableNo);

        break;
    }

    case 67:
    {
        // <idList> -> ID <N3>

        // parse tree node for ID and N3
        ParseTreeNode *idNode = parentNode->children->head;
        ParseTreeNode *n3Node = idNode->sibling;

        // ast Nodes for IDList. N3, ID
        node = createASTNode(parentNode->token, parentNode->variableNo);
        node->children = applyASTRules(n3Node);

        astNode *curr = node->children;
        while (curr != NULL)
        {
            curr->parent = node;
            curr = curr->sibling;
        }

        astNode *newNode = createASTNode(idNode->token, idNode->variableNo);

        // adding newNode to the beginning
        node->children = addNodeToBeg(node->children, newNode, node);

        break;
    }

    case 68:
    {
        // <N3>  -> COMMA ID <N3>

        // parse teee Node for ID, N3
        ParseTreeNode *idNode = parentNode->children->head->sibling;
        ParseTreeNode *N3 = idNode->sibling;

        // ast node for N3
        node = applyASTRules(N3);
        astNode *newNode = createASTNode(idNode->token, idNode->variableNo);

        // joining nodes
        node = addNodeToBeg(node, newNode, NULL);

        break;
    }

    case 69:
    {
        // <N3> -> epsilon

        node = NULL; // initializing node to null list

        break;
    }

    case 70:
    {
        // <expression> -> <arithmeticOrBooleanExpr>
        node = applyASTRules(parentNode->children->head);

        break;
    }

    case 71:
    {
        // <expression> -> <U>
        // similar to case 70

        node = applyASTRules(parentNode->children->head);

        break;
    }

    case 72:
    {
        // <U> -><unary_op> <new_NT>

        // parse tree nodes for unary_op and new_NT
        ParseTreeNode *unaryOpNode = parentNode->children->head;
        ParseTreeNode *newNTNode = unaryOpNode->sibling;

        // ast node for U
        node = createASTNode(parentNode->token, parentNode->variableNo);

        // ast nodes for unary_op and new_NT
        astNode *children[2];
        children[0] = applyASTRules(unaryOpNode);
        children[1] = applyASTRules(newNTNode);

        joinNodes(children, 2, node);

        break;
    }

    case 73:
    {
        // <new_NT> -> BO <arithmeticExpr> BC

        // parse Tree Node for arithmeticExpr
        ParseTreeNode *arithmeticExprNode = parentNode->children->head->sibling;

        // apply rule on arithmetic expr
        node = applyASTRules(arithmeticExprNode);

        break;
    }

    case 74:
    {
        // <new_NT> <var_id_num>

        // parse tree node for var_id_num
        ParseTreeNode *varIdNumNode = parentNode->children->head;

        // apply rule on var_id_num
        node = applyASTRules(varIdNumNode);

        break;
    }

    case 75:
    {
        // <var_id_num> -> ID

        // parse tree node for ID
        ParseTreeNode *idNode = parentNode->children->head;

        // ast node for var_id_num
        node = createASTNode(idNode->token, idNode->variableNo);

        break;
    }

    case 76:
    {
        // <var_id_num> -> NUM

        // parse tree node for NUM
        ParseTreeNode *numNode = parentNode->children->head;

        // ast node for var_id_num
        node = createASTNode(numNode->token, numNode->variableNo);

        break;
    }

    case 77:
    {
        // <var_id_num> -> RNUM

        // parse tree node for RNUM
        ParseTreeNode *rnumNode = parentNode->children->head;

        // ast node for var_id_num
        node = createASTNode(rnumNode->token, rnumNode->variableNo);

        break;
    }

    case 78:
    {
        // <unary_op> -> PLUS

        // parse tree node for PLUS
        ParseTreeNode *plusNode = parentNode->children->head;

        // ast node for unary_op
        node = createASTNode(parentNode->token, parentNode->variableNo);
        node->children = createASTNode(plusNode->token, plusNode->variableNo);
        node->children->parent = node;

        break;
    }

    case 79:
    {
        // <unary_op> -> MINUS

        // parse tree node for MINUS
        ParseTreeNode *minusNode = parentNode->children->head;

        // ast node for unary_op
        node = createASTNode(parentNode->token, parentNode->variableNo);
        node->children = createASTNode(minusNode->token, minusNode->variableNo);
        node->children->parent = node;

        break;
    }

    case 80:
    {
        // <arithmeticOrBooleanExpr> -> <AnyTerm> <N7>

        // parse tree nodes for AnyTerm and N7
        ParseTreeNode *anyTermNode = parentNode->children->head;
        ParseTreeNode *n7Node = anyTermNode->sibling;

        // ast node for arithmeticOrBooleanExpr
        node = applyASTRules(n7Node);

        // ast node for anyterm
        astNode *newNode = applyASTRules(anyTermNode);

        if (node == NULL)
        {
            node = newNode;
        }
        else
        {
            // joining nodes
            node->children = addNodeToBeg(node->children, newNode, node);
        }
        break;
    }

    case 81:

    {
        // <N7> -> <logicalOp> <AnyTerm> <N7>

        // parse tree nodes for logicalOp, AnyTerm and N7
        ParseTreeNode *logicalOpNode = parentNode->children->head;
        ParseTreeNode *anyTermNode = logicalOpNode->sibling;
        ParseTreeNode *n7Node = anyTermNode->sibling;

        // ast node for N7 (parent)
        node = applyASTRules(logicalOpNode);

        // astNode for anyterm
        astNode *newNode = applyASTRules(anyTermNode); // node for anyterm

        // astNode for N7 (child)
        node->children = addNodeToBeg(node->children, applyASTRules(n7Node), node);

        if (node->children == NULL) // when N7 (child) is epsilon
        {

            node->children = addNodeToBeg(node->children, newNode, node);
        }
        else // when N7 (child) is not epsilon
        {
            node->children->children = addNodeToBeg(node->children->children, newNode, node->children);
        }

        break;
    }

    case 82:
    {
        // <N7> -> epsilon

        node = NULL; // initializing node to null

        break;
    }

    case 83:
    {
        // <AnyTerm> -> <arithmeticExpr> <N8>

        // parse tree nodes for arithmeticExpr and N8
        ParseTreeNode *arithmeticExprNode = parentNode->children->head;
        ParseTreeNode *n8Node = arithmeticExprNode->sibling;

        // ast node for AnyTerm
        node = applyASTRules(n8Node);

        // ast node for arithmeticExpr
        astNode *newNode = applyASTRules(arithmeticExprNode);

        if (node == NULL) // if N8 is epsilon
        {
            node = newNode;
        }
        else
        {
            // if N8 is not epsilon
            node->children = addNodeToBeg(node->children, newNode, node);
        }

        break;
    }

    case 84:
    {
        // <AnyTerm> -> <boolConstt>
        // parse tree node for boolConstt
        ParseTreeNode *boolConsttNode = parentNode->children->head;

        // ast node for AnyTerm
        node = applyASTRules(boolConsttNode);

        break;
    }

    case 85:
    {
        // <N8> -> <relationalOp> <arithmeticExpr>

        // parse tree nodes for relationalOp and arithmeticExpr
        ParseTreeNode *relationalOpNode = parentNode->children->head;
        ParseTreeNode *arithmeticExprNode = relationalOpNode->sibling;

        // ast node for N8 (parent)
        node = applyASTRules(relationalOpNode);

        // ast node for arithmeticExpr
        astNode *newNode = applyASTRules(arithmeticExprNode);

        // connecting <arithmeticExpr> to <N8>
        node->children = addNodeToBeg(node->children, newNode, node);

        break;
    }

    case 86:
    {
        // <N8> -> epsilon

        node = NULL; // initializing node to null

        break;
    }

    case 87:
    {
        // <arithmeticExpr> -> <term> <N4>

        // parse tree nodes for term and N4
        ParseTreeNode *termNode = parentNode->children->head;
        ParseTreeNode *n4Node = termNode->sibling;

        // ast node for arithmeticExpr
        node = applyASTRules(n4Node);

        // ast node for term
        astNode *newNode = applyASTRules(termNode);

        if (node == NULL) // if N4 is epsilon
        {
            node = newNode;
        }
        else
        {
            // if N4 is not epsilon
            astNode *temp = node, *previous = node;
            while(temp->children!=NULL){
                if(temp->token->tokenType == PLUS ||temp->token->tokenType == MINUS) previous = temp;
                temp = temp->children;
            }
            previous->children = addNodeToBeg(previous->children, newNode, previous);
        }

        break;
    }

    case 88:
    {
        // <N4> -> <op1> <term> <N4>

        // parse tree nodes for op1, term and N4
        ParseTreeNode *op1Node = parentNode->children->head;
        ParseTreeNode *termNode = op1Node->sibling;
        ParseTreeNode *n4Node = termNode->sibling;

        // ast node for N4 (parent)
        astNode* forN4Node = applyASTRules(op1Node);

        // ast node for term
        astNode *newNode = applyASTRules(termNode);
        forN4Node->children = newNode;
        // ast node fro N4 (child)
        node = applyASTRules(n4Node);

        if (node == NULL) // if N4 (child) is epsilon
        {
            node = forN4Node;
        }
        else // if N4 (child) is not epsilon
        {
            astNode *temp = node, *previous = node;
            while(temp->children!=NULL){
                if(temp->token->tokenType == PLUS ||temp->token->tokenType == MINUS) previous = temp;
                temp = temp->children;
            }
            previous->children = addNodeToBeg(previous->children, forN4Node, previous);
        }

        break;
    }

    case 89:
    {
        // <N4> -> epsilon

        node = NULL; // initializing node to null

        break;
    }

    case 90:
    {
        // <term> -> <factor> <N5>

        // parse tree nodes for factor and N5
        ParseTreeNode *factorNode = parentNode->children->head;
        ParseTreeNode *n5Node = factorNode->sibling;

        // ast node for term
        node = applyASTRules(n5Node);

        // ast node for factor
        astNode *newNode = applyASTRules(factorNode);

        if (node == NULL) // if N5 is epsilon
        {
            node = newNode;
        }
        else
        {
            // if N5 is not epsilon
            astNode *temp = node, *previous = node;
            while(temp->children!=NULL){
                previous = temp;
                temp = temp->children;
            }
            previous->children = addNodeToBeg(previous->children, newNode, previous);
        }

        break;
    }

    case 91:
    {
        // <N5> -> <op2> <factor> <N5>

        // parse tree nodes for op2, factor and N5
        ParseTreeNode *op2Node = parentNode->children->head;
        ParseTreeNode *factorNode = op2Node->sibling;
        ParseTreeNode *n5Node = factorNode->sibling;

        // ast node for N5 (parent)
        astNode* forN5node = applyASTRules(op2Node);
        
        // ast node for factor
        astNode *newNode = applyASTRules(factorNode);
        forN5node->children = newNode;
        // ast node for N5 (child)
        node = applyASTRules(n5Node);

        if (node == NULL) // if N5 (child) is epsilon
        {
            node = forN5node;
        }
        else // if N5 (child) is not epsilon
        {
            astNode *temp = node, *previous = node;
            while(temp->children!=NULL){
                previous = temp;
                temp = temp->children;
            }
            previous->children = addNodeToBeg(previous->children, forN5node, previous);        
        }

        break;
    }

    case 92:
    {
        // <N5> -> epsilon

        node = NULL; // initializing node to null

        break;
    }

    case 93:
    {
        // <factor> -> BO <arithmeticOrBooleanExpr> BC

        // parse tree node for arithmeticOrBooleanExpr
        ParseTreeNode *arithmeticOrBooleanExprNode = parentNode->children->head->sibling;

        // ast node for factor
        node = applyASTRules(arithmeticOrBooleanExprNode);

        break;
    }

    case 94:
    {
        // <factor> -> NUM

        // parse tree node for NUM
        ParseTreeNode *numNode = parentNode->children->head;

        // ast node for factor
        node = createASTNode(numNode->token, numNode->variableNo);

        break;
    }

    case 95:
    {
        // <factor> -> RNUM

        // parse tree node for RNUM
        ParseTreeNode *rnumNode = parentNode->children->head;

        // ast node for factor
        node = createASTNode(rnumNode->token, rnumNode->variableNo);

        break;
    }

    case 96:
    {
        // <factor> -> <boolConstt>

        // parse tree node for boolConstt
        ParseTreeNode *boolConsttNode = parentNode->children->head;

        // ast node for factor
        node = applyASTRules(boolConsttNode);

        break;
    }

    case 97:
    {
        // <factor> -> ID <N_11>

        // parse tree nodes for ID and N_11
        ParseTreeNode *idNode = parentNode->children->head;
        ParseTreeNode *n_11Node = idNode->sibling;

        // ast node for factor
        node = createASTNode(idNode->token, idNode->variableNo);
        node->children = applyASTRules(n_11Node);
        if (node->children != NULL)
            node->children->parent = node;

        break;
    }

    case 98:
    {
        // <arrExpr> -> <arrTerm> <arr_N4>

        // parse Tree Node for arrTerm and arr_N4
        ParseTreeNode *arrTermNode = parentNode->children->head;
        ParseTreeNode *arr_N4Node = arrTermNode->sibling;

        // ast node for arrExpr is syntesized by arr_N4
        node = applyASTRules(arr_N4Node);

        // ast node for arrTerm
        astNode *newNode = applyASTRules(arrTermNode);

        if (node == NULL) // if arr_N4 is epsilon
        {
            node = newNode;
        }
        else
        {
            // if arr_N4 is not epsilon
            node->children = addNodeToBeg(node->children, newNode, node);
        }

        break;
    }

    case 99:
    {
        // <arr_N4> <op1> <arrTerm> <arr_N4>

        // parse tree nodes for op1, arrTerm and arr_N4
        ParseTreeNode *op1Node = parentNode->children->head;
        ParseTreeNode *arrTermNode = op1Node->sibling;
        ParseTreeNode *arr_N4Node = arrTermNode->sibling;

        // ast node for arr_N4 (parent)
        node = applyASTRules(op1Node);

        // ast node for arrTerm
        astNode *newNode = applyASTRules(arrTermNode);

        // ast node for arr_N4 (child)
        node->children = addNodeToBeg(node->children, applyASTRules(arr_N4Node), node);

        if (node->children == NULL) // if arr_N4 (child) is epsilon
        {
            node->children = addNodeToBeg(node->children, newNode, node);
        }
        else // if arr_N4 (child) is not epsilon
        {
            node->children->children = addNodeToBeg(node->children->children, newNode, node->children);
        }

        break;
    }

    case 100:
    {
        // <arr_N4> -> epsilon

        node = NULL; // initializing node to null

        break;
    }

    case 101:
    {
        // <arrTerm> -> <arrFactor> <arr_N5>

        // parse tree nodes for arrFactor and arr_N5
        ParseTreeNode *arrFactorNode = parentNode->children->head;
        ParseTreeNode *arr_N5Node = arrFactorNode->sibling;

        // ast node for arrTerm is synthesized by arr_N5
        node = applyASTRules(arr_N5Node);

        // ast node for arrFactor
        astNode *newNode = applyASTRules(arrFactorNode);

        if (node == NULL) // if arr_N5 is epsilon
        {
            node = newNode;
        }
        else
        {
            // if arr_N5 is not epsilon
            node->children = addNodeToBeg(node->children, newNode, node);
        }

        break;
    }

    case 102:
    {
        // <arr_N5> -> <op2> <arrFactor> <arr_N5>

        // parse tree nodes for op2, arrFactor and arr_N5
        ParseTreeNode *op2Node = parentNode->children->head;
        ParseTreeNode *arrFactorNode = op2Node->sibling;
        ParseTreeNode *arr_N5Node = arrFactorNode->sibling;

        // ast node for arr_N5 (parent)
        node = applyASTRules(op2Node);

        // ast node for arrFactor
        astNode *newNode = applyASTRules(arrFactorNode);

        // ast node for arr_N5 (child)
        node->children = addNodeToBeg(node->children, applyASTRules(arr_N5Node), node);

        if (node->children == NULL) // if arr_N5 (child) is epsilon
        {
            node->children = addNodeToBeg(node->children, newNode, node);
        }
        else // if arr_N5 (child) is not epsilon
        {
            node->children->children = addNodeToBeg(node->children->children, newNode, node->children);
        }

        break;
    }

    case 103:
    {
        // <arr_N5> -> epsilon
        node = NULL; // initializing node to null
        break;
    }

    case 104:
    {
        // <arrFactor> -> ID

        // parse tree node for ID
        ParseTreeNode *idNode = parentNode->children->head;

        // ast node for arrFactor
        node = createASTNode(idNode->token, idNode->variableNo);

        break;
    }

    case 105:
    {
        // <arrFactor> -> NUM
        // parse tree node for NUM
        ParseTreeNode *numNode = parentNode->children->head;

        // ast node for arrFactor
        node = createASTNode(numNode->token, numNode->variableNo);

        break;
    }

    case 106:
    {
        // <arrFactor> -> <boolConstt>

        // parse tree node for boolConstt
        ParseTreeNode *boolConsttNode = parentNode->children->head;

        // ast node for arrFactor
        node = applyASTRules(boolConsttNode);

        break;
    }

    case 107:
    {
        // <arrFactor> -> BO <arrExpr> BC
        node = applyASTRules(parentNode->children->head->sibling);
        break;
    }

    case 108:
    {
        // <element_index_with_expressions> -> <arrExpr>
        node = applyASTRules(parentNode->children->head);
        break;
    }

    case 109:
    {
        // <element_index_with_expressions> -> <unary_op> <N_10>
        astNode *children[2];
        ParseTreeNode *curr = parentNode->children->head;
        node = createASTNode(parentNode->token, parentNode->variableNo);

        int i = 0;
        if (curr == NULL)
        {
            printf("Some error\n");
        }
        while (curr != NULL)
        {
            children[i] = applyASTRules(curr);
            curr = curr->sibling;
            i++;
        }
        if (children[1])
        {
            joinNodes(children, 2, node);
            // printf("Yes chiildren 1 existed\n");
        }
        else
            joinNodes(children, 1, node);
            
        break;
    }

    case 110:
    {
        // <N_10> -> <new_index>
        node = applyASTRules(parentNode->children->head);
        break;
    }

    case 111:
    {
        // <N_10> -> BO <arrExpr> BC
        node = applyASTRules(parentNode->children->head->sibling);
        break;
    }

    case 112:
    {
        // <op1> -> PLUS

        // parse tree node for PLUS
        ParseTreeNode *plusNode = parentNode->children->head;

        // ast node for op1
        node = createASTNode(plusNode->token, plusNode->variableNo);

        break;
    }

    case 113:
    {
        // <op1> -> MINUS

        // parse tree node for MINUS
        ParseTreeNode *minusNode = parentNode->children->head;

        // ast node for op1
        node = createASTNode(minusNode->token, minusNode->variableNo);

        break;
    }

    case 114:
    {
        // <op1> -> MUL

        // parse tree node for MUL
        ParseTreeNode *mulNode = parentNode->children->head;

        // ast node for op1
        node = createASTNode(mulNode->token, mulNode->variableNo);

        break;
    }

    case 115:
    {
        // <op1> -> DIV

        // parse tree node for DIV
        ParseTreeNode *divNode = parentNode->children->head;

        // ast node for op1
        node = createASTNode(divNode->token, divNode->variableNo);

        break;
    }

    case 116:
    {
        // <logicalOp> -> AND

        // parse tree node for AND
        ParseTreeNode *andNode = parentNode->children->head;

        // ast node for logicalOp
        node = createASTNode(andNode->token, andNode->variableNo);

        break;
    }

    case 117:
    {
        //<logicalOp> -> OR

        // parse tree node for OR
        ParseTreeNode *orNode = parentNode->children->head;

        // ast node for logicalOp
        node = createASTNode(orNode->token, orNode->variableNo);

        break;
    }
    case 118:
    {
        //<relationalOp> -> LT

        // parse tree node for LT
        ParseTreeNode *ltNode = parentNode->children->head;

        // ast node for relationalOp
        node = createASTNode(ltNode->token, ltNode->variableNo);

        break;
    }
    case 119:
    {
        //<relationalOp> -> LE

        // parse tree node for LE
        ParseTreeNode *leNode = parentNode->children->head;

        // ast node for relationalOp
        node = createASTNode(leNode->token, leNode->variableNo);

        break;
    }

    case 120:
    {
        //<relationalOp> -> GT

        // parse tree node for GT
        ParseTreeNode *gtNode = parentNode->children->head;

        // ast node for relationalOp
        node = createASTNode(gtNode->token, gtNode->variableNo);

        break;
    }
    case 121:
    {
        //<relationalOp> -> GE

        // parse tree node for GE
        ParseTreeNode *geNode = parentNode->children->head;

        // ast node for relationalOp
        node = createASTNode(geNode->token, geNode->variableNo);
        break;
    }
    case 122:
    {
        //<relationalOp> ->EQ
        // parse tree node for EQ
        ParseTreeNode *eqNode = parentNode->children->head;

        // ast node for relationalOp
        node = createASTNode(eqNode->token, eqNode->variableNo);
        break;
    }
    case 123:
    {
        //<relationalOp> -> NE
        // parse tree node for NE
        ParseTreeNode *neNode = parentNode->children->head;

        // ast node for relationalOp
        node = createASTNode(neNode->token, neNode->variableNo);
        break;
    }

    case 124:
    {
        //<declareStmt> -> DECLARE <idList> COLON <dataType> SEMICOL

        // parse tree nodes for idList and dataType
        ParseTreeNode *idListNode = parentNode->children->head->sibling;
        ParseTreeNode *dataTypeNode = idListNode->sibling->sibling;

        // ast node for declareStmt
        node = createASTNode(parentNode->token, parentNode->variableNo);

        // ast Nodes for children
        astNode *children[2];

        // ast node for idList
        children[0] = applyASTRules(idListNode);
        children[1] = applyASTRules(dataTypeNode);

        joinNodes(children, 2, node);

        break;
    }

    case 125:
    {
        //<conditionalStmt> -> SWITCH BO ID BC START_TK <caseStmts> <default> END

        // parse tree nodes for ID, caseStmts and default, SWITCH
        ParseTreeNode *idNode = parentNode->children->head->sibling->sibling;
        ParseTreeNode *caseStmtsNode = idNode->sibling->sibling->sibling;
        ParseTreeNode *defaultNode = caseStmtsNode->sibling;
        ParseTreeNode *switchNode = parentNode->children->head;
        // ParseTreeNode *startNode = idNode->sibling->sibling;
        ParseTreeNode *endNode = defaultNode->sibling;

        // ast node for conditionalStmt
        node = createASTNode(switchNode->token, switchNode->variableNo);

        // astNodes for children
        astNode *children[3];

        children[0] = createASTNode(idNode->token, idNode->variableNo);
        children[1] = applyASTRules(caseStmtsNode);
        children[2] = applyASTRules(defaultNode);
        node->scope.start = switchNode->token->lineNo;
        node->scope.end = endNode->token->lineNo;

        if (children[2])
            joinNodes(children, 3, node);
        else
            joinNodes(children, 2, node);

        break;
    }

    case 126:
    {
        //<caseStmts> -> CASE <value> COLON <statements> BREAK SEMICOL <N9>

        // parse tree nodes for value, statements and N9
        ParseTreeNode *valueNode = parentNode->children->head->sibling;
        ParseTreeNode *statementsNode = valueNode->sibling->sibling;
        ParseTreeNode *n9Node = statementsNode->sibling->sibling->sibling;
        ParseTreeNode *caseNode = parentNode->children->head;
        ParseTreeNode *breakNode = statementsNode->sibling;

        // ast node for caseStmts
        node = createASTNode(parentNode->token, parentNode->variableNo);
        // ast node for value
        astNode *newNode = applyASTRules(valueNode);
        // ast node for statements
        newNode->children = applyASTRules(statementsNode);
        // ast node for N9
        newNode->sibling = applyASTRules(n9Node);

        newNode->children->parent = newNode;
        newNode->parent = node;
        newNode->scope.start = caseNode->token->lineNo;
        newNode->scope.end = breakNode->token->lineNo;

        node->children = newNode;
        astNode *curr = newNode->sibling;

        while (curr != NULL)
        {
            curr->parent = node;
            curr = curr->sibling;
        }

        break;
    }

    case 127:
    {
        //<N9> -> CASE <value> COLON <statements> BREAK SEMICOL <N9>

        // parse tree nodes for value, statements and N9
        ParseTreeNode *valueNode = parentNode->children->head->sibling;
        ParseTreeNode *statementsNode = valueNode->sibling->sibling;
        ParseTreeNode *n9Node = statementsNode->sibling->sibling->sibling;
        ParseTreeNode *caseNode = parentNode->children->head;
        ParseTreeNode *breakNode = statementsNode->sibling;

        // ast node for Value
        node = applyASTRules(valueNode);
        node->scope.start = caseNode->token->lineNo;
        node->scope.end = breakNode->token->lineNo;

        // ast node for statements
        astNode *newNode = applyASTRules(statementsNode);
        node->children = newNode;
        newNode->parent = node;

        // ast node for N9 (child)
        node->sibling = applyASTRules(n9Node);

        break;
    }

    case 128:
    {
        // <N9> -> epsilon
        node = NULL;
        break;
    }

    case 129:
    {
        // <value> -> NUM

        // parse tree node for NUM
        ParseTreeNode *numNode = parentNode->children->head;

        // ast node for num
        node = createASTNode(numNode->token, numNode->variableNo);

        break;
    }

    case 130:
    {
        // <value> ->TRUE

        // parse tree node for TRUE
        ParseTreeNode *trueNode = parentNode->children->head;

        // ast node for num
        node = createASTNode(trueNode->token, trueNode->variableNo);

        break;
    }

    case 131:
    {
        // <value> -> FALSE

        // parse tree node for FALSE
        ParseTreeNode *falseNode = parentNode->children->head;

        // ast node for num
        node = createASTNode(falseNode->token, falseNode->variableNo);

        break;
    }

    case 132:
    {
        // <default> -> DEFAULT COLON <statements> BREAK SEMICOL

        // parse tree nodes for statements
        ParseTreeNode *statementsNode = parentNode->children->head->sibling->sibling;
        ParseTreeNode *breakNode = statementsNode->sibling;

        // ast node for default
        node = createASTNode(parentNode->children->head->token, parentNode->children->head->variableNo);
        node->scope.start = node->token->lineNo;
        node->scope.end = breakNode->token->lineNo;

        // ast node for statements
        astNode *newNode = applyASTRules(statementsNode);
        // joining statements to default
        node->children = addNodeToBeg(node->children, newNode, node);
        break;
    }

    case 133:
    {
        // <default> -> epsilon
        node = NULL;
        break;
    }

    case 134:
    {
        //<iterativeStmt> -> FOR BO ID IN <range_for_loop> BC START_TK <statements> END

        // parse tree nodes for ID, range_for_loop and statements
        ParseTreeNode *idNode = parentNode->children->head->sibling->sibling;
        ParseTreeNode *range_for_loopNode = idNode->sibling->sibling;
        ParseTreeNode *statementsNode = range_for_loopNode->sibling->sibling->sibling;

        // ast node for FOR
        node = createASTNode(parentNode->children->head->token, parentNode->children->head->variableNo);
        node->scope.start = node->token->lineNo;
        node->scope.end = statementsNode->sibling->token->lineNo;

        // ast nodes for children
        astNode *children[3];

        children[0] = createASTNode(idNode->token, idNode->variableNo);
        children[1] = applyASTRules(range_for_loopNode);
        children[2] = applyASTRules(statementsNode);

        joinNodes(children, 3, node);

        break;
    }

    case 135:
    {
        //<iterativeStmt> -> WHILE BO <arithmeticOrBooleanExpr> BC START_TK <statements> END

        // parse tree nodes for arithmeticOrBooleanExpr and statements
        ParseTreeNode *arithmeticOrBooleanExprNode = parentNode->children->head->sibling->sibling;
        ParseTreeNode *statementsNode = arithmeticOrBooleanExprNode->sibling->sibling->sibling;

        // ast node for WHILE
        node = createASTNode(parentNode->children->head->token, parentNode->children->head->variableNo);
        node->scope.start = node->token->lineNo;
        node->scope.end = statementsNode->sibling->token->lineNo;
        // ast nodes for children
        astNode *children[2];

        children[0] = applyASTRules(arithmeticOrBooleanExprNode);
        children[1] = applyASTRules(statementsNode);

        joinNodes(children, 2, node);

        break;
    }

    case 136:
    {
        //<range_for_loop> -> <index_for_loop> RANGEOP <index_for_loop>

        // parse tree nodes for index_for_loop1 and index_for_loop2
        ParseTreeNode *index_for_loop1Node = parentNode->children->head;
        ParseTreeNode *index_for_loop2Node = index_for_loop1Node->sibling->sibling;

        // ast node for range_for_loop
        node = createASTNode(parentNode->token, parentNode->variableNo);

        // ast nodes for children
        astNode *children[2];

        children[0] = applyASTRules(index_for_loop1Node);
        children[1] = applyASTRules(index_for_loop2Node);

        joinNodes(children, 2, node);

        break;
    }

    case 137:
    {
        //<index_for_loop> -> <sign_for_loop> <new_index_for_loop>

        // parse tree nodes for sign_for_loop and new_index_for_loop
        ParseTreeNode *sign_for_loopNode = parentNode->children->head;
        ParseTreeNode *new_index_for_loopNode = sign_for_loopNode->sibling;

        // ast node for index_for_loop
        node = createASTNode(parentNode->token, parentNode->variableNo);

        // ast nodes for children
        astNode *children[2];

        children[0] = applyASTRules(sign_for_loopNode);
        children[1] = applyASTRules(new_index_for_loopNode);

        if (children[0] != NULL)
            joinNodes(children, 2, node);
        else
            joinNodes(children + 1, 1, node);

        break;
    }

    case 138:
    {
        //<new_index_for_loop> -> NUM

        // parse tree node for NUM
        ParseTreeNode *numNode = parentNode->children->head;

        // ast node for new_index_for_loop
        node = createASTNode(numNode->token, numNode->variableNo);

        break;
    }

    case 139:
    {
        //<sign_for_loop> -> PLUS

        // parse tree node for PLUS
        ParseTreeNode *plusNode = parentNode->children->head;

        // ast node for sign_for_loop
        node = createASTNode(plusNode->token, plusNode->variableNo);

        break;
    }

    case 140:
    {
        //<sign_for_loop> -> MINUS

        // parse tree node for MINUS
        ParseTreeNode *minusNode = parentNode->children->head;

        // ast node for sign_for_loop
        node = createASTNode(minusNode->token, minusNode->variableNo);

        break;
    }

    case 141:
    {
        //<sign_for_loop> epsilon
        node = NULL;
        break;
    }

    default:
        printf("Error: Invalid Rule Number\n");
    }

    // printf("Variable = %s, Rule = %d  done!\n", NoToCharMapping[parentNode->variableNo], parentNode->ruleNo);
    return node;
}

AST *createAST(ParseTree *tree)
{
    AST *ast = (AST *)malloc(sizeof(AST));
    printf("Creating AST...\n");
    // printf("variable no of parse Tree Root: %d\n", tree->root->variableNo);

    astNode *rootNode = applyASTRules(tree->root);
    ast->root = rootNode;
    return ast;
}



void printAST(astNode *root, FILE *fp, int noOfTabs)
{
    if (root == NULL)
        return;

    for (int i = 0; i < noOfTabs; i++)
        fprintf(fp, "\t");

    if (root->variableNo < DOLLAR)
        fprintf(fp, " %d.%s %s\n", noOfTabs, NoToCharMapping[root->variableNo], root->token->lexeme);
    else
        fprintf(fp, " %d.%s\n", noOfTabs, NoToCharMapping[root->variableNo]);

    astNode *child = root->children;
    while (child)
    {
        printAST(child, fp, noOfTabs + 1);
        child = child->sibling;
    }
}

void generateAST(AST* ast)
{

    // TODO -- INITIALISE GLOBAL HASHMAPS TO NULL

    
    FILE *fp2 = fopen(astOutput, "w");
    // print that traversal order of ast is preorder, print this in bigger font
    printf("\033[1mAST generated in \033[32m");
    printf("\033[33mPREORDER TRAVERSAL ORDER \033[0m");
    printf("\033[1m(root->left children->other children)\033[32m\n\n");

    printAST(ast->root, fp2, 1);
    // printf("Hello \n%d \n", search(CharToNoMapping, "<program'>"));
    fclose(fp2);
    printOnConsole(astOutput);
    printf("\n\n\n");
    printf("\033[1mNOTE :: THIS output is also printed in the file \033[32m \033[36m%s\033[32m\n\033[1mPLEASE CHECK THIS FILE FOR BETTER VIEW\033[32m", astOutput);
    printf("\n\n\n");

    // return ast;
}