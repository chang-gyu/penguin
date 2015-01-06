#ifndef __CONTROL_RPC__
#define __CONTROL_RPC__

#include <util/list.h>
#include <control/vmspec.h>

#define RPC_MAGIC		"PNRPC"
#define RPC_MAGIC_SIZE		5
#define RPC_VERSION		1
#define RPC_BUFFER_SIZE		8192

typedef enum {
	RPC_TYPE_HELLO_REQ = 1,
	RPC_TYPE_HELLO_RES,
	RPC_TYPE_VM_CREATE_REQ,
	RPC_TYPE_VM_CREATE_RES,
	RPC_TYPE_VM_GET_REQ,
	RPC_TYPE_VM_GET_RES,
	RPC_TYPE_VM_SET_REQ,
	RPC_TYPE_VM_SET_RES,
	RPC_TYPE_VM_DELETE_REQ,
	RPC_TYPE_VM_DELETE_RES,		// 10
	RPC_TYPE_VM_LIST_REQ,
	RPC_TYPE_VM_LIST_RES,
	RPC_TYPE_STATUS_GET_REQ,
	RPC_TYPE_STATUS_GET_RES,
	RPC_TYPE_STATUS_SET_REQ,
	RPC_TYPE_STATUS_SET_RES,
	RPC_TYPE_STORAGE_DOWNLOAD_REQ,
	RPC_TYPE_STORAGE_DOWNLOAD_RES,
	RPC_TYPE_STORAGE_UPLOAD_REQ,
	RPC_TYPE_STORAGE_UPLOAD_RES,	// 20
	RPC_TYPE_STDIO_REQ,
	RPC_TYPE_STDIO_RES,		// 22
	RPC_TYPE_END,
} RPC_TYPE;

typedef struct _RPC RPC;

struct _RPC {
	// Connection information
	int		ver;
	
	// I/O
	int(*read)(RPC* rpc, void* buf, int size);
	int(*write)(RPC* rpc, void* buf, int size);
	void(*close)(RPC* rpc);
	
	uint8_t		rbuf[RPC_BUFFER_SIZE];
	int		rbuf_read;
	int		rbuf_index;
	
	uint8_t		wbuf[RPC_BUFFER_SIZE];
	int		wbuf_index;
	
	// Callbacks
	void(*hello_callback)(void* context);
	void* hello_context;
	void(*vm_create_callback)(uint32_t id, void* context);
	void* vm_create_context;
	void(*vm_create_handler)(VMSpec* vm, void* context, void(*callback)(uint32_t id));
	void* vm_create_handler_context;
	void(*vm_get_callback)(VMSpec* vm, void* context);
	void* vm_get_context;
	void(*vm_get_handler)(uint32_t id, void* context, void(*callback)(VMSpec* vm));
	void* vm_get_handler_context;
	void(*vm_set_callback)(bool result, void* context);
	void* vm_set_context;
	void(*vm_set_handler)(VMSpec* vm, void* context, void(*callback)(bool result));
	void* vm_set_handler_context;
	void(*vm_delete_callback)(bool result, void* context);
	void* vm_delete_context;
	void(*vm_delete_handler)(uint32_t id, void* context, void(*callback)(bool result));
	void* vm_delete_handler_context;
	void(*vm_list_callback)(uint32_t* ids, uint16_t count, void* context);
	void* vm_list_context;
	void(*vm_list_handler)(uint32_t* ids, int size, void* context, void(*callback)(int size));
	void* vm_list_handler_context;
	void(*status_get_callback)(VMStatus status, void* context);
	void* status_get_context;
	void(*status_get_handler)(uint32_t id, void* context, void(*callback)(VMStatus status));
	void* status_get_handler_context;
	void(*status_set_callback)(bool result, void* context);
	void* status_set_context;
	void(*status_set_handler)(uint32_t id, VMStatus status, void* context, void(*callback)(bool result));
	void* status_set_handler_context;
	int32_t(*storage_download_callback)(uint32_t offset, void* buf, int32_t size, void* context);
	void* storage_download_context;
	uint32_t storage_download_id;
	uint32_t storage_download_offset;
	void(*storage_download_handler)(uint32_t id, uint32_t offset, void** buf, int32_t size, void* context, void(*callback)(int32_t size));
	void* storage_download_handler_context;
	uint32_t storage_upload_id;
	uint32_t storage_upload_offset;
	int32_t(*storage_upload_callback)(uint32_t offset, void** buf, int32_t size, void* context);
	void* storage_upload_context;
	void(*storage_upload_handler)(uint32_t id, uint32_t offset, void* buf, int32_t size, void* context, void(*callback)(int32_t size));
	void* storage_upload_handler_context;
	void(*stdio_callback)(uint16_t written, void* context);
	void* stdio_context;
	void(*stdio_handler)(uint32_t id, uint8_t thread_id, int fd, char* str, uint16_t size, void* context, void(*callback)(uint16_t size));
	void* stdio_handler_context;
	
