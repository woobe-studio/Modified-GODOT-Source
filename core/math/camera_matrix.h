/**************************************************************************/
/*  camera_matrix.h                                                       */
/**************************************************************************/


#ifndef CAMERA_MATRIX_H
#define CAMERA_MATRIX_H

#include "core/math/rect2.h"
#include "core/math/transform.h"

struct CameraMatrix {
	enum Planes {
		PLANE_NEAR,
		PLANE_FAR,
		PLANE_LEFT,
		PLANE_TOP,
		PLANE_RIGHT,
		PLANE_BOTTOM
	};

	real_t matrix[4][4];

	void set_identity();
	void set_zero();
	void set_light_bias();
	void set_light_atlas_rect(const Rect2 &p_rect);
	void set_perspective(real_t p_fovy_degrees, real_t p_aspect, real_t p_z_near, real_t p_z_far, bool p_flip_fov = false);
	void set_perspective(real_t p_fovy_degrees, real_t p_aspect, real_t p_z_near, real_t p_z_far, bool p_flip_fov, int p_eye, real_t p_intraocular_dist, real_t p_convergence_dist);
	void set_orthogonal(real_t p_left, real_t p_right, real_t p_bottom, real_t p_top, real_t p_znear, real_t p_zfar);
	void set_orthogonal(real_t p_size, real_t p_aspect, real_t p_znear, real_t p_zfar, bool p_flip_fov = false);
	void set_frustum(real_t p_left, real_t p_right, real_t p_bottom, real_t p_top, real_t p_near, real_t p_far);
	void set_frustum(real_t p_size, real_t p_aspect, Vector2 p_offset, real_t p_near, real_t p_far, bool p_flip_fov = false);

	static real_t get_fovy(real_t p_fovx, real_t p_aspect) {
		return Math::rad2deg(Math::atan(p_aspect * Math::tan(Math::deg2rad(p_fovx) * 0.5)) * 2.0);
	}

	real_t get_z_far() const;
	real_t get_z_near() const;
	real_t get_aspect() const;
	real_t get_fov() const;
	bool is_orthogonal() const;

	Vector<Plane> get_projection_planes(const Transform &p_transform) const;

	bool get_endpoints(const Transform &p_transform, Vector3 *p_8points) const;
	Vector2 get_viewport_half_extents() const;

	void invert();
	CameraMatrix inverse() const;

	CameraMatrix operator*(const CameraMatrix &p_matrix) const;

	Plane xform4(const Plane &p_vec4) const;
	_FORCE_INLINE_ Vector3 xform(const Vector3 &p_vec3) const;

	operator String() const;

	void make_scale(const Vector3 &p_scale);
	int get_pixels_per_meter(int p_for_pixel_width) const;
	operator Transform() const;

	CameraMatrix();
	CameraMatrix(const Transform &p_transform);
	~CameraMatrix();
};

Vector3 CameraMatrix::xform(const Vector3 &p_vec3) const {
	Vector3 ret;
	ret.x = matrix[0][0] * p_vec3.x + matrix[1][0] * p_vec3.y + matrix[2][0] * p_vec3.z + matrix[3][0];
	ret.y = matrix[0][1] * p_vec3.x + matrix[1][1] * p_vec3.y + matrix[2][1] * p_vec3.z + matrix[3][1];
	ret.z = matrix[0][2] * p_vec3.x + matrix[1][2] * p_vec3.y + matrix[2][2] * p_vec3.z + matrix[3][2];
	real_t w = matrix[0][3] * p_vec3.x + matrix[1][3] * p_vec3.y + matrix[2][3] * p_vec3.z + matrix[3][3];
	return ret / w;
}

#endif // CAMERA_MATRIX_H
