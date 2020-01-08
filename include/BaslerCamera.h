//###########################################################################
// This file is part of LImA, a Library for Image Acquisition
//
// Copyright (C) : 2009-2011
// European Synchrotron Radiation Facility
// BP 220, Grenoble 38043
// FRANCE
//
// This is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3 of the License, or
// (at your option) any later version.
//
// This software is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, see <http://www.gnu.org/licenses/>.
//###########################################################################
#ifndef BASLERCAMERA_H
#define BASLERCAMERA_H

#include <stdlib.h>
#include <limits>

#if defined (__GNUC__) && (__GNUC__ == 3) && defined (__ELF__)
#   define GENAPI_DECL __attribute__((visibility("default")))
#   define GENAPI_DECL_ABSTRACT __attribute__((visibility("default")))
#endif

#include <pylon/PylonIncludes.h>
#include <pylon/gige/BaslerGigEDeviceInfo.h>

#include "lima/HwMaxImageSizeCallback.h"
#include "lima/HwBufferMgr.h"
#include "lima/HwEventCtrlObj.h"
#include "BaslerCompatibility.h"

using namespace Pylon;
using namespace std;

#if defined( USE_1394 )
// Settings to use  Basler 1394 cameras
#include <pylon/1394/Basler1394Camera.h>
typedef Pylon::CBasler1394Camera Camera_t;
using namespace Basler_IIDC1394CameraParams;
using namespace Basler_IIDC1394StreamParams;

#elif defined ( USE_GIGE )
// settings to use Basler GigE cameras
#include <pylon/gige/BaslerGigECamera.h>
typedef Pylon::CBaslerGigECamera Camera_t;
using namespace Basler_GigECameraParams;
using namespace Basler_GigEStreamParams;
#else
#error Camera type is not specified. For example, define USE_GIGE for using GigE cameras
#endif

#define REPORT_EVENT(desc) { \
    Event *my_event = new Event(Hardware,Event::Info, Event::Camera, Event::Default,desc); \
    m_cam.getEventCtrlObj()->reportEvent(my_event);  \					
} \

namespace lima
{
namespace Basler
{
/*******************************************************************
 * \class Camera
 * \brief object controlling the basler camera via Pylon driver
 *******************************************************************/
class VideoCtrlObj;
class LIBBASLER_API Camera
{
    DEB_CLASS_NAMESPC(DebModCamera, "Camera", "Basler");
    friend class Interface;
    friend class VideoCtrlObj;
    friend class SyncCtrlObj;
 public:

     // CCA
    enum LastImageMode 
    {
        MODE_RGB24,
        MODE_RGB32,
        MODE_GRAY8,
    };

    enum Status {
      Ready, Exposure, Readout, Latency, Fault
    };

    enum LineSource {
      Off, ExposureActive, FrameTriggerWait, LineTriggerWait,
      Timer1Active, Timer2Active, Timer3Active, Timer4Active, TimerActive,
      UserOutput1, UserOutput2, UserOutput3, UserOutput4, UserOutput,
      TriggerReady, SerialTx, AcquisitionTriggerWait, ShaftEncoderModuleOut, FrequencyConverter,
      PatternGenerator1, PatternGenerator2, PatternGenerator3, PatternGenerator4,
      AcquisitionTriggerReady,
    };

    enum TrigActivation {
        RisingEdge=Basler_GigECamera::TriggerActivation_RisingEdge,
        FallingEdge=Basler_GigECamera::TriggerActivation_FallingEdge,
        AnyEdge=Basler_GigECamera::TriggerActivation_AnyEdge,
        LevelHigh=Basler_GigECamera::TriggerActivation_LevelHigh,
        LevelLow=Basler_GigECamera::TriggerActivation_LevelLow
    };

    enum TestImageSelector {
      TestImage_Off=Basler_GigECamera::TestImageSelector_Off,
      TestImage_1=Basler_GigECamera::TestImageSelector_Testimage1,
      TestImage_2=Basler_GigECamera::TestImageSelector_Testimage2,
      TestImage_3=Basler_GigECamera::TestImageSelector_Testimage3,
      TestImage_4=Basler_GigECamera::TestImageSelector_Testimage4,
      TestImage_5=Basler_GigECamera::TestImageSelector_Testimage5,
      TestImage_6=Basler_GigECamera::TestImageSelector_Testimage6,
      TestImage_7=Basler_GigECamera::TestImageSelector_Testimage7,
    };
    
    Camera(const std::string& camera_id,int packet_size = -1,int received_priority = 0);
    ~Camera();

    void prepareAcq();
    void startAcq();
    void stopAcq();
    
    // -- detector info object
    void getImageType(ImageType& type);
    void setImageType(ImageType type);

    void getDetectorType(std::string& type);
    void getDetectorModel(std::string& model);
    void getDetectorImageSize(Size& size);
    
    // -- Buffer control object
    HwBufferCtrlObj* getBufferCtrlObj();
    HwEventCtrlObj* getEventCtrlObj();
    
    //-- Synch control object
    void setTrigMode(TrigMode  mode);
    void getTrigMode(TrigMode& mode);

    void setTrigActivation(TrigActivation activation);
    void getTrigActivation(TrigActivation& activation) const;

    void setExpTime(double  exp_time);
    void getExpTime(double& exp_time);

