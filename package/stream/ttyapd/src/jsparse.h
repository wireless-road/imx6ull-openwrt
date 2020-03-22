#ifndef INC_JSPARSE
#define INC_JSPARSE


typedef enum cmd_t_{
	CMD_STATUS = 0,
	CMD_SCAN,
	CMD_GET_ESSID,
	CMD_GET_ENCTYPE,
	CMD_GET_ENCKEY,
	CMD_SET_ESSID,
	CMD_SET_ENCTYPE,
	CMD_SET_ENCKEY,
	CMD_COMMIT,
	CMD_ERROR,
}cmd_t;

cmd_t jsparse_get_cmd(unsigned char *input, unsigned char **arg);
unsigned char *jsparse_make_json_result(unsigned char *result);

#endif
