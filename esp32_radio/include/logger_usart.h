#ifndef LOGGER_USART_H
#define LOGGER_USART_H

/*********************** EXT LIB ***********************/

/*********************** LOCAL LIB ***********************/
#include "common.h"

/********************* DEFINITIONS *********************/
//USART INTERFACES
#define DEBUG_USART Serial
#define PANEL_USART Serial2
//BUFFER THINGS
#define RX_BUF_SIZE 100
#define RX_STARTING_CH    '{'
#define RX_ENDING_CH      '}'

/********************* DATA STRUCTS ********************/
typedef enum{
  SM_BUF_EMPTY,
  SM_BUF_STARTED,
  SM_BUF_FINISHED,
  SM_BUF_OVERFLOW
}RX_BUF_STATE_T;

/********************** PROTOTYPES *********************/
void TaskUSART(void *pvParameters);
void TaskDEBUG(void *pvParameters);

#endif
