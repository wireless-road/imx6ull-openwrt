#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <json-c/json.h>

#include "main.h"
#include "jsparse.h"


/* Indexes must be in accordance to cmd_t in jsparse.h */
const char *JSPARSE_CMD_STR[] = {
	"status",
	"scan",
	"get_essid",
	"get_enctype",
	"get_enckey",
	"set_essid",
	"set_enctype",
	"set_enckey",
	"commit",
	NULL,
};

cmd_t jsparse_get_cmd(unsigned char *input, unsigned char **arg){
	cmd_t ret=CMD_ERROR;
	json_object *jobj = json_tokener_parse(input);
	enum json_type type;
	int i=0;
	const unsigned char *sval;
	
	*arg = NULL;
	
	/* Not valid json received.*/
	if(!jobj)
		return CMD_ERROR;
	
	json_object_object_foreach(jobj, key, val) {
		type = json_object_get_type(val);
		switch (type) {
			case json_type_string:
				sval = json_object_get_string(val);
				if(!strcasecmp(key, "cmd")){
					while(JSPARSE_CMD_STR[i]){
						if(!strcasecmp(JSPARSE_CMD_STR[i], sval)){
							ret = i;
							break;
						}
						i++;
					}
				}else if(!strcasecmp(key, "arg")){
					*arg = calloc(strlen(sval) + 1, sizeof(unsigned char));
					strcpy(*arg, sval);
				}
				break;
			default:
				break;
		}
	}
	
	json_object_put(jobj);
	
	return ret;
}

unsigned char *jsparse_make_json_result(unsigned char *result){
	json_object *jobj = json_object_new_object();
	json_object *jstr = json_object_new_string(result);
	const char *str;
	unsigned char *retbuf;
	json_object_object_add(jobj, "result", jstr);
	
	str = json_object_to_json_string(jobj);
	retbuf = calloc(strlen(str) + 1, sizeof(unsigned char));
	strcpy(retbuf, str);
	json_object_put(jobj);
	return retbuf;
}
