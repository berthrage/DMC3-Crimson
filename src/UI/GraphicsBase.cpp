#pragma once

#include "GraphicsBase.hpp"

#include <format>
#include <comdef.h>
#include <d3dcompiler.h>

#include "..\ThirdParty\stb\stb_image.h"

#define GLM_ENABLE_EXPERIMENTAL
#include "..\ThirdParty\glm\gtc\matrix_transform.hpp"
#include "..\ThirdParty\glm\gtx\euler_angles.hpp"

#pragma comment(lib, "d3dcompiler")

namespace Graphics
{
    using Microsoft::WRL::ComPtr;

    bool ErrorHandled::ErrorCheck(HRESULT hr, std::string file, std::string function, size_t line)
    {
        if (SUCCEEDED(hr))
            return true;

        SetError(hr, std::format("File: {:s} | Function: {:s}() | Line: {:d}", file, function, line));

        return false;
    }

    void ErrorHandled::SetError(HRESULT hr, std::string position)
    {
        LPVOID lpMsgBuf;
        DWORD dwFormatFlags = FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS;

        DWORD dwMessageSize = FormatMessageA(
            dwFormatFlags,
            NULL,
            hr,
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            (LPSTR)&lpMsgBuf,
            0,
            NULL
        );

        std::string errorMessage;
        if (dwMessageSize) {
            errorMessage = static_cast<char*>(lpMsgBuf);
            LocalFree(lpMsgBuf);
        }
        else {
            errorMessage = "Unknown error code";
        }

        m_ErrorCode = hr;
        m_ErrorStr = std::format("<{:s}> : {:s}", position, errorMessage);
    }

    Drawable::Drawable(const std::vector<Vertex_t>& vertices, const std::vector<uint32_t>& indices)
        : m_Vertices(vertices), m_Indices(indices)
    {}

    Drawable::~Drawable()
    {}

    bool Drawable::AllocateGPUVertexBuffer(ID3D11Device* pD3D11Device, ID3D11Buffer** ppVertexBuffer,
        const void* pShaderBytecodeWithInputSignature, size_t bytecodeLength,
        D3D11_USAGE usage, UINT cpuAccessFlags)
    {
        HRESULT hr;

        D3D11_BUFFER_DESC bufferDesc = {};
        bufferDesc.Usage = usage;
        bufferDesc.ByteWidth = sizeof(m_Vertices.size() * sizeof(decltype(m_Vertices)::value_type));
        bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        bufferDesc.CPUAccessFlags = cpuAccessFlags;

        D3D11_SUBRESOURCE_DATA initData = {};
        initData.pSysMem = m_Vertices.data();

        hr = pD3D11Device->CreateBuffer(&bufferDesc, &initData, ppVertexBuffer);

        return ErrorCheck(hr, __FILE__, __FUNCTION__, __LINE__);
    }

    bool Drawable::AllocateGPUIndexBuffer(ID3D11Device* pD3D11Device, ID3D11Buffer** ppIndexBuffer,
        const void* pShaderBytecodeWithInputSignature, size_t bytecodeLengt,
        D3D11_USAGE usage, UINT cpuAccessFlags)
    {
        HRESULT hr;

        D3D11_BUFFER_DESC bufferDesc = {};
        bufferDesc.Usage = usage;
        bufferDesc.ByteWidth = sizeof(m_Indices.size() * sizeof(decltype(m_Indices)::value_type));
        bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
        bufferDesc.CPUAccessFlags = cpuAccessFlags;

        D3D11_SUBRESOURCE_DATA initData = {};
        initData.pSysMem = m_Indices.data();

        hr = pD3D11Device->CreateBuffer(&bufferDesc, &initData, ppIndexBuffer);

        return ErrorCheck(hr, __FILE__, __FUNCTION__, __LINE__);
    }

    bool Drawable::CreateGPUInputLayout(ID3D11Device* pD3D11Device, void* pShaderBytecodeWithInputSignature,
        size_t bytecodeLength, ID3D11InputLayout** ppInputLayout)
    {
        HRESULT hr = pD3D11Device->CreateInputLayout(s_VertexInputLayout,
            sizeof(s_VertexInputLayout) / sizeof(D3D11_INPUT_ELEMENT_DESC),
            pShaderBytecodeWithInputSignature, bytecodeLength, ppInputLayout);

        return ErrorCheck(hr, __FILE__, __FUNCTION__, __LINE__);
    }

    void Drawable::Move(glm::vec3 vector)
    {
        m_TranslationVector = vector;

        UpdateTransformationMatrix();
    }

    void Drawable::Scale(glm::vec3 vector)
    {
        m_ScaleVector = vector;

        UpdateTransformationMatrix();
    }

    void Drawable::Scale(float factor)
    {
        m_ScaleVector = { factor, factor, factor };

        UpdateTransformationMatrix();
    }

    void Drawable::Rotate(float angle, glm::vec3 vector)
    {
        m_RotationAngle = angle;
        m_RotationAxisVector = vector;

        UpdateTransformationMatrix();
    }

    void Drawable::UpdateTransformationMatrix()
    {
        m_TransformationMatrix = glm::mat4(1.0f);

        m_TransformationMatrix = glm::translate(m_TransformationMatrix, m_TranslationVector);
        m_TransformationMatrix = glm::rotate(m_TransformationMatrix, m_RotationAngle, m_RotationAxisVector);
        m_TransformationMatrix = glm::scale(m_TransformationMatrix, m_ScaleVector);
    }
}
