/* trace.h
 *  Copyright (C) 2001-2007, The Perl Foundation.
 *  SVN Info
 *     $Id$
 *  Overview:
 *     Tracing support for runops_cores.c.
 *  Data Structure and Algorithms:
 *  History:
 *  Notes:
 *  References:
 */

#ifndef PARROT_TRACE_H_GUARD
#define PARROT_TRACE_H_GUARD

#include "parrot/parrot.h"

/* HEADERIZER BEGIN: src/trace.c */
/* Don't modify between HEADERIZER BEGIN / HEADERIZER END.  Your changes will be lost. */

int trace_key_dump(PARROT_INTERP, ARGIN(const PMC *key))
        __attribute__nonnull__(1)
        __attribute__nonnull__(2);

void trace_op(PARROT_INTERP,
    ARGIN(const opcode_t *code_start),
    ARGIN(const opcode_t *code_end),
    ARGIN_NULLOK(const opcode_t *pc))
        __attribute__nonnull__(1)
        __attribute__nonnull__(2)
        __attribute__nonnull__(3);

void trace_op_dump(PARROT_INTERP,
    ARGIN(const opcode_t *code_start),
    ARGIN(const opcode_t *pc))
        __attribute__nonnull__(1)
        __attribute__nonnull__(2)
        __attribute__nonnull__(3);

void trace_pmc_dump(PARROT_INTERP, ARGIN_NULLOK(PMC *pmc))
        __attribute__nonnull__(1);

/* Don't modify between HEADERIZER BEGIN / HEADERIZER END.  Your changes will be lost. */
/* HEADERIZER END: src/trace.c */

#endif /* PARROT_TRACE_H_GUARD */


/*
 * Local variables:
 *   c-file-style: "parrot"
 * End:
 * vim: expandtab shiftwidth=4:
 */
