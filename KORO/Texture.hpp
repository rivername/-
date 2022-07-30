#include <map>

#include "DX9.hpp"
#include "Debug.hpp"

#pragma once

class Texture
{
public:
    static inline SmartPointer(Texture) instance;

    Texture();
    ~Texture();

public:
    // ファイルパスからテクスチャを生成し、返す
    // 生成済みの場合返すのみ
    IDirect3DTexture9* GetTexture(string path);

private:
    IDirect3DTexture9* CreateTexture(string path);

    bool IsFailed(string path) const;

    std::map<string, IDirect3DTexture9*> createdTextures;

    std::vector<string> failedPaths;
};
