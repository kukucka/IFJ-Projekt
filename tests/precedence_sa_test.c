#include "../src/precedence_sa.h"
#include "../src/lexical.h"
#include "../src/precedence_sa.h"
#include <stdlib.h>
#include <stdio.h>
#include "../src/error.h"

int _print_t(ATLeaf *tree, int is_left, int offset, int depth, char s[20][255]);
void print_t(ATLeaf *tree);

int main(){
    load_file("../tests/precedence-test.ifj");
    printf("\n\n____________________________________________________\n");

    printf("Expr: 6 + 5 + 4 * 3\nReturn EOL?");
    PrecendentOutput * out = precedence_analysis(NULL);
    if(out->ReturnToken->type == type_eol)
        printf("Correct\n");
    else
        printf("Wrong\n");
    if(out->StatusCode == OK)
        print_t(out->Tree);
    dispose_at(out->Tree);

    printf("Expr: 4 * (5 + 3 * 2) - 4 \n");
    out = precedence_analysis(NULL);
    if(out->StatusCode == OK)
        print_t(out->Tree);
    dispose_at(out->Tree);

    printf("Expr: 5 / 4 *(4 + 3 \\ 2) \n");
    out = precedence_analysis(NULL);
    if(out->StatusCode == OK)
        print_t(out->Tree);
    dispose_at(out->Tree);

    printf("Expr: (5 + 4) > 4\n");
    out = precedence_analysis(NULL);
    if(out->StatusCode == OK)
        print_t(out->Tree);
    dispose_at(out->Tree);

    printf("Expr: 5 <> 3 \n");
    out = precedence_analysis(NULL);
    if(out->StatusCode == OK)
        print_t(out->Tree);
    dispose_at(out->Tree);

    printf("Expr: (8/4)>(4*(3+2)-4) \n");
    out = precedence_analysis(NULL);
    if(out->StatusCode == OK)
        print_t(out->Tree);
    dispose_at(out->Tree);

    printf("Expr: 5 = 3  \n");
    out = precedence_analysis(NULL);
    if(out->StatusCode == OK)
        print_t(out->Tree);
    dispose_at(out->Tree);

    printf("Expr: 5 <= 3  \n");
    out = precedence_analysis(NULL);
    if(out->StatusCode == OK)
        print_t(out->Tree);
    dispose_at(out->Tree);

    printf("Expr: 5 => 3; Return semicolon? \n");
    out = precedence_analysis(NULL);
    if(out->StatusCode == OK)
        print_t(out->Tree);
    if(out->ReturnToken->type == type_semicolon)
        printf("Correct\n");
    else
        printf("Wrong\n");
    dispose_at(out->Tree);
    get_token();

    printf("Expr: 5 + 3 4 * 5  \n");
    out = precedence_analysis(NULL);
    if(out->StatusCode == OK)
        print_t(out->Tree);
    dispose_at(out->Tree);

    printf("Expr: 5 + + 3 + - 3  4 * 5  \n");
    out = precedence_analysis(NULL);
    if(out->StatusCode == OK)
        print_t(out->Tree);
    dispose_at(out->Tree);

    printf("Expr: 4 * \"5\"  \n");
    out = precedence_analysis(NULL);
    if(out->StatusCode == OK)
        print_t(out->Tree);
    dispose_at(out->Tree);

    destruct_token_storage();   
    dispose_at(out->Tree);
    printf("\n\n____________________________________________________\n");
    close_file();
}

int _print_t(ATLeaf *tree, int is_left, int offset, int depth, char s[20][255])
{
    char b[20];
    int width = 5;

    if (!tree) return 0;

    if(tree->data.type == type_operator){
        if(tree->data.Atr.op_value == op_add)
            sprintf(b, "(%s)", " + ");
        else if(tree->data.Atr.op_value == op_sub)
            sprintf(b, "(%s)", " - ");
        else if(tree->data.Atr.op_value == op_mul)
            sprintf(b, "(%s)", " * ");
        else if(tree->data.Atr.op_value == op_slash)
            sprintf(b, "(%s)", " / ");
        else if(tree->data.Atr.op_value == op_division_int)
            sprintf(b, "(%s)", " \\ ");
        else if(tree->data.Atr.op_value == op_lesser)
            sprintf(b, "(%s)", " < ");    
        else if(tree->data.Atr.op_value == op_greater)
            sprintf(b, "(%s)", " > ");            
        else if(tree->data.Atr.op_value == op_lesser_equal)
            sprintf(b, "(%s)", " <=");    
        else if(tree->data.Atr.op_value == op_greater_equal)
            sprintf(b, "(%s)", " >=");
        else if(tree->data.Atr.op_value == op_assign)
            sprintf(b, "(%s)", " = ");
        else if(tree->data.Atr.op_value == op_not_equal)
            sprintf(b, "(%s)", " <>");
    }
    else
        sprintf(b, "(%03d)", tree->data.Atr.token->atribute.int_value);


    int left  = _print_t(tree->left,  1, offset,                depth + 1, s);
    int right = _print_t(tree->right, 0, offset + left + width, depth + 1, s);

#ifdef COMPACT
    for (int i = 0; i < width; i++)
        s[depth][offset + left + i] = b[i];

    if (depth && is_left) {

        for (int i = 0; i < width + right; i++)
            s[depth - 1][offset + left + width/2 + i] = '-';

        s[depth - 1][offset + left + width/2] = '.';

    } else if (depth && !is_left) {

        for (int i = 0; i < left + width; i++)
            s[depth - 1][offset - width/2 + i] = '-';

        s[depth - 1][offset + left + width/2] = '.';
    }
#else
    for (int i = 0; i < width; i++)
        s[2 * depth][offset + left + i] = b[i];

    if (depth && is_left) {

        for (int i = 0; i < width + right; i++)
            s[2 * depth - 1][offset + left + width/2 + i] = '-';

        s[2 * depth - 1][offset + left + width/2] = '+';
        s[2 * depth - 1][offset + left + width + right + width/2] = '+';

    } else if (depth && !is_left) {

        for (int i = 0; i < left + width; i++)
            s[2 * depth - 1][offset - width/2 + i] = '-';

        s[2 * depth - 1][offset + left + width/2] = '+';
        s[2 * depth - 1][offset - width/2 - 1] = '+';
    }
#endif

    return left + width + right;
}


void print_t(ATLeaf *tree)
{
    char s[20][255];
    for (int i = 0; i < 20; i++)
        sprintf(s[i], "%80s", " ");

    _print_t(tree, 0, 0, 0, s);

    //Vypis
    for (int i = 0; i < 10; i++)
        printf("%s\n", s[i]);
}