# How to create a socket under Windows/Linux?
For this section, you need to know followings:
## 1. Preparation
### (1) under Windows:
Under Windows, you need to:
* include header file:
```C 
// Under new version of Windows, "winsock2.h" will also be included while complile "winsock.h"
#include <winsock.h>
```
* Explicitly notify the compiler to link the socket library
```C
#pragma comment (lib,"wsock32.lib")
```
* Initialize the DLL \
Note that under Windows, socket is implemented in the form of DLL. A counter is maintained inside the DLL. Only the first call to WSAStartup() can actually load the DLL into memory, so you need to call WSAStartup() at the beginning so you can use other socket API. And the subsequent call of WSAStartup() just simply increase the counter. Note that WSACleanup() is just the opposite. Each call of WSACleanup() decreases the counter by 1, and when the counter decreases to 0, the DLL will be unloaded from memory! Therefore, you should call WSACleanup() as many times as you call WSAStartup().
```C
//at the beginning of program
WSAData wsa;
WSAStartup(0x101, &wsa);

//at the end of program
WSACleanup();
```
### (2) under Linux:
Under Linux, you need to:
* include header file:
```C
#include <netinet/in.h> //contains most of socket APIs
#include <unistd.h> //contains lots of function prototypes of system services: read(), write(), getpid(), close(), etc
```
## 2. Related functions:

```C
int socket(int domain, int type, int protocol);
```
* parameters:
    * **domain:**        setup network communication domain, commonly use *AF_INET* (for TCP/IP-ipv4), others contain *AF_UNIX* (for local communication), *AF_INET6* (for TCP/IP–IPv6), *AF_OSI*
    * **type:**          socket type, such as SOCK_STREAM (for TCP), SOCK_DGRAM (for UDP), SOCK_RAW, etc. 
    * **protocol:**      assign the protocol that the socket used, such as IPPROTO_TCP, IPPROTO_UDP, IPPROTO_STCP, etc. If don't want to assign, use 0
* return value:
    * If the socket create successfully, it will return a index of the created socket which is an int. \
    * If it failed, it will return -1.\
    Under Windows, you can use WSAGetLastError() to get the error code. \
    Under Linux, you can get the error code via errno \

* Appendix:
1. Windows Error Code: \
        Socket error 0 – Directly send error \
        Socket error 10004 – Interrupted function \
        Socket error 10013 – Permission denied \
        Socket error 10014 – Bad address \
        Socket error 10022 – Invalid argument \
        Socket error 10024 – Too many open files \
        Socket error 10035 – Resource temporarily unavailable \
        Socket error 10036 – Operation now in progress \
        Socket error 10037 – Operation already in progress \
        Socket error 10038 – Socket operation on non-socket \
        Socket error 10039 – Destination address required \
        Socket error 10040 – Message too long \
        Socket error 10041 – Protocol wrong type for socket \
        Socket error 10042 – Bad protocol option \
        Socket error 10043 – Protocol not supported \
        Socket error 10044 – Socket type not supported \
        Socket error 10045 – Operation not supported \
        Socket error 10046 – Protocol family not supported \
        Socket error 10047 – Address family not supported by protocol family \
        Socket error 10048 – Address already in use \
        Socket error 10049 – Cannot assign requested address \
        Socket error 10050 – Network is down \
        Socket error 10051 – Network is unreachable \
        Socket error 10052 – Network dropped connection on reset \
        Socket error 10053 – Software caused connection abort \
        Socket error 10054 – connection reset by peer \
        Socket error 10055 – No buffer space available \
        Socket error 10056 – Socket is already connected \
        Socket error 10057 – Socket is not connected \
        Socket error 10058 – Cannot send after socket shutdown \
        Socket error 10060 – Connection timed out \
        Socket error 10061 – Connection refused \
        Socket error 10064 – Host is down \
        Socket error 10065 – No route to host \
        Socket error 10067 – Too many processes \
        Socket error 10091 – Network subsystem is unavailable \
        Socket error 10092 – WINSOCK.DLL version out of range \
        Socket error 10093 – Successful WSAStartup not yet performed \
        Socket error 10094 – Graceful shutdown in progress \
        Socket error 11001 – Host not found \
        Socket error 11002 – Non-authoritative host not found \
        Socket error 11003 – This is a non-recoverable error \
        Socket error 11004 – Valid name, no data record of requested type
    
