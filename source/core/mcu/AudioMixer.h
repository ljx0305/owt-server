/*
 * Copyright 2014 Intel Corporation All Rights Reserved. 
 * 
 * The source code contained or described herein and all documents related to the 
 * source code ("Material") are owned by Intel Corporation or its suppliers or 
 * licensors. Title to the Material remains with Intel Corporation or its suppliers 
 * and licensors. The Material contains trade secrets and proprietary and 
 * confidential information of Intel or its suppliers and licensors. The Material 
 * is protected by worldwide copyright and trade secret laws and treaty provisions. 
 * No part of the Material may be used, copied, reproduced, modified, published, 
 * uploaded, posted, transmitted, distributed, or disclosed in any way without 
 * Intel's prior express written permission.
 * 
 * No license under any patent, copyright, trade secret or other intellectual 
 * property right is granted to or conferred upon you by disclosure or delivery of 
 * the Materials, either expressly, by implication, inducement, estoppel or 
 * otherwise. Any license under such intellectual property rights must be express 
 * and approved by Intel in writing.
 */

#ifndef AudioMixer_h
#define AudioMixer_h

#include <boost/asio.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <logger.h>
#include <MediaDefinitions.h>
#include <WoogeenTransport.h>
#include <webrtc/voice_engine/include/voe_base.h>
#include <webrtc/voice_engine/include/voe_video_sync.h>

namespace mcu {

class AudioMixer : public erizo::MediaSink {
    DECLARE_LOGGER();

public:
    AudioMixer(erizo::RTPDataReceiver*);
    virtual ~AudioMixer();

    int32_t addSource(erizo::MediaSource*);
    int32_t removeSource(erizo::MediaSource*);

    // Implement the MediaSink interfaces.
    int deliverAudioData(char*, int len, erizo::MediaSource*);
    int deliverVideoData(char*, int len, erizo::MediaSource*);

    webrtc::VoEVideoSync* avSyncInterface();

private:
    int32_t performMix(const boost::system::error_code&);
    int32_t updateAudioFrame();

    struct VoiceChannel {
        int32_t id;
        boost::shared_ptr<woogeen_base::WoogeenTransport<erizo::AUDIO>> transport;
    };

    webrtc::VoiceEngine* m_voiceEngine;

    VoiceChannel m_outChannel;
    std::map<erizo::MediaSource*, VoiceChannel> m_inChannels;
    boost::shared_mutex m_sourceMutex;

    boost::scoped_ptr<boost::thread> m_audioMixingThread;
    boost::asio::io_service m_ioService;
    boost::scoped_ptr<boost::asio::deadline_timer> m_timer;
    std::atomic<bool> m_isClosing;
};

inline webrtc::VoEVideoSync* AudioMixer::avSyncInterface()
{
    return m_voiceEngine ? webrtc::VoEVideoSync::GetInterface(m_voiceEngine) : nullptr;
}

} /* namespace mcu */

#endif /* AudioMixer_h */