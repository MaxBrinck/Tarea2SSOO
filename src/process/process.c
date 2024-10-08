#include "process.h"
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "../queue/queue.h"
// Función para crear un nuevo proceso
Process* create_process(char **data) {
    Process *p = (Process *)malloc(sizeof(Process));
    if (p == NULL) {
        perror("Error al asignar memoria para el proceso");
        return NULL;
    }

    // Asegúrate de que Nombre tenga suficiente espacio
    p->Nombre = (char *)malloc(strlen(data[0]) + 1);
    if (p->Nombre == NULL) {
        perror("Error al asignar memoria para el nombre del proceso");
        free(p);
        return NULL;
    }

    strcpy(p->Nombre, data[0]);
    p->pid = atoi(data[1]);
    p->T_INICIO = atoi(data[2]);
    p->Tiempo_ejecucion = atoi(data[3]);
    p->Numero_rafagas = atoi(data[4]);
    p->Tiempo_espera = atoi(data[5]);
    p->Deadline = atoi(data[6]);
    p->Estado = WAITING;  // Inicializar el estado
    p->tlcpu = -1;       // Inicializar tlcpu a 0 o al tiempo actual si lo necesitas

    return p;
}





 
 // Función para recorrer la cola y realizar una acción con cada proceso
void wait_ready(Queue* queue, int tiempo_global) {
    //printf("wait_ready\n");
    Node* current = queue->front; // Comenzar desde el frente de la cola
           
    // Recorrer mientras haya nodos
    while (current != NULL) {
    // Aquí puedes realizar cualquier acción con el proceso
        Process* proceso = current->process;
 
        if (proceso->Estado == WAITING && proceso->Tiempo_espera + proceso->tlcpu <=  tiempo_global && proceso->tlcpu != -1){
            proceso->Estado = READY;
        }
               
            // Mover al siguiente nodo
        current = current->next;
    }
}

void first_in(Queue* queue, int tiempo_global, Queue* high_priority_queue) {
    //printf("first in\n");
    Node* current = queue->front; // Comenzar desde el frente de la cola
    // Recorrer mientras haya nodos
    while (current != NULL) {
    // Aquí puedes realizar cualquier acción con el proceso
        Process* proceso = current->process;
        printf("Proceso entra en el while de first_in\n");
        //printf("estado del proceso %d tiempo_inicio  %d  tiempo_global", proceso->Estado);
        if (proceso->Estado == WAITING && proceso->T_INICIO == tiempo_global && proceso->tlcpu == -1){
            printf("Estado: %d, T_INICIO: %d, tlcpu: %d, tiempo_global: %d\n", proceso->Estado, proceso->T_INICIO, proceso->tlcpu, tiempo_global);

            proceso->Estado = READY;

            proceso->q_h = high_priority_queue->quantum_ejecucion;
            printf("Proceso cumple condicione para entrar\n");
            enqueue(high_priority_queue, proceso);
            printf("Proceso en high %d\n", high_priority_queue->front->process->pid);
        }
               
            // Mover al siguiente nodo
        current = current->next;
    }
}

void verificar_y_sumar_deadline(Process *proceso, int tiempo_global) {
    int x;

    if(proceso->tlcpu == -1){
        x=0;
    }
    else{
        x=proceso->tlcpu;
    }
    // Verifica si el proceso está pasado su deadline
    int tiempo_pasado_deadline = (tiempo_global - x) - proceso->Deadline;
    
    // Si está ejecutando pasado su deadline
    if (tiempo_pasado_deadline > 0) {
        // Sumar el tiempo pasado al total
        proceso->tiempo_pasado += tiempo_pasado_deadline;
    }
}


