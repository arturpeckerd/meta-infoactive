/*
 * Copyright 2007, Crank Software Inc. All Rights Reserved.
 *
 * For more information email info@cranksoftware.com.
 */

/**
 * This is an API for sending and receiving events to other GRE IO
 * based clients.  The data is sent/received along a unidirectional
 * queue that can be implemented on top of a pipe/mqueue/shared memory
 * or some other data channel.
 */
#ifndef _GREIO_H_
#define _GREIO_H_

#if defined(GRE_TARGET_OS_android) || defined(ANDROID)
#define GREIO_INMEM
//#define GREIO_UNIX_PIPE
#elif defined(GRE_TARGET_OS_ios)
#define GREIO_INMEM
#elif defined(GRE_TARGET_OS_freertos)
#define GREIO_INMEM
#elif defined(GRE_TARGET_OS_integrity)
#define GREIO_INMEM
#elif defined(GRE_TARGET_OS_linux) || defined(__linux__)
#define GREIO_SYSVMQ
//#define GREIO_UNIX_PIPE
//#define GREIO_MQ
#elif defined(GRE_TARGET_OS_macos) || defined(__APPLE__)
#define GREIO_SYSVMQ
//#define GREIO_UNIX_PIPE
#elif defined(GRE_TARGET_OS_qnx) || defined(__QNXNTO__)
#define GREIO_MQ
#elif defined(GRE_TARGET_OS_ucosii)
#define GREIO_INMEM
#elif defined(GRE_TARGET_OS_wince) || defined(GRE_TARGET_OS_wincompact7) || defined(WINCE)
#define GREIO_CEMSMQ
#elif defined(GRE_TARGET_OS_win32) || defined(WIN32)
#define GREIO_MSMAILSLOT
//#define GREIO_WIN32_PIPE
#else
#error No default GREIO implementation defined for this platform
#endif

#include <stdint.h>

#define GRE_IO_TYPE_RDONLY		0x0         ///< Open channel for read access
#define GRE_IO_TYPE_XRDONLY		0x1         ///< Open channel for exclusive read access
#define GRE_IO_TYPE_WRONLY		0x2         ///< Open channel for write access
#define GRE_IO_TYPE_MASK		0xF         ///< Type mask

#define GRE_IO_FLAG_NONBLOCK	0x0010		///< Open channel nonblocking
#define GRE_IO_FLAG_MASK		0xFFF0		///< Special flag mask

#define GRE_IO_MAX_NAME_LEN		25          ///< Maximum name gre_io_open name length

#define GRE_IO_MAX_MSG_UNLIMITED	0

