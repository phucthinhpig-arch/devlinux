#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

typedef enum
{
    PERM_READ    = (1U << 0U),
    PERM_WRITE   = (1U << 1U),
    PERM_EXECUTE = (1U << 2U),
    PERM_DELETE  = (1U << 3U)
} sys_perms_e;

bool has_permission(uint8_t user_perms, uint8_t required_perms)
{
    uint8_t result = (uint8_t)(user_perms & required_perms);
    return (result == required_perms);
}

static void print_permissions(uint8_t perms)
{
    printf("0x%02X", perms);
    
    printf(" (");
    bool first = true;
    
    if ((perms & PERM_READ) != 0U)
    {
        printf("%sRead", first ? "" : "|");
        first = false;
    }
    
    if ((perms & PERM_WRITE) != 0U)
    {
        printf("%sWrite", first ? "" : "|");
        first = false;
    }
    
    if ((perms & PERM_EXECUTE) != 0U)
    {
        printf("%sExecute", first ? "" : "|");
        first = false;
    }
    
    if ((perms & PERM_DELETE) != 0U)
    {
        printf("%sDelete", first ? "" : "|");
        first = false;
    }
    
    if (first)
    {
        printf("None");
    }
    
    printf(")");
}

static void test_permission(uint8_t user_perms, 
                           uint8_t required_perms, 
                           const char * const p_description)
{
    bool granted = has_permission(user_perms, required_perms);
    
    printf("  %s: ", p_description);
    printf("User ");
    print_permissions(user_perms);
    printf(" needs ");
    print_permissions(required_perms);
    printf(" -> ");
    
    if (granted)
    {
        printf("GRANTED\n");
    }
    else
    {
        printf("DENIED\n");
    }
}

int main(void)
{
    printf("=== Bitmask Permissions Tester ===\n\n");
    
    printf("=== Enum Size Analysis ===\n");
    printf("Enum size: %zu bytes ", sizeof(sys_perms_e));
    
#ifdef _SIZEOF_INT_
    printf("(Standard GCC on %s", 
           (_SIZEOF_INT_ == 4) ? "x86" : "other");
#endif
    
#ifdef __ARM_ARCH
    printf(" - ARM Cortex-M");
#endif
    
#ifdef _SHORT_ENUMS_
    printf(" - With -fshort-enums");
#endif
    
    printf(")\n\n");
    
    printf("=== Permission Tests ===\n");
    
    test_permission((uint8_t)(PERM_READ | PERM_WRITE), 
                   (uint8_t)PERM_READ, 
                   "Check Read permission");
    
    test_permission((uint8_t)(PERM_READ | PERM_WRITE), 
                   (uint8_t)PERM_EXECUTE, 
                   "Check Execute permission");
    
    test_permission((uint8_t)(PERM_READ | PERM_WRITE), 
                   (uint8_t)(PERM_READ | PERM_WRITE), 
                   "Check Read AND Write");
    
    test_permission((uint8_t)(PERM_READ | PERM_WRITE), 
                   (uint8_t)(PERM_READ | PERM_DELETE), 
                   "Check Read AND Delete");
    
    test_permission((uint8_t)(PERM_READ | PERM_WRITE | PERM_EXECUTE | PERM_DELETE), 
                   (uint8_t)(PERM_READ | PERM_WRITE | PERM_EXECUTE | PERM_DELETE), 
                   "Check ALL permissions (Full access)");
    
    test_permission((uint8_t)(PERM_EXECUTE | PERM_DELETE), 
                   (uint8_t)PERM_EXECUTE, 
                   "Check Execute permission (Execute|Delete)");
    
    test_permission((uint8_t)(PERM_READ | PERM_DELETE), 
                   (uint8_t)(PERM_WRITE | PERM_EXECUTE), 
                   "Check Write AND Execute (complex)");
    
    test_permission(0U, 
                   (uint8_t)PERM_READ, 
                   "Check Read (No permissions)");
    
    test_permission((uint8_t)PERM_READ, 
                   0U, 
                   "Check No permissions required");
    
    printf("\n=== Summary ===\n");
    printf("Total tests: 9\n");
    printf("All tests completed successfully!\n");
    
    return 0;
}