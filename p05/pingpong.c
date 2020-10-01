#include <stdio.h>
#include <stdlib.h>
#include <ucontext.h>
#include <signal.h>
#include <sys/time.h>
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
int ticksTarefa=20;


void task_yield ();
void main();
void dispatcher_body ();

// estrutura que define um tratador de sinal (deve ser global ou static)
struct sigaction action ;

// estrutura de inicialização to timer
struct itimerval timer;

// tradador de interrupções
void tratador (int signum);
// Define se uma tarefa é de usuário ou nao
void task_setUsuario (task_t *task, int flag);

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

    //inicia o Timer
    action.sa_handler = tratador ;
    sigemptyset (&action.sa_mask) ;
    action.sa_flags = 0 ;
    if (sigaction (SIGALRM, &action, 0) < 0)
    {
        perror ("Erro em sigaction: ") ;
        exit (1) ;
    }
    // ajusta valores do temporizador
    timer.it_value.tv_usec = 1000 ;      // primeiro disparo, em micro-segundos
    timer.it_value.tv_sec  = 0 ;      // primeiro disparo, em segundos
    timer.it_interval.tv_usec = 1000 ;   // disparos subsequentes, em micro-segundos
    timer.it_interval.tv_sec  = 0 ;   // disparos subsequentes, em segundos

    // arma o temporizador ITIMER_REAL (vide man setitimer)
    if (setitimer (ITIMER_REAL, &timer, 0) < 0)
    {
        perror ("Erro em setitimer: ") ;
        exit (1) ;
    }
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
    //prioridade padrao de zero
    task_setprio(task, 0);
    //por padrão, tasks criadas são de nível usuário
    task_setUsuario (task, 1);
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
    int i, j; //contador para navegação da fila de prontas e numero de tarefas na fila
    task_t *auxprox, *auxatual; //variaveis para retorno e comparação de prioridades
    j = queue_size((queue_t *)filaDeProntas);
    auxatual = filaDeProntas; //tava aqui o erro kkkk, era "auxatual = *filaDeProntas"
    auxprox = auxatual->next;
    for(i=0; i<j-1; i++){
        //se a prioridade de retorno tem a prioridade maior ela é ajustada
        if(auxatual->priod>auxprox->priod)
            auxatual=auxprox;
        //se as duas possuem a mesma prioridade dinamica é usada a estatica para desempate
        else if (auxatual->priod==auxprox->priod){
            if(auxatual->prios>auxprox->prios)
                auxatual=auxprox;
        }
        //proxima tarefa a ser comparada
        auxprox=auxprox->next;
    }
    //criei outra variavel para não confundir com as de cima
    //aqui será ajustada a prioridade dinamica das tarefas não escolhidas
    task_t *aux = *filaDeProntas;
    int alfa = -1;
    for(i=0; i<j; i++){
        //compara se é a tarefa escolhida ou se possui a prioridade máxima
        if((aux->priod!=-20)&&(aux!=auxatual))
            //se nao é, há o ajuste da prioridade
            aux->priod=aux->priod+alfa;
        aux=aux->next;
    }
    //a prioridade da tarefa escolhida retorna ao seu valor estático
    auxatual->priod=auxatual->prios;
    return auxatual;
}

void task_yield () {
    queue_append((queue_t **) &filaDeProntas, (queue_t*) AtualTask);
    task_switch(&Dispatcher);
}


void dispatcher_body () // dispatcher é uma tarefa
{
    task_setUsuario (AtualTask, 0);
    queue_remove((queue_t **)&filaDeProntas,(queue_t*) &MainTask);
    task_t* next;
    int userTasks = queue_size((queue_t *)filaDeProntas);
    int j;
    while ( userTasks > 0 )
    {
        next = scheduler() ; // scheduler é uma função
        if (next)
        {
            // ações antes de lançar a tarefa "next", se houverem
            ticksTarefa=20;
            queue_append((queue_t **) &filaDeProntas, (queue_t*) &Dispatcher);
            task_switch (next) ; // transfere controle para a tarefa "next"
            userTasks = queue_size((queue_t *)filaDeProntas); //caso task_exit, número de prontas vai diminuir
            }
            // ações após retornar da tarefa "next", se houverem
        }
    task_exit(0) ; // encerra a tarefa dispatcher
}

void task_setprio (task_t *task, int prio){
    //limites de valor para a prioridade
    if((prio<-20)||(prio>20)){
        printf("valor de prioridade invalido");
        return NULL;
    }
    task->prios = prio;
    task->priod = prio;
}

int task_getprio (task_t *task){
    return AtualTask->priod;
}
void task_setUsuario (task_t *task, int flag){
    task->flagUsuario=flag;
}

void tratador (int signum)
{
    if(!(ticksTarefa) && (AtualTask->flagUsuario))
        task_yield();
    else
        {if(ticksTarefa)
            ticksTarefa--;}
}
