/*
    C-Dogs SDL
    A port of the legendary (and fun) action/arcade cdogs.
    Copyright (c) 2013-2014, Cong Xu
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:

    Redistributions of source code must retain the above copyright notice, this
    list of conditions and the following disclaimer.
    Redistributions in binary form must reproduce the above copyright notice,
    this list of conditions and the following disclaimer in the documentation
    and/or other materials provided with the distribution.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
    AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
    ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
    LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
    CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
    SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
    INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
    CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
    ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
    POSSIBILITY OF SUCH DAMAGE.
*/
#include "grafx_bg.h"

#include "actors.h"
#include "ai.h"
#include "draw.h"
#include "drawtools.h"
#include "objs.h"
#include "quick_play.h"
#include "triggers.h"

void GrafxMakeRandomBackground(
	GraphicsDevice *device,
	CampaignOptions *co, struct MissionOptions *mo, Map *map)
{
	HSV tint;
	CampaignSettingInit(&co->Setting);
	ActorsInit();
	ObjsInit();
	MobObjsInit();
	SetupQuickPlayCampaign(&co->Setting, &gConfig.QuickPlay);
	co->seed = rand();
	tint.h = rand() * 360.0 / RAND_MAX;
	tint.s = rand() * 1.0 / RAND_MAX;
	tint.v = 0.5;
	DrawBuffer buffer;
	DrawBufferInit(&buffer, Vec2iNew(X_TILES, Y_TILES), device);
	co->MissionIndex = 0;
	GrafxMakeBackground(
		device, &buffer, co, mo, map, tint, 0, 1, Vec2iZero(), NULL);
	DrawBufferTerminate(&buffer);
	ActorsTerminate();
	ObjsTerminate();
	MobObjsTerminate();
	RemoveAllWatches();
	MissionOptionsTerminate(mo);
	CampaignSettingTerminate(&co->Setting);
	co->seed = gConfig.Game.RandomSeed;
}

void GrafxDrawBackground(
	GraphicsDevice *g, DrawBuffer *buffer,
	HSV tint, Vec2i pos, GrafxDrawExtra *extra)
{
	Vec2i v;

	DrawBufferSetFromMap(buffer, &gMap, pos, X_TILES);
	DrawBufferDraw(buffer, Vec2iZero(), extra);

	for (v.y = 0; v.y < g->cachedConfig.Res.y; v.y++)
	{
		for (v.x = 0; v.x < g->cachedConfig.Res.x; v.x++)
		{
			DrawPointTint(g, v, tint);
		}
	}
	memcpy(g->bkg, g->buf, GraphicsGetMemSize(&g->cachedConfig));
	memset(g->buf, 0, GraphicsGetMemSize(&g->cachedConfig));
}

void GrafxMakeBackground(
	GraphicsDevice *device, DrawBuffer *buffer,
	CampaignOptions *co, struct MissionOptions *mo, Map *map, HSV tint,
	int isEditor, int buildTables, Vec2i pos, GrafxDrawExtra *extra)
{
	CampaignAndMissionSetup(buildTables, co, mo);
	MapLoad(map, mo, co, &co->Setting.characters);
	InitializeBadGuys();
	CreateEnemies();
	MapMarkAllAsVisited(map);
	if (isEditor)
	{
		MapShowExitArea(map);
	}
	else
	{
		pos = Vec2iCenterOfTile(Vec2iScaleDiv(map->Size, 2));
	}

	GrafxDrawBackground(device, buffer, tint, pos, extra);
}

void GraphicsBlitBkg(GraphicsDevice *device)
{
	memcpy(device->buf, device->bkg, GraphicsGetMemSize(&device->cachedConfig));
}