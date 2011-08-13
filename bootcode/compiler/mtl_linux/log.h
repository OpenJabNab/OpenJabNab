#ifndef _LOG_H_
# define _LOG_H_

typedef enum {
	LOG_INIT = 1,         // les messages écrits pendant l'init
	LOG_VM = 2,           // les messages écrits par le lapin
	LOG_SIMUNET = 4,      // les logs de simulation réseau
	LOG_SIMUAUDIO = 8,    // les logs de simulation audio
	LOG_SIMULEDS = 16,    // les logs de simulation des leds
	LOG_SIMUMOTORS = 32   // les messages écrits pendant l'init
} e_logtypes;

void my_printf(e_logtypes t, const char *fmt, ...);
void my_printf_set_options(const char *s);
int my_printf_set_logfile(const char *filename);
void my_printf_set_max_log_size(int n);
void my_printf_set_max_log_time(int n);
void my_printf_set_do_log_time(int n);

#endif // ! _LOG_H_