    void setLatTime(double  lat_time);
    void getLatTime(double& lat_time);

    void getExposureTimeRange(double& min_expo, double& max_expo) const;
    void getLatTimeRange(double& min_lat, double& max_lat) const;    

    void setNbFrames(int  nb_frames);
    void getNbFrames(int& nb_frames);
    void getNbHwAcquiredFrames(int &nb_acq_frames);

    void checkRoi(const Roi& set_roi, Roi& hw_roi);
    void setRoi(const Roi& set_roi);
    void getRoi(Roi& hw_roi);    

    void checkBin(Bin&);
    void setBin(const Bin&);
    void getBin(Bin&);

    void getStatus(Camera::Status& status);
    
    // -- Transport Layer
    void setPacketSize(int isize);
    void getPacketSize(int& isize);    
    void setInterPacketDelay(int ipd);
    void getInterPacketDelay(int& ipd);
    void getMaxThroughput(int& ipd);    
    void getCurrentThroughput(int& ipd);
    void getBandwidthAssigned(int& ipd);

    void setSocketBufferSize(int sbs);
        
    void setFrameTransmissionDelay(int ftd);

    // -- basler specific, LIMA don't worry about it !
    void getFrameRate(double& frame_rate) const;
    bool isBinningAvailable() const;
    bool isRoiAvailable() const;
    void setTimeout(int TO);
    void reset();

    bool isGainAvailable() const;
    void setGain(double gain);
    void getGain(double& gain) const;

    bool isAutoGainAvailable() const;
    void setAutoGain(bool auto_gain);
    void getAutoGain(bool& auto_gain) const;

    bool isTemperatureAvailable() const;
    void getTemperature(double& temperature);    
    void isColor(bool& color_flag) const;
    void hasVideoCapability(bool& video_flag) const;

    // -- change output line source
    void setOutput1LineSource(LineSource);
    void getOutput1LineSource(LineSource&) const;

    // -- change acq frame count
    void setAcquisitionFrameCount(int AFC);
    void getAcquisitionFrameCount(int& AFC) const;

    // -- change AcquisitionFrameRateEnable
    void setAcquisitionFrameRateEnable(bool AFRE);
    void getAcquisitionFrameRateEnable(bool& AFRE) const;

    // -- change acq frame count
    void setAcquisitionFrameRateAbs(int AFRA);
    void getAcquisitionFrameRateAbs(int& AFRA) const;

    // -- Pylon buffers statistics
    void getStatisticsTotalBufferCount(long& count);    
    void getStatisticsFailedBufferCount(long& count);

    // -- Pylon test image selectors
    void setTestImageSelector(TestImageSelector set);
    void getTestImageSelector(TestImageSelector& set) const;

    // CCA
    void freeImage(unsigned char * & out_image);
    void keepLastImage(const char * in_buffer, uint32_t in_width, uint32_t in_height, VideoMode in_mode);
    void getLastImage(unsigned char * & out_image, uint32_t & out_width, uint32_t & out_height, Camera::LastImageMode & out_mode);

 private:
    enum BufferMode {TmpBuffer, SoftBuffer};
    class _AcqThread;
    friend class _AcqThread;
    void _stopAcq(bool);
    void _setStatus(Camera::Status status,bool force);
    void _freeStreamGrabber();
    void _allocTmpBuffer();
    void _initStreamGrabber(BufferMode mode);
    void _startAcq();
    void _readTrigMode();
    void _forceVideoMode(bool force);

    // CCA
    unsigned char * allocImage(size_t in_size_in_bytes);

    static const int NB_TMP_BUFFER = 2;
    
    //- lima stuff
    SoftBufferCtrlObj		m_buffer_ctrl_obj;
    HwEventCtrlObj            m_event_ctrl_obj;
    int                         m_nb_frames;    
    Camera::Status              m_status;
    volatile bool               m_wait_flag;
    volatile bool               m_quit;
    volatile bool               m_thread_running;
    int                         m_image_number;
    double                      m_exp_time;
    int                         m_timeout;
    double                      m_latency_time;
    int                         m_socketBufferSize;
    
    //- basler stuff 
    std::string                 m_camera_id;
    std::string                 m_detector_model;
    std::string                 m_detector_type;
    Size                        m_detector_size;
    
    //- Pylon stuff
    PylonAutoInitTerm             auto_init_term_;
    DeviceInfoList_t              devices_;
    Camera_t*                     Camera_;
    Camera_t::StreamGrabber_t*    StreamGrabber_;
    WaitObjectEx                  WaitObject_;
    size_t                        ImageSize_;
    _AcqThread*                   m_acq_thread;
    Cond                          m_cond;
    int                           m_receive_priority;
    bool			  m_color_flag;
    bool			  m_video_flag_mode;
    void*			  m_tmp_buffer[NB_TMP_BUFFER];
    VideoCtrlObj*		  m_video;
    TrigMode			  m_trigger_mode;

    // CCA
    Cond                          m_cond_last_image     ;
    unsigned char *               m_last_image          ;
    LastImageMode                 m_last_image_mode     ;
    unsigned char                 m_last_image_byte_mode;
    uint32_t                      m_last_image_width    ;
    uint32_t                      m_last_image_height   ;
};
} // namespace Basler
} // namespace lima


#endif // BASLERCAMERA_H

