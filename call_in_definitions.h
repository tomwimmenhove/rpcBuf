#ifndef CALL_IN_DEFINITIONS_H
#define CALL_IN_DEFINITIONS_H

#include "call_definitions.h"

#endif /* CALL_IN_DEFINITIONS_H */

#ifdef DEFINE_CALL
#undef DEFINE_CALL
#endif /* DEFINE_CALL */
#define DEFINE_CALL DEFINE_CALL_IN

#ifdef DEFINE_CALL_START
#undef DEFINE_CALL_START
#endif /* DEFINE_CALL_START */
#define DEFINE_CALL_START DEFINE_CALL_IN_START

#ifdef DEFINE_CALL_END
#undef DEFINE_CALL_END
#endif /* DEFINE_CALL_END */
#define DEFINE_CALL_END DEFINE_CALL_IN_END
