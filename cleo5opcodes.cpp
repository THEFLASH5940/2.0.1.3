#include <mod/amlmod.h>
#include <mod/logger.h>
#include <cleohelpers.h>

// There wont be that much opcodes, because some of them are in their own plugins

extern void (*UpdateCompareFlag)(void*, uint8_t);
extern GTAScript **pActiveScripts;
void CleoReturnGeneric(void* handle, bool returnArgs, int returnArgCount);


// Debug plugin
CLEO_Fn(DEBUG_ON)
{
    // Need additional work
}
CLEO_Fn(DEBUG_OFF)
{
    // Need additional work
}
CLEO_Fn(BREAKPOINT)
{
    // Need additional work
}
CLEO_Fn(TRACE)
{
    // Need additional work
}
CLEO_Fn(LOG_TO_FILE)
{
    // Need additional work
}

// MemOps plugin
  // A scripter should use 0DD7 on mobile (get image base) if he works with GTASA address.
  // Not adding "add_ib" argument here for compatibility
CLEO_Fn(COPY_MEMORY)
{
    uint32_t source = cleo->ReadParam(handle)->u;
    uint32_t target = cleo->ReadParam(handle)->u;
    int size = cleo->ReadParam(handle)->i;
    aml->Write(source, target, size);
}
CLEO_Fn(READ_MEMORY_WITH_OFFSET)
{
    uint32_t source = cleo->ReadParam(handle)->u;
    uint32_t offset = cleo->ReadParam(handle)->u;
    int size = cleo->ReadParam(handle)->i;

    // Need additional work
}
CLEO_Fn(WRITE_MEMORY_WITH_OFFSET)
{
    uint32_t source = cleo->ReadParam(handle)->u;
    uint32_t offset = cleo->ReadParam(handle)->u;
    int size = cleo->ReadParam(handle)->i;

    // Need additional work
}
CLEO_Fn(FORGET_MEMORY)
{
    FreeMem((void*)cleo->ReadParam(handle)->u);
}
CLEO_Fn(GET_SCRIPT_STRUCT_JUST_CREATED)
{
    GTAScript* head = (GTAScript*)handle;
    while(head->prev != NULL)
    {
        head = head->prev;
    }
    cleo->GetPointerToScriptVar(handle)->i = (int)head;
}
CLEO_Fn(IS_SCRIPT_RUNNING)
{
    GTAScript* scriptStruct = (GTAScript*)cleo->ReadParam(handle)->i;
    for (GTAScript* script = *pActiveScripts; script != NULL; script = script->next)
    {
        if(scriptStruct == script)
        {
            UpdateCompareFlag(handle, true);
            return;
        }
    }
    UpdateCompareFlag(handle, false);
}
CLEO_Fn(GET_SCRIPT_STRUCT_FROM_FILENAME)
{
    // Need additional work
}
CLEO_Fn(IS_MEMORY_EQUAL)
{
    void* source = (void*)cleo->ReadParam(handle)->u;
    void* target = (void*)cleo->ReadParam(handle)->u;
    int size = cleo->ReadParam(handle)->i;
    UpdateCompareFlag(handle, (memcmp(source, target, size)) == 0);
}

// Text plugin
CLEO_Fn(IS_TEXT_EMPTY)
{
    char buf[MAX_STR_LEN];
    CLEO_ReadStringEx(handle, buf, sizeof(buf));
    UpdateCompareFlag(handle, buf[0] == 0);
}
CLEO_Fn(IS_TEXT_EQUAL)
{
    char bufa[MAX_STR_LEN], bufb[MAX_STR_LEN];
    CLEO_ReadStringEx(handle, bufa, sizeof(bufa));
    CLEO_ReadStringEx(handle, bufb, sizeof(bufb));
    bool ignoreCase = !!cleo->ReadParam(handle)->i;

    UpdateCompareFlag(handle, (ignoreCase ? strcasecmp(bufa, bufb) : strcmp(bufa, bufb)) == 0);
}
CLEO_Fn(IS_TEXT_IN_TEXT)
{
    char bufa[MAX_STR_LEN], bufb[MAX_STR_LEN];
    CLEO_ReadStringEx(handle, bufa, sizeof(bufa));
    CLEO_ReadStringEx(handle, bufb, sizeof(bufb));
    bool ignoreCase = !!cleo->ReadParam(handle)->i;

    UpdateCompareFlag(handle, (ignoreCase ? strcasestr(bufa, bufb) : strstr(bufa, bufb)) != NULL);
}
CLEO_Fn(IS_TEXT_PREFIX)
{
    char bufa[MAX_STR_LEN], bufb[MAX_STR_LEN];
    CLEO_ReadStringEx(handle, bufa, sizeof(bufa));
    CLEO_ReadStringEx(handle, bufb, sizeof(bufb));
    bool ignoreCase = !!cleo->ReadParam(handle)->i;
    int prefixlen = strlen(bufb);

    UpdateCompareFlag(handle, (ignoreCase ? strncasecmp(bufa, bufb, prefixlen) : strncmp(bufa, bufb, prefixlen)) == 0);
}
CLEO_Fn(IS_TEXT_SUFFIX)
{
    char bufa[MAX_STR_LEN], bufb[MAX_STR_LEN];
    CLEO_ReadStringEx(handle, bufa, sizeof(bufa));
    CLEO_ReadStringEx(handle, bufb, sizeof(bufb));
    bool ignoreCase = !!cleo->ReadParam(handle)->i;
    int suffixOff = strlen(bufa) - strlen(bufb);

    UpdateCompareFlag(handle, (ignoreCase ? strcasecmp(bufa + suffixOff, bufb) : strcmp(bufa + suffixOff, bufb)) == 0);
}
CLEO_Fn(DISPLAY_TEXT_FORMATTED)
{
    // Need additional work
}

