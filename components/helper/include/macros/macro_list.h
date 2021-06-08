#ifndef MACRO_LIST_H__
#define MACRO_LIST_H__

#define BEGIN_LIST(type, list_name)		type list_name[] = {
#define END_LIST					NULL}
#define LOOP_LIST(list,funct,...)	while(list++){ \
										funct(__VA_ARGS__); \
									}

#endif /*MACRO_LIST_H__  */
