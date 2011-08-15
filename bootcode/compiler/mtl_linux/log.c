/**
	 Routines simples pour pouvoir logger les sorties du compilateur
*/

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <time.h>
#include <stdlib.h>

#include "log.h"

static int my_msk = LOG_INIT | LOG_VM | LOG_SIMUNET;
#define BUFSIZE 16384
static char tmpbuf[BUFSIZE];

// siml : Stopped In the Midle of a Line. A 1 quand on a pas affiché
// tout le buffer parce qu'il ne se terminait pas par '\n'
static int siml=0;
static e_logtypes siml_t=LOG_INIT;
static int siml_idx=0;

// infos concernant le log
static FILE *outstream=NULL; // flux vers lequel on logge
static char *outfilename=NULL; // nom de fichier correspondant
static int max_log_size=0; // taille max d'un fichier de log
static int max_log_time=0; // temps max de log par fichier, en secondes
static time_t last_log_roll=0; // depuis quand on logge dans le fichier de log courant
static int current_log_size=0; // la taille courant du log
static time_t time_first_log=0; // la date du premier log enregistre
static int do_log_time=0; // si 1, on ajoute le nombre de secondes depuis le reveil du lapin au debut de chaque ligne

static void my_printf_check_log_roll(void);

/**
	 Renvoie, selon le type passé en argument, le préfixe à utiliser dans le log
 */
static const char *get_prefix(e_logtypes t)
{
	const char *res = NULL;
	int i;
	switch (t) {
	case LOG_INIT:
		res = "init"; break;
	case LOG_VM:
		res = "vm"; break;
	case LOG_SIMUNET:
		res = "simunet"; break;
	case LOG_SIMUAUDIO:
		res = "simuaudio"; break;
	case LOG_SIMULEDS:
		res = "simuleds"; break;
	case LOG_SIMUMOTORS:
		res = "simumotors"; break;
	}
	return res;
}

/**
	 Affiche s en ajoutant [pfx] au début de chaque ligne.

	 Si force==1, on affiche même s'il n'y a pas de '\n' (et on en rajoute un)
 */
static int print_line_by_line(e_logtypes t, char *s, int force)
{
	char *tmp;
	int nbwritten=0;
	char pfx[256];
	if (do_log_time)
		snprintf(pfx,255, "%06d %s", time(NULL) - time_first_log, get_prefix(t));
	else
		strncpy(pfx, get_prefix(t), 255);
	while (*s!='\0')
		{
			tmp = strchr(s, '\n');
			if (NULL!=tmp) {
				*tmp='\0';
				current_log_size += fprintf(outstream, "[%s] %s\n", pfx, s);
				*tmp='\n';
				nbwritten+=(tmp-s)+1;
				s=tmp+1;
			}
			else {
				if (force) {
					current_log_size += fprintf(outstream, "[%s] %s\n", pfx, s);
					nbwritten+=strlen(s);
				}
				break;
			}
		}
	return nbwritten;
}

/**
	 Comme printf, filtré par les paramètrages du log.

	 t : le type de message affiché
 */
void my_printf(e_logtypes t, const char *fmt, ...)
{
	if (NULL==outstream)
		outstream = stdout;

	my_printf_check_log_roll();

	if (0==time_first_log && do_log_time)
		{
			char tmp[256];
			time_first_log = time(NULL);
			snprintf(tmp, 255, "First log time: %s", ctime(&time_first_log));
			print_line_by_line(LOG_INIT, tmp, 0);
		}

	int len, nbwritten;

	if (0==((int)t & my_msk))
		return;

	if (siml && siml_t!=t)
		{
			// il faut "flusher" ce qu'il reste dans le buffer
			print_line_by_line(siml_t, tmpbuf, 1);
			siml=0;
			siml_idx=0;
		}

	va_list args;
	va_start(args, fmt);
	len=vsnprintf(tmpbuf+siml_idx, BUFSIZE-1-siml_idx, fmt , args);
	va_end(args);

	// 	make_writeable(tmpbuf); ? s'il y a des caractères non imprimables ?
	nbwritten=print_line_by_line(t, tmpbuf, 0);

	len += siml_idx;
	if(nbwritten<len)
		{
			siml=1;
			siml_t=t;
			siml_idx=len-nbwritten; // +siml_idx au cas ou ce n'est pas la premiere fois qu'on rajoute qqe chose dans le buffer.
			if (nbwritten!=0)
				// on déplace ce qu'il reste à écrire au début de tmpbuf
				memmove(tmpbuf, tmpbuf+nbwritten, len-nbwritten);
		}
	else
		{
			siml=0;
			siml_idx=0;
		}
}

