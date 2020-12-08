#include <stdexcept>
#include "Window.h"
#include <map>

static const int gre[16] = {	0,   0,   0,   0, 168, 168, 168, 168,
								0,   0,   0,   0, 255, 255, 255, 255, };
static const int red[16] = {	0,   0, 168, 168,   0,   0, 168, 168,
				  	  	  	  	0,   0, 255, 255,   0,   0, 255, 255, };
static const int blu[16] = {   	0, 168,   0, 168,   0, 168,   0, 168,
								0, 255,   0, 255,   0, 255,   0, 255, };


Window::Window(int width, int height)
: _width(width), _height(height)
{
	_window = std::shared_ptr<SDL_Window>(
			SDL_CreateWindow(
					"ZX Spectrum Emu",
					SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
					width, height,
					SDL_WINDOW_SHOWN),
			SDL_DestroyWindow);
	if (_window == nullptr)
		throw std::runtime_error(
				std::string("Window creation failed: ") +
				SDL_GetError());

	_renderer = std::shared_ptr<SDL_Renderer>(
			SDL_CreateRenderer(_window.get(), -1, SDL_RENDERER_ACCELERATED),
			SDL_DestroyRenderer);
	if (_renderer == nullptr)
		throw std::runtime_error(
				std::string("Renderer creation failed: ") +
				SDL_GetError());
}

static std::map<SDL_Scancode, std::pair<unsigned, unsigned>> s_keymap {
	{ SDL_SCANCODE_Q, { 5, 0 } },
	{ SDL_SCANCODE_W, { 5, 1 } },
	{ SDL_SCANCODE_E, { 5, 2 } },
	{ SDL_SCANCODE_R, { 5, 3 } },
	{ SDL_SCANCODE_T, { 5, 4 } },
	{ SDL_SCANCODE_Y, { 2, 4 } },
	{ SDL_SCANCODE_U, { 2, 3 } },
	{ SDL_SCANCODE_I, { 2, 2 } },
	{ SDL_SCANCODE_O, { 2, 1 } },
	{ SDL_SCANCODE_P, { 2, 0 } },

	{ SDL_SCANCODE_A, { 6, 0 } },
	{ SDL_SCANCODE_S, { 6, 1 } },
	{ SDL_SCANCODE_D, { 6, 2 } },
	{ SDL_SCANCODE_F, { 6, 3 } },
	{ SDL_SCANCODE_G, { 6, 4 } },
	{ SDL_SCANCODE_H, { 1, 4 } },
	{ SDL_SCANCODE_J, { 1, 3 } },
	{ SDL_SCANCODE_K, { 1, 2 } },
	{ SDL_SCANCODE_L, { 1, 1 } },
	{ SDL_SCANCODE_RETURN, { 1, 0 } },

	{ SDL_SCANCODE_LSHIFT, { 7, 0 } },
	{ SDL_SCANCODE_Z, { 7, 1 } },
	{ SDL_SCANCODE_X, { 7, 2 } },
	{ SDL_SCANCODE_C, { 7, 3 } },
	{ SDL_SCANCODE_V, { 7, 4 } },
	{ SDL_SCANCODE_B, { 0, 4 } },
	{ SDL_SCANCODE_N, { 0, 3 } },
	{ SDL_SCANCODE_M, { 0, 2 } },
	{ SDL_SCANCODE_RSHIFT, { 0, 1 } },
	{ SDL_SCANCODE_SPACE, { 0, 0 } },

	{ SDL_SCANCODE_1, { 4, 0 } },
	{ SDL_SCANCODE_2, { 4, 1 } },
	{ SDL_SCANCODE_3, { 4, 2 } },
	{ SDL_SCANCODE_4, { 4, 3 } },
	{ SDL_SCANCODE_5, { 4, 4 } },
	{ SDL_SCANCODE_6, { 3, 4 } },
	{ SDL_SCANCODE_7, { 3, 3 } },
	{ SDL_SCANCODE_8, { 3, 2 } },
	{ SDL_SCANCODE_9, { 3, 1 } },
	{ SDL_SCANCODE_0, { 3, 0 } },

};

