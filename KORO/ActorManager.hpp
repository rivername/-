#pragma once

#include <d3dx9.h>

#include "PatternDataLoader.hpp"
#include "KoroActorPatternDataParam.hpp"

class Actor;

class ActorManager final
{
public:
	ActorManager() = delete;
	~ActorManager() = delete;

public:

	static void Setup();

	// Actorを生成し、それを返す
	// 生成に失敗したら nullptr を返す
	static Actor* Generate
    (
              KoroActorType type,
        const D3DXMATRIX*   worldMatrix = nullptr,
              Transform*    parent      = nullptr,
        const char*         id          = nullptr
    );

public:
	static inline std::unique_ptr<PatternDataLoader<KoroActorPatternDataByte>> patternLoader = nullptr;
};