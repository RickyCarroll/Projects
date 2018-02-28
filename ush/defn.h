//Depends on both ush.c, expand.c and builtin.c
int rc_expand (char *orig, char *new, int newsize);
int rc_builtin (char **argv, int *argc);
void processline (char *line);
