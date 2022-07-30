#include "ActorManager.hpp"

#include "Game.hpp"
#include "Cube.hpp"
#include "XModel.hpp"
#include "TriggerBox.hpp"
#include "StartPoint.hpp"
#include "GoalPoint.hpp"

void ActorManager::Setup()
{
	if (!ActorManager::patternLoader)
	{
		ActorManager::patternLoader.reset(new PatternDataLoader<KoroActorPatternDataByte>("dataDev", "data/contents.KORO"));

		// ファイル数分データを読み込み
		for (auto i = 0; i < ActorManager::patternLoader->fileCount; i++)
		{
		    ActorManager::patternLoader->BeginLoad(i);
		    ActorManager::patternLoader->LoadStringValue("id",       ActorManager::patternLoader->out.id, CHAR_SIZE_ID);
		    ActorManager::patternLoader->LoadEnumValue("type",      &ActorManager::patternLoader->out.actorType);
		    ActorManager::patternLoader->LoadEnumValue("cube-type", &ActorManager::patternLoader->out.cubeInfo.type);
		    ActorManager::patternLoader->LoadColorValue("color",    &ActorManager::patternLoader->out.color);
		    ActorManager::patternLoader->LoadStringValue("tex0",     ActorManager::patternLoader->out.cubeInfo.texPaths[0], CHAR_SIZE_DEFAULT);
		    ActorManager::patternLoader->LoadStringValue("tex1",     ActorManager::patternLoader->out.cubeInfo.texPaths[1], CHAR_SIZE_DEFAULT);
		    ActorManager::patternLoader->LoadStringValue("tex2",     ActorManager::patternLoader->out.cubeInfo.texPaths[2], CHAR_SIZE_DEFAULT);
		    ActorManager::patternLoader->LoadStringValue("tex3",     ActorManager::patternLoader->out.cubeInfo.texPaths[3], CHAR_SIZE_DEFAULT);
		    ActorManager::patternLoader->LoadStringValue("tex4",     ActorManager::patternLoader->out.cubeInfo.texPaths[4], CHAR_SIZE_DEFAULT);
		    ActorManager::patternLoader->LoadStringValue("tex5",     ActorManager::patternLoader->out.cubeInfo.texPaths[5], CHAR_SIZE_DEFAULT);
		    ActorManager::patternLoader->LoadStringValue("x-path",   ActorManager::patternLoader->out.xModelInfo.x_path,    CHAR_SIZE_DEFAULT);
		    ActorManager::patternLoader->LoadStringValue("tex0",     ActorManager::patternLoader->out.xModelInfo.tex_path,  CHAR_SIZE_DEFAULT);
		    ActorManager::patternLoader->EndLoad();
		}
	}
}

Actor* ActorManager::Generate(KoroActorType type, const D3DXMATRIX* worldMatrix, Transform* parent, const char* id)
{
	Actor* newActor = nullptr;

	switch (type)
	{
	case KoroActorType::CUBE:
		newActor = new Cube(worldMatrix, parent, id);
		break;

	case KoroActorType::X_MODEL:
		newActor = new XModel(worldMatrix, parent, id);
		break;

	case KoroActorType::TRIGGER_BOX:
		newActor = new TriggerBox(worldMatrix, parent);
		break;

	case KoroActorType::START_POINT:
		newActor = StartPoint::Instantiate(worldMatrix, parent);
		break;

	case KoroActorType::GOAL_POINT:
		newActor = GoalPoint::Instantiate(worldMatrix, parent);
		break;

	//case GenerateType::HOGE:
	//	newActor = new Hoge(worldMatrix);
	//	break;

	default:
		break;
	}

	return newActor;
}
