#include <sstream>
#include <iomanip>
#include "Engine.h"
#include "DrawUtils.h"
#include "hook.h"
#include "AccuracyCounter.h"
#include "Timers.h"

void DoVariableStatusNotifier()
{
	int startVal = 380;
	auto printVar = [&](var_t varId, const char * varName)
	{
		float * color;
		startVal -= 12;
		auto v = ImportExport::Variables[varId];
		if(v->IsEnabled())
			color = Colors::Green;
		else
			color = Colors::Red;
		auto s = stringstream();
		s << varName << " " << v->Value;
		DrawUtils::FillRectangle(530.0f, startVal, 105, 12, Colors::TransBlack);
		DrawUtils::DrawStringExt(530.0f, startVal+1.0f, s.str().c_str(), color, true, false, 7, 8);
	};
	printVar(AUTOFIRE, XORSTR("Autofire"));
	printVar(AIM, XORSTR("Aim"));
}

void DoAccuracyCounter()
{
	int startVal = 392;
	auto printVar = [&](const std::pair<string,double> p)
	{
		float * color = Colors::White;
		startVal += 12;
		auto s = stringstream();
		s << p.first << " " << setprecision(2) << p.second;
		DrawUtils::FillRectangle(530.0f, startVal, 110.0, 12, Colors::TransBlack);
		DrawUtils::DrawStringExt(530.0f, startVal+1.0f, s.str().c_str(), color, true, false, 7, 8);
	};
	auto data = AccuracyManager::GetAccuracies();
	for_each(data.cbegin(), data.cend(), printVar);
}

void DrawRectangles()
{
	auto v = ImportExport::Variables[RECTS]->IntValue();
	auto self = ImportExport::Entities->GetPlayer(Engine::GetClientNum());
	idVec3 bboxBottom(0.0f, 0.0f, -24.0f);
	idVec3 bboxTop(0.0f, 0.0f, 32.0f);

	if(v)
	{
		for(int i = 0; i < MAX_CLIENTS; ++i)
		{
			auto player = ImportExport::Entities->Players()[i];

			if(i == Engine::GetClientNum() || !player->IsValid() || (!player->IsEnemyOf(self) && v == 1))
				continue;

			float screenTop[2];
			float screenBottom[2];
			if(!DrawUtils::WorldToScreen((player->Origin() + bboxTop).ToFloatPtr(), screenTop)
				|| !DrawUtils::WorldToScreen((player->Origin() + bboxBottom).ToFloatPtr(), screenBottom))
				continue;

			auto height = screenBottom[1] - screenTop[1];

			screenTop[0] -= height * (12.0/56.0);
			screenBottom[0] += height * (12.0/56.0);

			DrawUtils::DrawRectangle(screenTop[0], screenTop[1], screenBottom[0] - screenTop[0], screenBottom[1] - screenTop[1], player->IsEnemyOf(self) ? Colors::Red : Colors::Blue);
		}
	}
}

void DoESP()
{
	int esp = ImportExport::Variables[ESP]->IntValue();
	if(esp > 0)
	{
		auto espFontHSize = ImportExport::Variables[ESPFONTSIZE]->IntValue();
		auto espFontWSize = (int)(espFontHSize / 3.0 * 2.0);
		for_each(begin(ImportExport::Entities->Players ()), end(ImportExport::Entities->Players ()), [esp, espFontHSize, espFontWSize] (const IPlayer* p) {
			if(ImportExport::AimBot->IsValidEnemy(p->Id()))
			{
				float screen[2];
				if(DrawUtils::WorldToScreen(p->Origin().ToFloatPtr(), screen))
				{
					if(esp >= 2) //HEALTH ESP
					{
						float * color;
						string msg;
						auto h = healths[p->Id()];
						if(h >= 75)
						{
							msg = XORSTR(">75");
							color = Colors::White;
						}
						else if(h >= 50)
						{
							msg = XORSTR(">50");
							color = Colors::Green;
						}
						else if(h >= 25)
						{
							msg = XORSTR(">25");
							color = Colors::Yellow;
						}
						else
						{
							msg = XORSTR("<25");
							color = Colors::Red;
						}
						DrawUtils::DrawStringExt(screen[0], screen[1]-4-espFontHSize, msg.c_str(), color, true, true, espFontWSize, espFontHSize);
					}
					if(esp == 1 || esp == 3) //NAMES ESP
						DrawUtils::DrawStringExt(screen[0], screen[1]-3, p->Name().c_str(), Colors::White, true, true, espFontWSize-1, espFontHSize-1);
				}
			}
		});
	}
}

