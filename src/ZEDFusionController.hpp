#ifndef __ZED_MC_CONTROLLER_H__
#define __ZED_MC_CONTROLLER_H__


#include <sl/Camera.hpp>
#include <sl/Fusion.hpp>
#ifdef _WIN32
#include <Windows.h>
#endif
#include <cuda.h>
#include "cuda_runtime.h"
#include "sl/c_api/types_c.h"

static std::mutex mglobalmutex;

class ZEDFusionController {
public:
    ZEDFusionController();
    ~ZEDFusionController();

    static ZEDFusionController* get() {
        if (!instance) // Only allow one instance of class to be generated.
            instance = new ZEDFusionController();
        return instance;
    }

    static void destroyInstance() {
        if (!instance) // Only allow one instance of class to be generated.
            delete instance;
        instance = nullptr;

    }

    static bool isNotCreated() {
        return (instance == nullptr);
    }


	void close();

    SL_FUSION_ERROR_CODE process();

    SL_FUSION_ERROR_CODE subscribe(struct SL_CameraIdentifier* uuid, struct SL_CommunicationParameters* params, struct SL_Vector3* pose_translation, struct SL_Quaternion* pose_rotation);

    SL_FUSION_ERROR_CODE updatePose(struct SL_CameraIdentifier* uuid, struct SL_Vector3* pose_translation, struct SL_Quaternion* pose_rotation);
    
    SL_SENDER_ERROR_CODE getSenderState(SL_CameraIdentifier* uuid);

    void readFusionConfigFile(char json_config_filename[256], SL_COORDINATE_SYSTEM coord_system, SL_UNIT unit, SL_FusionConfiguration* configs,int& nb_cameras);

    /////////////////////////////////////////////////////////////////////
    ///////////////////// Object Detection Fusion ///////////////////////
    /////////////////////////////////////////////////////////////////////

    ///
    /// \brief Init multicam parameters
    /// \param [in] init parameters
    /// \return SL_FUSION_ERROR_CODE
    ///
    SL_FUSION_ERROR_CODE init(struct SL_InitFusionParameters* init_parameters);

    ///
    /// \brief enables body tracking fusion module
    /// \param [in] parameters defined by \ref SL_BodyTrackingFusionParameters
    /// \return SL_FUSION_ERROR_CODE
    ///
    SL_FUSION_ERROR_CODE enableBodyTracking(struct SL_BodyTrackingFusionParameters* params);

    /**
     * \brief get the stats of a given camera in the Fusion API side
     * It can be the received FPS, drop frame, latency, etc
     * \param metrics : structure containing all the metrics available
     * \return SL_FUSION_ERROR_CODE
     */
    SL_FUSION_ERROR_CODE getProcessMetrics(SL_FusionMetrics* metrics);

    ///
    /// \brief disable Object detection fusion module
    /// \return
    ///
	void disableBodyTracking();

    ///
    /// \brief retrieves a list of bodies (in SL_Bodies class type) seen by all cameras and merged as if it was seen by a single super-camera.
    /// \note Internal calls retrieveObjects() for all listed cameras, then merged into a single SL_Bodies
    /// \param [out] bodies: list of objects seen by all available cameras
    /// \note Only the 3d informations is available in the returned object.
    ///
    SL_FUSION_ERROR_CODE retrieveBodies(struct SL_Bodies* bodies, struct SL_BodyTrackingFusionRuntimeParameters* rt, struct SL_CameraIdentifier uuid);

    /////////////////////////////////////////////////////////////////////
    ///////////////////// Positional Tracking Fusion ///////////////////////
    /////////////////////////////////////////////////////////////////////


    /**
    * \brief enable positional tracking fusion.
    * \note note that for the alpha version of the API, the positional tracking fusion doesn't support the area memory feature
    *
    * \return ERROR_CODE
    */
    SL_FUSION_ERROR_CODE enablePositionalTracking(SL_PositionalTrackingFusionParameters* params);

    /**
     * \brief Get the Fused Position of the camera system
     *
     * \param camera_pose will contain the camera pose in world position (world position is given by the calibration of the cameras system)
     * \param reference_frame defines the reference from which you want the pose to be expressed. Default : \ref REFERENCE_FRAME "REFERENCE_FRAME::WORLD".
     * \param uuid Camera identifier
     * \return POSITIONAL_TRACKING_STATE is the current state of the tracking process
     */
    SL_POSITIONAL_TRACKING_STATE getPosition(SL_PoseData* pose, enum SL_REFERENCE_FRAME reference_frame, struct SL_CameraIdentifier* uuid, SL_POSITION_TYPE retrieve_type);

    ///
    /// \brief disable Positional Tracking fusion module
    /// \return
    ///
    void disablePositionalTracking();


    void ingestGNSSData(SL_GNSSData* data, bool radian);

    SL_POSITIONAL_TRACKING_STATE getCurrentGNSSData(SL_GNSSData* data, bool radian);

    SL_POSITIONAL_TRACKING_STATE getGeoPose(SL_GeoPose* pose, bool radian);

    SL_POSITIONAL_TRACKING_STATE geoToCamera(SL_LatLng* in, SL_PoseData* out, bool radian);

    SL_POSITIONAL_TRACKING_STATE cameraToGeo(SL_PoseData* in, SL_GeoPose* out, bool radian);

	void destroy();

private:
	sl::BodyTrackingFusionParameters BT_fusion_init_params;
    static ZEDFusionController* instance;
public :
    sl::Fusion fusion;
};

#endif