/*********************************************************************************************************************
 * @file:        app_fifo.h
 * @author:      
 * @version:     V01.00
 * @date:        
 * @Description: 
 * @ChangeList:  01. ����
*/

#ifndef _APP_FIFO_H__
#define _APP_FIFO_H__

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

/** @} */
#define FIFO_ERROR_BASE_NUM      (0x0)          ///< Global error base
#define FIFO_SUCCESS                           (FIFO_ERROR_BASE_NUM + 0)  ///< Successful command
#define FIFO_ERROR_SVC_HANDLER_MISSING         (FIFO_ERROR_BASE_NUM + 1)  ///< SVC handler is missing
#define FIFO_ERROR_SOFTDEVICE_NOT_ENABLED      (FIFO_ERROR_BASE_NUM + 2)  ///< SoftDevice has not been enabled
#define FIFO_ERROR_INTERNAL                    (FIFO_ERROR_BASE_NUM + 3)  ///< Internal Error
#define FIFO_ERROR_NO_MEM                      (FIFO_ERROR_BASE_NUM + 4)  ///< No Memory for operation
#define FIFO_ERROR_NOT_FOUND                   (FIFO_ERROR_BASE_NUM + 5)  ///< Not found
#define FIFO_ERROR_NOT_SUPPORTED               (FIFO_ERROR_BASE_NUM + 6)  ///< Not supported
#define FIFO_ERROR_INVALID_PARAM               (FIFO_ERROR_BASE_NUM + 7)  ///< Invalid Parameter
#define FIFO_ERROR_INVALID_STATE               (FIFO_ERROR_BASE_NUM + 8)  ///< Invalid state, operation disallowed in this state
#define FIFO_ERROR_INVALID_LENGTH              (FIFO_ERROR_BASE_NUM + 9)  ///< Invalid Length
#define FIFO_ERROR_INVALID_FLAGS               (FIFO_ERROR_BASE_NUM + 10) ///< Invalid Flags
#define FIFO_ERROR_INVALID_DATA                (FIFO_ERROR_BASE_NUM + 11) ///< Invalid Data
#define FIFO_ERROR_DATA_SIZE                   (FIFO_ERROR_BASE_NUM + 12) ///< Invalid Data size
#define FIFO_ERROR_TIMEOUT                     (FIFO_ERROR_BASE_NUM + 13) ///< Operation timed out
#define FIFO_ERROR_NULL                        (FIFO_ERROR_BASE_NUM + 14) ///< Null Pointer
#define FIFO_ERROR_FORBIDDEN                   (FIFO_ERROR_BASE_NUM + 15) ///< Forbidden Operation
#define FIFO_ERROR_INVALID_ADDR                (FIFO_ERROR_BASE_NUM + 16) ///< Bad Memory Address
#define FIFO_ERROR_BUSY                        (FIFO_ERROR_BASE_NUM + 17) ///< Busy
#define FIFO_ERROR_CONN_COUNT                  (FIFO_ERROR_BASE_NUM + 18) ///< Maximum connection count exceeded.
#define FIFO_ERROR_RESOURCES                   (FIFO_ERROR_BASE_NUM + 19) ///< Not enough resources for operation

/**@brief   A FIFO instance structure.
 * @details Keeps track of which bytes to read and write next.
 *          Also, it keeps the information about which memory is allocated for the buffer
 *          and its size. This structure must be initialized by app_fifo_init() before use.
 */
typedef struct
{
    uint8_t *          p_buf;           /**< Pointer to FIFO buffer memory.                      */
    uint16_t           buf_size_mask;   /**< Read/write index mask. Also used for size checking. */
    volatile uint32_t  read_pos;        /**< Next read position in the FIFO buffer.              */
    volatile uint32_t  write_pos;       /**< Next write position in the FIFO buffer.             */
} app_fifo_t;

/**@brief Function for initializing the FIFO.
 *
 * @param[out] p_fifo   FIFO object.
 * @param[in]  p_buf    FIFO buffer for storing data. The buffer size must be a power of two.
 * @param[in]  buf_size Size of the FIFO buffer provided. This size must be a power of two.
 *
 * @retval     NRF_SUCCESS              If initialization was successful.
 * @retval     NRF_ERROR_NULL           If a NULL pointer is provided as buffer.
 * @retval     NRF_ERROR_INVALID_LENGTH If size of buffer provided is not a power of two.
 */
uint32_t app_fifo_init(app_fifo_t * p_fifo, uint8_t * p_buf, uint16_t buf_size);