2. Linux Error Number:
   ```c++
        #define EPERM 1             /* Operation not permitted */ 
        #define ENOENT 2            /* No such file or directory */ 
        #define ESRCH 3             /* No such process */
        #define EINTR 4             /* Interrupted system call */
        #define EIO 5               /* I/O error */
        #define ENXIO 6             /* No such device or address */
        #define E2BIG 7             /* Argument list too long */
        #define ENOEXEC 8           /* Exec format error */
        #define EBADF 9             /* Bad file number */
        #define ECHILD 10           /* No child processes */
        #define EAGAIN 11           /* Try again */
        #define ENOMEM 12           /* Out of memory */
        #define EACCES 13           /* Permission denied */
        #define EFAULT 14           /* Bad address */
        #define ENOTBLK 15          /* Block device required */
        #define EBUSY 16            /* Device or resource busy */
        #define EEXIST 17           /* File exists */
        #define EXDEV 18            /* Cross-device link */
        #define ENODEV 19           /* No such device */
        #define ENOTDIR 20          /* Not a directory */
        #define EISDIR 21           /* Is a directory */
        #define EINVAL 22           /* Invalid argument */
        #define ENFILE 23           /* File table overflow */
        #define ENOTTY 25           /* Not a typewriter */
        #define ETXTBSY 26          /* Text file busy */
        #define EFBIG 27            /* File too large */
        #define ENOSPC 28           /* No space left on device */
        #define ESPIPE 29           /* Illegal seek */
        #define EROFS 30            /* Read-only file system */
        #define EMLINK 31           /* Too many links */
        #define EPIPE 32            /* Broken pipe */
        #define EDOM 33             /* Math argument out of domain of func */
        #define ERANGE 34           /* Math result not representable */
        #define EDEADLK 35          /* Resource deadlock would occur */
        #define ENAMETOOLONG 36     /* File name too long */
        #define ENOLCK 37           /* No record locks available */
        #define ENOSYS 38           /* Function not implemented */
        #define ENOTEMPTY 39        /* Directory not empty */
        #define ELOOP 40            /* Too many symbolic links encountered */
        #define EWOULDBLOCK EAGAIN  /* Operation would block */
        #define ENOMSG 42           /* No message of desired type */
        #define EIDRM 43            /* Identifier removed */
        #define ECHRNG 44           /* Channel number out of range */
        #define EL2NSYNC 45         /* Level 2 not synchronized */
        #define EL3HLT 46           /* Level 3 halted */
        #define EL3RST 47           /* Level 3 reset */
        #define ELNRNG 48           /* Link number out of range */
        #define EUNATCH 49          /* Protocol driver not attached */
        #define ENOCSI 50           /* No CSI structure available */
        #define EL2HLT 51           /* Level 2 halted */
        #define EBADE 52            /* Invalid exchange */
        #define EBADR 53            /* Invalid request descriptor */
        #define EXFULL 54           /* Exchange full */
        #define ENOANO 55           /* No anode */
        #define EBADRQC 56          /* Invalid request code */
        #define EBADSLT 57          /* Invalid slot */
        #define EDEADLOCK EDEADLK
        #define EBFONT 59           /* Bad font file format */
        #define ENOSTR 60           /* Device not a stream */
        #define ENODATA 61          /* No data available */
        #define ETIME 62            /* Timer expired */
        #define ENOSR 63            /* Out of streams resources */
        #define ENONET 64           /* Machine is not on the network */
        #define ENOPKG 65           /* Package not installed */
        #define EREMOTE 66          /* Object is remote */
        #define ENOLINK 67          /* Link has been severed */
        #define EADV 68             /* Advertise error */
        #define ESRMNT 69           /* Srmount error */
        #define ECOMM 70            /* Communication error on send */
        #define EPROTO 71           /* Protocol error */
        #define EMULTIHOP 72        /* Multihop attempted */
        #define EDOTDOT 73          /* RFS specific error */
        #define EBADMSG 74          /* Not a data message */
        #define EOVERFLOW 75        /* Value too large for defined data type */
        #define ENOTUNIQ 76         /* Name not unique on network */
        #define EBADFD 77           /* File descriptor in bad state */
        #define EREMCHG 78          /* Remote address changed */
        #define ELIBACC 79          /* Can not access a needed shared library */
        #define ELIBBAD 80          /* Accessing a corrupted shared library */
        #define ELIBSCN 81          /* .lib section in a.out corrupted */
        #define ELIBMAX 82          /* Attempting to link in too many shared libraries */
        #define ELIBEXEC 83         /* Cannot exec a shared library directly */
        #define EILSEQ 84           /* Illegal byte sequence */
        #define ERESTART 85         /* Interrupted system call should be restarted */
        #define ESTRPIPE 86         /* Streams pipe error */
        #define EUSERS 87           /* Too many users */
        #define ENOTSOCK 88         /* Socket operation on non-socket */
        #define EDESTADDRREQ 89     /* Destination address required */
        #define EMSGSIZE 90         /* Message too long */
        #define EPROTOTYPE 91       /* Protocol wrong type for socket */
        #define ENOPROTOOPT 92      /* Protocol not available */
        #define EPROTONOSUPPORT 93  /* Protocol not supported */
        #define ESOCKTNOSUPPORT 94  /* Socket type not supported */
        #define EOPNOTSUPP 95       /* Operation not supported on transport endpoint */
        #define EPFNOSUPPORT 96     /* Protocol family not supported */
        #define EAFNOSUPPORT 97     /* Address family not supported by protocol */
        #define EADDRINUSE 98       /* Address already in use */
        #define EADDRNOTAVAIL 99    /* Cannot assign requested address */
        #define ENETDOWN 100        /* Network is down */
        #define ENETUNREACH 101     /* Network is unreachable */
        #define ENETRESET 102       /* Network dropped connection because of reset */
        #define ECONNABORTED 103    /* Software caused connection abort */
        #define ECONNRESET 104      /* Connection reset by peer */
        #define ENOBUFS 105         /* No buffer space available */
        #define EISCONN 106         /* Transport endpoint is already connected */
        #define ENOTCONN 107        /* Transport endpoint is not connected */
        #define ESHUTDOWN 108       /* Cannot send after transport endpoint shutdown */
        #define ETOOMANYREFS 109    /* Too many references: cannot splice */
        #define ETIMEDOUT 110       /* Connection timed out */
        #define ECONNREFUSED 111    /* Connection refused */
        #define EHOSTDOWN 112       /* Host is down */
        #define EHOSTUNREACH 113    /* No route to host */
        #define EALREADY 114        /* Operation already in progress */
        #define EINPROGRESS 115     /* Operation now in progress */
        #define ESTALE 116          /* Stale NFS file handle */
        #define EUCLEAN 117         /* Structure needs cleaning */
        #define ENOTNAM 118         /* Not a XENIX named type file */
        #define ENAVAIL 119         /* No XENIX semaphores available */
        #define EISNAM 120          /* Is a named type file */
        #define ENOKEY 126          /* Required key not available */
        #define EKEYEXPIRED 127     /* Key has expired */
        #define EKEYREVOKED 128     /* Key has been revoked */
        #define EKEYREJECTED 129    /* Key was rejected by service */
        #define EOWNERDEAD 130      /* Owner died */
        #define ENOTRECOVERABLE 131 /* State not recoverable */
        #define ERFKILL 132         /* Operation not possible due to RF-kill */
        #define EHWPOISON 133       /* Memory page has hardware error */
    ```