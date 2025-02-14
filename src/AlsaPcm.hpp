/*
 *  Alsa pcm device wrapper
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 *
 * Copyright (C) 2016 EPAM Systems Inc.
 */

#ifndef SRC_ALSAPCM_HPP_
#define SRC_ALSAPCM_HPP_

#include <alsa/asoundlib.h>

#include <xen/be/Exception.hpp>
#include <xen/be/Log.hpp>
#include <xen/be/Utils.hpp>

#include "SoundItf.hpp"

namespace Alsa {

/***************************************************************************//**
 * @defgroup alsa
 * Alsa related classes.
 ******************************************************************************/

/***************************************************************************//**
 * Exception generated by alsa.
 * @ingroup alsa
 ******************************************************************************/
class Exception : public XenBackend::Exception
{
public:
	using XenBackend::Exception::Exception;

private:
	std::string formatMessage(const std::string& msg, int errCode) const override
	{
		return msg + " (" + snd_strerror(errCode) + ")";
	}
};

/***************************************************************************//**
 * Provides alsa pcm functionality.
 * @ingroup alsa
 ******************************************************************************/
class AlsaPcm : public SoundItf::PcmDevice
{
public:
	/**
	 * @param type stream type
	 * @param name pcm device name
	 */
	explicit AlsaPcm(SoundItf::StreamType type,
					 const std::string& deviceName = "default");
	~AlsaPcm();

	/**
	 * Queries the device for HW intervals and masks.
	 * @req HW parameters that the frontend wants to set
	 * @resp refined HW parameters that backend can support
	 */
	void queryHwRanges(SoundItf::PcmParamRanges& req, SoundItf::PcmParamRanges& resp) override;

	/**
	 * Opens the pcm device.
	 * @param params pcm parameters
	 */
	void open(const SoundItf::PcmParams& params) override;

	/**
	 * Closes the pcm device.
	 */
	void close() override;

	/**
	 * Reads data from the pcm device.
	 * @param buffer buffer where to put data
	 * @param size   number of bytes to read
	 */
	void read(uint8_t* buffer, size_t size) override;

	/**
	 * Writes data to the pcm device.
	 * @param buffer buffer with data
	 * @param size   number of bytes to write
	 */
	void write(uint8_t* buffer, size_t size) override;

	/**
	 * Starts the pcm device.
	 */
	void start() override;

	/**
	 * Stops the pcm device.
	 */
	void stop() override;

	/**
	 * Pauses the pcm device.
	 */
	void pause() override;

	/**
	 * Resumes the pcm device.
	 */
	void resume() override;

	/**
	 * Sets progress callback.
	 * @param cbk callback
	 */
	void setProgressCbk(SoundItf::ProgressCbk cbk) override
	{
		mProgressCbk = cbk;
	}

private:

	const snd_pcm_uframes_t cDefaultPeriodFrames = 4096;
	const snd_pcm_uframes_t cDefaultBufferFrames = 16384;

	struct PcmFormat
	{
		uint8_t sndif;
		snd_pcm_format_t alsa;
	};

	static PcmFormat sPcmFormat[];

	snd_pcm_t* mHandle;
	std::string mDeviceName;
	SoundItf::StreamType mType;
	XenBackend::Timer mTimer;
	std::chrono::milliseconds mTimerPeriodMs;
	XenBackend::Log mLog;
	uint32_t dev_status;
	uint32_t query_status;

	SoundItf::PcmParams mParams;

	SoundItf::ProgressCbk mProgressCbk;
	snd_pcm_uframes_t mFrameWritten;
	snd_pcm_uframes_t mFrameUnderrun;

	snd_pcm_t* mHwQueryHandle;
	snd_pcm_hw_params_t* mHwQueryParams;

	void setHwParams(const SoundItf::PcmParams& params);
	void setSwParams();
	void getTimeStamp();
	snd_pcm_format_t convertPcmFormat(uint8_t format);

	void queryOpen();
	void queryClose();

	void queryHwParamRate(snd_pcm_hw_params_t* hwParams,
			      SoundItf::PcmParamRanges& req,
			      SoundItf::PcmParamRanges& resp);
	void queryHwParamBuffer(snd_pcm_hw_params_t* hwParams,
				SoundItf::PcmParamRanges& req,
				SoundItf::PcmParamRanges& resp);
	void queryHwParamChannels(snd_pcm_hw_params_t* hwParams,
				  SoundItf::PcmParamRanges& req,
				  SoundItf::PcmParamRanges& resp);
	void queryHwParamPeriod(snd_pcm_hw_params_t* hwParams,
				SoundItf::PcmParamRanges& req,
				SoundItf::PcmParamRanges& resp);
	void queryHwParamFormats(snd_pcm_hw_params_t* hwParams,
				 SoundItf::PcmParamRanges& req,
				 SoundItf::PcmParamRanges& resp);
};

}

#endif /* SRC_ALSAPCM_HPP_ */