/**
	 Permet de contrôler les logs qu'on veut afficher (ils s'affichent
	 tous par défaut)
 */
void my_printf_set_options(const char *s)
{
	my_msk=0;

	char *token, *cpy;

	cpy = token = strdup(s);
	token = strtok(cpy, ",");
	while (NULL!=token) {
		if (!strcmp(token,"init"))
			my_msk |= LOG_INIT;
		else if (!strcmp(token,"vm"))
			my_msk |= LOG_VM;
		else if (!strcmp(token,"simunet"))
			my_msk |= LOG_SIMUNET;
		else if (!strcmp(token,"simuaudio"))
			my_msk |= LOG_SIMUAUDIO;
		else if (!strcmp(token,"simuleds"))
			my_msk |= LOG_SIMULEDS;
		else if (!strcmp(token,"simumotors"))
			my_msk |= LOG_SIMUMOTORS;
		else {
				fprintf(stderr, "erreur: %s n'est pas une option valide pour les logs\n");
				break;
		}
		token = strtok(NULL, ",");
	}
	free(cpy);
}

/**
	 Spécifie le nom du fichier dans lequel on veut logger la sortie du lapin

	 retourne 0 si tout s'est bien passé, 1 sinon
 */
int my_printf_set_logfile(const char *filename)
{
	FILE *f = fopen(filename, "w");

	if (NULL==f)
		{
			fprintf(stderr, "Could not open file %s to log in it.\n", filename);
			return 1;
		}

	if (NULL!=outstream)
		{
			fclose(outstream);
			outstream=NULL;
		}
	if (NULL!=outfilename)
		{
			free(outfilename);
			outfilename=NULL;
		}

	outstream=f;
	outfilename=strdup(filename);
	current_log_size=0;
	last_log_roll=time(NULL);

	return 0;
}


static int my_printf_rotate_log_n(int n);

/**
	 Effectue la rotation des logs

	 - on decale tous les <log>.<n> existants en <log>.<n+1>
   - le log courant devient <log>.0
	 - on reouvre le fichier <log> pour logger dedans
 */
static int my_printf_rotate_log(void)
{
	// décaler tous les logs existants et <log> en <log>.0
	my_printf_rotate_log_n(-1);
	char *tmp;
	tmp=strdup(outfilename);
	my_printf_set_logfile(tmp);
	free(tmp);
}

/**
	 si n==-1, alors déplace <log> en <log>.0

	 renomme le fichier <log>.<n> en fichier <log>.<n+1>, et pareil pour
	 tous les suivants.

	 /!\ fonction réentrante
 */
static int my_printf_rotate_log_n(int n)
{
  char tmpn[255];
  char tmpnplusun[255];
	struct stat ststat;

	if (n<0)
		strcpy(tmpn, outfilename);
	else
		snprintf(tmpn, 255, "%s.%d", outfilename, n);
	snprintf(tmpnplusun, 255, "%s.%d", outfilename, n+1);

	if (!stat(tmpnplusun, &ststat))
		my_printf_rotate_log_n(n+1);
	rename(tmpn, tmpnplusun);
}

/**
	 Effectue la rotation des logs si besoin
 */
static void my_printf_check_log_roll(void)
{
	int need_roll=0;

	if (0!=max_log_size)
		need_roll += (current_log_size > max_log_size);
	if (0!=max_log_time)
		{
			need_roll += (((int)time(NULL) - last_log_roll) > max_log_time);
		}

	if (need_roll)
		my_printf_rotate_log();
}

void my_printf_set_max_log_size(int n)
{
	max_log_size=n;
}

void my_printf_set_max_log_time(int n)
{
	max_log_time=n;
}

void my_printf_set_do_log_time(int n)
{
	do_log_time=n;
}
