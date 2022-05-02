#pragma once
class AudioManager
{
private:
	float audio_volume;
public:
	void setVolume(float volume) {
		audio_volume = volume;
	}
	float getVolume() {
		return audio_volume;
	}
};