#ifdef __cplusplus
extern "C"{
#endif

/**
 * Opaque handle for IO operations.
 */
typedef struct _gre_io_ gre_io_t;

//TODO: Make this a single object instead of the pointer thing ...
typedef struct _gre_io_serialized_data {
		char    *buffer;            ///< Serialized buffer data
		int      buffer_nbytes;     ///< Number of bytes allocated for the buffer (capacity)
		int      data_nbytes;       ///< Number of bytes the data is occupying (size)
} gre_io_serialized_data_t;

typedef struct _multi_data_set {
	uint16_t	size;
	uint16_t	data_offset;
	uint32_t	data_size;
	char		format[8];  //this may need to be revisited if the the format string become bigger thaen 3 bytes
} gre_io_mdata_t;

/**
 * Open an IO connection using a named connection.
 *
 * @param io_name The name of the io-channel to use
 * @param flags The mode you want to open the queue in
 * @return handle to greio channel
 */
gre_io_t *gre_io_open(const char *io_name, int flag, ...);

/**
 * Close an io connection.  Any pending clients will return
 *
 * with an error on their action.
 * @param handle A valid handle created with gre_io_open()
 */
void      gre_io_close(gre_io_t *handle);

/**
 * Send a serialized buffer to the handle.
 * @param handle A valid handle created with gre_io_open()
 * @param buffer A data buffer containing a serialized event
 * @return -1 on failure anything else is success
 */
int gre_io_send(gre_io_t *handle, gre_io_serialized_data_t *buffer);

/**
 * Receive a serialized event.  This call blocks until an event is received
 * or until the channel is destroyed.
 *
 * @param handle A valid handle created with gre_io_open()
 * @param buffer A pointer to a serialized buffer pointer.  If the
 *  buffer is NULL then a new buffer is allocated otherwise the buffer
 *  provided is used to store the received event.
 * @return The size of the message received or -1 on failure.
 */
int gre_io_receive(gre_io_t *handle, gre_io_serialized_data_t **buffer);

/**
 * This creates a data buffer to hold the serialized event data required
 * for transmission.  This allows a resizable buffer to be allocated once
 * and then re-used for multiple event transmissions.
 *
 * If the provided buffer is not NULL and already contains data within
 * its buffers, then the resizing of the buffer will transfer the contents
 * of the old internal buffer to the new buffer, similar to the realloc()
 * function.  The buffer may be used in this fashion when data is being
 * streamed so that multiple buffers are not required.
 *
 * @param buffer The buffer to be sized, or NULL to allocate a new buffer
 * @param nbytes The number of bytes this buffer should be able to support
 * @return A buffer with room for a message nbytes in size or NULL if no space available
 */
gre_io_serialized_data_t * gre_io_size_buffer(gre_io_serialized_data_t *buffer, int nbytes);

/**
 * This sets the data payload of the buffer to zero (0) bytes.
 * The buffer is not resized or free'ed, but data content in
 * the buffer should be considered lost.
 *
 * @param buffer The buffer to be de-allocated
 */
void gre_io_zero_buffer(gre_io_serialized_data_t *buffer);

/**
 * This de-allocates a buffer that has been allocated through the
 * calls to gre_io_* functions.
 *
 * @param buffer The buffer to be de-allocated
 */
void                       gre_io_free_buffer(gre_io_serialized_data_t *buffer);

/**
 * Transform event data elements (see <io_mgr.h>) into a serialized data buffer.
 *
 * @param buffer The buffer that will contain the serialized data or NULL if a new buffer should be allocated
 * @param event_addr The name of the event target, or NULL to send to the default target
 * @param event_name The name of the event to send, or NULL to send an empty event
 * @param event_format The format of the data (see <data_format.h>, or NULL if no data is being sent
 * @param event_data A pointer do the data to transmit, or NULL if no data is transmitted
 * @param event_nbytes The number of data bytes to transmit, or NULL if no data is transmitted
 * @return A buffer with the serialized data or NULL on error (errno is set)
 */
gre_io_serialized_data_t * gre_io_serialize(gre_io_serialized_data_t *buffer,
                                            const char *event_addr,
                                            const char *event_name,
                                            const char *event_format,
                                            const void *event_data, int event_nbytes);

/**
 * Transform a serialized buffer into event data elements (see <io_mgr.h>).  The
 * pointers returned point back into the content of the serialized buffer so the
 * buffer can't be de-allocated until clients are finished manipulating the data
 * elements.
 *
 * @param buffer The buffer containing the serialized data
 * @param event_addr Location to store the event target
 * @param event_name Location to store the event name
 * @param event_format  Location to store the event format
 * @param event_data  Location to store the event data
 * @return The number of bytes in the event_data structure
 */
int gre_io_unserialize(gre_io_serialized_data_t *buffer,
                        char **event_addr,
                        char **event_name,
                        char **event_format,
                        void **event_data);

/**
 * Add a data key/value pair to a serialized buffer.  
 * This call can be used to add serialize multiple data
 * sets into a single send. This buffer can be later sent via
 * the gre_io_send_mdata call.
 *
 * @param mbuffer The buffer containing the serialized data
 * @param key_name Name of the variable to be set in data manager
 * @param data_format The format of the data (see <data_format.h>, or NULL if no data is being sent
 * @param data  A pointer do the data to transmit
 * @param data_nbytes  The number of data bytes to transmit, or NULL if no data is transmitted
 * @return -1 on failure anything else is success
 */
int gre_io_add_mdata(gre_io_serialized_data_t **mbuffer,
				const char *key_name,
				const char *data_format,
				const void *data, int data_nbytes);

/**
 * Send a serialized buffer of mdata (data manager key/value pairs) to the handle.
 *
 * @param handle A valid handle created with gre_io_open()
 * @param md_buffer A data buffer containing a serialized event
 * @return -1 on failure anything else is success
 */
int gre_io_send_mdata(gre_io_t *handle, gre_io_serialized_data_t *md_buffer);

/**
 * Grow serialized buffer.
 *
 * @param handle A valid handle created with gre_io_open()
 * @param buffer A data buffer
 * @return -1 on failure anything else is success
 */
int gre_io_grow_buffer(gre_io_t *handle, gre_io_serialized_data_t *buffer);

/**
 * Get the maximum size of a message sent over IO channel
 *
 * @param handle A valid handle created with gre_io_open()
 * @return -1 on error, success the size in bytes
 */
int gre_io_max_message(gre_io_t *handle);

#ifdef __cplusplus
}
#endif

#endif /*_GREIO_H_*/