void Window::handle_event(const SDL_Event &event)
{
	switch(event.type) {
	case SDL_QUIT:
		_want_quit = true;
		break;
	case SDL_KEYDOWN:
		if (event.key.keysym.scancode == SDL_SCANCODE_ESCAPE)
			cpu.reset();
		if (event.key.keysym.scancode == SDL_SCANCODE_F6)
			cpu.save_state_sna("save.sna");
		if (event.key.keysym.scancode == SDL_SCANCODE_F9)
			cpu.load_state_sna("save.sna");
		if (event.key.keysym.scancode == SDL_SCANCODE_F10)
		if (event.key.keysym.scancode == SDL_SCANCODE_F11)

		{
			auto k = s_keymap.find(event.key.keysym.scancode);
			if (k == s_keymap.end()) break;
			io.keydown(k->second.first, k->second.second);
		}
		break;
	case SDL_KEYUP:
		{
			auto k = s_keymap.find(event.key.keysym.scancode);
			if (k == s_keymap.end()) break;
			io.keyup(k->second.first, k->second.second);
		}
		break;
	default:
		;
	}
}

void Window::handle_keys(const Uint8 *keys)
{
}

void Window::do_logic()
{
	cpu.intr(0);
//	cpu.ticks(71590);
	cpu.ticks(25000);
}

void Window::render()
{
	unsigned bdr = io.border();

	SDL_SetRenderDrawColor(_renderer.get(), red[bdr], gre[bdr], blu[bdr], 255);
	SDL_RenderClear(_renderer.get());

	for (unsigned row = 0; row < 192; ++row) {
		for (unsigned col = 0; col < 32; ++col) {
			unsigned point = 32 * row + col;

			unsigned a4a0   =  point        & 0x001f;
			unsigned a10a8  = (point >> 8)  & 0x0007;
			unsigned a7a5   = (point >> 5)  & 0x0007;
			unsigned a12a11 = (point >> 11) & 0x0003;

			unsigned raster_addr =
					0x4000 |
					(a12a11 << 11) |
					(  a7a5 << 8) |
					( a10a8 << 5) |
					(  a4a0 << 0);

			unsigned attr_addr =
					0x5800 |
					(a12a11 << 8) |
					( a10a8 << 5) |
					(  a4a0 << 0);

			uint8_t val = ram.read(raster_addr);
			uint8_t attr = ram.read(attr_addr);
			uint8_t paper_col = ((attr >> 3) & 0x07) | ((attr & 0x40) >> 3);
			uint8_t ink_col =   (attr & 0x07) | ((attr & 0x40) >> 3);
[[maybe_unused]]uint8_t flash = (paper_col >> 7) & 1;

			for (unsigned px = 0; px < 8; ++px) {

				SDL_Rect pixel {
					int(160 + PIXEL_SCALE * 8 * col + PIXEL_SCALE * px),
					int(120 + PIXEL_SCALE * row),
					PIXEL_SCALE, PIXEL_SCALE
				};
				if ((1 << (7 - px)) & val)
					SDL_SetRenderDrawColor(_renderer.get(),
							red[ink_col], gre[ink_col], blu[ink_col], 255);
				else
					SDL_SetRenderDrawColor(_renderer.get(),
							red[paper_col], gre[paper_col], blu[paper_col], 255);
				SDL_RenderFillRect(_renderer.get(), &pixel);
			}
		}
	}
}


void Window::main() {
	SDL_Event event;
	auto keys = SDL_GetKeyboardState(nullptr);

	while (_want_quit == false) {
		while (SDL_PollEvent(&event))
			handle_event(event);
		handle_keys(keys);

		do_logic();

		render();
		SDL_RenderPresent(_renderer.get());
	}
}