	// Private data
	uint8_t		data[0];
};

// Client side APIs
#ifdef LINUX
RPC* rpc_open(const char* host, int port, int timeout);
RPC* rpc_listen(int port);
RPC* rpc_accept(RPC* rpc);
bool rpc_is_closed(RPC* rpc);
#endif /* LINUX */

int rpc_hello(RPC* rpc, void(*callback)(void* context), void* context);

int rpc_vm_create(RPC* rpc, VMSpec* vm, void(*callback)(uint32_t id, void* context), void* context);
int rpc_vm_get(RPC* rpc, uint32_t id, void(*callback)(VMSpec* vm, void* context), void* context);
int rpc_vm_set(RPC* rpc, VMSpec* vm, void(*callback)(bool result, void* context), void* context);
int rpc_vm_delete(RPC* rpc, uint32_t id, void(*callback)(bool result, void* context), void* context);
int rpc_vm_list(RPC* rpc, void(*callback)(uint32_t* ids, uint16_t count, void* context), void* context);

int rpc_status_get(RPC* rpc, uint32_t id, void(*callback)(VMStatus status, void* context), void* context);
int rpc_status_set(RPC* rpc, uint32_t id, VMStatus status, void(*callback)(bool result, void* context), void* context);

int rpc_storage_download(RPC* rpc, uint32_t id, int32_t(*callback)(uint32_t offset, void* buf, int32_t size, void* context), void* context);
int rpc_storage_upload(RPC* rpc, uint32_t id, int32_t(*callback)(uint32_t offset, void** buf, int32_t size, void* context), void* context);

int rpc_stdio(RPC* rpc, uint32_t id, uint8_t thread_id, int fd, const char* str, uint16_t size, void(*callback)(uint16_t written, void* context), void* context);

// Server side APIs
void rpc_vm_create_handler(RPC* rpc, void(*handler)(VMSpec* vm, void* context, void(*callback)(uint32_t id)), void* context);
void rpc_vm_get_handler(RPC* rpc, void(*handler)(uint32_t id, void* context, void(*callback)(VMSpec* vm)), void* context);
void rpc_vm_set_handler(RPC* rpc, void(*handler)(VMSpec* vm, void* context, void(*callback)(bool result)), void* context);
void rpc_vm_delete_handler(RPC* rpc, void(*handler)(uint32_t id, void* context, void(*callback)(bool result)), void* context);
void rpc_vm_list_handler(RPC* rpc, void(*handler)(uint32_t* ids, int size, void* context, void(*callback)(int size)), void* context);

void rpc_status_get_handler(RPC* rpc, void(*handler)(uint32_t id, void* context, void(*callback)(VMStatus status)), void* context);
void rpc_status_set_handler(RPC* rpc, void(*handler)(uint32_t id, VMStatus status, void* context, void(*callback)(bool result)), void* context);

void rpc_storage_download_handler(RPC* rpc, void(*handler)(uint32_t id, uint32_t offset, void** buf, int32_t size, void* context, void(*callback)(int32_t size)), void* context);
void rpc_storage_upload_handler(RPC* rpc, void(*handler)(uint32_t id, uint32_t offset, void* buf, int32_t size, void* context, void(*callback)(int32_t size)), void* context);

void rpc_stdio_handler(RPC* rpc, void(*handler)(uint32_t id, uint8_t thread_id, int fd, char* str, uint16_t size, void* context, void(*callback)(uint16_t size)), void* context);

bool rpc_is_active(RPC* rpc);
bool rpc_loop(RPC* rpc);

// Utility
void rpc_vm_dump(VMSpec* vm);
 
#endif /* __CONTROL_RPC__ */
