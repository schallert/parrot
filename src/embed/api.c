#include "parrot/parrot.h"
#include "parrot/api.h"
#include "embed_private.h"

/* HEADERIZER HFILE: include/parrot/api.h */

PARROT_API
PARROT_CANNOT_RETURN_NULL
PARROT_MALLOC
INTVAL
Parrot_api_make_interpreter(ARGIN_NULLOK(PMC *parent), INTVAL flags, ARGIN_NULLOK(Parrot_Init_Args *args), ARGOUT(PMC **interp))
{
    ASSERT_ARGS(Parrot_api_make_interpreter)
    int alt_stacktop;
    void *stacktop_ptr = &alt_stacktop;
    Parrot_set_config_hash();
    {
        Parrot_Interp * const parent_raw = PMC_IS_NULL(parent) ? NULL : GET_RAW_INTERP(parent);
        Parrot_Interp * const interp_raw = allocate_interpreter(parent_raw, flags);
        if (args) {
            if (args->stack_top)
                stacktop_ptr = args->stacktop;
            if (args->gc_system) {
                const INTVAL sysid = Parrot_gc_get_system_id(interp, args->gc_system);
                if (sysid == -1)
                    EMBED_API_FAILURE(interp_pmc, interp);
                interp->gc_sys->sys_type = sysid;
            }
            if (args->gc_threshold)
                interp->gc_threshold = args->gc_threshold;
            if (args->hash_seed)
                interp->hash_seed = args->hash_seed;
        }
    }
    initialize_interpreter(interp_raw, stacktop);
    *interp_pmc = VTABLE_get_pmc_keyed_int(interp, iglobals, (INTVAL)IGLOBALS_INTERPRETER);
    return !PMC_IS_NULL(*interp);
}

PARROT_API
INTVAL
Parrot_api_set_runcore(ARGIN(PMC *interp_pmc), Parrot_Run_core_t core, Parrot_Uint trace)
{
    ASSERT_ARGS(Parrot_api_set_runcore)
    if (trace)
        core = PARROT_SLOW_CORE;
    EMBED_API_CALLIN(interp_pmc, interp)
    Parrot_set_trace(interp, (Parrot_trace_flags)trace);
    Parrot_set_run_core(interp, core);
    EMBED_API_CALLOUT(interp_pmc, interp)
}

PARROT_API
INTVAL
Parrot_api_debug_flag(ARGMOD(PMC *interp_pmc), INTVAL flags, INTVAL set)
{
    ASSERT_ARGS(Parrot_api_debug_flag)
    EMBED_API_CALLIN(interp_pmc, interp);
    if (set)
        interp->debug_flags |= flags;
    else
        interp->debug_flags &= ~flags;
    EMBED_API_CALLOUT(interp_pmc, interp);
}

PARROT_API
INTVAL
Parrot_api_flag(ARGMOD(PMC *interp_pmc), INTVAL flags, INTVAL set)
{
    ASSERT_ARGS(Parrot_api_flag)
    EMBED_API_CALLIN(interp_pmc, interp);
    if (set) {
        Interp_flags_SET(interp, flag);
        if (flag & (PARROT_BOUNDS_FLAG | PARROT_PROFILE_FLAG))
            Parrot_runcore_switch(interp, Parrot_str_new_constant(interp, "slow"));
    }
    else
        Interp_flags_CLEAR(interp, flag);
    EMBED_API_CALLOUT(interp_pmc, interp);
}

PARROT_API
INTVAL
Parrot_api_set_executable_name(ARGMOD(PMC *interp_pmc), ARGIN(const char * name))
{
    ASSERT_ARGS(Parrot_api_set_executable_name)
    EMBED_API_CALLIN(interp_pmc, interp);
    STRING * const name_str = Parrot_str_new(interp, name, 0);
    PMC * const name_pmc = Parrot_pmc_new(interp, enum_class_String);
    VTABLE_set_string_native(interp, name_pmc, name_str);
    VTABLE_set_pmc_keyed_int(interp, interp->iglobals, IGLOBALS_EXECUTABLE,
        name_pmc);
    EMBED_API_CALLOUT(interp_pmc, interp);
}

PARROT_API
INTVAL
Parrot_api_destroy_interpreter(ARGIN(PMC *interp_pmc))
{
    ASSERT_ARGS(Parrot_api_destroy_interpreter)
    EMBED_API_CALLIN(interp_pmc, interp);
    Parrot_destroy(interp);
    Parrot_exit(interp, 0);
    EMBED_API_CALLOUT(interp_pmc, interp);
}

/*

=item C<PARROT_API INTVAL Parrot_api_load_bytecode_file(PMC *interp_pmc, const
char *filename, PMC **pbc)>

Load a bytecode file and return a bytecode PMC.

=cut

*/

PARROT_API
INTVAL
Parrot_api_load_bytecode_file(ARGMOD(PMC *interp_pmc), ARGIN(const char *filename), ARGOUT(PMC **pbc))
{
    ASSERT_ARGS(Parrot_api_load_bytecode_file)
    EMBED_API_CALLIN(interp_pmc, interp);
    PackFile * const pf = Parrot_pbc_read(interp, filename, 0);
    if (!pf)
        EMBED_API_FAILURE(interp_pmc, interp);
    *pbc = Parrot_pmc_new(interp, enum_class_PackFile);
    VTABLE_set_pointer(interp, *pbc, pf);
    EMBED_API_CALLOUT(interp_pmc, interp);
}

