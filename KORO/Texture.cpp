#include "Texture.hpp"

Texture::Texture()
{
    Texture::instance.reset(this);
}

Texture::~Texture()
{
    // 全テクスチャの解放
    for (auto& createdTexture : this->createdTextures)
    {
        SAFE_RELEASE(createdTexture.second);
    }
}

IDirect3DTexture9* Texture::GetTexture(string path)
{
    // 既に生成に失敗していたら無視
    if (this->IsFailed(path)) return nullptr;

    // テクスチャ生成済みの場合は既存のものを返す
    if (this->createdTextures.contains(path))
    {
        return this->createdTextures.at(path);
    }

    // 新規生成して返す
    return this->CreateTexture(path);
}

IDirect3DTexture9* Texture::CreateTexture(string path)
{
    IDirect3DTexture9* tempTexture;

    if (SUCCEEDED(D3DXCreateTextureFromFile(
            DX9::instance->device3d,
            path.c_str(),
            &tempTexture
        )))
    {
        // 生成済みに登録
        this->createdTextures.emplace(path, tempTexture);

        return tempTexture;
    }

    // 生成失敗配列に登録
    this->failedPaths.push_back(path);

    return nullptr;
}

bool Texture::IsFailed(string path) const
{
    for (auto& failedPath : this->failedPaths)
    {
        if (failedPath == path) return true;
    }
    return false;
}
