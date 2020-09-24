#include <stdio.h>
#include <stdlib.h>
#include <ucontext.h>
#include "pingpong.h"
#include "datatypes.h"
#include "queue.h"


// operating system check
#if defined(_WIN32) || (!defined(__unix__) && !defined(__unix) && (!defined(__APPLE__) || !defined(__MACH__)))
#warning Este codigo foi planejado para ambientes UNIX (LInux, *BSD, MacOS). A compilacao e execucao em outros ambientes e responsabilidade do usuario.
#endif


task_t *InitTask, *AtualTask, **filaDeProntas;
task_t MainTask, Dispatcher;
int contadorTarefas;

void task_yield ();
void main();
void dispatcher_body ();

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
    //iniciando fila de prontas
    filaDeProntas=0;
    task_create(&Dispatcher, dispatcher_body, "Despachante");
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
    queue_append ((queue_t**) &filaDeProntas, (queue_t*) task);
    contadorTarefas++;
    return task->tid;
}
int task_switch (task_t *task){
    //int retorno;
    //retorno = swapcontext(&(AtualTask->contextoTarefa),&(task->contextoTarefa));
    //AtualTask = task;
    //return retorno;
    queue_remove((queue_t **) &filaDeProntas, (queue_t*) task);
    task_t *aux;
    aux = AtualTask;
    AtualTask = task;
    return swapcontext(&(aux->contextoTarefa), &(AtualTask->contextoTarefa));
    }


void task_exit(int exit_code){
    //if (exit_code){
    //    task_switch(&Dispatcher);
    //    userTasks--;}
    if(AtualTask==&Dispatcher){
        queue_append ((queue_t**) &filaDeProntas, (queue_t*) &MainTask);
        task_switch(&MainTask);
    }
    else
        task_switch(&Dispatcher);
    contadorTarefas--;
    }

int task_id(){
    return AtualTask->tid;}

task_t* scheduler(){
    //política First Come First served
    return filaDeProntas;
}

void task_yield () {
    queue_append((queue_t **) &filaDeProntas, (queue_t*) AtualTask);
    task_switch(&Dispatcher);
}


void dispatcher_body () // dispatcher é uma tarefa
{
    queue_remove((queue_t **)&filaDeProntas,(queue_t*) &MainTask);
    task_t* next;
    int userTasks = queue_size((queue_t *)filaDeProntas);
    while ( userTasks > 0 )
    {
        next = scheduler() ; // scheduler é uma função
        if (next)
        {
            // ações antes de lançar a tarefa "next", se houverem
            queue_append((queue_t **) &filaDeProntas, (queue_t*) &Dispatcher);
            task_switch (next) ; // transfere controle para a tarefa "next"
            userTasks = queue_size((queue_t *)filaDeProntas); //caso task_exit, número de prontas vai diminuir
            // ações após retornar da tarefa "next", se houverem
        }
    }
    task_exit(0) ; // encerra a tarefa dispatcher
}