PARROT_API
INTVAL
Parrot_api_run_bytecode(ARGMOD(PMC *interp_pmc), ARGIN(PMC *pbc), ARGIN(PMC *mainargs))
{
    ASSERT_ARGS(Parrot_api_run_bytecode)
    EMBED_API_CALLIN(interp_pmc, interp);
    PackFile * const pf = VTABLE_get_pointer(interp, pbc);
    if (!pf)
        EMBED_API_FAILURE(interp_pmc, interp);
    Parrot_pbc_load(interp, pf);
    PackFile_fixup_subs(interp, PBC_IMMEDIATE, NULL);
    PackFile_fixup_subs(interp, PBC_POSTCOMP, NULL);
    PackFile_fixup_subs(interp, PBC_MAIN, NULL);
    EMBED_API_CALLOUT(interp_pmc, interp);
}

PARROT_API
INTVAL
Parrot_api_build_argv_array(ARGMOD(PMC *interp_pmc), INTVAL argc, ARGIN(char **argv), ARGOUT(PMC **args))
{
    ASSERT_ARGS(Parrot_api_build_argv_array)
    EMBED_API_CALLIN(interp_pmc, interp);
    PMC * const userargv = Parrot_pmc_new(interp, enum_class_ResizableStringArray);
    INTVAL i;

    for (i = 0; i < argc; ++i) {
        /* Run through argv, adding everything to the array */
        STRING * const arg = Parrot_str_new_init(interp, argv[i], strlen(argv[i]),
                Parrot_utf8_encoding_ptr, PObj_external_FLAG);
        VTABLE_push_string(interp, userargv, arg);
    }
    *args = userargv;
    EMBED_API_CALLIN(interp_pmc, interp);
}

PARROT_API
INTVAL
Parrot_api_set_warnings(ARGMOD(PMC *interp_pmc), INTVAL flags)
{
    ASSERT_ARGS(Parrot_api_set_warnings)
    EMBED_API_CALLIN(interp_pmc, interp);
    /* Activates the given warnings.  (Macro from warnings.h.) */
    PARROT_WARNINGS_on(interp, (Parrot_warnclass)flags);
    EMBED_API_CALLOUT(interp_pmc, interp);
}

PARROT_API
INTVAL
Parrot_api_set_output_file(ARGMOD(PMC *interp_pmc), ARGIN(const char * filename))
{
    ASSERT_ARGS(Parrot_api_set_output_file)
    EMBED_API_CALLIN(interp_pmc, interp);
    if (!filename && !interp->output_file)
        interp->output_file = "-";
    else
        interp->output_file = filename;
    EMBED_API_CALLOUT(interp_pmc, interp);
}

PARROT_API
INTVAL
Parrt_api_add_library_search_path(ARGMOD(PMC *interp_pmc), ARGIN(const char *path))
{
    ASSERT_ARGS(Parrot_api_add_library_search_path)
    EMBED_API_CALLIN(interp_pmc, interp);
    Parrot_lib_add_path_from_cstring(interp, path, PARROT_LIB_PATH_LIBRARY);
    EMBED_API_CALLOUT(interp_pmc, interp);
}

PARROT_API
INTVAL
Parrt_api_add_include_search_path(ARGMOD(PMC *interp_pmc), ARGIN(const char *path))
{
    ASSERT_ARGS(Parrot_api_add_include_search_path)
    EMBED_API_CALLIN(interp_pmc, interp);
    Parrot_lib_add_path_from_cstring(interp, path, PARROT_LIB_PATH_INCLUDE);
    EMBED_API_CALLOUT(interp_pmc, interp);
}

PARROT_API
INTVAL
Parrt_api_add_dynext_search_path(ARGMOD(PMC *interp_pmc), ARGIN(const char *path))
{
    ASSERT_ARGS(Parrot_api_add_dynext_search_path)
    EMBED_API_CALLIN(interp_pmc, interp);
    Parrot_lib_add_path_from_cstring(interp, path, PARROT_LIB_PATH_DYNEXT);
    EMBED_API_CALLOUT(interp_pmc, interp);
}

/*

=item C<PARROT_API INTVAL Parrot_api_set_stdhandles(PMC *interp_pmc, INTVAL
stdin, INTVAL stdout, INTVAL stderr)>

Set the std file descriptors for the embedded interpreter. Any file descriptor
passed as argument and set to C<PIO_INVALID_HANDLE> is ignored.

=cut

*/

PARROT_API
INTVAL
Parrot_api_set_stdhandles(ARGIN(PMC *interp_pmc), INTVAL stdin, INTVAL stdout, INTVAL stderr)
{
  ASSERT_ARGS(Parrot_api_set_stdhandles)
  EMBED_API_CALLIN(interp_pmc, interp);

  if(PIO_INVALID_HANDLE != (PIOHANDLE)stdin) {
    PMC * const pmc = Parrot_pmc_new(interp, enum_class_FileHandle);
    Parrot_io_set_os_handle(interp, pmc, (PIOHANDLE)stdin);
    Parrot_io_sethandle(interp,PIO_STDIN_FILENO,pmc);
  }

  if(PIO_INVALID_HANDLE != (PIOHANDLE)stdout) {
    PMC * const pmc = Parrot_pmc_new(interp, enum_class_FileHandle);
    Parrot_io_set_os_handle(interp, pmc, (PIOHANDLE)stdout);
    Parrot_io_sethandle(interp,PIO_STDOUT_FILENO,pmc);
  }

  if(PIO_INVALID_HANDLE != (PIOHANDLE)stderr) {
    PMC * const pmc = Parrot_pmc_new(interp, enum_class_FileHandle);
    Parrot_io_set_os_handle(interp, pmc, (PIOHANDLE)stderr);
    Parrot_io_sethandle(interp,PIO_STDERR_FILENO,pmc);
  }

  EMBED_API_CALLOUT(interp_pmc, interp);
}