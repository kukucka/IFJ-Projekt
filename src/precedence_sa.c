#include "token.h"
#include "precedence_sa.h"
#include "lexical.h"
#include "stack.h"
#include <stdlib.h>
#include <stdio.h>
#include "abstract_tree.h"
#include "error.h"

static int precedence_table[][15] = {  
                                            {2,2,1,1,1,2,2,2,2,2,2,1,2,1,2},
                                            {2,2,1,1,1,2,2,2,2,2,2,1,2,1,2},
                                            {2,2,2,2,2,2,2,2,2,2,2,1,2,1,2},
                                            {2,2,2,2,2,2,2,2,2,2,2,1,2,1,2},
                                            {2,2,2,2,2,2,2,2,2,2,2,1,2,1,2},
                                            {1,1,1,1,1,-1,-1,-1,-1,-1,-1,1,2,1,2},
                                            {1,1,1,1,1,-1,-1,-1,-1,-1,-1,1,2,1,2},
                                            {1,1,1,1,1,-1,-1,-1,-1,-1,-1,1,2,1,2},
                                            {1,1,1,1,1,-1,-1,-1,-1,-1,-1,1,2,1,2},
                                            {1,1,1,1,1,-1,-1,-1,-1,-1,-1,1,2,1,2},
                                            {1,1,1,1,1,-1,-1,-1,-1,-1,-1,1,2,1,2},
                                            {1,1,1,1,1,1,1,1,1,1,1,1,0,1,-1},
                                            {2,2,2,2,2,2,2,2,2,2,2,-1,2,-1,2},
                                            {2,2,2,2,2,2,2,2,2,2,2,-1,2,-1,2},
                                            {1,1,1,1,1,1,1,1,1,1,1,1,-1,1,-1}
                                        };

PrecendentOutput * precedence_analysis(Token* last_token){
    PrecendentOutput * out = malloc(sizeof(PrecendentOutput));
    if(out == NULL)
        return NULL;

    out->Tree = NULL;
    out->ReturnToken = NULL;

    int logical = 0;
    int readNextToken = 1;
    tStack *s = (tStack*) malloc(sizeof(struct Stack));
    if(s == NULL){
        out->StatusCode = COMPILER_ERROR;
        return out;
    }
    stackInit(s);
    Token * current = last_token;
    if(current == NULL)
        current = get_token();
    
    while(readNextToken || getTerminalData(s) != NULL){
        SData * termData =  getTerminalData(s);
        Token* token = NULL;
        if(termData != NULL)
            token = termData->Atr.Token;
        if(current != NULL){
            if(current->type == type_eol || current->type == type_semicolon || (current->type == type_keyword && current->atribute.keyword_value == kw_then)){
                readNextToken = 0;
            }
        }
        
        int operation = precedence_operation(token,current);
        if(operation == -1){
            out->ReturnToken = current;
            out->StatusCode = SEMANTIC_ERROR;
            return out;
        }
        else if(operation == 0){
            SData *data = malloc(sizeof(SData));
            if(data == NULL){
                out->ReturnToken = current;
                out->StatusCode = COMPILER_ERROR;
                return out;
            }
            data->Type = type_token;
            data->Atr.Token = current;
            stackPush(s,data);
        }
        else if(operation == 1){
            addHandler(s,termData);
            SData *data = malloc(sizeof(SData));
            if(data == NULL){
                out->ReturnToken = current;
                out->StatusCode = COMPILER_ERROR;
                return out;
            }
            data->Type = type_token;
            data->Atr.Token = current;
            stackPush(s,data);
        }
        else if(operation == 2){
            int rule = findRule(s);
            if( rule == -1){
                out->ReturnToken = current;
                out->StatusCode = SEMANTIC_ERROR;
                return out;
            }
            else if(operation >= 7 && operation <= 12){
                logical = 1;
            }
            continue;
        }
        if(readNextToken)
            current = get_token();
    }
    
    out->ReturnToken = current;
    if(!stackEmpty(s)){
        out->Tree = stackTop(s)->Atr.Leaf;
        if(logical)
            out->Type = type_bool;
        else
            out->Type = stackTop(s)->DataType;

    }
    out->StatusCode = OK;
    return out;
}

