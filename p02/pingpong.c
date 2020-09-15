#include <stdio.h>
#include <stdlib.h>
#include <ucontext.h>
#include "pingpong.h"
#include "datatypes.h"

// operating system check
#if defined(_WIN32) || (!defined(__unix__) && !defined(__unix) && (!defined(__APPLE__) || !defined(__MACH__)))
#warning Este codigo foi planejado para ambientes UNIX (LInux, *BSD, MacOS). A compilacao e execucao em outros ambientes e responsabilidade do usuario.
#endif


task_t *InitTask, *AtualTask;
task_t MainTask;
int contadorTarefas;

void main();
void pingpong_init(){

    /* desativa o buffer da saida padrao (stdout), usado pela função printf */
    setvbuf (stdout, 0, _IONBF, 0) ;
    char *stack;
    contadorTarefas=0;
    getcontext(&(MainTask.contextoTarefa));
    stack=malloc (STACKSIZE);
    if (stack)
    {
      (MainTask.contextoTarefa).uc_stack.ss_sp = stack ;
      (MainTask.contextoTarefa).uc_stack.ss_size = STACKSIZE;
      (MainTask.contextoTarefa).uc_stack.ss_flags = 0;
      (MainTask.contextoTarefa).uc_link = 0;
    }
    else
    {
      perror ("Erro na criação da pilha: ");
      exit (1);
    }
    makecontext (&(MainTask.contextoTarefa),(void*)(*main), contadorTarefas, "Main Task");
    contadorTarefas++;
    AtualTask=&MainTask;
}

int task_create (task_t *task, void (*start_routine)(void *),void *arg){
    char *stack ;
    getcontext(&(task->contextoTarefa));
    stack = malloc (STACKSIZE) ;
    if (stack)
    {
      (task->contextoTarefa).uc_stack.ss_sp = stack ;
      (task->contextoTarefa).uc_stack.ss_size = STACKSIZE;
      (task->contextoTarefa).uc_stack.ss_flags = 0;
      (task->contextoTarefa).uc_link = 0;
    }
    else
    {
      perror ("Erro na criação da pilha: ");
      return -1;
    }
    makecontext (&(task->contextoTarefa), (void*) (*start_routine), contadorTarefas, arg);
    task->tid = contadorTarefas;
    contadorTarefas++;
    return task->tid;
}
int task_switch (task_t *task){
    //int retorno;
    //retorno = swapcontext(&(AtualTask->contextoTarefa),&(task->contextoTarefa));
    //AtualTask = task;
    //return retorno;
    task_t *aux;
    aux = AtualTask;
    AtualTask = task;
    return swapcontext(&(aux->contextoTarefa),&(AtualTask->contextoTarefa));
    }


void task_exit(int exit_code){
    task_switch(&MainTask);
    }

int task_id(){
    return AtualTask->tid;}

