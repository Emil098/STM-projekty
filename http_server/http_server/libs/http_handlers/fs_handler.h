/*
 * fs_handle.h
 *
 *  Created on: Feb 17, 2026
 *      Author: emilr
 */

#ifndef FS_HANDLE_H_
#define FS_HANDLE_H_

#include "lwip.h"
#include "fs.h"
#include "fsdata.h"


void fs_handlers_init(void);
void *fs_state_init(struct fs_file *file, const char *name);

#endif /* FS_HANDLE_H_ */
