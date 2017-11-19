#include "frame.h"
#include <stdlib.h>
#include <stdio.h>



void FS_init(TFstack *stack)
{
	stack->top = NULL;
}



bool FS_empty(TFstack *stack)
{	
	return (stack->top == NULL);
} 



void FS_destruct(TFstack *stack)
{
	if (!(FS_empty(stack)))
	{
		Tframe *temp = stack->top;
		
		while(temp)
		{
			Tframe *current = temp;
			temp = temp->next;

			for (int i = 0; i < current->var_count; i++)
			{
				if (current->vars[i].type == type_str)
				{
					free(current->vars[i].value.string);	
				}

				free(current->vars[i].id);
			}

			free(current->return_value);
			free(current->vars);
			free(current);
		}
	}

	free(stack);
}




Tframe *FS_top(TFstack *stack)
{
	if (!(FS_empty(stack)))
	{
		return stack->top;

	} else
	{
		return NULL;
	}
}



void FS_pop(TFstack *stack)
{
	if (!(FS_empty(stack)))
	{
		stack->top = stack->top->next;

	} else
	{
		; // chyba 55
	}
}



void FS_push(TFstack *stack, Tframe *frame)
{
	if(frame != NULL)
	{
		frame->next = stack->top;
		stack->top = frame;

	} else 
	{
		; // chyba 55 
	}
}



void create_frame()
{
	if (temp_frame != NULL)
	{
		temp_frame->return_value = NULL;
		temp_frame->vars = NULL;
		temp_frame->next = NULL;
		temp_frame->var_count = 0;

	} else
	{
		temp_frame = (Tframe *) malloc(sizeof(Tframe));
		if (temp_frame == NULL)
		{
			fprintf(stderr, "Memory allocation failed!\n");
			return ;
		}

		temp_frame->return_value = NULL;
		temp_frame->vars = NULL;
		temp_frame->var_count = 0;
		temp_frame->next = NULL;
	}

}



void def_return_value(Tframe *frame, Tvariable *var)
{	
	/*frame->return_value = (Tvariable *) malloc(sizeof(Tvariable));
	if (frame->return_value == NULL)
	{
		fprintf(stderr, "Memory allocation failed!\n");
		return ;
	}*/

	frame->return_value = var;
}



void add_var_to_frame(Tframe *frame, Tvariable *var)
{
	(frame->var_count)++;
	
	if (frame->var_count == 1)
	{
		frame->vars = (Tvariable *) malloc(sizeof(Tvariable));
		if (frame->vars == NULL)
		{
			fprintf(stderr, "Memory allocation failed!\n");
			return ;
		}

		frame->vars[frame->var_count-1] = *var;

	} else 
	{
		frame->vars = (Tvariable *) realloc(frame->vars, sizeof(Tvariable) * frame->var_count);
		
		if (frame->vars == NULL)
		{
			fprintf(stderr, "Memory allocation failed!\n");
			return ;
		}

		frame->vars[frame->var_count-1] = *var;
	}
}


void push_frame(TFstack *stack, Tvariable *var, unsigned next_instr)
{
	if (temp_frame != NULL)
	{
		Tframe *local_frame = (Tframe *) malloc(sizeof(Tframe));
		if (local_frame == NULL)
		{
			fprintf(stderr, "Memory allocation failed!\n");
			return ;
		}

		*local_frame = *temp_frame;

		def_return_value(local_frame, var);

		local_frame->next_instr = next_instr;

		FS_push(stack, local_frame);

		if (temp_frame->var_count != 0)
		{
			free(temp_frame->vars);
			
		}
		if (temp_frame->return_value != NULL)
		{
			free(temp_frame->return_value);
		}

		free(temp_frame);
		temp_frame = NULL;
	} else
	{
		; // chyba 55
	}
}

void pop_frame(TFstack *stack)
{
	if (FS_empty(stack))
	{
		; // chyba 55
	} else
	{
		Tframe *local_frame;
		local_frame = FS_top(stack);

		*temp_frame = *local_frame;

		FS_pop(stack);

		Tframe *test = FS_top(stack);

		if (test != NULL)
		{
			get_return_value(temp_frame, FS_top(stack));
		}

		if (local_frame->var_count != 0)
		{
			free(local_frame->vars);
			
		}

		if (local_frame->return_value != NULL)
		{
			free(local_frame->return_value);
		}

		free(local_frame);
		free(temp_frame);
	}	
}

void get_return_value(Tframe *end_frame, Tframe *frame)
{
	add_var_to_frame(frame, end_frame->return_value);
}

/*void destruct_frame(TFstack *stack, Tframe *frame)
{

}*/
/*
int main()
{
	TFstack *stack = (TFstack *) malloc(sizeof(TFstack));
	FS_init(stack);
	create_frame();
	Tvariable *var = malloc(sizeof(Tvariable));
	var->value.value_int = 10;
	var->defined = true;
	var->id = "ahoj";
	var->type = type_int;
	unsigned instr = 0;
	push_frame(stack, var, instr);
	Tframe *frame = FS_top(stack);
	printf("%s %d %d %d \n", frame->return_value->id, frame->return_value->value.value_int,\
								frame->next_instr, frame->var_count);
	create_frame();
	pop_frame(stack);
	//free(temp_frame);
	FS_destruct(stack);
}
*/