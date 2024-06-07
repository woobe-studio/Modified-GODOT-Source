/**************************************************************************/
/*  audio_stream.h                                                        */
/**************************************************************************/


#ifndef AUDIO_STREAM_H
#define AUDIO_STREAM_H

#include "core/image.h"
#include "core/resource.h"
#include "servers/audio/audio_filter_sw.h"
#include "servers/audio_server.h"

class AudioStreamPlayback : public Reference {
	GDCLASS(AudioStreamPlayback, Reference);

public:
	virtual void start(float p_from_pos = 0.0) = 0;
	virtual void stop() = 0;
	virtual bool is_playing() const = 0;

	virtual int get_loop_count() const = 0; //times it looped

	virtual float get_playback_position() const = 0;
	virtual void seek(float p_time) = 0;

	virtual void mix(AudioFrame *p_buffer, float p_rate_scale, int p_frames) = 0;
};

class AudioStreamPlaybackResampled : public AudioStreamPlayback {
	GDCLASS(AudioStreamPlaybackResampled, AudioStreamPlayback);

	enum {
		FP_BITS = 16, //fixed point used for resampling
		FP_LEN = (1 << FP_BITS),
		FP_MASK = FP_LEN - 1,
		INTERNAL_BUFFER_LEN = 256,
		CUBIC_INTERP_HISTORY = 4
	};

	AudioFrame internal_buffer[INTERNAL_BUFFER_LEN + CUBIC_INTERP_HISTORY];
	uint64_t mix_offset;

protected:
	void _begin_resample();
	virtual void _mix_internal(AudioFrame *p_buffer, int p_frames) = 0;
	virtual float get_stream_sampling_rate() = 0;

public:
	virtual void mix(AudioFrame *p_buffer, float p_rate_scale, int p_frames);

	AudioStreamPlaybackResampled() { mix_offset = 0; }
};

class AudioStream : public Resource {
	GDCLASS(AudioStream, Resource);
	OBJ_SAVE_TYPE(AudioStream); // Saves derived classes with common type so they can be interchanged.

protected:
	static void _bind_methods();

public:
	virtual Ref<AudioStreamPlayback> instance_playback() = 0;

	virtual float get_length() const = 0; //if supported, otherwise return 0
};

// Microphone

class AudioStreamPlaybackMicrophone;

class AudioStreamMicrophone : public AudioStream {
	GDCLASS(AudioStreamMicrophone, AudioStream);
	friend class AudioStreamPlaybackMicrophone;

	Set<AudioStreamPlaybackMicrophone *> playbacks;

protected:
	static void _bind_methods();

public:
	virtual Ref<AudioStreamPlayback> instance_playback();

	virtual float get_length() const; //if supported, otherwise return 0

	AudioStreamMicrophone();
};

class AudioStreamPlaybackMicrophone : public AudioStreamPlaybackResampled {
	GDCLASS(AudioStreamPlaybackMicrophone, AudioStreamPlaybackResampled);
	friend class AudioStreamMicrophone;

	bool active;
	unsigned int input_ofs;

	Ref<AudioStreamMicrophone> microphone;

protected:
	virtual void _mix_internal(AudioFrame *p_buffer, int p_frames);
	virtual float get_stream_sampling_rate();

public:
	virtual void mix(AudioFrame *p_buffer, float p_rate_scale, int p_frames);

	virtual void start(float p_from_pos = 0.0);
	virtual void stop();
	virtual bool is_playing() const;

	virtual int get_loop_count() const; //times it looped

	virtual float get_playback_position() const;
	virtual void seek(float p_time);

	~AudioStreamPlaybackMicrophone();
	AudioStreamPlaybackMicrophone();
};

//

class AudioStreamPlaybackRandomPitch;

class AudioStreamRandomPitch : public AudioStream {
	GDCLASS(AudioStreamRandomPitch, AudioStream);
	friend class AudioStreamPlaybackRandomPitch;

	Set<AudioStreamPlaybackRandomPitch *> playbacks;
	Ref<AudioStream> audio_stream;
	float random_pitch;

protected:
	static void _bind_methods();

public:
	void set_audio_stream(const Ref<AudioStream> &p_audio_stream);
	Ref<AudioStream> get_audio_stream() const;

	void set_random_pitch(float p_pitch);
	float get_random_pitch() const;

	virtual Ref<AudioStreamPlayback> instance_playback();

	virtual float get_length() const; //if supported, otherwise return 0

	AudioStreamRandomPitch();
};

class AudioStreamPlaybackRandomPitch : public AudioStreamPlayback {
	GDCLASS(AudioStreamPlaybackRandomPitch, AudioStreamPlayback);
	friend class AudioStreamRandomPitch;

	Ref<AudioStreamRandomPitch> random_pitch;
	Ref<AudioStreamPlayback> playback;
	Ref<AudioStreamPlayback> playing;
	float pitch_scale;

public:
	virtual void start(float p_from_pos = 0.0);
	virtual void stop();
	virtual bool is_playing() const;

	virtual int get_loop_count() const; //times it looped

	virtual float get_playback_position() const;
	virtual void seek(float p_time);

	virtual void mix(AudioFrame *p_buffer, float p_rate_scale, int p_frames);

	~AudioStreamPlaybackRandomPitch();
};

#endif // AUDIO_STREAM_H
