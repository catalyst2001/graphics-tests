#pragma once
#include <stdio.h>

typedef void *HCONCMD;
typedef void (*p_ConCmdCallback)(int argc, const char **argv, void *p_UserData);

enum CmdLibConVarChangeResult
{
	CmdLib_ConVarChange_Continue = 0,
	CmdLib_ConVarChange_Restrict
};
typedef CmdLibConVarChangeResult (*p_ConVarChangeCallback)(const char *p_Name, const char *p_OldValue, const char *p_NewValue);

enum CmdLibMsgType
{
	CmdLibMsg_Success = 0,
	CmdLibMsg_Warning,
	CmdLibMsg_Error
};
typedef void (*p_CmdLibConMsgCallback)(const char *p_MsgLine, size_t Length, CmdLibMsgType MsgType);

// ---------------
// CmdLib concmd flags
// ---------------
#define FCONCMD_CONCOMMAND (1 << 1)
#define FCONCMD_CONVAR (1 << 2)
#define FCMDLIB_APPEND_COLORKEYS (1 << 3)

// ---------------
// CmdLib queries
// ---------------
enum CmdLibQueryTypes
{
	CmdLib_QueryVersion = 0,
	CmdLib_QueryCmdMaxlen,
	CmdLib_QueryCmdBufferLength,
	CmdLib_QueryCmdMaxArgs,
	CmdLib_QueryNumberOfCmds,
	CmdLib_QueryNumberOfConVars
};

enum CmdLibConFlagOperation
{
	CmdLib_ConFlag_Get = 0,
	CmdLib_ConFlag_Set
};

// ---------------
// Basic concmd information
// ---------------
struct concmd_info_s
{
	int nFlags;
	const char *p_Name;
	const char *p_Value;
	const char *p_Description;
	void *p_Callback; //
};

#define CMDLIB_DEFAULT_BUFFER_SIZE (84000) //84 kb
#define CMDLIB_DEFAULT_CONCMD_LENGTH (1024) //1024 bytes
#define CMDLIB_DEFAULT_CONCMD_ARGS (64) //64 args
#define CMDLIB_DEFAULT_CONVAR_VALUE_LENGTH (256) //256 bytes

// ---------------
// low-Level cmdlib interface
// ---------------
class ILibCmd
{
public:
	// -----------------
	// Cmdlib
	// -----------------

	// Init cmdlib
	virtual int              cmdlib_initialize(size_t nConCmdMaxlen, size_t nConCmdValMaxlen, size_t nConCmdMaxArgs, size_t nCmdBufferMaxlen) = 0;

	// Deinit cmdlib
	virtual int              cmdlib_shutdown() = 0;

	// requests information
	virtual int              cmdlib_query(void *p_Dst, CmdLibQueryTypes QueryType) = 0;

	// last error code
	virtual int              cmdlib_last_error_code() = 0;

	// last error string
	virtual const char      *cmdlib_last_error_string() = 0;


	// -----------------
	// ConCmds
	// -----------------

	// registers a console command or variable
	virtual HCONCMD          concmd_register(const char *p_ConCommandName, const char *p_ConCommandDescription, void *p_UserData, int nFlags) = 0;

	// gets or sets a new callback
	// To get an existing one, pass NULL to NewCallback
	// To set a new one, pass the address of the new callback procedure.The return value is the address of the old callback routine.
	virtual p_ConCmdCallback concmd_callback(HCONCMD hConCmd, p_ConCmdCallback NewCallback) = 0;

	// get or change the flag of a console command or variable
	virtual int              concmd_flag(HCONCMD hConCmd, long *p_Flag, CmdLibConFlagOperation ConCmdFlagOperation) = 0;

	// removes a console command or variable
	virtual int              concmd_unregister(HCONCMD hConCmd) = 0;

	// find for a console command or variable
	virtual HCONCMD          concmd_find(const char *p_ConCmdName) = 0;

	// move to the next console command or variable
	virtual HCONCMD          concmd_next(HCONCMD hLastConCmd) = 0;

	// get information about a console command or variable
	virtual int              concmd_info(HCONCMD hConCmd, concmd_info_s *p_DstConCmdInfo) = 0;


	// -----------------
	// Console interface
	// -----------------

	// change global flag
	// this controls access to commands and variables
	virtual void             con_global_flag(long *p_Flag, CmdLibConFlagOperation ConCmdFlagOperation) = 0;
	
	// execute command in command buffer and clear it
	virtual bool             con_exec_buf() = 0;

	// execute command line
	virtual bool             con_exec_cmd(const char *p_CommandLine) = 0;

	// execute config file
	virtual bool             con_exec_file(const char *p_Filename) = 0;

	// gets or sets the address of the callback procedure for displaying messages
	virtual p_CmdLibConMsgCallback con_register_message_callback(p_CmdLibConMsgCallback p_ConMsgCallback) = 0;
};