/**@brief Function for adding an element to the FIFO.
 *
 * @param[in]  p_fifo   Pointer to the FIFO.
 * @param[in]  byte     Data byte to add to the FIFO.
 *
 * @retval     NRF_SUCCESS              If an element has been successfully added to the FIFO.
 * @retval     NRF_ERROR_NO_MEM         If the FIFO is full.
 */
uint32_t app_fifo_put(app_fifo_t * p_fifo, uint8_t byte);

/**@brief Function for getting the next element from the FIFO.
 *
 * @param[in]  p_fifo   Pointer to the FIFO.
 * @param[out] p_byte   Byte fetched from the FIFO.
 *
 * @retval     NRF_SUCCESS              If an element was returned.
 * @retval     NRF_ERROR_NOT_FOUND      If there are no more elements in the queue.
 */
uint32_t app_fifo_get(app_fifo_t * p_fifo, uint8_t * p_byte);

/**@brief Function for looking at an element in the FIFO, without consuming it.
 *
 * @param[in]  p_fifo   Pointer to the FIFO.
 * @param[in]  index    Which element to look at. The lower the index, the earlier it was put.
 * @param[out] p_byte   Byte fetched from the FIFO.
 *
 * @retval     NRF_SUCCESS              If an element was returned.
 * @retval     NRF_ERROR_NOT_FOUND      If there are no more elements in the queue, or the index was
 *                                      too large.
 */
uint32_t app_fifo_peek(app_fifo_t * p_fifo, uint16_t index, uint8_t * p_byte);

/**@brief Function for flushing the FIFO.
 *
 * @param[in]  p_fifo   Pointer to the FIFO.
 *
 * @retval     NRF_SUCCESS              If the FIFO was flushed successfully.
 */
uint32_t app_fifo_flush(app_fifo_t * p_fifo);

/**@brief Function for reading bytes from the FIFO.
 *
 * This function can also be used to get the number of bytes in the FIFO.
 *
 * @param[in]    p_fifo        Pointer to the FIFO. Must not be NULL.
 * @param[out]   p_byte_array  Memory pointer where the read bytes are fetched from the FIFO.
 *                             Can be NULL. If NULL, the number of bytes that can be read in the FIFO
 *                             are returned in the p_size parameter.
 * @param[inout] p_size        Address to memory indicating the maximum number of bytes to be read.
 *                             The provided memory is overwritten with the actual number of bytes
 *                             read if the procedure was successful. This field must not be NULL.
 *                             If p_byte_array is set to NULL by the application, this parameter
 *                             returns the number of bytes in the FIFO.
 *
 * @retval     NRF_SUCCESS          If the procedure is successful. The actual number of bytes read might
 *                                  be less than the requested maximum, depending on how many elements exist
 *                                  in the FIFO. Even if less bytes are returned, the procedure is considered
 *                                  successful.
 * @retval     NRF_ERROR_NULL       If a NULL parameter was passed for a parameter that must not
 *                                  be NULL.
 * @retval     NRF_ERROR_NOT_FOUND  If the FIFO is empty.
 */
uint32_t app_fifo_read(app_fifo_t * p_fifo, uint8_t * p_byte_array, uint32_t * p_size);

/**@brief Function for writing bytes to the FIFO.
 *
 * This function can also be used to get the available size on the FIFO.
 *
 * @param[in]  p_fifo       Pointer to the FIFO. Must not be NULL.
 * @param[in]  p_byte_array Memory pointer containing the bytes to be written to the FIFO.
 *                          Can be NULL. If NULL, this function returns the number of bytes
 *                          that can be written to the FIFO.
 * @param[inout] p_size     Address to memory indicating the maximum number of bytes to be written.
 *                          The provided memory is overwritten with the number of bytes that were actually
 *                          written if the procedure is successful. This field must not be NULL.
 *                          If p_byte_array is set to NULL by the application, this parameter
 *                          returns the number of bytes available in the FIFO.
 *
 * @retval     NRF_SUCCESS       If the procedure is successful. The actual number of bytes written might
 *                               be less than the requested maximum, depending on how much room there is in
 *                               the FIFO. Even if less bytes are written, the procedure is considered
 *                               successful. If the write was partial, the application should use
 *                               subsequent calls to attempt writing the data again.
 * @retval     NRF_ERROR_NULL    If a NULL parameter was passed for a parameter that must not
 *                               be NULL.
 * @retval     NRF_ERROR_NO_MEM  If the FIFO is full.
 *
 */
uint32_t app_fifo_write(app_fifo_t * p_fifo, uint8_t const * p_byte_array, uint32_t * p_size);


#ifdef __cplusplus
}
#endif

#endif // APP_FIFO_H__

/** @} */
