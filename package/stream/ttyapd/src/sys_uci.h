#ifndef INC_SYS_UCI
#define INC_SYS_UCI


enum {
	SYS_UCI_RET_OK =		0,
	SYS_UCI_RET_NOTFOUND =	1,
	SYS_UCI_RET_NULLINPUT =	2,
	SYS_UCI_RET_ERROR =		3,
};

int sys_uci_get(char *opt, unsigned char **res);
int sys_uci_add(char *cfg, char *section);
int sys_uci_set(char *text, char *value);
int sys_uci_del(char *opt);
int sys_uci_commit(char *opt);

void sys_wifireset(void);

#endif
