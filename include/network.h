/*
 * Copyright (C) 2023 Coder.AN
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */
#ifndef __NETWORK_H__
#define __NETWORK_H__


#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h> 
#include <unistd.h> 
#include <netdb.h>  
#include <net/if.h>  
#include <arpa/inet.h> 
#include <sys/ioctl.h>  
#include <sys/types.h>  
#include <sys/time.h> 

#define ip_to_str(str, ip) \
    sprintf(str, "%u.%u.%u.%u", ip & 0x000000FF, (ip & 0x0000FF00) >> 8, (ip & 0x00FF0000) >> 16, (ip & 0xFF000000) >> 24)

uint32_t get_local_ip(const char *eth_inf);

#endif