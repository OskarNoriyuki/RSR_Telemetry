#include "logger_usart.h"

static const char *tag = "usarts";

//GLOBAL variables
char recvStr[RX_BUF_SIZE];
char lastMsgStr[RX_BUF_SIZE];
RX_BUF_STATE_T smRxBuf = SM_BUF_EMPTY;
int rxBufIndex = 0;
bool rxBufCollected = true;
bool rxBufReady = false;

void TaskUSART(void *pvParameters){
    char c;
    int breakW = 0;
    int auxCnt = 0;

    //PANEL_USART.begin(115200);

    for(;;){
        vTaskDelay(100/portTICK_PERIOD_MS);
        breakW = 0;
        auxCnt = 0;

        switch(smRxBuf){
        case SM_BUF_EMPTY:
            while(PANEL_USART.available() && breakW == 0){
            c = PANEL_USART.read();
            if(c == RX_STARTING_CH){
                smRxBuf = SM_BUF_STARTED;
                rxBufIndex = 0;
                breakW = 1;
            }
            }
            break;
        case SM_BUF_STARTED:
            while(PANEL_USART.available() && breakW == 0){
            c = PANEL_USART.read();
            if(c != RX_ENDING_CH){
                recvStr[rxBufIndex] = c;
                rxBufIndex++;
                if(rxBufIndex == RX_BUF_SIZE){
                smRxBuf = SM_BUF_OVERFLOW;
                breakW = 1;
                }
            }else{
                smRxBuf = SM_BUF_FINISHED;
                rxBufCollected = false;
                rxBufReady = true;
                recvStr[rxBufIndex + 1] = '\0';
                strcpy(lastMsgStr, recvStr);
                breakW = 1;
            }
            }
            break;
        case SM_BUF_FINISHED:
            if(rxBufCollected){
            smRxBuf = SM_BUF_EMPTY;
            rxBufReady = false;
            }
            break;
        case SM_BUF_OVERFLOW:
            DEBUG_USART.println("Error: RX2 overflow");
            smRxBuf = SM_BUF_EMPTY;
            break;
        }
    }
}

void TaskDEBUG(void *pvParameters){
    for(;;){
        vTaskDelay(500/portTICK_PERIOD_MS);
        //debug start
        if(rxBufReady){
            Serial.print("Message received: ");
            Serial.println(recvStr);
            rxBufCollected = true;
        }
    }
}