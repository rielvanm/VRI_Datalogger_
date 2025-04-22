#ifndef SD_FAILURE_HANDLER_H
#define SD_FAILURE_HANDLER_H

/**
 * @file SD_Failure_Handler.h
 * @brief Declares a handler function for SD card write failures during logging.
 *
 * This function should be implemented to handle unexpected SD card errors,
 * such as disconnection, write failure, or file corruption.
 * Typical actions might include stopping data logging, notifying the user,
 * switching to backup storage, or flashing an error message on the display.
 */

/**
 * @brief Handles an SD card failure that occurs during logging.
 * 
 * This function should be defined elsewhere (e.g., in a file called SD_Failure_Handler.cpp)
 * to respond appropriately when writing to the SD card fails.
 */
void handleSdFailureDuringLogging();

#endif
