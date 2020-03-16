#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <uci.h>

#include "sys_uci.h"

int sys_uci_get(char *opt, unsigned char **res)
{
	struct uci_context *ctx;
	struct uci_ptr ptr;
	struct uci_element *e;
	char * lopt;

	lopt = calloc(strlen(opt) + 1,sizeof(char));
	snprintf(lopt, strlen(opt) + 1, opt);

	ctx = uci_alloc_context();

	if (uci_lookup_ptr(ctx, &ptr, lopt, true) != UCI_OK) {
		uci_free_context(ctx);
		free(lopt);
		return SYS_UCI_RET_NOTFOUND;
	}

	if (!(ptr.flags & UCI_LOOKUP_COMPLETE)) {
		ctx->err = UCI_ERR_NOTFOUND;
		uci_free_context(ctx);
		free(lopt);
		return UCI_ERR_NOTFOUND;
	}

	e = ptr.last;

	switch (e->type) {
		case UCI_TYPE_SECTION:
			*res = calloc(strlen(ptr.s->type) + 1, sizeof(char));
			memcpy(*res, ptr.s->type, strlen(ptr.s->type));
			break;
		case UCI_TYPE_OPTION:
			switch (ptr.o->type) {
				case UCI_TYPE_STRING:
					*res = calloc(strlen(ptr.o->v.string) + 1, sizeof(char));
					memcpy(*res, ptr.o->v.string, strlen(ptr.o->v.string));
					break;
				case UCI_TYPE_LIST:
					/* TODO: If needed lists in future. */
					break;
			}
			break;
		default:
			break;
	}

	uci_free_context(ctx);
	free(lopt);
	return SYS_UCI_RET_OK;
}

int sys_uci_add(char *cfg, char * section)
{
	struct uci_package *p = NULL;
	struct uci_section *s = NULL;
	struct uci_context *ctx;
	char *lcfg = calloc( strlen(cfg)+1, sizeof(char));;
	char *lsection = calloc( strlen(section)+1, sizeof(char));;
	int ret = UCI_OK;

	if ( (!cfg) || (!section)) {
		return SYS_UCI_RET_NULLINPUT;
	}
	snprintf(lcfg, strlen(cfg) + 1, cfg);
	snprintf(lsection, strlen(section) + 1, section);

	ctx = uci_alloc_context();
	ret = uci_load(ctx, lcfg, &p);
	if (ret != UCI_OK)
		goto done;

	ret = uci_add_section(ctx, p, lsection, &s);
	if (ret == UCI_OK)
		ret = uci_save(ctx, p);

done:
	uci_free_context(ctx);
	free(lcfg);
	free(lsection);
	return (ret == UCI_OK ? SYS_UCI_RET_OK : SYS_UCI_RET_ERROR);
}

int sys_uci_set(char * opt, char *value)
{
	struct uci_context *ctx;
	struct uci_ptr ptr;
	char *cmd;
	int ret = UCI_OK;
	cmd = calloc( strlen(opt) + strlen(value) + 2, sizeof(char));

	snprintf(cmd, strlen(opt) + strlen(value) + 2, "%s=%s", opt, value);
	ctx = uci_alloc_context();

	if (uci_lookup_ptr(ctx, &ptr, cmd, true) != UCI_OK) {
		uci_free_context(ctx);
		free(cmd);
		return SYS_UCI_RET_NOTFOUND;
	}

	ret = uci_set(ctx, &ptr);

	if (ret == UCI_OK)
		ret = uci_save(ctx, ptr.p);
	else {
		uci_free_context(ctx);
		free(cmd);
		return SYS_UCI_RET_NOTFOUND;
	}
	uci_free_context(ctx);
	free(cmd);
	return (ret == UCI_OK ? SYS_UCI_RET_OK : SYS_UCI_RET_ERROR);
}

int sys_uci_del(char * opt)
{
	struct uci_context *ctx;
	struct uci_ptr ptr;
	char *lopt;
	int ret = UCI_OK;
	int dummy;
	lopt = calloc( strlen(opt) + 1, sizeof(char));

	snprintf(lopt, strlen(opt) + 1, opt);
	ctx = uci_alloc_context();

	if (uci_lookup_ptr(ctx, &ptr, lopt, true) != UCI_OK) {
		uci_free_context(ctx);
		free(lopt);
		return SYS_UCI_RET_NOTFOUND;
	}
	if (ptr.value && !sscanf(ptr.value, "%d", &dummy)) {
		uci_free_context(ctx);
		free(lopt);
		return SYS_UCI_RET_NOTFOUND;
	}
	ret = uci_delete(ctx, &ptr);

	if (ret == UCI_OK)
		ret = uci_save(ctx, ptr.p);
	else {
		uci_free_context(ctx);
		free(lopt);
		return SYS_UCI_RET_ERROR;
	}
	uci_free_context(ctx);
	free(lopt);
	return (ret == UCI_OK ? SYS_UCI_RET_OK : SYS_UCI_RET_ERROR);
}

int sys_uci_commit(char * opt)
{
	struct uci_context *ctx;
	struct uci_ptr ptr;
	char *lopt;
	lopt = calloc( strlen(opt) + 1, sizeof(char));

	snprintf(lopt, strlen(opt) + 1, opt);
	ctx = uci_alloc_context();

	if (uci_lookup_ptr(ctx, &ptr, lopt, true) != UCI_OK) {
		uci_free_context(ctx);
		free(lopt);
		return SYS_UCI_RET_NOTFOUND;
	}

	if (uci_commit(ctx, &ptr.p, false) != UCI_OK) {
		uci_free_context(ctx);
		free(lopt);
		return SYS_UCI_RET_NOTFOUND;
	}

	uci_free_context(ctx);
	free(lopt);
	return SYS_UCI_RET_OK;
}

void sys_wifireset(void){
	pid_t ret = fork();
	if (ret == 0) {
		char *params[2]  = {"wifi", 0};
		execv("wifi", params);
		exit(0);
	} else {
		waitpid(ret, NULL, 0);
	}
}
