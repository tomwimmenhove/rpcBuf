/* 
 * This file is part of the rpcbuf distribution (https://github.com/tomwimmenhove/rpcbuf).
 * Copyright (c) 2021 Tom Wimmenhove.
 * 
 * This program is free software: you can redistribute it and/or modify  
 * it under the terms of the GNU General Public License as published by  
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but 
 * WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU 
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License 
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef CALL_OUT_DEFINITIONS_H
#define CALL_OUT_DEFINITIONS_H

#include "call_definitions.h"

#endif /* CALL_OUT_DEFINITIONS_H */

#ifdef DEFINE_CALL
#undef DEFINE_CALL
#endif /* DEFINE_CALL */
#define DEFINE_CALL DEFINE_CALL_OUT

#ifdef DEFINE_CALL_START
#undef DEFINE_CALL_START
#endif /* DEFINE_CALL_START */
#define DEFINE_CALL_START DEFINE_CALL_OUT_START

#ifdef DEFINE_CALL_END
#undef DEFINE_CALL_END
#endif /* DEFINE_CALL_END */
#define DEFINE_CALL_END DEFINE_CALL_OUT_END
