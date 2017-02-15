// This is the main DLL file.

#include "stdafx.h";
#include "ceresdotnet.h";

namespace ceresdotnet{
	/*
	// Returns a pointer to the camera corresponding to a image.
	EuclideanCamera *CameraForImage(vector<EuclideanCamera> *all_cameras,
		const int image) {
		if (image < 0 || image >= all_cameras->size()) {
			return NULL;
		}
		EuclideanCamera *camera = &(*all_cameras)[image];
		if (camera->image == -1) {
			return NULL;
		}
		return camera;
	}

	const EuclideanCamera *CameraForImage(
		const vector<EuclideanCamera> &all_cameras,
		const int image) {
		if (image < 0 || image >= all_cameras.size()) {
			return NULL;
		}
		const EuclideanCamera *camera = &all_cameras[image];
		if (camera->image == -1) {
			return NULL;
		}
		return camera;
	}
	// Returns maximal image number at which marker exists.
	int MaxImage(const vector<Marker> &all_markers) {
		if (all_markers.size() == 0) {
			return -1;
		}

		int max_image = all_markers[0].image;
		for (int i = 1; i < all_markers.size(); i++) {
			max_image = std::max(max_image, all_markers[i].image);
		}
		return max_image;
	}

	// Returns a pointer to the point corresponding to a track.
	EuclideanPoint *PointForTrack(vector<EuclideanPoint> *all_points, int track) {
		for (size_t i = 0; i < all_points->size(); i++)
		{
			if ((*all_points)[i].track == track){
				return &(*all_points)[i];
			}
		}
		return NULL;
	}



	// Print a message to the log which camera intrinsics are gonna to be optimized.
	void BundleIntrinsicsLogMessage(const int bundle_intrinsics) {
		if (bundle_intrinsics == BUNDLE_NO_INTRINSICS) {
			LOG(INFO) << "Bundling only camera positions.";
		}
		else {
			std::string bundling_message = "";
#define APPEND_BUNDLING_INTRINSICS(name, flag) \
    if (bundle_intrinsics & flag) { \
      if (!bundling_message.empty()) { \
        bundling_message += ", "; \
	  	  	  	        } \
      bundling_message += name; \
				    } (void)0
			APPEND_BUNDLING_INTRINSICS("f", BUNDLE_FOCAL_LENGTH);
			APPEND_BUNDLING_INTRINSICS("px, py", BUNDLE_PRINCIPAL_POINT);
			APPEND_BUNDLING_INTRINSICS("k1", BUNDLE_RADIAL_K1);
			APPEND_BUNDLING_INTRINSICS("k2", BUNDLE_RADIAL_K2);
			APPEND_BUNDLING_INTRINSICS("p1", BUNDLE_TANGENTIAL_P1);
			APPEND_BUNDLING_INTRINSICS("p2", BUNDLE_TANGENTIAL_P2);
			LOG(INFO) << "Bundling " << bundling_message << ".";
		}
	};
	// Print a message to the log containing all the camera intriniscs values.
	void PrintCameraIntrinsics(const char *text, const double *camera_intrinsics) {

		std::ostringstream intrinsics_output;
		intrinsics_output <<
			" fx=" << camera_intrinsics[OFFSET_FOCAL_LENGTH_X] <<
			" fy=" << camera_intrinsics[OFFSET_FOCAL_LENGTH_Y];
		intrinsics_output <<
			" cx=" << camera_intrinsics[OFFSET_PRINCIPAL_POINT_X] <<
			" cy=" << camera_intrinsics[OFFSET_PRINCIPAL_POINT_Y];
#define APPEND_DISTORTION_COEFFICIENT(name, offset) \
									  { \
    if (camera_intrinsics[offset] != 0.0) { \
      intrinsics_output << " " name "=" << camera_intrinsics[offset];  \
					    } \
									  } (void)0
		APPEND_DISTORTION_COEFFICIENT("k1", OFFSET_K1);
		APPEND_DISTORTION_COEFFICIENT("k2", OFFSET_K2);
		APPEND_DISTORTION_COEFFICIENT("k3", OFFSET_K3);
		APPEND_DISTORTION_COEFFICIENT("p1", OFFSET_P1);
		APPEND_DISTORTION_COEFFICIENT("p2", OFFSET_P2);
#undef APPEND_DISTORTION_COEFFICIENT
		LOG(INFO) << text << intrinsics_output.str();
	}
	// Print a message to the log containing all the camera intriniscs values.
	void PrintCameraIntrinsics2(const char *text, const double *camera_intrinsics) {

		std::ostringstream intrinsics_output;
		intrinsics_output << "fx=" << camera_intrinsics[OFFSET_FOCAL_LENGTH_X];
		intrinsics_output << "fy=" << camera_intrinsics[OFFSET_FOCAL_LENGTH_Y];
		intrinsics_output <<
			" cx=" << camera_intrinsics[OFFSET_PRINCIPAL_POINT_X] <<
			" cy=" << camera_intrinsics[OFFSET_PRINCIPAL_POINT_Y];
#define APPEND_DISTORTION_COEFFICIENT(name, offset) \
									  { \
    if (camera_intrinsics[offset] != 0.0) { \
      intrinsics_output << " " name "=" << camera_intrinsics[offset];  \
					    } \
									  } (void)0
		APPEND_DISTORTION_COEFFICIENT("k1", OFFSET_K1);
		APPEND_DISTORTION_COEFFICIENT("k2", OFFSET_K2);
		APPEND_DISTORTION_COEFFICIENT("k3", OFFSET_K3);
		APPEND_DISTORTION_COEFFICIENT("p1", OFFSET_P1);
		APPEND_DISTORTION_COEFFICIENT("p2", OFFSET_P2);
#undef APPEND_DISTORTION_COEFFICIENT
		LOG(INFO) << text << intrinsics_output.str();
	}

	// Get a vector of camera's rotations denoted by angle axis
	// conjuncted with translations into single block
	//
	// Element with index i matches to a rotation+translation for
	// camera at image i.
	Vec6* PackCamerasRotationAndTranslation(
		const vector<Marker> &all_markers,
		const vector<EuclideanCamera> &all_cameras) {

		vector<Vec6> all_cameras_R_t;

		int max_image = MaxImage(all_markers);
		Vec6* all_cameras_R_t2 = (Vec6*)malloc(sizeof(Vec6)*(max_image + 1));
		//all_cameras_R_t.resize(max_image + 1);
		for (int i = 0; i <= max_image; i++) {
			const EuclideanCamera *camera = CameraForImage(all_cameras, i);
			if (!camera) {
				continue;
			}
			ceres::RotationMatrixToAngleAxis(&camera->R(0, 0),
				&all_cameras_R_t2[i](0));
			all_cameras_R_t2[i].tail<3>() = camera->t;
		}
		return all_cameras_R_t2;
	}

	// Convert cameras rotations fro mangle axis back to rotation matrix.
	void UnpackCamerasRotationAndTranslation(
		const vector<Marker> &all_markers,
		const vector<Vec6> &all_cameras_R_t,
		vector<EuclideanCamera> *all_cameras) {
		int max_image = MaxImage(all_markers);
		for (int i = 0; i <= max_image; i++) {
			EuclideanCamera *camera = CameraForImage(all_cameras, i);
			if (!camera) {
				continue;
			}
			ceres::AngleAxisToRotationMatrix(&all_cameras_R_t[i](0),
				&camera->R(0, 0));
			camera->t = all_cameras_R_t[i].tail<3>();
		}
	}
	*/
}