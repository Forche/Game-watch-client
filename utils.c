/*
 * conexiones.c
 *
 *  Created on: 2 mar. 2019
 *      Author: utnso
 */

#include "utils.h"

//TODO
/*
 * Recibe un paquete a serializar, y un puntero a un int en el que dejar
 * el tamaño del stream de bytes serializados que devuelve
 */
void* serializar_paquete(t_paquete* paquete, int *bytes)
{

}

int crear_conexion(char *ip, char* puerto)
{
	struct addrinfo hints;
	struct addrinfo *server_info;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	getaddrinfo(ip, puerto, &hints, &server_info);

	int socket_cliente = socket(server_info->ai_family, server_info->ai_socktype, server_info->ai_protocol);

	if(connect(socket_cliente, server_info->ai_addr, server_info->ai_addrlen) == -1)
		printf("error");

	freeaddrinfo(server_info);

	return socket_cliente;
}

//TODO
void enviar_mensaje(char* mensaje, int socket_cliente)
{
	t_mensaje *ptr_paqueteMensaje = (t_mensaje*) malloc(sizeof(t_mensaje));
	ptr_paqueteMensaje->msg_len = strlen(mensaje) + 1;
	ptr_paqueteMensaje->msg = mensaje;
	t_mensaje paqueteMensaje = (*ptr_paqueteMensaje);

	t_buffer *buffer = malloc(sizeof(t_buffer));
	buffer->size = sizeof(int) + paqueteMensaje.msg_len;

	void *stream = malloc(buffer->size);
	int offset = 0;

	memcpy(stream + offset, &paqueteMensaje.msg_len, sizeof(int));
	offset += sizeof(int);
	memcpy(stream + offset, &paqueteMensaje.msg, paqueteMensaje.msg_len);
	buffer->stream = stream;

//	free(paqueteMensaje.msg);
//	free(ptr_paqueteMensaje->msg); WHY ROMPE (rompe por tener misma dir de memoria que el parametro?)
	free(ptr_paqueteMensaje);

	t_paquete *paquete = malloc(sizeof(t_paquete));
	paquete->codigo_operacion = MENSAJE;
	paquete->buffer = buffer;

	void *a_enviar = malloc(buffer->size + sizeof(op_code) + sizeof(int));
	offset = 0;

	memcpy(a_enviar + offset, &(paquete->codigo_operacion), sizeof(op_code));
	offset += sizeof(op_code);
	memcpy(a_enviar + offset, &(paquete->buffer->size), sizeof(int));
	offset += sizeof(int);
	memcpy(a_enviar + offset, paquete->buffer->stream, paquete->buffer->size);

	send(socket_cliente, a_enviar, sizeof(op_code) + buffer->size + sizeof(int), 0);


	free(a_enviar);
	free(paquete->buffer->stream);
	free(paquete->buffer);
	free(paquete);
}

//TODO
char* recibir_mensaje(int socket_cliente)
{
	t_paquete* paquete = malloc(sizeof(t_paquete));
	paquete->buffer = malloc(sizeof(t_buffer));
	recv(socket_cliente, &(paquete->codigo_operacion), sizeof(int), 0);
	recv(socket_cliente, &(paquete->buffer->size), sizeof(int), 0);
	paquete->buffer->stream = malloc(paquete->buffer->size);
	recv(socket_cliente, paquete->buffer->stream, paquete->buffer->size, 0);

	char *msg;

	switch(paquete->codigo_operacion) {
	    case MENSAJE: ;
	        t_mensaje* mensaje = deserializar_mensaje(paquete->buffer);
	        msg = malloc(mensaje->msg_len);
	        strcpy(msg, mensaje->msg);
	        break;
	}

	return msg;
}

t_mensaje* deserializar_mensaje(t_buffer* buffer) {
	t_mensaje* mensaje = malloc(buffer->size);

    void* stream = buffer->stream;

    // Por último, para obtener el nombre, primero recibimos el tamaño y luego el texto en sí:
    memcpy(&(mensaje->msg_len), stream, sizeof(int));
    stream += sizeof(int);
    memcpy(&(mensaje->msg), stream, mensaje->msg_len);

    return mensaje;
}

void liberar_conexion(int socket_cliente)
{
	close(socket_cliente);
}
