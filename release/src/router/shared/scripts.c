#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdarg.h>
#include <dirent.h>
#include <ctype.h>
#include <bcmnvram.h>
#include "shutils.h"
#include "shared.h"

int _xstart(const char *cmd, ...)
{
	va_list ap;
	char *argv[16];
	int argc;
	int pid;

	argv[0] = (char *)cmd;
	argc = 1;
	va_start(ap, cmd);
	while ((argv[argc++] = va_arg(ap, char *)) != NULL) {
		//
	}
	va_end(ap);

	return _eval(argv, NULL, 0, &pid);
}

long fappend(FILE *out, const char *fname)
{
	FILE *in;
	char buf[1024];
	int n;
	long r;

	if ((in = fopen(fname, "r")) == NULL) return -1;
	r = 0;
	while ((n = fread(buf, 1, sizeof(buf), in)) > 0) {
		if (fwrite(buf, 1, n, out) != n) {
			r = -1;
			break;
		}
		else {
			r += n;
		}
	}
	fclose(in);
	return r;
}

long fappend_file(const char *path, const char *fname)
{
	FILE *f;
	int r = -1;

	if (f_exists(fname) && (f = fopen(path, "a")) != NULL) {
		r = fappend(f, fname);
		fclose(f);
	}
	return r;
}

void run_custom_script(char *name, char *args)
{
	struct stat sb;
	char script[120];

	snprintf(script, sizeof(script), "/jffs/scripts/%s", name);

	if(f_exists(script)) {
		if (nvram_match("jffs2_scripts", "0")) {
			logmessage("custom_script", "Found %s, but custom script execution is disabled!", name);
			return;
		}
		if ((stat(script, &sb) == 0) && ((sb.st_mode & S_IXUSR) == 0)) {
			logmessage("custom_script", "Found %s, but script is not marked executable!", name);
			return;
		}
		if (args)
			logmessage("custom_script" ,"Running %s (args: %s)", script, args);
		else
			logmessage("custom_script" ,"Running %s", script);
		xstart(script, args);
	}
}

void run_custom_script_blocking(char *name, char *arg1, char *arg2)
{
	struct stat sb;
	char script[120];
    char *cmd[4];
	int pid;
    int timeout = 120;
	char *error;    

	snprintf(script, sizeof(script), "/jffs/scripts/%s", name);

	if(f_exists(script)) {
		if (nvram_match("jffs2_scripts", "0")) {
			logmessage("custom_script", "Found %s, but custom script execution is disabled!", name);
			return;
		}
		if ((stat(script, &sb) == 0) && ((sb.st_mode & S_IXUSR) == 0)) {
			logmessage("custom_script", "Found %s, but script is not marked executable!", name);
			return;
		}
		if (arg1)
			logmessage("custom_script" ,"Running %s (args: %s %s)", script, arg1, (arg2 ? arg2 : ""));
		else
			logmessage("custom_script" ,"Running %s", script);

        cmd[0] = script;
		cmd[1] = arg1;
		cmd[2] = arg2;
		cmd[3] = NULL;
		_eval( cmd, NULL, timeout, (timeout ? NULL : &pid));
	}

}

void run_postconf(char *name, char *config)
{
	char filename[64];

	snprintf(filename, sizeof (filename), "%s", name);
	run_custom_script_blocking(filename, config, NULL);
}


void use_custom_config(char *config, char *target)
{
        char filename[256];

        snprintf(filename, sizeof(filename), "/jffs/configs/%s", config);

	if (check_if_file_exist(filename)) {
		if (nvram_match("jffs2_scripts", "0")) {
			logmessage("custom_config", "Found %s, but custom configs are disabled!", filename);
			return;
		}
		logmessage("custom_config", "Using custom %s config file.", filename);
		eval("cp", filename, target, NULL);
	}
}


void append_custom_config(char *config, FILE *fp)
{
	char filename[256];

	snprintf(filename, sizeof(filename), "/jffs/configs/%s.add", config);

	if (check_if_file_exist(filename)) {
		if (nvram_match("jffs2_scripts", "0")) {
			logmessage("custom_config", "Found %s, but custom configs are disabled!", filename);
			return;
		}
		logmessage("custom_config", "Appending content of %s.", filename);
		fappend(fp, filename);
	}
}