int precedence_operation(Token* stack_token,Token* lexical_token){
    int index0,index1 = 0;
    Token * token = NULL;
    for(int i=0;i<2;i++){
        int curr_index = 0;
        if(i == 0){
            token = stack_token;
        }
        else{
            token = lexical_token;
        }
        if(token == NULL || token->type == type_eol || token->type == type_semicolon || (token->type == type_keyword && token->atribute.keyword_value == kw_then)){
            curr_index = 14;            
        }
        else if(token->type == type_operator){
            if(token->atribute.operator_value == op_add){
                curr_index = 0;
            }
            else if(token->atribute.operator_value == op_sub){
                curr_index = 1;
            }
            else if(token->atribute.operator_value == op_mul){
                curr_index = 2;
            }
            else if(token->atribute.operator_value == op_slash){
                curr_index = 3;
            }
            else if(token->atribute.operator_value == op_division_int){
                curr_index = 4;
            }
            else if(token->atribute.operator_value == op_assign){
                curr_index = 5;                    
            }
            else if(token->atribute.operator_value == op_not_equal){
                curr_index = 6;
            }
            else if(token->atribute.operator_value == op_lesser){
                curr_index = 7;
            }
            else if(token->atribute.operator_value == op_greater){
                curr_index = 8;
            }
            else if(token->atribute.operator_value == op_lesser_equal){
                curr_index = 9;
            }
            else if(token->atribute.operator_value == op_greater_equal){
                curr_index = 10;
            }
            else if(token->atribute.operator_value == op_bracket){
                curr_index = 11;
            }
            else if(token->atribute.operator_value == op_bracket_end){
                curr_index = 12;
            }
            else{
                curr_index = -1;
            }
        }
        else if(token->type == type_id || token->type == type_integer || token->type == type_string || token->type == type_double){
            curr_index = 13;        
        }
        else{
            curr_index = -1;
        }
        if(i == 0){
            index0 = curr_index;
        }
        else{
            index1 = curr_index;
        }
    }
    
    if(index0 == -1 || index1 == -1){
        printf("Syntax error\n");
        return -1;
    }
    int oper = precedence_table[index0][index1];
    if(oper == -1){
        printf("Syntax error\n");
    }
    return oper;
}

