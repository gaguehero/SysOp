// PingPongOS - PingPong Operating System
// Prof. Carlos A. Maziero, DAINF UTFPR
// Versão 1.0 -- Março de 2015
//
// Estruturas de dados internas do sistema operacional
#include <ucontext.h>
#ifndef __DATATYPES__
#define __DATATYPES__

// Estrutura que define uma tarefa
typedef struct task_t
{
    struct task_t *prev, *next; //utlizar com a biblioteca de filas
    int tid; //ID da tarefa
    ucontext_t contextoTarefa; //descritor da tarefa
    void *stack; //pilha
    struct task_t *parent; //tarefa pai
    int priod, prios; //prioridades estatica e dinamica
    //enum status_t status;
  // preencher quando necessário
} task_t ;

// estrutura que define um semáforo
typedef struct
{
  // preencher quando necessário
} semaphore_t ;

// estrutura que define um mutex
typedef struct
{
  // preencher quando necessário
} mutex_t ;

// estrutura que define uma barreira
typedef struct
{
  // preencher quando necessário
} barrier_t ;

// estrutura que define uma fila de mensagens
typedef struct
{
  // preencher quando necessário
} mqueue_t ;

#endif