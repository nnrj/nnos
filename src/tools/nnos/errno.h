#if (!defined(ERRNO_H))

#define ERRNO_H	1

#if (defined(__cplusplus))
	extern "C" {
#endif

extern int errno;

#define	ENOENT		2	/* 目录不存在 */
#define	ERANGE		34	/* 溢出 */

#if (defined(__cplusplus))
	}
#endif

#endif
