#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

typedef enum
{
    eNotWaitingNotification = 0,
    eWaitingNotification,
    eNotified
} eNotifyValue;

typedef struct
{
    volatile StackType_t *pxTopOfStack;

#if (portUSING_MPU_WRAPPERS == 1)
    xMPU_SETTINGS xMPUSettings;
#endif

    ListItem_t xGenericListItem;
    ListItem_t xEventListItem;
    UBaseType_t uxPriority;
    StackType_t *pxStack;
    char pcTaskName[configMAX_TASK_NAME_LEN];
    BaseType_t xCoreID;

#if (portSTACK_GROWTH > 0 || configENABLE_TASK_SNAPSHOT == 1)
    StackType_t *pxEndOfStack;
#endif

#if (portCRITICAL_NESTING_IN_TCB == 1)
    UBaseType_t uxCriticalNesting;
    uint32_t uxOldInterruptState;
#endif

#if (configUSE_TRACE_FACILITY == 1)
    UBaseType_t uxTCBNumber;
    UBaseType_t uxTaskNumber;
#endif

#if (configUSE_MUTEXES == 1)
    UBaseType_t uxBasePriority;
    UBaseType_t uxMutexesHeld;
#endif

#if (configUSE_APPLICATION_TASK_TAG == 1)
    TaskHookFunction_t pxTaskTag;
#endif

#if (configNUM_THREAD_LOCAL_STORAGE_POINTERS > 0)
    void *pvThreadLocalStoragePointers[configNUM_THREAD_LOCAL_STORAGE_POINTERS];
#if (configTHREAD_LOCAL_STORAGE_DELETE_CALLBACKS)
    TlsDeleteCallbackFunction_t pvThreadLocalStoragePointersDelCallback[configNUM_THREAD_LOCAL_STORAGE_POINTERS];
#endif
#endif

#if (configGENERATE_RUN_TIME_STATS == 1)
    uint32_t ulRunTimeCounter;
#endif

#if (configUSE_NEWLIB_REENTRANT == 1)
    struct _reent xNewLib_reent;
#endif

#if (configUSE_TASK_NOTIFICATIONS == 1)
    volatile uint32_t ulNotifiedValue;
    volatile eNotifyValue eNotifyState;
#endif
#if (tskSTATIC_AND_DYNAMIC_ALLOCATION_POSSIBLE != 0)
    uint8_t ucStaticallyAllocated;
#endif
} task_control_block_t;