// CLEO 5
CLEO_Fn(CLEO_RETURN_WITH)
{
    int argCount = GetVarArgCount(handle);
    int result = cleo->ReadParam(handle)->i;
    UpdateCompareFlag(handle, result != 0);
    CleoReturnGeneric(handle, true, argCount - 1);
}
CLEO_Fn(CLEO_RETURN_FAIL)
{
    int argCount = GetVarArgCount(handle);
    UpdateCompareFlag(handle, false);
    CleoReturnGeneric(handle, false, 0);
}

void Init5Opcodes()
{
    // Debug plugin
    CLEO_RegisterOpcode(0x00C3, DEBUG_ON); // 
    CLEO_RegisterOpcode(0x00C4, DEBUG_OFF); // 
    CLEO_RegisterOpcode(0x2100, BREAKPOINT); // 
    CLEO_RegisterOpcode(0x2101, TRACE); // 
    CLEO_RegisterOpcode(0x2102, LOG_TO_FILE); // 

    // MemOps plugin (early CLEO **always** had it in themselves. What's the purpose of removing it?!)
    // Literally brainless move... #1
    CLEO_RegisterOpcode(0x2400, COPY_MEMORY); // 2400=3, copy_memory %1d% to %2d% size %3d%
    CLEO_RegisterOpcode(0x2401, READ_MEMORY_WITH_OFFSET); // 2401=4, read_memory_with_offset %1d% offset %2d% size %3d% store_to %4d%
    CLEO_RegisterOpcode(0x2402, WRITE_MEMORY_WITH_OFFSET); // 2402=4, write_memory_with_offset %1d% offset %2d% size %3d% value %4d%
    CLEO_RegisterOpcode(0x2403, FORGET_MEMORY); // 2403=1, forget_memory %1d%
    CLEO_RegisterOpcode(0x2404, GET_SCRIPT_STRUCT_JUST_CREATED); // 2404=1, get_script_struct_just_created %1d%
    CLEO_RegisterOpcode(0x2405, IS_SCRIPT_RUNNING); // 2405=1, is_script_running %1d%
    CLEO_RegisterOpcode(0x2406, GET_SCRIPT_STRUCT_FROM_FILENAME); // 2406=1, get_script_struct_from_filename %1s%
    CLEO_RegisterOpcode(0x2407, IS_MEMORY_EQUAL); // 2407=3, is_memory_equal address_a %1d% address_b %2d% size %d3%

    // Text plugin
    // Literally brainless move... #2
    CLEO_RegisterOpcode(0x2600, IS_TEXT_EMPTY); // 2600=1, is_text_empty %1s%
    CLEO_RegisterOpcode(0x2601, IS_TEXT_EQUAL); // 2601=3, is_text_equal %1s% another %2s% ignore_case %3d%
    CLEO_RegisterOpcode(0x2602, IS_TEXT_IN_TEXT); // 2602=3, is_text_in_text %1s% sub_text %2s% ignore_case %3d%
    CLEO_RegisterOpcode(0x2603, IS_TEXT_PREFIX); // 2603=3, is_text_prefix %1s% prefix %2s% ignore_case %3d%
    CLEO_RegisterOpcode(0x2604, IS_TEXT_SUFFIX); // 2604=3, is_text_suffix %1s% suffix %2s% ignore_case %3d% // originally it's sufix *facepalm*
    CLEO_RegisterOpcode(0x2605, DISPLAY_TEXT_FORMATTED); // 2605=-1, display_text_formatted offset_left %1d% offset_top %2d% format %3d% args

    // CLEO 5
    CLEO_RegisterOpcode(0x2002, CLEO_RETURN_WITH); // 2002=-1, cleo_return_with ...
    CLEO_RegisterOpcode(0x2003, CLEO_RETURN_FAIL); // 2003=-1, cleo_return_fail
}