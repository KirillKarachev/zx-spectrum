#ifndef AUDIODRIVER_H_
#define AUDIODRIVER_H_

#include <SDL2/SDL.h>

class AudioDriver
{
protected:
	SDL_AudioDeviceID _dev;
	SDL_AudioSpec _spec;

	short _level;
public:
	AudioDriver();
	~AudioDriver();

	void callback(short * buffer, size_t len);

	void set_level(short level) { _level = level; }
};




#endif /* AUDIODRIVER_H_ */
