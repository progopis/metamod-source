/* ======== SourceMM ========
* Copyright (C) 2004-2005 SourceMM Development Team
* No warranties of any kind
*
* License: zlib/libpng
*
* Author(s): David "BAILOPAN" Anderson
* ============================
*/

#include "concommands.h"
#include "CPlugin.h"

/**
 * @brief Console Command Implementations
 * @file concommands.cpp
 */

SMConVarAccessor g_SMConVarAccessor;

bool SMConVarAccessor::RegisterConCommandBase(ConCommandBase *pCommand)
{
	pCommand->SetNext( NULL );
	g_Engine.icvar->RegisterConCommandBase(pCommand);

	return true;
}

ConVar metamod_version("metamod_version", SOURCEMM_VERSION, FCVAR_REPLICATED | FCVAR_SPONLY, "Metamod:Source Version");

CON_COMMAND(meta, "Metamod:Source Menu")
{
	IVEngineServer *e = g_Engine.engine;

	int args = e->Cmd_Argc();

	if (args >= 2)
	{
		const char *command = e->Cmd_Argv(1);
		if (strcmp(command, "credits") == 0)
		{
			Msg("Metamod:Source was developed by:\n");
			Msg("  SourceHook: Pavol \"PM OnoTo\" Marko\n");
			Msg("  GameDLL/Plugins: David \"BAILOPAN\" Anderson\n");
			Msg("  GameDLL: Scott \"Damaged Soul\" Ehlert\n");
			Msg("For more information, see the official website\n");
			Msg("http://www.sourcemm.net/\n");
			
			return;
		} else if (strcmp(command, "version") == 0) {
			Msg("Metamod:Source version %s\n", SOURCEMM_VERSION);
			Msg("Compiled on: %s\n", SOURCEMM_DATE);
			Msg("Plugin interface version: %d/%d\n", PLAPI_VERSION, PLAPI_MIN_VERSION);
			Msg("http://www.sourcemm.net/\n");

			return;
		} else if (strcmp(command, "game") == 0) {
			Msg("GameDLL Information\n");
			Msg("  Mod path: %s\n", g_ModPath.c_str());
			Msg("  Dll path: %s\n", g_BinPath.c_str());

			return;
		} else if (strcmp(command, "refresh") == 0) {
			char full_path[255];
#if defined WIN32 || defined _WIN32
			snprintf(full_path, sizeof(full_path)-1, "%s\\%s", g_ModPath.c_str(), "metaplugins.ini");
#else
			snprintf(full_path, sizeof(full_path)-1, "%s/%s", g_ModPath.c_str(), "metaplugins.ini");
#endif
			LoadPluginsFromFile(full_path);

			return;
		} else if (strcmp(command, "list") == 0) {
			SourceMM::CPluginManager::CPlugin *pl;
			PluginIter i;
			const char *status="";

			Msg("[Id] %-16.15s  %-8.7s  %-12.11s %-8.7s\n", "Name", "Version", "Author", "Status");
			for (i=g_PluginMngr._begin(); i!=g_PluginMngr._end(); i++)
			{
				pl = (*i);
				if (!pl)
					break;
				if (pl->m_Status == Pl_Paused)
				{
					status = "PAUSE";
				} else if (pl->m_Status == Pl_Running) {
					status = "RUN";
				} else if (pl->m_Status == Pl_Refused) {
					status = "FAIL";
				} else if (pl->m_Status == Pl_Error) {
					status = "ERROR";
				} else if (pl->m_Status == Pl_NotFound) {
					status = "NOFILE";
				}
				Msg("[%02d] %-16.15s  %-8.7s  %-12.11s %-8.7s\n", pl->m_Id, pl->m_API->GetName(), pl->m_API->GetVersion(), pl->m_API->GetAuthor(), status);
			}

			return;
		} else if (strcmp(command, "info") == 0) {
			if (args >= 3)
			{
				int id = atoi(e->Cmd_Argv(2));
				SourceMM::CPluginManager::CPlugin *pl = g_PluginMngr.FindById(id);
				if (!pl)
				{
					Msg("Plugin %d not found.\n", id);
					return;
				}

				if (!pl->m_API)
				{
					Msg("Plugin %d is not loaded.\n", id);
				} else {
					if (pl->m_Status == Pl_Paused)
					{
						Msg("Plugin %d is paused.\n", id);
					} else if (pl->m_Status == Pl_Running) {
						Msg("Plugin %d is running.\n", id);
					}
					Msg("Name: \"%s\" by %s\n", pl->m_API->GetName(), pl->m_API->GetAuthor());
					Msg("Version: %s\n", pl->m_API->GetVersion());
					Msg("Description: %s\n", pl->m_API->GetDescription());
					Msg("License: %s\n", pl->m_API->GetLicense());
					Msg("URL: %s\n", pl->m_API->GetURL());
					Msg("Details: API %03d, Date: %s\n", pl->m_API->GetApiVersion(), pl->m_API->GetDate());
				}
				Msg("File: %s\n", pl->m_File.c_str());

				return;
			} else {
				Msg("Usage: meta info <id>\n");

				return;
			}
		}
	}

	Msg("Metamod:Source Menu\n");
	Msg("usage: meta <command> [arguments]\n");
	Msg("   credits   - About Metamod:Source\n");
	Msg("   game      - Information about GameDLL\n");
	Msg("   info      - Information about a plugin\n");
	Msg("   list      - List plugins\n");
	Msg("   refresh   - Reparse plugins file\n");
	Msg("   version   - Version information\n");
}