void DoRadar()
{
#define RADAR_SIZE 101
#define RADAR_HALF (RADAR_SIZE / 2)
#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480
	if(!ImportExport::Variables[RADAR]->IsEnabled())
		return;
	float radarcolor[4] = { 0.66f, 0.66f, 0.66f, 0.25f };
	float darkgrey[4] = { 0.33f, 0.33f, 0.33f, 1.0f };
	float colorblue[4] = { 0.0f, 0.0f, 1.0f, 1.0f };
	float colorred[4] = { 1.0f, 0.0f, 0.0f, 1.0f };
	float startY = ImportExport::Variables[RADAR_Y]->FloatValue();
	DrawUtils::FillRectangle(SCREEN_WIDTH-RADAR_SIZE, startY, RADAR_SIZE, RADAR_SIZE, radarcolor);
	DrawUtils::FillRectangle(SCREEN_WIDTH-RADAR_HALF-0.5f, startY, 1.0f, RADAR_SIZE, darkgrey);
	DrawUtils::FillRectangle(SCREEN_WIDTH-RADAR_SIZE, startY+RADAR_HALF-0.5f, RADAR_SIZE, 1.0f, darkgrey);
	std::vector<IPlayer*> players = ImportExport::Entities->Players();
	IPlayer * me = ImportExport::Entities->GetPlayer(Engine::GetClientNum ());
	float myYaw = me->ViewAngles().yaw;
	for_each(begin(players), end(players), [&](IPlayer* pl) {
		if(pl->Id() != (unsigned)Engine::GetClientNum () && pl->IsValid())
		{
			idVec3 diff = me->EyePosition() - pl->Origin();
			diff.z = 0;
			idAngles ang = diff.ToAngles();
			float normDist = diff.Length()/40.0f;
			float idealYaw = idMath::AngleNormalize180(ang.yaw /*- 90.0f*/ - myYaw);
			float x = sin(DEG2RAD(idealYaw))*min(normDist, RADAR_HALF) + (SCREEN_WIDTH - RADAR_HALF) - 1;
			float y = cos(DEG2RAD(idealYaw))*min(normDist, RADAR_HALF) + RADAR_HALF - 1;
			DrawUtils::FillRectangle(x, y+startY, 2, 2, me->IsEnemyOf(pl) ? colorred : colorblue);
		}
	});
}

void DoSpecList()
{
#define ITEM_HEIGHT 20
	if(!ImportExport::Variables[SPECLIST]->IsEnabled())
		return;
	float specListBg[4] = { 0.3f, 0.3f, 0.3f, 0.3f };
	float lastY = 220.0f;
	for(int i = 0; i < MAX_CLIENTS; ++i)
	{
		auto player = ImportExport::Entities->Players()[i];
		if(player->IsSpectating())
		{
			DrawUtils::FillRectangle(540.0, lastY, 100.0f, 12.0f, specListBg);
			DrawUtils::DrawStringExt(545.0f, lastY+2.0f, player->Name().c_str(), Colors::White, false, false, 7, 8, 10);
			lastY += 12.0f;
		}
	}
	if(lastY > 220.0f)
	{
		DrawUtils::FillRectangle(540.0f, 200.0f, 100.0, 20, specListBg);
		DrawUtils::DrawStringExt(545.0f, 205.0f, XORSTR("SpecList"), Colors::White, false, false, 10, 10, 10);
	}
}

void DrawTimers()
{
#define ITEM_HEIGHT 20
#define BOX_START 140.0f
#define FIRST_ITEM (BOX_START + ITEM_HEIGHT)

	if(!ImportExport::Variables[TIMERS]->IsEnabled())
		return;
	float timersBg[4] = { 0.3f, 0.3f, 0.3f, 0.3f };
	float lastY = FIRST_ITEM;

	for_each(begin(timers.GetTimers ()), end(timers.GetTimers ()), [&] (const itemTimer& timer) {
		string msg;
		if(timer.nextSpawn < Engine::GetTime()) {
			auto s = stringstream();
			s << timer.name << ": " << XORSTR("Available");
			msg = s.str();
		} else { 
			auto s = stringstream();
			s << timer.name << ": " << setprecision(2) << ((timer.nextSpawn - Engine::GetTime()) / 1000.0f);
			msg = s.str();
		}
		DrawUtils::FillRectangle(530.0f, lastY, 140.0f, 12.0f, timersBg);
		DrawUtils::DrawStringExt(530.0f, lastY+2.0f, msg.c_str (), Colors::White, false, false, 7, 8, msg.length ());
		lastY += 12.0f;
	});
	if(lastY > FIRST_ITEM)
	{
		DrawUtils::FillRectangle(530.0f, BOX_START, 140, 20, timersBg);
		DrawUtils::DrawStringExt(535.0f, BOX_START + 5.0f, XORSTR("Timers"), Colors::White, false, false, 7, 10, 6);
	}
}