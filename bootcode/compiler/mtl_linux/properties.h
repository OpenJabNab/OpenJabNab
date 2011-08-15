#ifndef PROPERTIES_H_
# define PROPERTIES_H_

int PropLoad(const char* file);
char* PropGet(const char *field);
int PropGetInt(const char *field);
void PropSet(const char *field, const char *val);
void PropDump();

#endif // ! PROPERTIES_H_