int findRule(tStack * s){
    int rule = 0;
    int state = 0;
    int estimate_precedence = 0;
    Token * token;
    ATLeaf * leaf1;
    ATLeaf * leaf2;
    Operators oper;

    while(rule == 0)
    {
        SData * data = stackTop(s);
        ATData aData;
        Tvariable_type dataType;
        Tvariable_type dataType1;
        if(data != NULL)
            stackPop(s);
        switch(state){
            case 0:
                if(data->Type == type_nonterm){
                    leaf1 = data->Atr.Leaf;
                    dataType = data->DataType;
                    state = 1;
                }
                else if(data->Type == type_token){
                    if(data->Atr.Token->type == type_operator && data->Atr.Token->atribute.operator_value == op_bracket_end){
                        estimate_precedence = 1;
                        state = 2;
                    }
                    else if(data->Atr.Token->type == type_string ){
                            dataType = type_str;
                            token = data->Atr.Token;
                            estimate_precedence = 13;
                            state = 3;
                    }
                    else if(data->Atr.Token->type == type_double){
                        dataType = type_doub;
                        token = data->Atr.Token;
                        estimate_precedence = 13;
                        state = 3;
                    }
                    else if(data->Atr.Token->type == type_integer){
                        dataType = type_int;
                        token = data->Atr.Token;
                        estimate_precedence = 13;
                        state = 3;
                    }
                    else if(data->Atr.Token->type == type_id){
                        // Detekce typu
                        token = data->Atr.Token;
                        estimate_precedence = 13;
                        state = 3;
                    }
                    else{
                        printf("Syntax error\n");
                        return -1;
                    }
                }
                else{
                    printf("Syntax error\n");
                    return -1;
                }
                break;
            case 1:
                if(data->Atr.Token->type == type_operator){
                    if(data->Atr.Token->atribute.operator_value == op_add){
                        estimate_precedence = 2;
                        state = 2;
                    }
                    else if(data->Atr.Token->atribute.operator_value == op_mul){
                        estimate_precedence = 3;
                        state = 2;
                    }
                    else if(data->Atr.Token->atribute.operator_value == op_sub){
                        estimate_precedence = 4;
                        state = 2;
                    }
                    else if(data->Atr.Token->atribute.operator_value == op_slash){
                        estimate_precedence = 5;
                        state = 2;
                    }
                    else if(data->Atr.Token->atribute.operator_value == op_division_int){
                        estimate_precedence = 6;
                        state = 2;
                    }
                    else if(data->Atr.Token->atribute.operator_value == op_assign){
                        estimate_precedence = 7;
                        state = 2;
                    }
                    else if(data->Atr.Token->atribute.operator_value == op_not_equal){
                        estimate_precedence = 8;
                        state = 2;
                    }
                    else if(data->Atr.Token->atribute.operator_value == op_lesser){
                        estimate_precedence = 9;
                        state = 2;
                    }
                    else if(data->Atr.Token->atribute.operator_value == op_greater){
                        estimate_precedence = 10;
                        state = 2;
                    }
                    else if(data->Atr.Token->atribute.operator_value == op_lesser_equal){
                        estimate_precedence = 11;
                        state = 2;
                    }
                    else if(data->Atr.Token->atribute.operator_value == op_greater_equal){
                        estimate_precedence = 12;
                        state = 2;
                    }
                    else{
                        printf("Syntax error\n");
                        return -1;
                    }
                    oper = data->Atr.Token->atribute.operator_value;
                }
                else{
                    printf("Syntax error\n");
                    return -1;
                }
                break;
            case 2:
                if(data->Type == type_nonterm){
                    if(estimate_precedence == 1){
                        leaf1 = data->Atr.Leaf;
                        dataType = data->DataType;
                        state = 4;
                    }
                    else{
                        leaf2 = data->Atr.Leaf;
                        dataType1 = data->DataType;
                        state = 3;
                    }
                }
                else{
                    printf("Syntax error\n");
                    return -1;
                }
                break;
            case 3:
                if(data->Type == type_handler){
                    SData * newData = malloc(sizeof(SData));
                    if(newData == NULL){
                        return -1;
                    }
                    newData->Type = type_nonterm;
                    
                    if(estimate_precedence == 13){
                        newData->DataType = dataType;
                        aData.type = type_token;
                        aData.Atr.token = token;
                        newData->Atr.Leaf = make_leaf(aData);
                    }
                    else if(estimate_precedence == 1){
                        newData->DataType = dataType;
                        newData->Atr.Leaf = leaf1;
                    }
                    else{
                        aData.type = type_operator;
                        aData.Atr.op_value = oper;
                        if(oper == op_add || oper == op_sub || oper == op_mul || oper == op_slash ||
                            oper == op_lesser || oper == op_greater || oper == op_lesser_equal || oper == op_greater_equal ||
                            oper == op_assign || oper == op_not_equal){
                            if(dataType == type_int && dataType1 == type_int){
                                if(oper == op_slash )
                                    newData->DataType = type_doub;
                                else 
                                    newData->DataType = type_int;
                            }
                            else if((dataType == type_doub && dataType1 == type_doub) ||
                                    (dataType == type_int && dataType1 == type_doub) ||
                                    (dataType == type_doub && dataType1 == type_int)){
                                newData->DataType = type_doub;
                            }
                            else if(dataType == type_str && dataType1 == type_str){
                                if(oper == op_add || oper == op_lesser || oper == op_greater || oper == op_lesser_equal || oper == op_greater_equal || oper == op_assign || oper == op_not_equal )
                                    newData->DataType = type_str;
                                else{
                                    printf("Semantic error\n");
                                    return -1;
                                }
                            }
                            else{
                                printf("Semantic error\n");
                                return -1;
                            }
                        }
                        else if(oper == op_division_int){
                            if(dataType == type_int && dataType1 == type_int)
                                    newData->DataType = type_int;
                            else{
                                printf("Semantic error\n");
                                return -1;
                            }
                        }
                        else{
                                printf("Semantic error\n");
                                return -1;
                            }
                        newData->Atr.Leaf = make_tree(leaf2,leaf1,aData);
                    }
                    
                    stackPush(s,newData);
                    rule = estimate_precedence;
                }
                else{
                    printf("Semantic error\n");
                    rule = -1;
                }
                break;
            case 4:
                if(data->Atr.Token->type == type_operator && data->Atr.Token->atribute.operator_value == op_bracket){
                       state = 3;
                }
                else{
                    printf("Syntax error\n");
                    return -1;
                }
                break;
        }
    }
    return rule;
}