void all_ready (Queue* low_priority_queue, Queue* high_priority_queue, int tiempo_global) {
    printf("Entra en all ready\n");
    Node* current_l = low_priority_queue->front;
    Node* current_h = high_priority_queue->front;
    int ocupado = 0;
    Process *prioritario = NULL; // Inicializar prioritario como NULL
    int prioridad = -100000000;
    int prioridad_h;
    int prioridad_l;

    // Hacemos una revisión para ver si hay algún proceso corriendo
    while (current_l != NULL) {
        Process* proceso_l = current_l->process;
        if (proceso_l->Estado == RUNNING) {
            ocupado = 1;
            return; // Ya hay un proceso corriendo, salimos
        }
        current_l = current_l->next; // Avanzar al siguiente nodo
    }

    while (current_h != NULL) {
        Process* proceso_h = current_h->process;
        if (proceso_h->Estado == RUNNING) {
            ocupado = 1;
            return; // Ya hay un proceso corriendo, salimos
        }
        current_h = current_h->next; // Avanzar al siguiente nodo
    }

    // Si no hay procesos corriendo
    if (ocupado == 0) {
        Node* current_h1 = high_priority_queue->front;
        while (current_h1 != NULL) {
            Process* proceso_h = current_h1->process;
            if (proceso_h->Estado == READY) {
                prioridad_h = (tiempo_global - proceso_h->tlcpu) - proceso_h->Deadline;
                if (prioritario == NULL || prioridad_h > prioridad) {
                    prioritario = proceso_h; // Asignar proceso con mayor prioridad
                    prioridad = prioridad_h;
                    printf("%d", prioridad);
                    printf("El proceso seleccionado en primera instancia es %d\n", prioritario->pid);
                } else if (prioridad == prioridad_h) {
                    if (prioritario->pid > proceso_h->pid) {
                        prioritario = proceso_h;
                        prioridad = prioridad_h;
                    }
                }
            }
            current_h1 = current_h1->next; // Avanzar al siguiente nodo
        }
    }

    if (ocupado == 0) {
        Node* current_l1 = low_priority_queue->front;
        while (current_l1 != NULL) {
            Process* proceso_l = current_l1->process;
            if (proceso_l->Estado == READY) {
                prioridad_l = (tiempo_global - proceso_l->tlcpu) - proceso_l->Deadline;
                if (prioritario == NULL || prioridad_l > prioridad) {
                    prioritario = proceso_l; // Asignar proceso con mayor prioridad
                    prioridad = prioridad_l;
                } else if (prioridad == prioridad_l) {
                    if (prioritario->pid > proceso_l->pid) {
                        prioritario = proceso_l;
                        prioridad = prioridad_l;
                    }
                }
            }
            current_l1 = current_l1->next; // Avanzar al siguiente nodo
        }
    }

    // Verificamos que prioritario no sea NULL antes de asignar el estado
    if (prioritario != NULL) {
        if (prioritario->p_ready == 0){
            prioritario->p_ready = 1;
            printf("Proceso %d activa su primer ready en tiempo %d\n", prioritario->pid, tiempo_global);
            prioritario->primero = tiempo_global;
        }
        verificar_y_sumar_deadline(prioritario, tiempo_global);
        prioritario->Estado = RUNNING;

        printf("Proceso %d en estado RUNNING\n", prioritario->pid);
    }
}





