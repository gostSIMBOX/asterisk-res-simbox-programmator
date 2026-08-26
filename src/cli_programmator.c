/*
 * Copyright (C) 2014-2026 Anton Dodonov (NativeMind)
 * https://github.com/Anton-Dodonov
 * http://linkedin.com/in/anton-dodonov/
 * mailto:anton.v.dodonov@gmail.com
 */

/* cli_diagmode, cli_changeimei, cli_dongle_update: moved verbatim out of
   res_simbox_core's cli.c on 2026-08-26 (sdd-res-simbox Task 3.3,
   Requirements v1.1 acceptance criterion 2). Function bodies are
   untouched from legacy; only their registration is new (this module's
   own AST_MODULE_INFO/CLI array instead of core's). complete_device()
   was promoted from static to EXPORT_DEF in res_simbox_core's cli.c/
   cli.h specifically so this file could keep calling it for CLI
   tab-completion. */

#ifdef HAVE_CONFIG_H
#include <svistok_config.h>
#endif /* HAVE_CONFIG_H */

#include <asterisk.h>
#include <asterisk/cli.h>
#include <asterisk/module.h>
#include <asterisk/logger.h>

#include <asterisk-res-simbox-core/src/chan_dongle.h>	/* struct pvt, find_device(), PVT_ID(), gpublic */
#include <asterisk-res-simbox-core/src/cli.h>		/* complete_device() */
#include <asterisk-res-simbox-core/src/share.h>	/* readpvtinfo() readpvtlimits() make_dongles_imsi_list() */

#include "ttyprog_svistok.c"	/* ttyprog_set_diagmode() ttyprog_changeimei() (via ttyprog_core.c/crc.c) */

#define MODULE_DESCRIPTION "SimBox Firmware Programmator CLI"

static char* cli_diagmode (struct ast_cli_entry* e, int cmd, struct ast_cli_args* a)
{
	const char * msg;
	int fd;

        struct pvt * pvt;

	switch (cmd)
	{
		case CLI_INIT:
			e->command =	"dongle diagmode";
			e->usage   =	"Usage: dongle diagmode <device>\n";
			return NULL;

		case CLI_GENERATE:
			if (a->pos == 2)
			{
				return complete_device (a->word, a->n);
			}
			return NULL;
	}

	if (a->argc != 3)
	{
		return CLI_SHOWUSAGE;
	}



	pvt = find_device (a->argv[2]);
	if (pvt)
	{
		pvt->diagmode=1;
		ast_mutex_unlock_pvt (pvt);
		ast_cli (a->fd, "[%s] Queued Diag Mode\nPlease remove sim\n", a->argv[2]);
	}
	else
	{
		ast_cli (a->fd, "Device %s not found\n", a->argv[2]);
	}

	return CLI_SUCCESS;
}

static char* cli_changeimei (struct ast_cli_entry* e, int cmd, struct ast_cli_args* a)
{
	const char * msg;
	int fd;

        struct pvt * pvt;

	switch (cmd)
	{
		case CLI_INIT:
			e->command =	"dongle changeimei";
			e->usage   =	"Usage: dongle diagmode <device> <num>\n";
			return NULL;

		case CLI_GENERATE:
			if (a->pos == 2)
			{
				return complete_device (a->word, a->n);
			}
			return NULL;
	}

	if (a->argc != 4)
	{
		return CLI_SHOWUSAGE;
	}



	pvt = find_device (a->argv[2]);
	if (pvt)
	{
		//pvt->changeimei=1;
		strcpy(pvt->newimei,a->argv[3]);

		ast_verb (3, "[%s] (instant) Changing imei on fd=%d, imei=%s\n", PVT_ID(pvt),pvt->audio_fd,pvt->newimei);
		ttyprog_changeimei(pvt->audio_fd,pvt->newimei);
//		disconnect_dongle(pvt);
		ast_mutex_unlock_pvt(pvt);
		ast_verb (3, "[%s] (instant) Changing imei OK\n", PVT_ID(pvt));
		ast_cli (a->fd, "[%s] (instant) Imei changed\nPlease restart\n", a->argv[2]);

		//ast_mutex_unlock_pvt (pvt);
		//ast_cli (a->fd, "[%s] Queued changeimei\nPlease restart\n", a->argv[2]);
	}
	else
	{
		ast_cli (a->fd, "Device %s not found\n", a->argv[2]);
	}

	return CLI_SUCCESS;
}

static char* cli_dongle_update(struct ast_cli_entry* e, int cmd, struct ast_cli_args* a)
{
	const char * msg;
	int status;
	void * msgid;

	switch (cmd)
	{
		case CLI_INIT:
			e->command = "dongle update";
			e->usage =
				"Usage: dongle update\n"
				"       update info.\n";
			return NULL;

		case CLI_GENERATE:
			if (a->pos == 2)
			{
				return complete_device (a->word, a->n);
			}
			return NULL;
	}

	if (a->argc != 2)
	{
		return CLI_SHOWUSAGE;
	}


        struct pvt* pvt;

//        AST_RWLIST_RDLOCK (&gpublic->devices);
	AST_RWLIST_TRAVERSE (&gpublic->devices, pvt, entry)
	{
		ast_verb(3,"readpvtinfo-- %s\n",PVT_ID(pvt));
		readpvtinfo(pvt);
		readpvtlimits(pvt);
	}
//	AST_RWLIST_UNLOCK (&gpublic->devices);
		ast_verb(3,"readpvtinfo-- OK %s\n","OK");

	make_dongles_imsi_list();

	return CLI_SUCCESS;
}

static struct ast_cli_entry cli_programmator[] = {
	AST_CLI_DEFINE (cli_diagmode,		"Set Daig Mode"),
	AST_CLI_DEFINE (cli_changeimei,		"Change imei"),
	AST_CLI_DEFINE (cli_dongle_update,	"Update dongles"),
};

static int load_module(void)
{
	return ast_cli_register_multiple (cli_programmator, ARRAY_LEN (cli_programmator)) ?
		AST_MODULE_LOAD_DECLINE : AST_MODULE_LOAD_SUCCESS;
}

static int unload_module(void)
{
	ast_cli_unregister_multiple (cli_programmator, ARRAY_LEN (cli_programmator));
	return 0;
}

AST_MODULE_INFO(ASTERISK_GPL_KEY, AST_MODFLAG_DEFAULT, MODULE_DESCRIPTION,
		.load = load_module,
		.unload = unload_module,
	       );
