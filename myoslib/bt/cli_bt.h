/** 
 **************************************************************
 * @file myoslib/bt/cli_bt.h
 * @author Cameron Stroud - 44344968
 * @date 03052020
 * @brief BT CLI header file
 ***************************************************************
 * EXTERNAL FUNCTIONS 
 ***************************************************************
 * 
 *************************************************************** 
 */

#ifndef S4434496_CLI_BT_H
#define S4434496_CLI_BT_H

/* Function prototypes -------------------------------------------------------*/
extern void cli_bt_init( void );
extern void cli_bt_deinit( void );

/* Commands ------------------------------------------------------------------*/
BaseType_t btCommand(char * pcWriteBuffer, size_t xWriteBufferLen,
                        const char * pcCommandString);


#endif // S4434496_CLI_BT_H