void Actualizar_runing(Queue* final, Queue* queue, Queue* low_priority_queue, Queue* high_priority_queue,int tiempo_global, int quantum_high, int quantum_low){
    printf("cuantum high %d, cuantum low%d\n" , high_priority_queue->quantum_ejecucion, low_priority_queue->quantum_ejecucion);
    
    //printf("act running\n");
    Node* current = queue->front;
    Node* siguiente = NULL;
    while (current != NULL){
        siguiente = current->next;
        Process* proceso = current->process;
        printf("while de actualizar running\n");
        
        
        if (proceso->Estado == RUNNING){
            proceso->tiempo_cpu += 1;
            if (proceso->p_ready == 1 && proceso->w_ready == 0) {
                proceso->response_time =  proceso->primero-proceso->T_INICIO;
                proceso->w_ready = 1;
                printf("Proceso %d entra a la cpu por primera vez en tiempo %d\n", proceso->pid, tiempo_global);
            };

            
            printf("pillo el running\n");
            printf("IDENTIFICADOR COLA %d\n AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA",queue->identificador);
            if(queue->identificador == 1){
                proceso->q_h-=1;
                printf("El quantum que tiene ejecutandose en high es %d  jjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjj \n", proceso->q_h);
                proceso->q = proceso->q_h;            
                }
            else {
                proceso->q_l-=1;
                printf("El quantum que tiene ejecutandose en low es %d  jjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjj \n", proceso->q_l);
                proceso->q = proceso->q_l;
            }



            if (proceso->q == 0){
                proceso->tlcpu = tiempo_global;
                if (proceso->T_pendiente !=0){
                    proceso->T_pendiente-=1;
                }
               


                //Lo que hay que hacer si se termina el quantum
                if (proceso->T_pendiente == 0){
                    //se termina el quantum pero terminaste la rafaga actual
                    printf("Se termina el quantum pero terminaste la rafaga actual\n");
                    printf("Numero de rafagas completas %d\n", proceso->rafagas_completas);

                    proceso->rafagas_completas+= 1; //Si no tiene tiempo pendiente se completa una rafaga
                    if (proceso->rafagas_completas == proceso->Numero_rafagas){
                    
                        //No queda quantum, terminaste rafaga actual y rafagas totales
                        printf("NUMERO DE RAFAGAS COMPLETAS SIN QUANTUM\n");
                        proceso->tlcpu = tiempo_global;
                        proceso->Estado = FINISHED; //si completo sus rafagas pasa terminar
                        proceso->turnaround_time = tiempo_global - proceso->T_INICIO;
                        proceso->tlcpu = tiempo_global;
                    
                        proceso->wainting_time = tiempo_global - proceso->T_INICIO - proceso->Tiempo_ejecucion*proceso->Numero_rafagas;
                        
                        printf("proceso %d termino en tiempo global  %d con un tiempo de inicio de %d con un tclpu de %d\n con un primero de %d", proceso->pid, tiempo_global, proceso->T_INICIO, proceso->tlcpu, proceso->primero);
                        enqueue(final, proceso);
                        dequeue(queue, proceso);
                        proceso->turnaround_time = tiempo_global - proceso->T_INICIO;
                        proceso->T_pendiente = 0;
                        
                    }
                    else {
                        //No queda quantum, terminaste rafaga actual, no totales
                        printf("No queda quantum, termina rafaga actual y no totales\n");
                        proceso->Estado = WAITING;
                        proceso->tlcpu = tiempo_global;
                        if (queue->identificador == 1) {

                            //Creo que se podria borrar pero dice que cuando cedes la cpu te mantienes en la lista que ya estabas
                            enqueue(high_priority_queue, proceso);
                            dequeue(queue, proceso);
                        }
                        proceso->T_pendiente = proceso->Tiempo_ejecucion;
                    }
                    //Nose si hay que hacer algo mas cuando terminas una rafaga pero aun quedan algunas pendientes
                    }
                else {
                    printf("HACIENDO UNA INTERRUPCIOOOON\n");
                    proceso->interrupciones +=1;
                    //Cuando no has terminado la rafaga y se te acaba el quantum
                    proceso->Estado = READY;
                    proceso->tlcpu = tiempo_global;
                    printf("No termina la rafaga y se acaba quantum\n");

                    if (queue->identificador == 1) {
                        enqueue(low_priority_queue, proceso);
                        dequeue(queue, proceso);
                        
                    }
                    //aca habria un else de si es low pero no pasa nada xd
                }
                if (queue->identificador == 1) {
                        queue->quantum_ejecucion = quantum_high;
                        proceso->q_h = queue->quantum_ejecucion;
                        //nose si tengo que hace proceso->q = proceso->q_h
                    }
                    else{
                        queue->quantum_ejecucion = quantum_low;
                        proceso->q_l = queue->quantum_ejecucion;
                        //nose si tengo que hace proceso->q = proceso->q_l
                    }   

            }
            else{
                if (proceso->T_pendiente !=0){
                    printf("SE ESTA RESTANDO UN TIEMPO Y EL QUANTUM NO ES CERO\n");
                    proceso->T_pendiente-=1;

                }
                //No se acabó el quantum
                printf("No se acaba el quantum\n");
                if (proceso->T_pendiente == 0){
                    //No se acaba el quantum pero si terminaste la rafaga
                    printf("No se acaba quantum pero si termina rafaga\n");

                    proceso->rafagas_completas += 1; //Si no tiene tiempo pendiente se completa una rafaga
                    printf("Numero de rafagas completas %d\n", proceso->rafagas_completas);
                    if (proceso->rafagas_completas == proceso->Numero_rafagas){
                        
                        //Queda quantum, terminaste rafaga actual y rafagas totales
                        printf("Queda quantum, termina rafaga actual y totales\n");
                        printf("proceso %d termino en tiempo global  %d con un tiempo de inicio de %d con un tclpu de %d\n con un tiempo en cpu de %d y un deadline de %d\n", proceso->pid, tiempo_global, proceso->T_INICIO, proceso->tlcpu, proceso->tiempo_cpu, proceso->Deadline);
                        proceso->Estado = FINISHED; //si completo sus rafagas pasa terminar
                        proceso->turnaround_time = tiempo_global - proceso->T_INICIO;
                        proceso->wainting_time = tiempo_global - proceso->T_INICIO - proceso->Tiempo_ejecucion*proceso->Numero_rafagas;
                        proceso->tlcpu = tiempo_global;
                
                        enqueue(final, proceso);
                        dequeue(queue, proceso);
                        proceso->turnaround_time = tiempo_global - proceso->T_INICIO;
                        
                    }
                    else {
                        //Queda quantum, no terminaste las rafagas totales pero si la actual
                        printf("Queda quantum, no termina rafaga totales pero si actual\n");
                        proceso->Estado = WAITING;
                        
                        //if (queue->identificador== 1) {

                            //Creo que lo podriamos borrar porque dice que al ceder cpu te mantienes en tu cola, no cambias a low                                                   COMENTE ESTO PORQUE SEGHUN YO ESTA MAL
                            //enqueue(high_priority_queue, proceso);
                            //dequeue(queue, proceso);
    
                        //}
                    }
                    proceso->T_pendiente = proceso->Tiempo_ejecucion;
                    proceso->tlcpu = tiempo_global;
                    //Nose si hay que hacer algo mas cuando terminas una rafaga pero aun quedan algunas pendientes
                    printf("Queda quantum pero si termina rafaga\n");

                    //if (queue->identificador == 1) {
                        //queue->quantum_ejecucion = quantum_high;
                        //proceso->q_h = queue->quantum_ejecucion;
                    //}
                    //else{
                        //queue->quantum_ejecucion = quantum_low;
                        //proceso->q_l = queue->quantum_ejecucion;
                    //}   


                    }
                else {
                    printf("Queda quantum y no terminaste la rafaga actual ni las totales\n");
                    printf("tiempo pendiente de %d es %d\n", proceso->pid, proceso->T_pendiente);   
                   
                    //Queda quantum y no terminaste la rafaga actual ni las totales
                    // no pasa nati
                    
                }
            


            }

         //En cada iteracion reducimos 1 el quantum de la ejecucion
        //Decrementamos en 1 su tiempo pendiente

            
            current = siguiente;

        printf("tiempo pendiente de %d es %d\n", proceso->pid, proceso->T_pendiente);    
        }
        else{
            current = siguiente;
            //proceso no esta running
        }
    }
}

void low_to_high(Queue* low_priority_queue, Queue* high_priority_queue, int tiempo_global){
    //printf("low_to_high\n");
    //funcion para mandar un proceso de low a high si es que 2*deadline < Tiempo_global - tlcpu

    Node* current = low_priority_queue->front; // Comenzar desde el frente de la cola
    Node* siguiente = NULL;
           
    // Recorrer mientras haya nodos
    while (current != NULL) {
        siguiente = current->next;
    // Aquí puedes realizar cualquier acción con el proceso
        Process *proceso = current->process;
        int formula = 2*proceso->Deadline;
        printf("Deadline %d, formula %d, tiempo global %d\n", proceso->Deadline, formula, tiempo_global);
        printf("global - tlcpu = %d", tiempo_global-proceso->tlcpu);

        if (formula < tiempo_global - proceso->tlcpu){;
            enqueue(high_priority_queue, proceso);
            printf("Proceso %d pasa a high\n", proceso->pid);
            dequeue(low_priority_queue, proceso);
            printf("YENDO A HIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIGH\n");
            
        }
               
            // Mover al siguiente nodo
        current = siguiente;
        

    }
}
       

