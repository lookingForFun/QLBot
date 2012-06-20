#ifndef CONSOLECOMMANDSH
#define CONSOLECOMMANDSH

#include "hookConsole.h"

void Cmd_AddTimer(eLine * p_cmd);
void Cmd_ToggleConsole(eLine *);
void Cmd_Exec(eLine * p_cmd);
void Cmd_Toggle(eLine * cmd);
void Cmd_Unbind(eLine * p_cmd);
void Cmd_Bind(eLine * p_cmd);
bool ScanForQuotes(_In_z_ const char * p_szSrc);

#endif