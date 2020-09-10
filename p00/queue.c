#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "queue.h"


void queue_append (queue_t **queue, queue_t *elem) {
    if(!queue){
        printf("ERRO: LISTA INEXISTENTE\n");
        return NULL;}

    if(elem->next!=NULL || elem->prev!=NULL){ //TESTA ELEMENTO
        printf("ERRO: ELEMENTO JA PRESENTE EM UMA LISTA\n");
        return NULL;
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
    queue_t *aux;
    queue_t *anterior, *proximo;
    anterior=elem->prev;
    proximo=elem->next;
    int flag=0;

    if(!queue){
        printf("ERRO: LISTA INEXISTENTE\n");
        return NULL;}

    if (*queue==NULL){ //LISTA VAZIA
        printf("ERRO: LISTA VAZIA\n");
        return NULL;
    }
    aux = *queue;
    do
    {
        if(aux==elem){ //testa se o elemento está presente na fila
            flag++;
            break;} //aux agora está na posição do elemento
        aux=aux->next;
    }
    while (aux != *queue);
    if (!flag){//elemento não pertence a lista
        printf("ERRO: ELEMENTO NÃO PERTENCE A LISTA\n");
        return NULL;
    }
    anterior->next=elem->next;
    proximo->prev=elem->prev;

    if(*queue==elem){ //caso especial da remoção do primeiro elemento;
        *queue=elem->next;
        if(*queue==elem) //caso especial2 da remoção do único elemento;
            *queue=NULL;}
    elem->next=NULL;
    elem->prev=NULL;
    return elem;
}

int queue_size (queue_t *queue){
    queue_t *aux;
    aux=queue;
    int contador=0;
    if (queue==NULL) //LISTA VAZIA
        return 0;
    do{
        aux=aux->next;
        contador++;}
    while(aux!=queue);
    return contador;
}
void queue_print (char *name, queue_t *queue, void print_elem (void*) ) {

}

