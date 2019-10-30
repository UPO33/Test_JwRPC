#include "SourenaSettings.h"

USourenaSetting::USourenaSetting()
{
#if WITH_EDITORONLY_DATA
	AdminClientDefaultPassword = "qwerty123";
	AdminClientDefaultUsername = "admin";
#endif
}
