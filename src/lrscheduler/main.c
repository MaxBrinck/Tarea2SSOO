#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../process/process.h"
#include "../queue/queue.h"
#include "../file_manager/manager.h"

#include <stdio.h>
#include <stdlib.h>
#include "../queue/queue.h"
#include "../process/process.h"

//borrrar



// Función para generar el archivo de salida
void generate_output_file(Queue *final, const char *filename) {
    // Abrir el archivo para escribir
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        fprintf(stderr, "Error al abrir el archivo de salida.\n");
        return;
    }

    // Crear un arreglo temporal para ordenar los procesos por PID
    int num_processes = final->size;
    Process **processes = (Process **)malloc(num_processes * sizeof(Process *));
    Node *current = final->front;
    int index = 0;

    // Copiar todos los procesos de la cola a un arreglo para poder ordenarlos
    while (current != NULL) {
        processes[index++] = current->process;
        current = current->next;
    }

    // Ordenar los procesos por PID de menor a mayor
    for (int i = 0; i < num_processes - 1; ++i) {
        for (int j = i + 1; j < num_processes; ++j) {
            if (processes[i]->pid > processes[j]->pid) {
                Process *temp = processes[i];
                processes[i] = processes[j];
                processes[j] = temp;
            }
        }
    }

    // Escribir los datos en el archivo
    fprintf(file, "Nombre del proceso,PID,Numero de interrupciones,Turnaround Time,Response Time,Waiting Time,Tiempos ejecucion pasado deadline\n");
    
    for (int i = 0; i < num_processes; ++i) {
        Process *p = processes[i];
        fprintf(file, "%s,%d,%d,%d,%d,%d,%d\n",
            p->Nombre,                  // Nombre del proceso
            p->pid,                     // PID
            p->interrupciones,           // Numero de interrupciones
            p->turnaround_time,          // Turnaround time
            p->response_time,            // Response time
            p->wainting_time,            // Waiting time
            p->T_pendiente               // Suma de tiempos pasado deadline (esto es solo un ejemplo, debes adaptar este campo si es diferente)
        );
    }

    // Liberar el arreglo temporal y cerrar el archivo
    free(processes);
    fclose(file);
}




void schedule(Queue *high_priority_queue, Queue *low_priority_queue, Queue *inicial, int quantum_high, int quantum_low, Queue *final){
    int tiempo_global = 0;
	
	while (1){

        //Primero se debe actualizar aquellos procesos que han finalizado su tiempo de espera
        wait_ready(low_priority_queue, tiempo_global);
        wait_ready(high_priority_queue, tiempo_global);

        //Ahora se debe actualizar los estados de los procesos en estado running
        Actualizar_runing(final, low_priority_queue, low_priority_queue, high_priority_queue, tiempo_global, quantum_high, quantum_low);
        Actualizar_runing(final, high_priority_queue, low_priority_queue, high_priority_queue, tiempo_global, quantum_high, quantum_low);
        
        //Se revisa aquellos procesos que deben entrar por primera vez
        first_in(inicial, tiempo_global);
        
        //Verificamos aquellos procesos que deben pasar de la cola low a la cola high
        low_to_high(low_priority_queue, high_priority_queue, tiempo_global);
        
        //Si No hay procesos running se entrega la cpu a uno
        all_ready(low_priority_queue, high_priority_queue, tiempo_global);




        int cantidad_inicial = inicial->size;
        int cantidad_final = final->size;

       printf("cantidad_incial: %d\n cantidad_final: %d\n", cantidad_inicial, cantidad_final);
        if (cantidad_inicial == cantidad_final){
            break;
        }

        
        

           

    tiempo_global++;
   

    }
}

int main(int argc, char const *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Uso: %s <nombre_del_archivo>\n", argv[0]);
        return EXIT_FAILURE;
    }

    char *file_name = (char *)argv[1];
    InputFile *input_file = read_file(file_name);

    printf("Nombre archivo: %s\n", file_name);
    printf("Cantidad de procesos: %d\n", input_file->len);
    printf("Procesos:\n");

    Queue *high_priority_queue = create_queue();
    Queue *low_priority_queue = create_queue();
	Queue *final = create_queue();
	Queue *inicial = create_queue();

    
    

    int var = 0;
    int q = 2; // Definir un quantum 
	int quantum_high = 2*q; //Definir un quantum para la cola de alta prioridad
	int quantum_low = q; //Definir un quantum para la cola de baja prioridad
    high_priority_queue->quantum_ejecucion = 2*q; //Inicializar el quantum de ejecucion
	low_priority_queue->quantum_ejecucion = q; //Inicializar el quantum de ejecucion
	high_priority_queue->identificador = 1; //Inicializar el identificador de la cola
	low_priority_queue->identificador = 0; //Inicializar el identificador de la cola
    

    
    
    for (int i = 0; i < input_file->len; ++i) {
        Process *proceso = (Process *)malloc(sizeof(Process));
        int pid = atoi(input_file->lines[i][1]);
        int tiempo_ejecucion = atoi(input_file->lines[i][3]);
        int numero_rafagas = atoi(input_file->lines[i][4]);
        int tiempo_espera = atoi(input_file->lines[i][5]);
        int deadline = atoi(input_file->lines[i][6]);
        int t_inicio = atoi(input_file->lines[i][2]);  // Agregar T_INICIO del archivo de entrada
        proceso->Nombre = (char *)malloc(strlen(input_file->lines[i][0]) + 1);
        strcpy(proceso->Nombre, input_file->lines[i][0]);
        proceso->pid = pid;
        proceso->Estado = WAITING;
        proceso->Tiempo_ejecucion = tiempo_ejecucion;
        proceso->Numero_rafagas = numero_rafagas;
        proceso->Tiempo_espera = tiempo_espera;
        proceso->Deadline = deadline;
        proceso->T_INICIO = t_inicio;
        proceso->tlcpu = -1; // Inicializar tlcpu
		proceso->rafagas_completas = 0;
		proceso->T_pendiente = tiempo_ejecucion;
		proceso->interrupciones = 0;
		proceso->wainting_time = 0;
        enqueue(inicial, proceso);

		

		

    }
    

    schedule(high_priority_queue, low_priority_queue, inicial, quantum_high, quantum_low, final);

    

    

    generate_output_file(final, "output_file.txt");

    input_file_destroy(input_file);
	freeQueue(high_priority_queue);
	freeQueue(low_priority_queue);
	freeQueue(final);
	freeQueue(inicial);
    // Liberar recursos

    return 0;
}
