/** 
 **************************************************************
 * @file myoslib/localisation/cli_loc.h
 * @author Cameron Stroud - 44344968
 * @date 19052020
 * @brief Localisation Commands header file
 ***************************************************************
 * EXTERNAL FUNCTIONS 
 ***************************************************************
 * 
 *************************************************************** 
 */

#ifndef S4434496_CLI_LOC_H
#define S4434496_CLI_LOC_H

/* Function prototypes -------------------------------------------------------*/
extern void cli_loc_init( void );
extern void cli_loc_deinit( void );

/* Commands ------------------------------------------------------------------*/
BaseType_t nodeCommand(char * pcWriteBuffer, size_t xWriteBufferLen,
                        const char * pcCommandString);
BaseType_t locCommand(char * pcWriteBuffer, size_t xWriteBufferLen,
                        const char * pcCommandString);


#endif // S4434496_CLI_LOC_H