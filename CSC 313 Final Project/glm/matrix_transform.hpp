// OpenGL Mathematics Copyright (c) 2005 - 2011 G-Truc Creation (www.g-truc.net)
// Created : 2009-04-29
// Updated : 2009-04-29
// Licence : This source is under MIT License
// File    : glm/gtc/matrix_transform.hpp
// Dependency:
// - GLM core
// - GLM_GTC_matrix_operation

#ifndef glm_gtc_matrix_transform
#define glm_gtc_matrix_transform

// Dependency:
#include "../glm.hpp"

#if(defined(GLM_MESSAGES) && !defined(glm_ext))
#       pragma message("GLM: GLM_GTC_matrix_transform extension included")
#endif

namespace glm {
	namespace gtc {
		namespace matrix_transform
		{
		
			template <typename T> detail::tmat4x4<T> translate(
				detail::tmat4x4<T> const & m,
				detail::tvec3<T> const & v);
			template <typename T> detail::tmat4x4<T> rotate(
				detail::tmat4x4<T> const & m,
				T const & angle,
				detail::tvec3<T> const & v);
			template <typename T> detail::tmat4x4<T> scale(
				detail::tmat4x4<T> const & m,
				detail::tvec3<T> const & v);
			template <typename T>detail::tmat4x4<T> ortho(
				T const & left,
				T const & right,
				T const & bottom,
				T const & top,
				T const & zNear,
				T const & zFar);
			
			template <typename T> detail::tmat4x4<T> ortho(
				T const & left,
				T const & right,
				T const & bottom,
				T const & top);
			
			template <typename T> detail::tmat4x4<T> frustum(
				T const & left,
				T const & right,
				T const & bottom,
				T const & top,
				T const & nearVal,
				T const & farVal);
			
			template <typename T> detail::tmat4x4<T> perspective(
				T const & fovy,
				T const & aspect,
				T const & zNear,
				T const & zFar);

			template <typename valType> detail::tmat4x4<valType> perspectiveFov(
				valType const & fov,
				valType const & width,
				valType const & height,
				valType const & zNear,
				valType const & zFar);

			template <typename T> detail::tmat4x4<T> infinitePerspective(
				T fovy, T aspect, T zNear);
			
			template <typename T> detail::tmat4x4<T> tweakedInfinitePerspective(
				T fovy, T aspect, T zNear);
			
			template <typename T, typename U> detail::tvec3<T> project(
				detail::tvec3<T> const & obj,
				detail::tmat4x4<T> const & model,
				detail::tmat4x4<T> const & proj,
				detail::tvec4<U> const & viewport);
			
			template <typename T, typename U> detail::tvec3<T> unProject(
				detail::tvec3<T> const & win,
				detail::tmat4x4<T> const & model,
				detail::tmat4x4<T> const & proj,
				detail::tvec4<U> const & viewport);
			
			template <typename T, typename U> detail::tmat4x4<T> pickMatrix(
				detail::tvec2<T> const & center,
				detail::tvec2<T> const & delta,
				detail::tvec4<U> const & viewport);
			
			template <typename T> detail::tmat4x4<T> lookAt(
				detail::tvec3<T> const & eye,
				detail::tvec3<T> const & center,
				detail::tvec3<T> const & up);
			
		}//namespace matrix_transform
		
	}//namespace gtc
	
}//namespace glm

#include "matrix_transform.inl"

namespace glm { using namespace gtc::matrix_transform; }

#endif//glm_gtc_matrix_transform
