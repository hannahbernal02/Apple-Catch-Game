// OpenGL Mathematics Copyright (c) 2005 - 2011 G-Truc Creation (www.g-truc.net)
// Created : 2005-12-21
// Updated : 2009-04-29
// Licence : This source is under MIT License
// File    : glm/gtx/transform.hpp
// Dependency:
// - GLM core
// - GLM_GTC_matric_transform

#ifndef glm_gtx_transform
#define glm_gtx_transform

// Dependency:
#include "../glm.hpp"
#include "../gtc/matrix_transform.hpp"

#if(defined(GLM_MESSAGES) && !defined(glm_ext))
#       pragma message("GLM: GLM_GTX_transform extension included")
#endif

namespace glm {
	 namespace gtx {
		 namespace transform
		 {
			 using namespace gtc::matrix_transform;
			
			 template <typename T>
			 detail::tmat4x4<T> translate(T x, T y, T z);

			 template <typename T>
			 detail::tmat4x4<T> translate(
				 detail::tmat4x4<T> const & m,
				 T x, T y, T z);

			 template <typename T>
			 detail::tmat4x4<T> translate(detail::tvec3<T> const & v);

			 template <typename T>
			 detail::tmat4x4<T> rotate(
				 T angle,
				 T x, T y, T z);
			 template <typename T>
			 detail::tmat4x4<T> rotate(
				 T angle,
				 detail::tvec3<T> const & v);

			 template <typename T>
			 detail::tmat4x4<T> rotate(
				 detail::tmat4x4<T> const & m,
				 T angle,
				 T x, T y, T z);
		
			 template <typename T>detail::tmat4x4<T> scale(
				 T x, T y, T z);

			 template <typename T>detail::tmat4x4<T> scale(
				 detail::tmat4x4<T> const & m,
				 T x, T y, T z);

			 template <typename T>detail::tmat4x4<T> scale(
				 detail::tvec3<T> const & v);
		 
		 }//namespace transform

	}//namespace gtx

}//namespace glm

#include "transform.inl"

namespace glm { using namespace gtx::transform; }

#endif//glm_gtx_transform
