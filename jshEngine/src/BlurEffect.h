#pragma once

#include "Graphics.h"

namespace jsh {

	class BlurEffect {

		uvec2 m_Resolution;

		Viewport m_Viewport;
		RenderTargetView  m_AuxRTV;
		Buffer m_BlurBuffer;
		Buffer m_CoefficientsBuffer;
		SamplerState m_SamplerState;
		BlendState m_BlendStateAlpha;

		uint32 m_Radius = 4;
		float m_Sigma = 10.f;
		uint8 m_BlurMode = 0;
		/*
			Blur mode 0 = Gaussian Blur
			Blur mode 1 = Alpha Gaussian Blur
			Blur mode 2 = Box Blur
			Blur mode 3 = Alpha Box Blur
			Blur mode 4 = Solid blur
		*/

		struct alignas(16) BlurData {
			float offset;
			int32 count;
			BOOL horizontal;

		private:
			float aux;
		};
		struct alignas(16) CoefficientsData {
			XMFLOAT4 coefficients[16];
		};

	public:
		BlurEffect();

		void Create(uvec2 resolution);
		void SetResolution(uint32 width, uint32 height);
		void Render(RenderTargetView input, RenderTargetView output, Viewport viewport, DepthStencilState* dss, TextureRes* dsv, uint32 stencilRef, CommandList cmd);

		inline void SetSolidMode(uint32 radius)
		{
			m_BlurMode = 4;
			m_Radius = radius;
		}
		inline void SetGaussianMode(uint32 radius, float sigma) noexcept 
		{
			m_BlurMode = 0;
			m_Radius = radius;
			m_Sigma = sigma;
		}
		inline void SetAlphaGaussianMode(uint32 radius, float sigma) noexcept
		{
			m_BlurMode = 1;
			m_Radius = radius;
			m_Sigma = sigma;
		}
		inline void SetBoxMode(uint32 radius) noexcept 
		{
			m_BlurMode = 2;
			m_Radius = radius;
		}
		inline void SetAlphaBoxMode(uint32 radius) noexcept 
		{
			m_BlurMode = 3;
			m_Radius = radius;
		}

	private:
		void LoadCoefficientsBoxMode(CoefficientsData& c) const noexcept;
		void LoadCoefficientsGaussianMode(CoefficientsData& c) const noexcept;
		void LoadCoefficientsSolidMode(CoefficientsData& c) const noexcept;
	};

}