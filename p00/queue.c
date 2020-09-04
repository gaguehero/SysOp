#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "queue.h"


void queue_append (queue_t **queue, queue_t *elem) {
    if(**queue==NULL){ //TESTA LISTA
        printf("ERRO: LISTA INEXISTENTE");
        return 0;
    }
    if(elem->next!=NULL || elem->prev!=NULL){ //TESTA ELEMENTO
        printf("ERRO: ELEMENTO JA PRESENTE EM UMA LISTA");
        return 0;
    }
    if (*queue==NULL){ //LISTA VAZIA
        *queue=elem;
        elem->next=elem;
        elem->prev=elem;
    }
    else{
        elem->next=*queue;
        elem->prev=(*queue)->prev; //AJUSTANDO OS PONTEIROS DO NOVO ELEMENTO
        (*queue)->prev->next=elem; //O ULTIMO ELEMENTO DA FILA APONTA PARA O ADICIONADO
        (*queue)->prev=elem; //O PRIMEIRO ELEMENTO TEM UM PONTEIRO DE PREV PARA O NOVO ELEMENTO
    }
}
queue_t *queue_remove (queue_t **queue, queue_t *elem){
    if(**queue==NULL){ //TESTA LISTA
        printf("ERRO: LISTA INEXISTENTE");
        return NULL;
    }
//fazer condicional para testar se elemento presente na lista fazer um for que busca um elemento com mesmo endereço de memória
    if (*queue==NULL){ //LISTA VAZIA
        printf("ERRO: LISTA VAZIA");
        return NULL;
    }
}




